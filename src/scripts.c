/******************************************************************************/
/*                                                                            */
/* Kudu Animator                                                              */
/* Copyright (C) 2005 Daniel Pekelharing                                      */
/* <redarrow@users.sourceforge.net>                                           */
/*                                                                            */
/* This program is free software; you can redistribute it and/or modify       */
/* it under the terms of the GNU General Public License as published by       */
/* the Free Software Foundation; either version 2 of the License, or          */
/* (at your option) any later version.                                        */
/*                                                                            */
/* This program is distributed in the hope that it will be useful,            */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of             */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              */
/* GNU General Public License for more details.                               */
/*                                                                            */
/* You should have received a copy of the GNU General Public License          */
/* along with this program; if not, write to the Free Software                */
/* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA  */
/*                                                                            */
/******************************************************************************/
#include "scripts.h"

static int script_init = FALSE;
static KuduScript *script_list = NULL, *sl_next_do = NULL;

/* The script struct has actually little to do with scripting itself */
/* it's just to create a handy list of discovered scripts */
KuduScript *kudu_script_new(KuduScript *previous_script)
{
	KuduScript *script, *old_next_script;

	script = (KuduScript*)malloc(sizeof(KuduScript));
	if (script == NULL) return NULL;

	if (previous_script == NULL) {
		script->previous_script = NULL;
		script->next_script = NULL;
	} else {
		old_next_script = previous_script->next_script;
		script->previous_script = previous_script;
		script->next_script = old_next_script;
		previous_script->next_script = script;

		if (old_next_script != NULL) old_next_script->previous_script = script;
	}

	script->filename = NULL;
	script->author = NULL;
	script->name = NULL;
	script->menu = NULL;
	script->type = KS_GENERAL;

	return script;
}

int kudu_script_set_filename(KuduScript *script, char *filename)
{
	if (script == NULL) return FALSE;

	if (filename == NULL) {
		if (script->filename != NULL) free(script->filename);
		return TRUE;
	}

	int len = strlen(filename)+1;

	if (script->filename == NULL) {
		script->filename = (char*)malloc(len);
	} else {
		if (strlen(script->filename) != len) script->filename = (char*)realloc(script->filename, len);
	}

	sprintf(script->filename, "%s\0", filename);

	return TRUE;
}

int kudu_script_set_name(KuduScript *script, char *name)
{
	if (script == NULL) return FALSE;

	if (name == NULL) {
		if (script->name != NULL) free(script->name);
		return TRUE;
	}

	int len = strlen(name)+1;

	if (script->name == NULL) {
		script->name = (char*)malloc(len);
	} else {
		if (strlen(script->name) != len) script->name = (char*)realloc(script->name, len);
	}

	sprintf(script->name, "%s\0", name);

	return TRUE;
}

int kudu_script_set_author(KuduScript *script, char *author)
{
	if (script == NULL) return FALSE;

	if (author == NULL) {
		if (script->author != NULL) free(script->author);
		return TRUE;
	}

	int len = strlen(author)+1;

	if (script->author == NULL) {
		script->author = (char*)malloc(len);
	} else {
		if (strlen(script->author) != len) script->author = (char*)realloc(script->author, len);
	}

	sprintf(script->author, "%s\0", author);

	return TRUE;
}

int kudu_script_set_menu(KuduScript *script, char *menu)
{
	if (script == NULL) return FALSE;

	if (menu == NULL) {
		if (script->menu != NULL) free(script->menu);
		return TRUE;
	}

	int len = strlen(menu)+1;

	if (script->menu == NULL) {
		script->menu = (char*)malloc(len);
	} else {
		if (strlen(script->menu) != len) script->menu = (char*)realloc(script->menu, len);
	}

	sprintf(script->menu, "%s\0", menu);

	return TRUE;
}

