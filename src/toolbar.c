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
#include "toolbar.h"

static KuduToolbarItem *k_toolbar = NULL;
static GtkWidget *m_toolbar = NULL;
static GtkAccelGroup *k_accel_group = NULL;
static GCallback k_action;
static char *k_path = NULL;
static GSList *radio_group = NULL;
static gulong last_signal_id = 0;

gulong kudu_toolbar_last_signal_id(void)
{
	return last_signal_id;
}

int kudu_toolbar_set_path(char *path)
{
	if (path == NULL) {
		if (k_path != NULL) free(k_path);
		k_path = NULL;
		return TRUE;
	}

	k_path = (char*)malloc(strlen(path)+4);
	sprintf(k_path, "<%s>/\0", path);

	return TRUE;
}

int kudu_toolbar_start_new(char *path, GtkAccelGroup *accel_group, GCallback callback)
{
	kudu_toolbar_set_path(path);

	m_toolbar = gtk_toolbar_new();

	k_accel_group = accel_group;
	k_action = callback;

	return TRUE;
}

GtkWidget *kudu_toolbar_end(void)
{
	kudu_toolbar_set_path(NULL);

	return m_toolbar;
}

int kudu_toolbar_item_set_accelerator(GtkToolItem *tool_item, char *accelerator)
{
	if ((k_accel_group == NULL) || (k_path == NULL))  return FALSE;
	const char *label = gtk_tool_button_get_label(GTK_TOOL_BUTTON(tool_item));
	char *sp;

	sp = (char*)malloc(strlen(k_path) + strlen(label) + 1);
	sprintf(sp, "%s%s\0", k_path, label);

/*	gtk_object_class_user_signal_new(GTK_OBJECT(tool_item), "activate", GTK_RUN_ACTION, GTK_TYPE_NONE, 1, GTK_TYPE_POINTER);*/

	/*GTK_WIDGET(tool_item)->activate_signal = g_signal_lookup("clicked", G_TYPE_INTERFACE);*/

	/*gtk_widget_set_accel_path(GTK_WIDGET(tool_item), sp, k_accel_group);*/

	if (accelerator == NULL) {
		free(sp);
		return TRUE;
	}

	guint accel_key;
	GdkModifierType accel_mods;
	gtk_accelerator_parse(accelerator, &accel_key, &accel_mods);
	gtk_accel_map_add_entry(sp, accel_key, accel_mods);

	free(sp);

	return TRUE;
}

void kudu_toolbar_new_radio_group(void)
{
	radio_group = NULL;
}

GtkToolItem *kudu_toolbar_add_radio_button(unsigned int action, char *label, char *image_f, char *accelerator)
{
	if (m_toolbar == NULL) return NULL;

	GtkToolItem *tool_item;
	GtkWidget *image;
	int position;

	tool_item = gtk_radio_tool_button_new(radio_group);
	last_signal_id = g_signal_connect(G_OBJECT(tool_item), "clicked", G_CALLBACK(k_action), (gpointer)action);

	if (label != NULL) gtk_tool_button_set_label(GTK_TOOL_BUTTON(tool_item), label);
	if (image_f != NULL) {
		image = gtk_image_new_from_file(image_f);
		gtk_tool_button_set_icon_widget(GTK_TOOL_BUTTON(tool_item), image);
	}

	position = gtk_toolbar_get_n_items(GTK_TOOLBAR(m_toolbar));
	gtk_toolbar_insert(GTK_TOOLBAR(m_toolbar), tool_item, -1);

	radio_group = gtk_radio_tool_button_get_group(GTK_RADIO_TOOL_BUTTON(tool_item));

	if (label != NULL) kudu_toolbar_item_set_accelerator(tool_item, accelerator);

	return tool_item;
}

GtkToolItem *kudu_toolbar_add_stock_item(unsigned int action, char *label, char *stock, char *accelerator)
{
	GtkToolItem *tool_item;

	tool_item = gtk_tool_button_new_from_stock(stock);
	last_signal_id = g_signal_connect(G_OBJECT(tool_item), "clicked", G_CALLBACK(k_action), (gpointer)action);

	gtk_tool_button_set_label(GTK_TOOL_BUTTON(tool_item), label);

	gtk_toolbar_insert(GTK_TOOLBAR(m_toolbar), tool_item, -1);

	if (label != NULL) kudu_toolbar_item_set_accelerator(tool_item, accelerator);

	return tool_item;
}

GtkToolItem *kudu_toolbar_add_seperator(int visible, int expand)
{
	if (m_toolbar == NULL) return NULL;

	GtkToolItem *tool_item;

	tool_item = gtk_separator_tool_item_new();
	gtk_separator_tool_item_set_draw(GTK_SEPARATOR_TOOL_ITEM(tool_item), visible);
	gtk_tool_item_set_expand(tool_item, expand);
	gtk_toolbar_insert(GTK_TOOLBAR(m_toolbar), tool_item, -1);

	return tool_item;
}






/* Routines for storing and retrieving toolbar items... */

int kudu_toolbar_store_init(void)
{
	if (k_toolbar != NULL) return FALSE;

	int a;

	k_toolbar = malloc((sizeof(KuduToolbarItem)*KT_NUM_ITEMS));
	if (k_toolbar == NULL) return FALSE;

	for (a = 0; a < KT_NUM_ITEMS; a++) {
		k_toolbar[a].tool_item = NULL;
	}

	return TRUE;
}