int kudu_script_destroy(KuduScript *script)
{
	if (script == NULL) return FALSE;

	KuduScript *previous, *next;

	previous = script->previous_script;
	next = script->next_script;

	if (previous != NULL) previous->next_script = next;
	if (next != NULL) next->previous_script = previous;

	kudu_script_set_filename(script, NULL);
	kudu_script_set_name(script, NULL);
	kudu_script_set_author(script, NULL);
	kudu_script_set_menu(script, NULL);

	free(script);

	return TRUE;
}

static void kudu_script_embed_run(GtkWidget *widget, KuduScript *script)
{
	kudu_script_run(script);
}

int kudu_script_register(KuduScript *script)
{
	if (script == NULL) return FALSE;

	GtkWidget *menu_item, *menu;

	switch (script->type) {
		case KS_EMBED:
			if (script->menu == NULL) break;
			if (strcasecmp(script->menu, "import") == 0) {
				menu = gtk_menu_item_get_submenu(GTK_MENU_ITEM(kudu_menu_get_item(KM_FILE_IMPORT)));
				if (menu == NULL) break;
				menu_item = gtk_menu_item_new_with_label(script->name);
				gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);
				g_signal_connect(G_OBJECT(menu_item), "activate", G_CALLBACK(kudu_script_embed_run), script);
			}
			break;
	}


	return TRUE;
}


int kudu_script_process(char *path, char *script)
{
	if ((path == NULL) || (script == NULL)) return FALSE;


	int a, b = FALSE, scanning = TRUE, opened = FALSE;
	FILE *inf;
	char *fullpath = NULL, head[1024], data[1024];
	KuduScript *new_script;

	a = strlen(path);
	if (path[a-1] != '/') {
		a++;
		b = TRUE;
	}
	a += strlen(script)+1;

	fullpath = (char*)malloc(a);

	if (b)	sprintf(fullpath, "%s/%s\0", path, script);
	else	sprintf(fullpath, "%s%s\0", path, script);

	if ((inf = fopen(fullpath, "r")) == NULL) {
		if (fullpath != NULL) free(fullpath);
		return FALSE;
	}

	fgets(head, 1024, inf);

	head[strlen(head)-1] = '\0';

	if (strcasecmp(head, "#!KUDU") != 0) {	/* File is not a valid Kudu Python script */
		fclose(inf);
		free(fullpath);
		return FALSE;
	}

	new_script = kudu_script_new(script_list);
	if (script_list == NULL) script_list = new_script;

	kudu_script_set_filename(new_script, fullpath);

	do {
		if (fgets(head, 1024, inf) == NULL) {
			scanning = FALSE;
		} else {
			head[strlen(head)-1] = '\0';
			if (strcmp(head, "") == 0) {
				if (opened < 2) scanning = TRUE;
				else	scanning = FALSE;
			} else {
				if (strncmp(head, "\"\"\"", 3) == 0) {
					opened++;
				} else if (!opened) {
					scanning = FALSE;
				} else {	/* If we get here then we have found some text between the triple quotes... */
					if (strncasecmp(head, "Name:", 5) == 0) {
						kudu_util_get_endstr(head, data, 5);
						kudu_script_set_name(new_script, data);
					} else if (strncasecmp(head, "Menu:", 5) == 0) {
						kudu_util_get_endstr(head, data, 5);
						kudu_script_set_menu(new_script, data);
					} else if (strncasecmp(head, "Author:", 7) == 0) {
						kudu_util_get_endstr(head, data, 7);
						kudu_script_set_author(new_script, data);
					} else if (strncasecmp(head, "Type:", 5) == 0) {
						kudu_util_get_endstr(head, data, 5);
						if (strcasecmp(data, "embed") == 0) {
							new_script->type = KS_EMBED;
						}
					}
				}
			}
		}


	} while (scanning);

	if (new_script->name == NULL) {
		a = strlen(script);
		a -= 3;
		if (a > 1023) a = 1023;
		strncpy(data, script, a);
		data[a] = '\0';
		kudu_script_set_name(new_script, data);
	}

	fclose(inf);
	if (fullpath != NULL) free(fullpath);

	kudu_script_register(new_script);

	return TRUE;
}