int kudu_toolbar_store_item(K_MainToolbar item, GtkToolItem *tool_item)
{
	if ((k_toolbar == NULL) || (item >= KT_NUM_ITEMS) || (tool_item == NULL)) return FALSE;

	k_toolbar[item].tool_item = tool_item;
	k_toolbar[item].signal = kudu_toolbar_last_signal_id();

	return TRUE;
}

GtkToolItem *kudu_toolbar_get_item(K_MainToolbar item)
{
	if ((k_toolbar == NULL) || (item >= KT_NUM_ITEMS)) return NULL;

	return k_toolbar[item].tool_item;
}

int kudu_toolbar_item_disable(K_MainToolbar item)
{
	if ((k_toolbar == NULL) || (item >= KT_NUM_ITEMS)) return FALSE;

	gtk_widget_set_sensitive(GTK_WIDGET(k_toolbar[item].tool_item), FALSE);

	return TRUE;
}

int kudu_toolbar_item_enable(K_MainToolbar item)
{
	if ((k_toolbar == NULL) || (item >= KT_NUM_ITEMS)) return FALSE;

	gtk_widget_set_sensitive(GTK_WIDGET(k_toolbar[item].tool_item), TRUE);

	return TRUE;
}

int kudu_toolbar_item_activate(K_MainToolbar item)
{
	if ((k_toolbar == NULL) || (item >= KT_NUM_ITEMS)) return FALSE;

	gtk_widget_activate(GTK_WIDGET(k_toolbar[item].tool_item));

	return TRUE;
}

int kudu_toolbar_item_activate_no_call(K_MainToolbar item)
{
	if ((k_toolbar == NULL) || (item >= KT_NUM_ITEMS)) return FALSE;

	g_signal_handler_block(k_toolbar[item].tool_item, k_toolbar[item].signal);
	gtk_widget_activate(GTK_WIDGET(k_toolbar[item].tool_item));
	g_signal_handler_unblock(k_toolbar[item].tool_item, k_toolbar[item].signal);

	return TRUE;
}

int kudu_toolbar_toggle_item_set_active_no_call(K_MainToolbar item, int state)
{
	if ((k_toolbar == NULL) || (item >= KT_NUM_ITEMS)) return FALSE;

	g_signal_handler_block(k_toolbar[item].tool_item, k_toolbar[item].signal);
	gtk_toggle_tool_button_set_active(GTK_TOGGLE_TOOL_BUTTON(k_toolbar[item].tool_item), state);
	g_signal_handler_unblock(k_toolbar[item].tool_item, k_toolbar[item].signal);

	return TRUE;
}


#define KT_RADIO(item, label, icon, accel) (kudu_toolbar_store_item(item, kudu_toolbar_add_radio_button(item, label, icon, accel)))

GtkWidget *kudu_main_toolbar_build(GtkAccelGroup *accel_group, GCallback action)
{
	GtkWidget *toolbar;
	GtkToolItem *tool_item;
	const char *image_dir = kudu_options_get_string(KO_DIR_KUDU_IMAGES);
	char *imgf = (char*)malloc(strlen(image_dir)+50);

	kudu_toolbar_store_init();

	kudu_toolbar_start_new("main-toolbar", accel_group, action);

	kudu_toolbar_add_stock_item(KT_FILE_OPEN, "Open", GTK_STOCK_OPEN, NULL);
	kudu_toolbar_add_stock_item(KT_FILE_SAVE, "Save", GTK_STOCK_SAVE, NULL);

	kudu_toolbar_add_seperator(TRUE, FALSE);
	kudu_toolbar_add_seperator(FALSE, TRUE);
	kudu_toolbar_add_seperator(TRUE, FALSE);

	kudu_toolbar_new_radio_group();
	sprintf(imgf, "%sobject.png\0", image_dir);
	KT_RADIO(KT_SELECT_OBJECTS, "Objects", imgf, "<ctrl><shift>O");

	sprintf(imgf, "%sbone.png\0", image_dir);
	KT_RADIO(KT_SELECT_BONES, "Bones", imgf, "<ctrl><shift>B");

	sprintf(imgf, "%sjoint.png\0", image_dir);
	KT_RADIO(KT_SELECT_JOINTS, "Joints", imgf, "<ctrl><shift>J");

	kudu_toolbar_add_seperator(TRUE, FALSE);

	sprintf(imgf, "%svertex.png\0", image_dir);
	KT_RADIO(KT_SELECT_VERTICES, "Vertices", imgf, "<ctrl><shift>V");

	sprintf(imgf, "%sedge.png\0", image_dir);
	KT_RADIO(KT_SELECT_EDGES, "Edges", imgf, "<ctrl><shift>E");

	sprintf(imgf, "%sface.png\0", image_dir);
	KT_RADIO(KT_SELECT_FACES, "Faces", imgf, "<ctrl><shift>F");

	sprintf(imgf, "%sshape.png\0", image_dir);
	KT_RADIO(KT_SELECT_SHAPES, "Shapes", imgf, "<ctrl><shift>S");

	kudu_toolbar_add_seperator(TRUE, FALSE);
	kudu_toolbar_add_seperator(FALSE, TRUE);

	toolbar = kudu_toolbar_end();

	free(imgf);

	return toolbar;
}






