int kudu_script_filter(const struct dirent *file)
{
	if (strncmp(&file->d_name[strlen(file->d_name)-3], ".py", 3) == 0) return TRUE;

	return FALSE;
}

int kudu_script_list_init(void)
{
	if (script_list != NULL) return FALSE;

	int a, b, c;
	char *script_path, *scripts;
	struct dirent **namelist;
	const char *scripts_dir = kudu_options_get_string(KO_DIR_KUDU_SCRIPTS);
	const char *user_scripts = kudu_options_get_string(KO_DIR_USER_SCRIPTS);

	a = strlen(scripts_dir)+1;
	b = strlen(user_scripts)+1;
	if (a+b > 0) {
		script_path = (char*)malloc(a+b+3);
		if ((a) && (b)) sprintf(script_path, ".;%s;%s\0", scripts_dir, user_scripts);
		else if (a) sprintf(script_path, ".;%s\0", scripts_dir);
		else if (b) sprintf(script_path, ".;%s\0", user_scripts);
	} else {
		script_path = (char*)malloc(2);
		sprintf(script_path, ".\0");
	}

	scripts = strtok(script_path, ";");

	do {
		a = scandir(scripts, &namelist, kudu_script_filter, alphasort);

		if (a > 0) {
			while (a--) {

				kudu_script_process(scripts, namelist[a]->d_name);
				free(namelist[a]);
			}
		}

		scripts = strtok(NULL, ";");
	} while (scripts != NULL);

	if (script_path != NULL) free(script_path);

	return TRUE;
}

int kudu_script_list_destroy(void)
{
	if (script_list == NULL) return FALSE;

	KuduScript *current = NULL, *destroy = NULL;

	do {
		destroy = current;
		if (current == NULL) current = script_list;
		else	current = current->next_script;

		if (destroy != NULL) kudu_script_destroy(destroy);

	} while (current->next_script != NULL);

	kudu_script_destroy(current);

	script_list = NULL;

	return TRUE;
}

int kudu_script_list_for_each_do(void)
{
	if (script_list == NULL) return FALSE;

	sl_next_do = script_list;

	return TRUE;
}

KuduScript *kudu_script_list_next_do(void)
{
	if (sl_next_do == NULL) return NULL;

	KuduScript *rs = sl_next_do;

	sl_next_do = sl_next_do->next_script;

	return rs;
}














static PyMethodDef kudusys_methods[] = {
	{NULL, NULL, 0, NULL}
};


int kudu_script_stop(void)
{
	if (!script_init) return FALSE;

	Py_Finalize();

	script_init = FALSE;

	return TRUE;
}

int kudu_script_start(KuduObject *object, KuduFrameList *frame_list, KuduSelectionList *selection_list, KuduSelectionList *selected_bones)
{
	if (script_init) return FALSE;

	Py_Initialize();
	script_init = TRUE;

	PyImport_AddModule("kudusys");
	Py_InitModule("kudusys", kudusys_methods);

	kudu_script_init_module_objects(object, frame_list, selection_list, selected_bones);
	/*kudu_script_init_module_options();*/
	kudu_script_init_module_gui();

	return TRUE;
}

static KuduObject **object;
static KuduFrameList **frame_list;
static KuduSelectionList **selection_list, **selected_bones;

void kudu_script_init_objects(KuduObject **obj, KuduFrameList **frames, KuduSelectionList **selection, KuduSelectionList **sbones)
{
	object = obj;
	frame_list = frames;
	selection_list = selection;
	selected_bones = sbones;
}


int kudu_script_run(KuduScript *script)
{

	if (script == NULL) return FALSE;

	FILE *file;
	int was_running = script_init;
	const char *filename = script->filename;

	if (filename == NULL) return FALSE;

	if ((file = fopen(filename, "r")) == NULL) return FALSE;

	kudu_script_start(*object, *frame_list, *selection_list, *selected_bones);

	PyRun_SimpleFile(file, filename);

	if (!was_running) kudu_script_stop();

	fclose(file);

	return TRUE;
}

