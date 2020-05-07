/******************************************************************************/
/*                                                                            */
/* Kudu Animator                                                              */
/* Copyright (C) 2005-2006 Daniel Pekelharing                                 */
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
#include "kudu.h"


int kudu_program_select_bone(KuduBone*);


/* extern Global Varibles */

K_PROGRAM program;

/* Global Varibles */

static KuduFrameList *frame_list = NULL;
static KuduCamera *camera;
static KuduObject *object;
static KuduSelectionList *selection_list = NULL, *selected_bones_list = NULL;

/* Const Declarations */

const char PROGRAM_MODE_NAME[8][16] = {"Skeleton Mode", "Skin Mode", "Attach Mode", "View Mode", "Sequence Mode", "Script Mode", "", "Playback Mode"};


/* Program */

static int kudu_program_select_object(KuduObject *obj)
{
	if (obj == NULL) return FALSE;

	if (program.selected_object == obj) return TRUE;

	if (program.selected_object != NULL) {
		kudu_selection_list_select_bones(selected_bones_list, program.selected_object->bone, SELECT_NOTHING);
		kudu_selection_list_select(selection_list, program.selected_object,
			kudu_options_get_int_no(KO_SELECTION_MODE, program.mode_opt_no), SELECT_NOTHING);
	}

	program.selected_object = obj;

	kudu_gui_objects_menu_select(obj);
	kudu_program_skeleton_list_regen();
	kudu_program_skin_list_regen();

	return TRUE;
}

int kudu_program_object_select(GtkWidget *menu_item, KuduObject *obj)
{
	if (!GTK_CHECK_MENU_ITEM(menu_item)->active) return FALSE;
	if (obj == NULL) return FALSE;

	kudu_program_select_object(obj);

	return TRUE;
}

int kudu_program_new(void)
{
	int return_val;
	KuduObject *new_obj;

	if (new_obj = kudu_object_new_with_defaults(object)) return_val = TRUE;
	else	return_val = FALSE;

	if (object == NULL) object = new_obj;

	program.selected_object = new_obj;

	kudu_gui_objects_menu_add(new_obj, G_CALLBACK(kudu_program_object_select));
	return return_val;
}

int kudu_program_close(void)
{
	if (object == NULL) return FALSE;

	int return_val;
	KuduObject *first = NULL, *ns = NULL;

	if (program.selected_object == object) {
		first = object->next_object;
	}

	ns = program.selected_object;
	if (ns->next_object == NULL) ns = ns->previous_object;
	else	ns = ns->next_object;

	kudu_gui_objects_menu_remove(program.selected_object);

	return_val = (kudu_object_destroy(program.selected_object));

	program.selected_object = ns;

	kudu_selection_list_remove_all_items(selection_list);
	kudu_selection_list_remove_all_items(selected_bones_list);

	return return_val;
}

gint kudu_program_exit(GtkWidget *widget, gpointer data)
{
	GtkWidget *mdialog;
	GtkWidget *main_win;

	main_win = STACK_GET_WIDGET("main_window");;

	mdialog = gtk_message_dialog_new(GTK_WINDOW(main_win), GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_QUESTION,
			GTK_BUTTONS_OK_CANCEL, "Exit? are you sure?");

	if (gtk_dialog_run(GTK_DIALOG(mdialog)) != GTK_RESPONSE_OK) {
		gtk_widget_destroy(mdialog);
		return TRUE;
	} else {
		/* Cleanup */
		kudu_object_destroy(object);
		kudu_selection_list_destroy(selection_list);
		kudu_selection_list_destroy(selected_bones_list);
		kudu_camera_destroy(camera);
		kudu_stack_list_destroy();
		kudu_hits_list_destroy();
		kudu_options_finish();
		#ifdef SUPPORT_PYTHON
		kudu_script_list_destroy();
		#endif

		gtk_widget_destroy(mdialog);
		//if (GTK_IS_WIDGET(data)) gtk_widget_destroy(GTK_WIDGET(data));
		gtk_widget_destroy(main_win);
		gtk_main_quit();
		return FALSE;
	}
}

void kudu_program_save_kudu_object(void)
{
	if (program.selected_object == NULL) {
		kudu_gui_message("There are no open objects to save!", GTK_MESSAGE_ERROR);
		return;
	}

	int ns = FALSE;

	if (program.selected_object->filename_length == 0) ns = TRUE;

	if (ns) {
		kudu_program_request_save_kudu_object();
		return;
	}

	kudu_object_save_to_file(program.selected_object, program.selected_object->filename);
}

void kudu_program_request_save_kudu_object(void)
{
	if (program.selected_object == NULL) {
		kudu_gui_message("There are no open objects to save!", GTK_MESSAGE_ERROR);
		return;
	}

	GtkWidget *file_selection;
	GtkFileFilter *filter;
	gchar *selected_filename, *current_path;
	int ns = FALSE;

	if (program.selected_object->filename_length == 0) ns = TRUE;

	if (ns) {
		file_selection = kudu_gui_save_dialog("Save a Kudu Object", "Kudu Animated Objects", "*.kudu",
			"untitled.kudu");
	} else {
		file_selection = kudu_gui_save_dialog("Save a Kudu Object", "Kudu Animated Objects", "*.kudu",
			program.selected_object->filename);
	}

	switch (gtk_dialog_run(GTK_DIALOG(file_selection))) {
		case GTK_RESPONSE_ACCEPT:
			selected_filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(file_selection));
			kudu_object_set_filename(program.selected_object, selected_filename);
			g_free(selected_filename);
			kudu_program_save_kudu_object();
			break;
	}

	current_path = gtk_file_chooser_get_current_folder(GTK_FILE_CHOOSER(file_selection));
	if (current_path != NULL) {
		kudu_options_set_string(KO_DIR_LAST_SAVE, current_path);
		g_free(current_path);
	}

	gtk_widget_destroy(file_selection);
}

void kudu_program_open_kudu_object(void)
{
	GtkWidget *file_selection;
	gchar *selected_filename;
	char *current_path;
	KuduObject *obj;

	file_selection = kudu_gui_open_dialog("Open a Kudu Object", "Kudu Animated Objects", "*.kudu");

	switch (gtk_dialog_run(GTK_DIALOG(file_selection))) {
		case GTK_RESPONSE_ACCEPT:
			selected_filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(file_selection));

			obj = kudu_object_new_from_file(object, selected_filename);
			if (obj == NULL) {
				gtk_widget_destroy(file_selection);
				kudu_gui_message("Invalid or corrupted danbuilder object file!", GTK_MESSAGE_ERROR);
				return;
			}

			if (object == NULL) object = obj;
			kudu_program_select_object(obj);

			kudu_gui_objects_menu_add(obj, G_CALLBACK(kudu_program_object_select));
			kudu_object_set_filename(obj, selected_filename);
			g_free(selected_filename);
			kudu_program_skin_list_regen();
			break;

	}

	current_path = gtk_file_chooser_get_current_folder(GTK_FILE_CHOOSER(file_selection));
	if (current_path != NULL) {
		kudu_options_set_string(KO_DIR_LAST_OPEN, current_path);
		g_free(current_path);
	}

	gtk_widget_destroy(file_selection);
}

void kudu_program_import_rwx(void)
{
	GtkWidget *file_selection;
	gchar *selected_filename;
	KuduObject *new_obj;

	file_selection = kudu_gui_open_dialog("Import a Renderware (rwx) Object", "Renderware Objects", "*.rwx");

	switch (gtk_dialog_run(GTK_DIALOG(file_selection))) {
		case GTK_RESPONSE_ACCEPT:
			selected_filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(file_selection));

			if ((new_obj = kudu_object_new_with_defaults(object)) == NULL) return;

			if (kudu_object_import_rwx_from_file(new_obj, selected_filename)) {
				kudu_gui_objects_menu_add(new_obj, G_CALLBACK(kudu_program_object_select));
				program.selected_object = new_obj;
				if (object == NULL) object = new_obj;
			} else {
				kudu_object_destroy(new_obj);
			}

			g_free(selected_filename);

			kudu_program_skin_list_regen();
			break;
	}

	gtk_widget_destroy(file_selection);
}

void kudu_program_import_obj(void)
{
	GtkWidget *file_selection;
	gchar *selected_filename;
	KuduObject *new_obj;

	file_selection = kudu_gui_open_dialog("Import a Wavefront (obj) Object", "Wavefront Objects", "*.obj");

	switch (gtk_dialog_run(GTK_DIALOG(file_selection))) {
		case GTK_RESPONSE_ACCEPT:
			selected_filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(file_selection));

			if ((new_obj = kudu_object_new_with_defaults(object)) == NULL) return;

			if (kudu_object_import_obj_from_file(new_obj, selected_filename)) {
				kudu_gui_objects_menu_add(new_obj, G_CALLBACK(kudu_program_object_select));
				program.selected_object = new_obj;
				if (object == NULL) object = new_obj;
			} else {
				kudu_object_destroy(new_obj);
			}

			g_free(selected_filename);

			kudu_program_skin_list_regen();
			break;
	}

	gtk_widget_destroy(file_selection);
}

void kudu_program_edit_mode_set(int mode)
{
	program.edit_mode = mode;

	if (mode == EDIT_MODE_NONE) return;

	kudu_skin_edit_anchor();

}

int kudu_program_selection_mode_set(int MODE, int regen)
{
	/*if (program.selection_mode == MODE) return TRUE;*/
/*	if (kudu_options_get_int_no(KO_SELECTION_MODE, program.mode_opt_no) == MODE) return TRUE;*/

	/*program.selection_mode = MODE;*/

	kudu_options_set_int_no(KO_SELECTION_MODE, program.mode_opt_no, MODE);

	if ((MODE > S_MESH) && (MODE < E_MESH)) {
		kudu_selection_list_set_type(selection_list, MODE, TRUE);
	} else {
		kudu_selection_list_set_type(selected_bones_list, MODE, TRUE);
	}

	/* Configure various menu items and stuff according to selection mode */
	if (program.mode == PROGRAM_MODE_EDIT) {

		switch (MODE) {
			case SELECT_BONES:
				kudu_menu_item_set_visible(KM_EDIT_BONES, TRUE);
				kudu_menu_item_set_visible(KM_EDIT_JOINTS, FALSE);
				break;
			case SELECT_JOINTS:
				kudu_menu_item_set_visible(KM_EDIT_BONES, FALSE);
				kudu_menu_item_set_visible(KM_EDIT_JOINTS, TRUE);
				break;
		}

	} else if (program.mode == PROGRAM_MODE_ANIMATION) {

		/* Set up menu items as most commonly used first */
		/* then individual selection modes need just adjust the differences */
		kudu_menu_item_set_visible(KM_EDIT_BONES, FALSE);
		kudu_menu_item_set_visible(KM_EDIT_SKIN, TRUE);

		switch (MODE) {
			case SELECT_OBJECTS:
				kudu_menu_item_set_visible(KM_EDIT_SKIN, FALSE);
				break;
			case SELECT_BONES:
				kudu_menu_item_set_visible(KM_EDIT_BONES, TRUE);
				kudu_menu_item_set_visible(KM_EDIT_SKIN, FALSE);
				break;
			case SELECT_JOINTS:
				kudu_menu_item_set_visible(KM_EDIT_BONES, FALSE);
				kudu_menu_item_set_visible(KM_EDIT_SKIN, FALSE);
				break;
			case SELECT_POINTS:
				break;
			case SELECT_EDGES:
				break;
			case SELECT_FACES:
				break;
			case SELECT_SHAPES:
				break;
		}
	}

	kudu_program_skeleton_list_regen();
	if (regen) kudu_program_skin_list_regen();

	return TRUE;
}

int kudu_program_set_mode(int mode)
{
	if (mode == program.mode) return TRUE;
	program.mode = mode;

	switch(mode) {
		case PROGRAM_MODE_EDIT:
			kudu_menu_item_set_label(KM_PROGRAM_SUB_MODE_1, "Skeleton Mode");
			kudu_menu_item_set_label(KM_PROGRAM_SUB_MODE_2, "Skin Mode");
			kudu_menu_item_set_label(KM_PROGRAM_SUB_MODE_3, "Attach Mode");
			kudu_menu_item_set_label(KM_PROGRAM_SUB_MODE_4, "View Mode");

			gtk_window_set_title(GTK_WINDOW(STACK_GET_WIDGET("main_window")), "Kudu Editor Mode");
			break;
		case PROGRAM_MODE_ANIMATION:
			kudu_menu_item_set_label(KM_PROGRAM_SUB_MODE_1, "Sequence Mode");
			kudu_menu_item_set_label(KM_PROGRAM_SUB_MODE_2, "Script Mode");
			kudu_menu_item_set_label(KM_PROGRAM_SUB_MODE_3, ".. Mode");
			kudu_menu_item_set_label(KM_PROGRAM_SUB_MODE_4, "Playback Mode");

			gtk_window_set_title(GTK_WINDOW(STACK_GET_WIDGET("main_window")), "Kudu Animation Mode");
			break;
	}

	program.sub_mode = -1;
	kudu_program_set_sub_mode(PROGRAM_MODE_SKELETON);

	kudu_object_update(program.selected_object);
	kudu_program_skin_list_regen();
	kudu_program_skeleton_list_regen();

	return TRUE;
}

int kudu_program_set_sub_mode(int MODE)
{
	GtkWidget *bone_detail, *skin_detail;
	GtkWidget *item;
	int a, ilist[4];

	if (program.sub_mode == MODE) return TRUE;
	program.sub_mode = MODE;

	if (MODE == PROGRAM_MODE_VIEW) {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	} else {
		glDisable(GL_BLEND);
	}

	/* Are view settings shared across modes ? */
	if (!kudu_options_enabled(KO_SHARE_VIEW_SETTINGS)) {
		if (program.mode == PROGRAM_MODE_EDIT) program.mode_opt_no = 0;
		else	program.mode_opt_no = 4;
		program.mode_opt_no += (program.sub_mode - 3);
	} else {
		program.mode_opt_no = 0;
	}

	switch (kudu_options_get_int_no(KO_BONES_DETAIL, program.mode_opt_no)) {
		case SKELETON_DETAIL_SHOW_ALL:
			kudu_menu_item_activate_no_call(KM_VIEW_BONES_SHOW_NAMES);
			break;
		case SKELETON_DETAIL_SHOW_SELECTED:
			kudu_menu_item_activate_no_call(KM_VIEW_BONES_SHOW_SELECTED_NAMES);
			break;
		case SKELETON_DETAIL_SHOW_NONE:
			kudu_menu_item_activate_no_call(KM_VIEW_BONES_HIDE_NAMES);
			break;
	}

	switch(kudu_options_get_int_no(KO_BONES_AXES, program.mode_opt_no)) {
		case SKELETON_DETAIL_SHOW_ALL:
			kudu_menu_item_activate_no_call(KM_VIEW_BONES_SHOW_AXES);
			break;
		case SKELETON_DETAIL_SHOW_SELECTED:
			kudu_menu_item_activate_no_call(KM_VIEW_BONES_SHOW_SELECTED_AXES);
			break;
		case SKELETON_DETAIL_SHOW_NONE:
			kudu_menu_item_activate_no_call(KM_VIEW_BONES_HIDE_AXES);
			break;
	}

	kudu_menu_check_item_set_state_no_call(KM_VIEW_BONES_SHOW, kudu_options_get_int_no(KO_BONES_VISIBLE, program.mode_opt_no));

	kudu_menu_check_item_set_state_no_call(KM_VIEW_BONES_SHOW_JOINTS, kudu_options_get_int_no(KO_JOINTS_VISIBLE, program.mode_opt_no));

	kudu_menu_check_item_set_state_no_call(KM_VIEW_SKIN_SHOW, kudu_options_get_int_no(KO_SKIN_VISIBLE, program.mode_opt_no));

	kudu_menu_check_item_set_state_no_call(KM_VIEW_SKIN_LIT, kudu_options_get_int_no(KO_SKIN_LIT, program.mode_opt_no));

	kudu_menu_check_item_set_state_no_call(KM_VIEW_SKIN_SMOOTH, kudu_options_get_int_no(KO_SKIN_SMOOTH, program.mode_opt_no));

	kudu_menu_check_item_set_state_no_call(KM_VIEW_SKIN_REAL_COLOURS, kudu_options_get_int_no(KO_SKIN_REAL_COLOURS, program.mode_opt_no));

	kudu_menu_check_item_set_state_no_call(KM_VIEW_SKIN_TEXTURED, kudu_options_get_int_no(KO_SKIN_TEXTURED, program.mode_opt_no));

	a = kudu_options_get_int_no(KO_VIEW_ORTHOGRAPHIC, program.mode_opt_no);
	kudu_menu_check_item_set_state(KM_VIEW_ORTHOGRAPHIC, a);
	kudu_camera_ortho(camera, a);

	switch (kudu_options_get_int_no(KO_SKIN_DETAIL, program.mode_opt_no)) {
		case K_RENDER_POINTS:
			kudu_menu_item_activate_no_call(KM_VIEW_SKIN_VERTEX);
			break;
		case K_RENDER_EDGES:
			kudu_menu_item_activate_no_call(KM_VIEW_SKIN_EDGE);
			break;
		case K_RENDER_FACES:
			kudu_menu_item_activate_no_call(KM_VIEW_SKIN_FACE);
			break;
	}

	switch (kudu_options_get_int_no(KO_SELECTION_MODE, program.mode_opt_no)) {
		case SELECT_OBJECTS:
			kudu_toolbar_toggle_item_set_active_no_call(KT_SELECT_OBJECTS, TRUE);
		break;
		case SELECT_BONES:
			kudu_toolbar_toggle_item_set_active_no_call(KT_SELECT_BONES, TRUE);
			break;
		case SELECT_JOINTS:
			kudu_toolbar_toggle_item_set_active_no_call(KT_SELECT_JOINTS, TRUE);
			break;
		case SELECT_POINTS:
			kudu_toolbar_toggle_item_set_active_no_call(KT_SELECT_VERTICES, TRUE);
			break;
		case SELECT_EDGES:
			kudu_toolbar_toggle_item_set_active_no_call(KT_SELECT_EDGES, TRUE);
			break;
		case SELECT_FACES:
			kudu_toolbar_toggle_item_set_active_no_call(KT_SELECT_FACES, TRUE);
			break;
		case SELECT_SHAPES:
			kudu_toolbar_toggle_item_set_active_no_call(KT_SELECT_SHAPES, TRUE);
			break;
	}

	/* Control various aspects of the menu layout depending on the mode/sub-mode */
	/* Some menu commands should only be available on a single mode/sub-mode combination */
	/* So we'll disable those right here and we'll re-enable them when we need them */
	/* Enabling/disabling is pretty cheap (I think) so there shouldn't be any performance concerns */

	kudu_menu_item_set_visible(KM_EDIT_ATTACHMENTS, FALSE);

	if (program.mode == PROGRAM_MODE_EDIT) {		/* Editor Mode */
		/* General layout for editor mode goes here */

		/* Bone editing commands must be visible in editor mode */
		kudu_menu_item_set_visible(KM_EDIT_BONES_NEW_ROOT, TRUE);
		kudu_menu_item_set_visible(KM_EDIT_BONES_SEP1, TRUE);
		kudu_menu_item_set_visible(KM_EDIT_BONES_ADD_CHILD, TRUE);
		kudu_menu_item_set_visible(KM_EDIT_BONES_INSERT_PARENT, TRUE);
		kudu_menu_item_set_visible(KM_EDIT_BONES_DELETE, TRUE);
		kudu_menu_item_set_visible(KM_EDIT_BONES_SEP2, TRUE);

		/* Set Material edit command to visible */
		kudu_menu_item_set_visible(KM_EDIT_MATERIALS, TRUE);

		/* Bone edit menu only needed for skeleton mode */
		kudu_menu_item_set_visible(KM_EDIT_BONES, FALSE);

		/* Skin edit sub-menu should only be visible in skin sub-mode so we disable it here */
		kudu_menu_item_set_visible(KM_EDIT_SKIN, FALSE);

		/* Edit Object menu needs some adjustment */
		kudu_menu_item_set_visible(KM_EDIT_OBJECT_MOVE_X, FALSE);
		kudu_menu_item_set_visible(KM_EDIT_OBJECT_MOVE_Y, FALSE);
		kudu_menu_item_set_visible(KM_EDIT_OBJECT_MOVE_Z, FALSE);
		kudu_menu_item_set_visible(KM_EDIT_OBJECT_SEP1, FALSE);

		/* Object selection is not used throughout editor mode so we disable it here */
		kudu_toolbar_item_disable(KT_SELECT_OBJECTS);

		/* Playback menu is not used at all in editor mode */
		kudu_menu_item_set_visible(KM_PLAYBACK, FALSE);

		/* Show edit and select menus */
		kudu_menu_item_set_visible(KM_EDIT, TRUE);
		kudu_menu_item_set_visible(KM_SELECT, TRUE);

		/* Hide frames menu */
		kudu_menu_item_set_visible(KM_FRAMES, FALSE);

		switch (program.sub_mode) {	/* sub mode specific layout control here */
			case PROGRAM_MODE_SKELETON:
				kudu_toolbar_item_enable(KT_SELECT_BONES);
				kudu_toolbar_item_enable(KT_SELECT_JOINTS);

				kudu_toolbar_item_disable(KT_SELECT_VERTICES);
				kudu_toolbar_item_disable(KT_SELECT_EDGES);
				kudu_toolbar_item_disable(KT_SELECT_FACES);
				kudu_toolbar_item_disable(KT_SELECT_SHAPES);

				/* Set bone edit menu to visible */
				kudu_menu_item_set_visible(KM_EDIT_BONES, TRUE);
				break;
			case PROGRAM_MODE_SKIN:
				kudu_toolbar_item_disable(KT_SELECT_BONES);
				kudu_toolbar_item_disable(KT_SELECT_JOINTS);

				kudu_toolbar_item_enable(KT_SELECT_VERTICES);
				kudu_toolbar_item_enable(KT_SELECT_EDGES);
				kudu_toolbar_item_enable(KT_SELECT_FACES);
				kudu_toolbar_item_enable(KT_SELECT_SHAPES);

				kudu_menu_item_set_visible(KM_EDIT_SKIN, TRUE);
				break;
			case PROGRAM_MODE_ATTACH:
				kudu_toolbar_item_enable(KT_SELECT_BONES);
				kudu_toolbar_item_disable(KT_SELECT_JOINTS);

				kudu_toolbar_item_enable(KT_SELECT_VERTICES);
				kudu_toolbar_item_enable(KT_SELECT_EDGES);
				kudu_toolbar_item_enable(KT_SELECT_FACES);
				kudu_toolbar_item_enable(KT_SELECT_SHAPES);

				/* Set attachment edit sub menu to visible */
				kudu_menu_item_set_visible(KM_EDIT_ATTACHMENTS, TRUE);
				break;
			case PROGRAM_MODE_VIEW:
				kudu_toolbar_item_disable(KT_SELECT_BONES);
				kudu_toolbar_item_disable(KT_SELECT_JOINTS);

				kudu_toolbar_item_disable(KT_SELECT_VERTICES);
				kudu_toolbar_item_disable(KT_SELECT_EDGES);
				kudu_toolbar_item_disable(KT_SELECT_FACES);
				kudu_toolbar_item_disable(KT_SELECT_SHAPES);

				/* Hide edit and select menus */
				kudu_menu_item_set_visible(KM_EDIT, FALSE);
				kudu_menu_item_set_visible(KM_SELECT, FALSE);
				break;
		}

	} else if (program.mode == PROGRAM_MODE_ANIMATION) {	/* Animation Mode */
		/* General layout for animation mode goes here */

		/* Show frames menu */
		kudu_menu_item_set_visible(KM_FRAMES, TRUE);

		/* Bone editing commands must be invisible in animation mode */
		kudu_menu_item_set_visible(KM_EDIT_BONES_NEW_ROOT, FALSE);
		kudu_menu_item_set_visible(KM_EDIT_BONES_SEP1, FALSE);
		kudu_menu_item_set_visible(KM_EDIT_BONES_ADD_CHILD, FALSE);
		kudu_menu_item_set_visible(KM_EDIT_BONES_INSERT_PARENT, FALSE);
		kudu_menu_item_set_visible(KM_EDIT_BONES_DELETE, FALSE);
		kudu_menu_item_set_visible(KM_EDIT_BONES_SEP2, FALSE);

		/* Set Material edit command to invisible */
		kudu_menu_item_set_visible(KM_EDIT_MATERIALS, FALSE);

		/* Selection modes are available in all sub-modes except playback */
		/* enable them now and we'll disable them again in playback sub-mode */
		kudu_toolbar_item_enable(KT_SELECT_OBJECTS);
		kudu_toolbar_item_enable(KT_SELECT_JOINTS);
		kudu_toolbar_item_enable(KT_SELECT_BONES);
		kudu_toolbar_item_enable(KT_SELECT_VERTICES);
		kudu_toolbar_item_enable(KT_SELECT_EDGES);
		kudu_toolbar_item_enable(KT_SELECT_FACES);
		kudu_toolbar_item_enable(KT_SELECT_SHAPES);

		/* Playback menu is only used in playback sub mode */
		kudu_menu_item_set_visible(KM_PLAYBACK, FALSE);

		/* Enable edit and select menus */
		kudu_menu_item_set_visible(KM_EDIT, TRUE);
		kudu_menu_item_set_visible(KM_SELECT, TRUE);

		/* Edit Object menu needs some adjustment */
		kudu_menu_item_set_visible(KM_EDIT_OBJECT_MOVE_X, TRUE);
		kudu_menu_item_set_visible(KM_EDIT_OBJECT_MOVE_Y, TRUE);
		kudu_menu_item_set_visible(KM_EDIT_OBJECT_MOVE_Z, TRUE);
		kudu_menu_item_set_visible(KM_EDIT_OBJECT_SEP1, TRUE);

		switch (program.sub_mode) {	/* sub mode specific layout control here */
			case PROGRAM_MODE_SEQUENCE:
				break;
			case PROGRAM_MODE_SCRIPT:
				break;
			case PROGRAM_MODE_ATTACH:
				break;
			case PROGRAM_MODE_PLAYBACK:
				kudu_toolbar_item_disable(KT_SELECT_OBJECTS);
				kudu_toolbar_item_disable(KT_SELECT_BONES);
				kudu_toolbar_item_disable(KT_SELECT_JOINTS);
				kudu_toolbar_item_disable(KT_SELECT_VERTICES);
				kudu_toolbar_item_disable(KT_SELECT_EDGES);
				kudu_toolbar_item_disable(KT_SELECT_FACES);
				kudu_toolbar_item_disable(KT_SELECT_SHAPES);

				kudu_menu_item_set_visible(KM_PLAYBACK, TRUE);

				/* Hide edit and select menus */
				kudu_menu_item_set_visible(KM_EDIT, FALSE);
				kudu_menu_item_set_visible(KM_SELECT, FALSE);

				/* Hide frames menu */
				kudu_menu_item_set_visible(KM_FRAMES, FALSE);

				kudu_menu_check_item_set_state(KM_PLAYBACK_LOOP, kudu_options_enabled(KO_PLAYBACK_LOOP));
				break;
		}
	}

	kudu_program_selection_mode_set(kudu_options_get_int_no(KO_SELECTION_MODE, program.mode_opt_no), FALSE);

	kudu_program_skin_list_regen();
	kudu_program_skeleton_list_regen();
}

int kudu_program_bone_mode_set(int MODE)
{
	if (camera->mode != CAMERA_MODE_FIXED) return FALSE;

	if (selected_bones_list->type != SELECT_BONES) return FALSE;

	if ((!kudu_selection_list_anything_selected(selected_bones_list)) && (MODE != BONE_MODE_FIXED)) {
		kudu_gui_message("Please select at least one bone!", GTK_MESSAGE_INFO);
		return FALSE;
	}

	if ((MODE == BONE_MODE_MOVEX) || (MODE == BONE_MODE_MOVEY) || (MODE == BONE_MODE_MOVEZ)) {
		if (!kudu_selection_list_bones_are_parents(selected_bones_list)) {
			kudu_gui_message("Only root bones may be directly moved!", GTK_MESSAGE_ERROR);
			return FALSE;
		}
	}

	if (MODE != BONE_MODE_FIXED) {
		kudu_gui_save_pointer();
		kudu_gui_hide_cursor();
	} else {
		kudu_gui_restore_pointer();
		kudu_gui_show_cursor();
	}

	program.bone_mode = MODE;

	kudu_bones_edit_anchor(selected_bones_list, MODE);

	program.offset[0] = 0.0;

	return TRUE;
}

int kudu_program_joint_mode_set(int mode)
{
	if (camera->mode != CAMERA_MODE_FIXED) return FALSE;

	if (selected_bones_list->type != SELECT_JOINTS) return FALSE;

	if ((!kudu_selection_list_anything_selected(selected_bones_list)) && (mode != JOINT_MODE_FIXED)) {
		kudu_gui_message("Please select at least one joint!", GTK_MESSAGE_INFO);
		return FALSE;
	}

	if (mode != JOINT_MODE_FIXED) {
		kudu_gui_save_pointer();
		kudu_gui_hide_cursor();
	} else {
		kudu_gui_restore_pointer();
		kudu_gui_show_cursor();
	}

	program.joint_mode = mode;

	kudu_joints_edit_anchor(selected_bones_list, mode);

	return TRUE;
}

int kudu_program_skeleton_calculate(void)
{
	return TRUE;
}

void kudu_program_set_skeleton_detail(int detail)
{
	kudu_options_set_int_no(KO_BONES_DETAIL, program.mode_opt_no, detail);

	kudu_program_skeleton_list_regen();
}

void kudu_program_set_skeleton_axes(int detail)
{
	kudu_options_set_int_no(KO_BONES_AXES, program.mode_opt_no, detail);

	kudu_program_skeleton_list_regen();
}

void kudu_program_set_skin_detail(int detail)
{
	kudu_options_set_int_no(KO_SKIN_DETAIL, program.mode_opt_no, detail);

	kudu_program_skin_list_regen();
}

int kudu_program_goto_frame(int frame)
{
	if (frame_list == NULL) return FALSE;
	if (object == NULL) return FALSE;

	kudu_frame_list_goto_frame(frame_list, frame);

	/*kudu_program_ani_regen();*/
	/*kudu_program_back_skin_list_regen();*/
	kudu_program_objects_update();

	if (program.selected_object != NULL) kudu_bone_magic_update_all(program.selected_object->bone);
	kudu_program_update_skin();
	kudu_program_skeleton_list_regen();

	return TRUE;
}

static int kudu_program_run_animation(int frame)
{
	if (frame_list == NULL) return FALSE;

	static int run = FALSE, delay = 38, odelay = 38, old_frame;
	static struct timeb sTime, eTime;
	int dTime, ddTime, loop;
	double tmp, tmp2;

	if (frame == -2) {
		run = FALSE;
	} else if (frame == -1) {
		run = TRUE;
		old_frame = frame_list->current_frame;
	}

	if (program.sub_mode != PROGRAM_MODE_PLAYBACK) run = FALSE;

	if (!run) {
		kudu_program_goto_frame(old_frame);
		return FALSE;
	}

	if (frame > frame_list->max_frame) frame = 0;
	else if (frame < 0) frame = 0;

	if (frame != 0) {
		tmp = (double)frame / 24;
		tmp2 = floor(frame / 24);

		if (tmp == tmp2) {
			ftime(&eTime);
			ddTime = (eTime.time - sTime.time);
			dTime = (eTime.millitm - sTime.millitm);
			if (ddTime > 1) dTime += 1000;
			if (ddTime < 1) dTime -= 1000;
			if (dTime > 1000) dTime = 1000;
			if (dTime < -1000) dTime = -1000;

			if (dTime > 0) delay -= (dTime / 100);
			if (dTime < 0) delay += -(dTime / 100);
			if (delay < 10) delay = 10;
			if (odelay != delay) odelay = delay;
			ftime(&sTime);
		}
	} else ftime(&sTime);

	kudu_program_goto_frame(frame);

	if (frame == frame_list->max_frame) loop = kudu_options_enabled(KO_PLAYBACK_LOOP);
	if ((frame < frame_list->max_frame) || (loop)) {
		g_timeout_add(delay, (GtkFunction)kudu_program_run_animation, (gpointer)frame+1);
	}	else kudu_program_goto_frame(0);

	return FALSE;
}

static void main_menu_action(GtkWidget *menu_item, gpointer callback_action)
{
	int a, tmp, key = GPOINTER_TO_INT(callback_action);
	float f[3];
	KuduBone *temp_bone, *current_bone;

	if GTK_IS_RADIO_MENU_ITEM(menu_item) {
		if (!gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(menu_item))) return;
	}

	switch (key) {
		/* File menu */
		case KM_FILE_NEW:
			kudu_program_new();
			kudu_program_skin_list_regen();
			kudu_program_skeleton_list_regen();
			break;
		case KM_FILE_OPEN:
			kudu_program_open_kudu_object();
			break;
		case KM_FILE_SAVE:
			kudu_program_save_kudu_object();
			break;
		case KM_FILE_SAVE_AS:
			kudu_program_request_save_kudu_object();
			break;
		case KM_FILE_CLOSE:
			kudu_program_close();
			break;
		case KM_FILE_EXIT:
			kudu_program_exit(menu_item, NULL);
			break;
		case KM_FILE_IMPORT_RWX:
			kudu_program_import_rwx();
			break;
		case KM_FILE_IMPORT_OBJ:
			kudu_program_import_obj();
			break;


		/* Edit menu */
		case KM_EDIT_BONES_ADD_CHILD:
			kudu_bones_edit_function(selected_bones_list, ADD_CHILD, SL_SELECT_NEW, program.selected_object);
			kudu_program_skeleton_list_regen();
			break;
		case KM_EDIT_BONES_INSERT_PARENT:
			kudu_bones_edit_function(selected_bones_list, ADD_PARENT, SL_SELECT_NEW, program.selected_object);
			kudu_program_skeleton_list_regen();
			break;
		case KM_EDIT_BONES_DELETE:
			kudu_bones_edit_function(selected_bones_list, DELETE, SL_SELECT_NEW | SL_KEEP_CURRENT, program.selected_object);
			if (program.selected_object != NULL) kudu_bone_update_all(program.selected_object->bone);
			kudu_program_skeleton_list_regen();
			break;
		case KM_EDIT_BONES_NEW_ROOT:
			if (program.selected_object == NULL) {
				kudu_gui_message("You have not opened a file or started a new one!", GTK_MESSAGE_ERROR);
				break;
			}
			if (program.selected_object->bone == NULL) {
				program.selected_object->bone = kudu_bone_new(program.selected_object);
				kudu_bone_update(program.selected_object->bone);
			} else {
				kudu_bone_update(kudu_bone_add_sibling(program.selected_object->bone));
			}
			kudu_program_skeleton_list_regen();
			break;
		/*case KM_EDIT_BONES_HANGLE:
			kudu_program_bone_mode_set(BONE_MODE_H_ANGLE);
			break;
		case KM_EDIT_BONES_VANGLE:
			kudu_program_bone_mode_set(BONE_MODE_V_ANGLE);
			break;*/
		case KM_EDIT_BONES_ROTATE:
			kudu_program_bone_mode_set(BONE_MODE_ROTATE);
			break;
		case KM_EDIT_BONES_STRETCH:
			kudu_program_bone_mode_set(BONE_MODE_STRETCH);
			break;
		/* Bone rotations about the local axes */
		case KM_EDIT_BONES_ROT_LX:
			kudu_program_bone_mode_set(BONE_MODE_ROT_LX);
			break;
		case KM_EDIT_BONES_ROT_LY:
			kudu_program_bone_mode_set(BONE_MODE_ROT_LY);
			break;
		case KM_EDIT_BONES_ROT_LZ:
			kudu_program_bone_mode_set(BONE_MODE_ROT_LZ);
			break;
		/* Bone rotations about the global axes */
		case KM_EDIT_BONES_ROT_GX:
			kudu_program_bone_mode_set(BONE_MODE_ROT_GX);
			break;
		case KM_EDIT_BONES_ROT_GY:
			kudu_program_bone_mode_set(BONE_MODE_ROT_GY);
			break;
		case KM_EDIT_BONES_ROT_GZ:
			kudu_program_bone_mode_set(BONE_MODE_ROT_GZ);
			break;
		/* Bone shift's along global axis */
		case KM_EDIT_BONES_MOVE_X:
			kudu_program_bone_mode_set(BONE_MODE_MOVEX);
			break;
		case KM_EDIT_BONES_MOVE_Y:
			kudu_program_bone_mode_set(BONE_MODE_MOVEY);
			break;
		case KM_EDIT_BONES_MOVE_Z:
			kudu_program_bone_mode_set(BONE_MODE_MOVEZ);
			break;
		case KM_EDIT_BONES_PROPERTIES:
			if (!kudu_selection_list_for_each_do(selected_bones_list)) {
				kudu_gui_message("No bones selected!", GTK_MESSAGE_ERROR);
				break;
			}
			while ((current_bone = (KuduBone*)kudu_selection_list_next_do()) != NULL) {
				kudu_gui_bone_properties_edit(current_bone);
			}
			break;

		case KM_EDIT_JOINTS_MOVE_X:
			kudu_program_joint_mode_set(JOINT_MODE_MOVEX);
			break;
		case KM_EDIT_JOINTS_MOVE_Y:
			kudu_program_joint_mode_set(JOINT_MODE_MOVEY);
			break;
		case KM_EDIT_JOINTS_MOVE_Z:
			kudu_program_joint_mode_set(JOINT_MODE_MOVEZ);
			break;

		case KM_EDIT_ATTACHMENTS_ATTACH:
			kudu_gui_vertex_properties_edit(program.selected_object, selection_list, selected_bones_list);
			break;
		case KM_EDIT_ATTACHMENTS_CLEAR:
			kudu_gui_vertex_properties_reset(selection_list);
			break;

		case KM_EDIT_OBJECT_PROPERTIES:
			if (program.selected_object == NULL) break;
			kudu_gui_object_edit(program.selected_object);
			break;
		case KM_EDIT_OBJECT_MOVE_X:
			kudu_program_edit_mode_set(EDIT_MODE_SHIFT_X);
			break;
		case KM_EDIT_OBJECT_MOVE_Y:
			kudu_program_edit_mode_set(EDIT_MODE_SHIFT_Y);
			break;
		case KM_EDIT_OBJECT_MOVE_Z:
			kudu_program_edit_mode_set(EDIT_MODE_SHIFT_Z);
			break;

		case KM_EDIT_SKIN_MOVE_X:
			kudu_program_edit_mode_set(EDIT_MODE_SHIFT_X);
			break;
		case KM_EDIT_SKIN_MOVE_Y:
			kudu_program_edit_mode_set(EDIT_MODE_SHIFT_Y);
			break;
		case KM_EDIT_SKIN_MOVE_Z:
			kudu_program_edit_mode_set(EDIT_MODE_SHIFT_Z);
			break;
		case KM_EDIT_SKIN_MATERIAL:
			kudu_gui_materials_pick_for_faces(program.selected_object, selection_list);
			break;

		case KM_EDIT_MATERIALS:
			kudu_gui_materials_edit(program.selected_object, NULL);
			break;

		case KM_EDIT_SCRIPTS:
			#ifdef SUPPORT_PYTHON
			kudu_gui_script_selector();
			#else
			kudu_gui_message("This Kudu binary was compiled without Python support!\n"
					 "These features will not be available.", GTK_MESSAGE_INFO);
			#endif
			break;

		case KM_EDIT_PREFERENCES:
			kudu_gui_options_dialog_show();
			break;


		/* Frames menu */
		case KM_FRAMES_KEY_ALL_WORLD:
			/* Still needs implementation */
			break;
		case KM_FRAMES_KEY_ALL_OBJECT:
			/* Still needs implementation */
			break;
		case KM_FRAMES_KEY_BONES_WORLD:
			/* Still needs implementation */
			break;
		case KM_FRAMES_KEY_BONES_OBJECT:
			kudu_sequence_set_frame(frame_list, program.selected_object, NULL, selected_bones_list, SET_BONES_OBJECT);
			break;
		case KM_FRAMES_KEY_BONES_SELECTED:
			kudu_sequence_set_frame(frame_list, program.selected_object, NULL, selected_bones_list, SET_BONES_SELECTED);
			break;
		case KM_FRAMES_KEY_VERTICES_WORLD:
			/* Still needs implementation */
			break;
		case KM_FRAMES_KEY_VERTICES_OBJECT:
			/* Still needs implementation */
			break;
		case KM_FRAMES_KEY_VERTICES_SELECTED:
			/* Still needs implementation */
			break;
		case KM_FRAMES_KEY_POSITIONS_WORLD:
			kudu_sequence_set_frame(frame_list, object, selection_list, selected_bones_list, SET_POSITION_SCENE);
			break;
		case KM_FRAMES_KEY_POSITIONS_OBJECT:
			kudu_sequence_set_frame(frame_list, program.selected_object, NULL, NULL, SET_POSITION_OBJECT);
			break;
		case KM_FRAMES_INCR_1F:
			if (frame_list == NULL) break;
			kudu_program_goto_frame(frame_list->current_frame+1);
			break;
		case KM_FRAMES_INCR_1S:
			if (frame_list == NULL) break;
			kudu_program_goto_frame(frame_list->current_frame+24);
			break;
		case KM_FRAMES_INCR_1M:
			if (frame_list == NULL) break;
			kudu_program_goto_frame(frame_list->current_frame+1440);
			break;
		case KM_FRAMES_INCR_NOND_1F:
			if (frame_list == NULL) break;
			frame_list->current_frame += 1;
			break;
		case KM_FRAMES_INCR_NOND_1S:
			if (frame_list == NULL) break;
			frame_list->current_frame += 24;
			break;
		case KM_FRAMES_INCR_NOND_1M:
			if (frame_list == NULL) break;
			frame_list->current_frame += 1440;
			break;
		case KM_FRAMES_DECR_1F:
			if (frame_list == NULL) break;
			kudu_program_goto_frame(frame_list->current_frame-1);
			break;
		case KM_FRAMES_DECR_1S:
			if (frame_list == NULL) break;
			kudu_program_goto_frame(frame_list->current_frame-24);
			break;
		case KM_FRAMES_DECR_1M:
			if (frame_list == NULL) break;
			kudu_program_goto_frame(frame_list->current_frame-1440);
			break;
		case KM_FRAMES_DECR_NOND_1F:
			if (frame_list == NULL) break;
			frame_list->current_frame -= 1;
			if (frame_list->current_frame < 0) frame_list->current_frame = 0;
			break;
		case KM_FRAMES_DECR_NOND_1S:
			if (frame_list == NULL) break;
			frame_list->current_frame -= 24;
			if (frame_list->current_frame < 0) frame_list->current_frame = 0;
			break;
		case KM_FRAMES_DECR_NOND_1M:
			if (frame_list == NULL) break;
			frame_list->current_frame -= 1440;
			if (frame_list->current_frame < 0) frame_list->current_frame = 0;
			break;

		/* Select menu */
		case KM_SELECT_BONES_NEXT:
			kudu_selection_list_bones_select_more(selected_bones_list, SELECT_BONES_NEXT_SIBLING, FALSE);
			kudu_program_skeleton_list_regen();
			break;
		case KM_SELECT_BONES_PREVIOUS:
			kudu_selection_list_bones_select_more(selected_bones_list, SELECT_BONES_PREVIOUS_SIBLING, FALSE);
			kudu_program_skeleton_list_regen();
			break;
		case KM_SELECT_BONES_FIRST_CHILD:
			kudu_selection_list_bones_select_more(selected_bones_list, SELECT_BONES_FIRST_CHILD, FALSE);
			kudu_program_skeleton_list_regen();
			break;
		case KM_SELECT_BONES_PARENT:
			kudu_selection_list_bones_select_more(selected_bones_list, SELECT_BONES_PARENTS, FALSE);
			kudu_program_skeleton_list_regen();
			break;
		case KM_SELECT_BONES_ALL_CHILDREN:
			kudu_selection_list_bones_select_more(selected_bones_list, SELECT_BONES_ALL_CHILDREN, FALSE);
			kudu_program_skeleton_list_regen();
			break;
		case KM_SELECT_ALL:
			if (program.selected_object == NULL) break;
			tmp = kudu_options_get_int_no(KO_SELECTION_MODE, program.mode_opt_no);
			if (tmp == SELECT_BONES) {
				kudu_selection_list_select_bones(selected_bones_list, program.selected_object->bone, SELECT_ALL);
				kudu_program_skeleton_list_regen();
			} else if (tmp == SELECT_JOINTS) {
				kudu_selection_list_select_joints(selected_bones_list, program.selected_object->joint, SELECT_ALL);
				kudu_program_skeleton_list_regen();
			} else {
				kudu_selection_list_select(selection_list, program.selected_object, tmp, SELECT_ALL);
				kudu_program_skin_list_regen();
			}
			break;
		case KM_SELECT_NONE:
			if (program.selected_object == NULL) break;
			tmp = kudu_options_get_int_no(KO_SELECTION_MODE, program.mode_opt_no);
			if (tmp == SELECT_BONES) {
				kudu_selection_list_select_bones(selected_bones_list, program.selected_object->bone, SELECT_NOTHING);
				kudu_program_skeleton_list_regen();
			} else if (tmp == SELECT_JOINTS) {
				kudu_selection_list_select_joints(selected_bones_list, program.selected_object->joint, SELECT_NOTHING);
				kudu_program_skeleton_list_regen();
			} else {
				kudu_selection_list_select(selection_list, program.selected_object, tmp, SELECT_NOTHING);
				kudu_program_skin_list_regen();
			}
			break;
		case KM_SELECT_INVERSE:
			if (program.selected_object == NULL) break;
			tmp = kudu_options_get_int_no(KO_SELECTION_MODE, program.mode_opt_no);
			if (tmp == SELECT_BONES) {
				kudu_selection_list_select_bones(selected_bones_list, program.selected_object->bone, SELECT_INVERSE);
				kudu_program_skeleton_list_regen();
			} else if (tmp == SELECT_JOINTS) {
				kudu_selection_list_select_joints(selected_bones_list, program.selected_object->joint, SELECT_INVERSE);
				kudu_program_skeleton_list_regen();
			} else {
				kudu_selection_list_select(selection_list, program.selected_object, tmp, SELECT_INVERSE);
				kudu_program_skin_list_regen();
			}
			break;

		/* View menu */
		case KM_VIEW_LOOK_XM:
			camera->ha = 270;
			camera->va = 180;
			goto camera_swing;
		case KM_VIEW_LOOK_XP:
			camera->ha = 90;
			camera->va = 180;
			goto camera_swing;
		case KM_VIEW_LOOK_ZM:
			camera->ha = 0;
			camera->va = 180;
			goto camera_swing;
		case KM_VIEW_LOOK_ZP:
			camera->ha = 180;
			camera->va = 180;
			goto camera_swing;
		case KM_VIEW_LOOK_YM:
			camera->ha = 0;
			camera->va = 90;
			goto camera_swing;
		case KM_VIEW_LOOK_YP:
			camera->ha = 0;
			camera->va = 270;
			goto camera_swing;
		case KM_VIEW_FOCUS_ORIGIN:
			kudu_camera_lookat(camera, 0, 0, 0);
			goto camera_swing;
		case KM_VIEW_FOCUS_SELECTION:	/* Focus camera on selection */
			switch (kudu_options_get_int_no(KO_SELECTION_MODE, program.mode_opt_no)) {
				case SELECT_OBJECTS:
					break;
				case SELECT_BONES:
				case SELECT_JOINTS:
					if (!kudu_selection_list_anything_selected(selected_bones_list)) break;
					kudu_selection_list_get_center_point(selected_bones_list, f);
					break;
				default:
					if (!kudu_selection_list_anything_selected(selection_list)) break;
					kudu_selection_list_get_center_point(selection_list, f);
					break;
			}

			if ((program.mode == PROGRAM_MODE_ANIMATION) && (program.selected_object != NULL)) {
				for (a = 0; a < 3; a++) f[a] += program.selected_object->position[a];
			}

			kudu_camera_lookat(camera, f[0], f[1], f[2]);

		/* Swing camera as set and redraw from new viewpoint */
		camera_swing:
			kudu_camera_swing(camera, 0.0, 0.0, 0);
			kudu_program_view_point_set();
			break;
		case KM_VIEW_ORTHOGRAPHIC:
			a = gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(menu_item));
			kudu_options_set_int_no(KO_VIEW_ORTHOGRAPHIC, program.mode_opt_no, a);
			kudu_camera_ortho(camera, a);
			break;

		/* Playback menu */
		case KM_PLAYBACK_PLAY:
			kudu_program_run_animation(-1);
			break;
		case KM_PLAYBACK_PAUSE:
			break;
		case KM_PLAYBACK_STOP:
			kudu_program_run_animation(-2);
			break;
		case KM_PLAYBACK_USE_CAMERA:
			break;
		case KM_PLAYBACK_LOOP:
			if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(menu_item)))
				kudu_options_enable(KO_PLAYBACK_LOOP);
			else	kudu_options_disable(KO_PLAYBACK_LOOP);
			break;


		/* Program menu */
		case KM_PROGRAM_MODE_EDIT:
			kudu_program_set_mode(PROGRAM_MODE_EDIT);
			break;
		case KM_PROGRAM_MODE_ANIMATION:
			kudu_program_set_mode(PROGRAM_MODE_ANIMATION);
			break;
		case KM_PROGRAM_SUB_MODE_1:
			kudu_program_set_sub_mode(PROGRAM_MODE_SKELETON);
			break;
		case KM_PROGRAM_SUB_MODE_2:
			kudu_program_set_sub_mode(PROGRAM_MODE_SKIN);
			break;
		case KM_PROGRAM_SUB_MODE_3:
			kudu_program_set_sub_mode(PROGRAM_MODE_ATTACH);
			break;
		case KM_PROGRAM_SUB_MODE_4:
			kudu_program_set_sub_mode(PROGRAM_MODE_VIEW);
			break;
		case KM_VIEW_BONES_SHOW:
			a = gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(menu_item));
			kudu_options_set_int_no(KO_BONES_VISIBLE, program.mode_opt_no, a);
			kudu_program_skeleton_list_regen();
			break;
		case KM_VIEW_BONES_SHOW_JOINTS:
			a = gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(menu_item));
			kudu_options_set_int_no(KO_JOINTS_VISIBLE, program.mode_opt_no, a);
			kudu_program_skeleton_list_regen();
			break;
		case KM_VIEW_BONES_SHOW_NAMES:
			kudu_program_set_skeleton_detail(SKELETON_DETAIL_SHOW_ALL);
			break;
		case KM_VIEW_BONES_SHOW_SELECTED_NAMES:
			kudu_program_set_skeleton_detail(SKELETON_DETAIL_SHOW_SELECTED);
			break;
		case KM_VIEW_BONES_HIDE_NAMES:
			kudu_program_set_skeleton_detail(SKELETON_DETAIL_SHOW_NONE);
			break;
		case KM_VIEW_BONES_SHOW_AXES:
			kudu_program_set_skeleton_axes(SKELETON_DETAIL_SHOW_ALL);
			break;
		case KM_VIEW_BONES_SHOW_SELECTED_AXES:
			kudu_program_set_skeleton_axes(SKELETON_DETAIL_SHOW_SELECTED);
			break;
		case KM_VIEW_BONES_HIDE_AXES:
			kudu_program_set_skeleton_axes(SKELETON_DETAIL_SHOW_NONE);
			break;
		case KM_VIEW_SKIN_SHOW:
			a = gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(menu_item));
			kudu_options_set_int_no(KO_SKIN_VISIBLE, program.mode_opt_no, a);
			kudu_program_skin_list_regen();
			break;
		case KM_VIEW_SKIN_SMOOTH:
			a = gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(menu_item));
			kudu_options_set_int_no(KO_SKIN_SMOOTH, program.mode_opt_no, a);
			kudu_program_skin_list_regen();
			break;
		case KM_VIEW_SKIN_LIT:
			a = gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(menu_item));
			kudu_options_set_int_no(KO_SKIN_LIT, program.mode_opt_no, a);
			kudu_program_skin_list_regen();
			break;
		case KM_VIEW_SKIN_REAL_COLOURS:
			a = gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(menu_item));
			kudu_options_set_int_no(KO_SKIN_REAL_COLOURS, program.mode_opt_no, a);
			kudu_program_skin_list_regen();
			break;
		case KM_VIEW_SKIN_TEXTURED:
			a = gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(menu_item));
			kudu_options_set_int_no(KO_SKIN_TEXTURED, program.mode_opt_no, a);
			kudu_program_skin_list_regen();
			break;
		case KM_VIEW_SKIN_VERTEX:
			kudu_program_set_skin_detail(K_RENDER_POINTS);
			break;
		case KM_VIEW_SKIN_EDGE:
			kudu_program_set_skin_detail(K_RENDER_EDGES);
			break;
		case KM_VIEW_SKIN_FACE:
			kudu_program_set_skin_detail(K_RENDER_FACES);
			break;

		/* Help Menu */
		case KM_HELP_ABOUT:
			kudu_about_dialog_run();
			break;

		default:
			g_print("*** Error *** Menu option %d not recognized!\n", callback_action);

	}

}

static void main_toolbar_action(GtkWidget *item, unsigned int action)
{
	if (GTK_IS_TOGGLE_TOOL_BUTTON(item)) 
		if (!gtk_toggle_tool_button_get_active(GTK_TOGGLE_TOOL_BUTTON(item))) return;

	switch (action) {
		case KT_FILE_OPEN:
			main_menu_action(NULL, GINT_TO_POINTER(KM_FILE_OPEN));
			break;
		case KT_FILE_SAVE:
			main_menu_action(NULL, GINT_TO_POINTER(KM_FILE_SAVE));
			break;
		case KT_SELECT_OBJECTS:
			kudu_program_selection_mode_set(SELECT_OBJECTS, TRUE);
			break;
		case KT_SELECT_BONES:
			kudu_program_selection_mode_set(SELECT_BONES, TRUE);
			break;
		case KT_SELECT_JOINTS:
			kudu_program_selection_mode_set(SELECT_JOINTS, TRUE);
			break;
		case KT_SELECT_VERTICES:
			kudu_program_selection_mode_set(SELECT_POINTS, TRUE);
			break;
		case KT_SELECT_EDGES:
			kudu_program_selection_mode_set(SELECT_EDGES, TRUE);
			break;
		case KT_SELECT_FACES:
			kudu_program_selection_mode_set(SELECT_FACES, TRUE);
			break;
		case KT_SELECT_SHAPES:
			kudu_program_selection_mode_set(SELECT_SHAPES, TRUE);
			break;
	}
}


void kudu_grid_list_regen(void)
{
	glNewList(program.grid_list, GL_COMPILE);
	glDisable(GL_LIGHTING);
	kudu_draw_grid(20, 20, 1.0, FALSE);
	kudu_draw_axes();
	kudu_draw_axes_labels(camera);
	glEnable(GL_LIGHTING);
	glEndList();
}

/* Construct selection lists and setup some options _not_ GL related stuff */
void kudu_program_init_objects(void)
{
	selection_list = kudu_selection_list_new();
	selected_bones_list = kudu_selection_list_new();
	kudu_selection_list_set_type(selected_bones_list, SELECT_BONES, TRUE);

	/* Initialize the selection hits buffer */
	program.hit.buf_size = kudu_options_get_int_no(KO_SELECTION_BUFFER, 0);
	if (program.hit.buf_size == 0) program.hit.buf_size = (1024*1024); /* 1 MB - default buffer size */
	program.hit.buffer = (unsigned int*)malloc(program.hit.buf_size);
	program.hit.hits = 0;


	/* Frame list - for storing frame sequence's in animation mode */
	frame_list = kudu_frame_list_new();

	program.selected_object = NULL;
	kudu_program_edit_mode_set(EDIT_MODE_NONE);

	/* Create the viewing camera */
	camera = kudu_camera_new();

	/* Init the hit list */
	kudu_hits_list_init();

	/* Object selection callback */
	kudu_selection_set_object_selector(kudu_program_select_object);
}

/* Initialize and setup mainly GL realted stuff */
static void kudu_program_init(GtkWidget *widget, gpointer data)
{
	GdkGLContext *glContext = gtk_widget_get_gl_context(widget);
	GdkGLDrawable *glDrawable = gtk_widget_get_gl_drawable(widget);

	if (!gdk_gl_drawable_gl_begin(glDrawable, glContext)) return;

	glClearColor(0.8, 0.8, 0.8, 1.0);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	/*glEnable(GL_ALPHA_TEST);
	glEnable(GL_NORMALIZE);
	glEnable(GL_AUTO_NORMAL);*/
	glEnable(GL_CULL_FACE);
	/*glEnable(GL_TEXTURE);
	glEnable(GL_TEXTURE_2D);*/

	float white_light[] = {1.0, 1.0, 1.0, 1.0};
	float light_pos[] = {5.0, 5.0, 5.0, 0.0};
	float light_ambient[] = {0.1, 0.1, 0.1, 1.0};

/*	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);*/
/*	glLightfv(GL_LIGHT0, GL_DIFFUSE, white_light);
	glLightfv(GL_LIGHT0, GL_SPECULAR, white_light);*/
/*	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, light_ambient);*/

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glPointSize(2.5);

	/* Init the bone "graphics" */
	kudu_graphics_init_bone_lists();

	/* Generate some needed gl list */
	program.displayList = kudu_graphics_gen_gl_lists(1);
	program.front_display_list = kudu_graphics_gen_gl_lists(1);
	program.skeleton_list = kudu_graphics_gen_gl_lists(1);
	program.skin_list = kudu_graphics_gen_gl_lists(1);
	program.object_list = kudu_graphics_gen_gl_lists(1);

	program.grid_list = kudu_graphics_gen_gl_lists(1);

	kudu_font_builtin_init();

	kudu_program_view_point_set();

	kudu_program_init_objects();

	kudu_program_set_mode(PROGRAM_MODE_EDIT);
	kudu_program_set_sub_mode(PROGRAM_MODE_SKELETON);

	kudu_camera_set_gl_matrix(camera, -1, -1);
	kudu_camera_set_gl_viewpoint(camera);

	#ifdef SUPPORT_PYTHON
	kudu_script_init_objects(&object, &frame_list, &selection_list, &selected_bones_list);
	#endif

	gdk_gl_drawable_gl_end(glDrawable);
}

int kudu_program_object_list_regen(void)
{
	/* If we are in Editor mode or have no objects to draw, then just clear this list */
	if ((object == NULL) || (program.mode == PROGRAM_MODE_EDIT)) {
		glNewList(program.object_list, GL_COMPILE);
		glEndList();
		return TRUE;
	}

	int render_options = 0;
	int skin_lit = kudu_options_get_int_no(KO_SKIN_LIT, program.mode_opt_no);
	int skin_detail = kudu_options_get_int_no(KO_SKIN_DETAIL, program.mode_opt_no);
	int skin_smooth = kudu_options_get_int_no(KO_SKIN_SMOOTH, program.mode_opt_no);
	int skin_colours = kudu_options_get_int_no(KO_SKIN_REAL_COLOURS, program.mode_opt_no);

	glNewList(program.object_list, GL_COMPILE);
	if (!skin_lit) glDisable(GL_LIGHTING);

	render_options = skin_detail;

	if (!skin_smooth) render_options += K_RENDER_FLAT_SHADING;
	if (skin_lit) render_options += K_RENDER_LIGHTING;
	if (!skin_colours) render_options += K_RENDER_NO_COLOURS;
	render_options += K_RENDER_OMIT_SELECTED_OBJECT + K_RENDER_TRANSLATE_POS;

	/*glPushMatrix();
	glTranslatef(program.selected_object->position[0], program.selected_object->position[1], program.selected_object->position[2]);*/
	kudu_draw_objects(object, render_options);
	/*glPopMatrix();*/

	glEndList();

	return TRUE;
}

int kudu_program_objects_update(void)
{
	if (object == NULL) return FALSE;

	int lit, smooth;


	KuduObject *obj;
	KuduShape *shape;

	if (!kudu_object_for_each_do(object)) return FALSE;

	while ((obj = kudu_object_next_do()) != NULL) {
		if (obj == program.selected_object) continue;
		if (obj->bone != NULL) kudu_bone_update_all(obj->bone);

		if (kudu_options_get_int_no(KO_SKIN_VISIBLE, program.mode_opt_no)) {
			lit = kudu_options_get_int_no(KO_SKIN_LIT, program.mode_opt_no);
			smooth = kudu_options_get_int_no(KO_SKIN_SMOOTH, program.mode_opt_no);

			if ((lit) && (smooth)) lit = TRUE;
			else	lit = FALSE;

			kudu_object_update_vertices(obj, lit);
		}
	}

	return TRUE;
}

/* Regenerate the skin list for the current object */
int kudu_program_skin_list_regen(void)
{
	int render_options = 0;
	KuduShape *shape;

	kudu_program_object_list_regen();

	/* If we don't have a valid selected object or skin is set to invisible, then just clear this list */
	if ((program.selected_object == NULL) || (!kudu_options_get_int_no(KO_SKIN_VISIBLE, program.mode_opt_no))) {
		glNewList(program.skin_list, GL_COMPILE);
		glEndList();
		return TRUE;
	}

	int skin_lit = kudu_options_get_int_no(KO_SKIN_LIT, program.mode_opt_no);
	int skin_detail = kudu_options_get_int_no(KO_SKIN_DETAIL, program.mode_opt_no);
	int skin_smooth = kudu_options_get_int_no(KO_SKIN_SMOOTH, program.mode_opt_no);
	int skin_colours = kudu_options_get_int_no(KO_SKIN_REAL_COLOURS, program.mode_opt_no);
	int skin_textures = kudu_options_get_int_no(KO_SKIN_TEXTURED, program.mode_opt_no);

	glNewList(program.skin_list, GL_COMPILE);
	if (!skin_lit) glDisable(GL_LIGHTING);

	render_options = skin_detail;

	if (program.sub_mode != PROGRAM_MODE_VIEW) {
		switch (kudu_options_get_int_no(KO_SELECTION_MODE, program.mode_opt_no)) {
			case SELECT_POINTS:
				if (render_options != K_RENDER_POINTS)
					render_options += K_RENDER_POINTS;
				render_options += K_RENDER_SHOW_SELECTED_POINTS;
				render_options += K_RENDER_POINTS_NO_COLOURS;
				glPointSize(5.0);
				break;
			case SELECT_EDGES:
				if (render_options != K_RENDER_EDGES)
					render_options += K_RENDER_EDGES;
				render_options += K_RENDER_SHOW_SELECTED_EDGES;
				render_options += K_RENDER_EDGES_NO_COLOURS;
				glLineWidth(2.0);
				break;
			case SELECT_FACES:
				if (render_options != K_RENDER_FACES)
					render_options += K_RENDER_FACES;
				if (!(render_options & K_RENDER_EDGES))
					render_options += K_RENDER_EDGES;
				render_options += K_RENDER_SHOW_SELECTED_FACES;
				render_options += K_RENDER_EDGES_NO_COLOURS;
				break;
			case SELECT_SHAPES:
				if (render_options != K_RENDER_FACES)
					render_options += K_RENDER_FACES;
				render_options += K_RENDER_SHOW_SELECTED_FACES;
				render_options += K_RENDER_SHOW_SELECTED_SHAPES;
				break;
		}
	}

	if (!skin_smooth) render_options += K_RENDER_FLAT_SHADING;
	if (skin_lit) render_options += K_RENDER_LIGHTING;
	if (!skin_colours) render_options += K_RENDER_NO_COLOURS;
	if ((render_options & K_RENDER_FACES) && (skin_textures)) {
		render_options += K_RENDER_TEXTURES;
		glEnable(GL_TEXTURE_2D);
	}

	if (program.mode == PROGRAM_MODE_ANIMATION) {
		glPushMatrix();
		glTranslatef(program.selected_object->position[0], program.selected_object->position[1],
				program.selected_object->position[2]);
	}
	kudu_draw_shapes(program.selected_object->skin, render_options);
	if (program.mode == PROGRAM_MODE_ANIMATION) glPopMatrix();

	glPointSize(1.0);
	glLineWidth(1.0);

	if (!skin_lit) glEnable(GL_LIGHTING);
	if (glIsEnabled(GL_TEXTURE_2D)) glDisable(GL_TEXTURE_2D);
	glEndList();

	return TRUE;
}

/* Updates the current object skin only if visible, then calls */
/* kudu_program_skin_list_regen to regenerate the skin list */
int kudu_program_update_skin(void)
{
	if ((program.mode == PROGRAM_MODE_EDIT) && (program.sub_mode == PROGRAM_MODE_SKELETON))  return TRUE;

	int lit, smooth;

	if (kudu_options_get_int_no(KO_SKIN_VISIBLE, program.mode_opt_no)) {
		lit = kudu_options_get_int_no(KO_SKIN_LIT, program.mode_opt_no);
		smooth = kudu_options_get_int_no(KO_SKIN_SMOOTH, program.mode_opt_no);

		if ((lit) && (smooth)) lit = TRUE;
		else	lit = FALSE;

		kudu_object_update_vertices(program.selected_object, lit);
	}

	kudu_program_skin_list_regen();
}

/* Regenerate the skeleton list for the current object */
int kudu_program_skeleton_list_regen(void)
{
	int render_options = K_RENDER_SHOW_SELECTED;

	glNewList(program.skeleton_list, GL_COMPILE);

	/* If we don't have a valid selected object or skeleton is set to invisible, then just clear this list */
	if ((program.selected_object == NULL) || (!kudu_options_get_int_no(KO_BONES_VISIBLE, program.mode_opt_no))) {
		glEndList();
		return TRUE;
	}

	glDisable(GL_LIGHTING);

	int detail = kudu_options_get_int_no(KO_BONES_DETAIL, program.mode_opt_no);
	int axes_detail = kudu_options_get_int_no(KO_BONES_AXES, program.mode_opt_no);

	if (detail == SKELETON_DETAIL_SHOW_ALL)	render_options += K_RENDER_SHOW_INFO;
	else if (detail == SKELETON_DETAIL_SHOW_SELECTED) render_options += K_RENDER_SHOW_SELECTED_INFO;

	if (axes_detail == SKELETON_DETAIL_SHOW_ALL)	render_options += K_RENDER_SHOW_AXES;
	else if (axes_detail == SKELETON_DETAIL_SHOW_SELECTED) render_options += K_RENDER_SHOW_SELECTED_AXES;

	if (kudu_options_get_int_no(KO_JOINTS_VISIBLE, program.mode_opt_no)) render_options += K_RENDER_JOINTS;

	if (program.mode == PROGRAM_MODE_ANIMATION) {
		glPushMatrix();
		glTranslatef(program.selected_object->position[0], program.selected_object->position[1], program.selected_object->position[2]);
	}
	if (program.selected_object->bone != NULL) kudu_draw_all_bones(program.selected_object->bone, render_options);
	if (program.mode == PROGRAM_MODE_ANIMATION) glPopMatrix();

	glEnable(GL_LIGHTING);
	glEndList();

	return TRUE;
}

static gboolean display(GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
	GdkGLContext *glContext = gtk_widget_get_gl_context(widget);
	GdkGLDrawable *glDrawable = gtk_widget_get_gl_drawable(widget);
	int a, b, c;
	char text[260], text2[260];
	/*static int flip = FALSE;*/

	if (!gdk_gl_drawable_gl_begin(glDrawable, glContext)) return FALSE;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/*if (flip) {
		glColorMask(TRUE, FALSE, FALSE, TRUE);
		flip = !flip;
		kudu_camera_swing(camera, -2.5, 0, 0);
		kudu_program_view_point_set();
	} else {
		glColorMask(FALSE, FALSE, TRUE, TRUE);
		flip = !flip;
		kudu_camera_swing(camera, 2.5, 0, 0);
		kudu_program_view_point_set();
	}*/

	glCallList(program.grid_list);
/*	glCallList(program.displayList);*/

	glCallList(program.skin_list);		/* Selected object skin */
	glCallList(program.skeleton_list);	/* Selected object skeleton */
	glCallList(program.object_list);	/* Unselected objects (in Animation mode) */

	glColor3f(1.0, 1.0, 0.0);
	sprintf(text, "Kudu Currently in: %s", PROGRAM_MODE_NAME[program.mode_opt_no]);
	kudu_font_builtin_write_2d(10, program.windowHeight-20, text);

	if (program.sub_mode != PROGRAM_MODE_VIEW) {
		switch (kudu_options_get_int_no(KO_SELECTION_MODE, program.mode_opt_no)) {
			case SELECT_BONES:
			case SELECT_JOINTS:
				kudu_selection_list_draw_info(selected_bones_list, 10, program.windowHeight-60);
				break;
			case SELECT_OBJECTS:
				break;
			default:
				kudu_selection_list_draw_info(selection_list, 10, program.windowHeight-60);
				/*if (selection_list->selected_items <= 0) break;*/
			/*	switch (selection_list->type) {
					case SELECT_POINTS:
						sprintf(text2, "vertices");
						break;
					case SELECT_EDGES:
						sprintf(text2, "edges");
						break;
					case SELECT_FACES:
						sprintf(text2, "faces");
						break;
					case SELECT_SHAPES:
						sprintf(text2, "shapes");
						break;
				}
				sprintf(text, "%d Selected %s", selection_list->selected_items, text2);
				kudu_font_builtin_write_2d(10, program.windowHeight-60, text);
				break;*/
		}
	}

	if (program.mode == PROGRAM_MODE_ANIMATION) {
		if (frame_list != NULL) {
			sprintf(text, "Frame: %d", frame_list->current_frame);
			kudu_font_builtin_write_2d(program.windowWidth - 250, program.windowHeight - 20, text);

			a = (((frame_list->current_frame / 24) / 60) / 60);
			b = (((frame_list->current_frame / 24) / 60) - (a * 60));
			c = (frame_list->current_frame / 24) - (b * 60) - ((a * 60) * 60);
			sprintf(text, "Current Time  : %02d:%02d:%02d", a, b, c);
			kudu_font_builtin_write_2d(program.windowWidth - 250, program.windowHeight - 40, text);
		}
	}

	if (program.mouse.dragging)
		kudu_draw_mouse_rectangle(program.mouse.sx, program.mouse.sy, program.mouse.ex, program.mouse.ey);

/*	glFlush();*/

	gdk_gl_drawable_swap_buffers(glDrawable);

	gdk_gl_drawable_gl_end(glDrawable);
}

int kudu_program_view_point_set(void)
{
	glLoadIdentity();

	kudu_camera_set_gl_viewpoint(camera);

	kudu_grid_list_regen();

	return TRUE;
}

static gboolean reshape(GtkWidget *widget, GdkEventConfigure *event, gpointer data)
{
	GdkGLContext *glContext = gtk_widget_get_gl_context(widget);
	GdkGLDrawable *glDrawable = gtk_widget_get_gl_drawable(widget);
	GtkWindow *window = (GtkWindow*)data;


	float w = widget->allocation.width;
	float h = widget->allocation.height;
	program.drawing_area_xpos = widget->allocation.x;
	program.drawing_area_ypos = widget->allocation.y;

	program.windowWidth = w;
	program.windowHeight = h;

	if (!gdk_gl_drawable_gl_begin(glDrawable, glContext)) return FALSE;

	kudu_camera_set_gl_matrix(camera, (int)w, (int)h);

	gdk_gl_drawable_gl_end(glDrawable);

	kudu_program_view_point_set();
	return FALSE;
}


static gboolean key_press(GtkWidget *widget, GdkEventKey *event, gpointer data)
{
	int kp;

	if (event->type == GDK_KEY_PRESS) kp = TRUE;
	else	kp = FALSE;

	KuduShape *shape;
	KuduFace *face;
	KuduEdge *edge;
	KuduVertex *vertex, *vl[3];
	KuduBone *bone;
	KuduMaterial *material;
	unsigned int modifiers = 0, event_type = event->type;

	if ((event->state & GDK_SHIFT_MASK) || (event->keyval == GDK_Shift_R) || (event->keyval == GDK_Shift_L)) modifiers += GDK_SHIFT_MASK;
	if ((event->state & GDK_CONTROL_MASK) || (event->keyval == GDK_Control_R) || (event->keyval == GDK_Control_L)) modifiers += GDK_CONTROL_MASK;
	if ((event->state & GDK_MOD1_MASK) || (event->keyval == GDK_Alt_R) || (event->keyval == GDK_Alt_L)) modifiers += GDK_MOD1_MASK;

	program.mouse.modifiers = modifiers;

	if (event_type == GDK_KEY_RELEASE) {
		if ((event->keyval == GDK_Shift_R) || (event->keyval == GDK_Shift_L)) program.mouse.modifiers -= GDK_SHIFT_MASK;
		if ((event->keyval == GDK_Control_R) || (event->keyval == GDK_Control_L)) program.mouse.modifiers -= GDK_CONTROL_MASK;
		if ((event->keyval == GDK_Alt_R) || (event->keyval == GDK_Alt_L)) program.mouse.modifiers -= GDK_MOD1_MASK;
	}

	if (event->type == GDK_KEY_PRESS) program.mouse.key = event->keyval;
	else program.mouse.key = 0;

	if (!program.mouse.dragging)
		if (kudu_camera_determine_mode(camera, &program.mouse) > 0) return TRUE;


	switch (event->keyval){
		/*case GDK_q:
		case GDK_Q:
			if (kp) break;
			if (camera->mode != CAMERA_MODE_FIXED) {
				if (camera->mode == CAMERA_MODE_TUMBLE) kudu_camera_push_mode(camera, CAMERA_MODE_TRACK);
				else	if (camera->mode == CAMERA_MODE_TRACK)
						kudu_camera_pop_mode(camera);
			}
			break;*/
		case GDK_Delete:
			if (kp) break;
			if (kudu_options_get_int_no(KO_SELECTION_MODE, program.mode_opt_no) == SELECT_FACES) {
				if (!kudu_selection_list_for_each_do(selection_list)) break;
				while ((face = (KuduFace*)kudu_selection_list_next_do()) != NULL) {
					printf("\nFace %d  uv coords and vertices\n", face->id);
					material = face->material;
					if (material != NULL) {
						printf("Face has material: %d\n", material->id);
						if (material->texture != NULL) printf("Material has a texture\n");
						else	printf("Material has no texture\n");
					} else printf("Face has no material\n");

					if (!kudu_face_for_each_edge_do(face)) break;

					while ((edge = kudu_face_for_each_edge_next_do()) != NULL) {
						printf("On Edge %d :: ", edge->id);
						if (edge->left_face == face) {
							printf("vertex: %d :: ", edge->end_vertex->id);
							printf("%4.2f, %4.2f | ", edge->e_uv[0], edge->e_uv[1]);
							printf("%4.2f, %4.2f, %4.2f\n", edge->end_vertex->v[0], edge->end_vertex->v[1],
								edge->end_vertex->v[2]);
						} else if (edge->right_face == face) {
							printf("vertex: %d :: ", edge->start_vertex->id);
							printf("%4.2f, %4.2f | ", edge->s_uv[0], edge->s_uv[1]);
							printf("%4.2f, %4.2f, %4.2f\n", edge->start_vertex->v[0], edge->start_vertex->v[1],
								edge->start_vertex->v[2]);
						}
					}
				}
			}
		break;
		case GDK_Insert:
			if (kp) break;
			printf("\n");
			if (kudu_options_get_int_no(KO_SELECTION_MODE, program.mode_opt_no) == SELECT_SHAPES) {
				if (!kudu_selection_list_for_each_do(selection_list)) break;
				while ((shape = (KuduShape*)kudu_selection_list_next_do()) != NULL) {
					printf("Shape: %d\n", shape->id);
				}
			}
			if (kudu_options_get_int_no(KO_SELECTION_MODE, program.mode_opt_no) == SELECT_EDGES) {
				if (!kudu_selection_list_for_each_do(selection_list)) break;
				while ((edge = (KuduEdge*)kudu_selection_list_next_do()) != NULL) {
					kudu_diag_print_edge(edge);
				}
			}
			if (kudu_options_get_int_no(KO_SELECTION_MODE, program.mode_opt_no) == SELECT_POINTS) {
				if (program.sub_mode == PROGRAM_MODE_ATTACH) {
					if (!kudu_selection_list_for_each_do(selection_list)) break;
					while ((vertex = (KuduVertex*)kudu_selection_list_next_do()) != NULL) {
						kudu_vertex_print_attachments(vertex);
					}
				} else {
					if (!kudu_selection_list_for_each_do(selection_list)) break;
					while ((vertex = (KuduVertex*)kudu_selection_list_next_do()) != NULL) {
						printf("Vertex: %d,  Edge: %d  pos: %4.2f, %4.2f, %4.2f  normal: %4.2f, %4.2f, %4.2f \n",
												vertex->id, vertex->edge->id,
												vertex->v[0], vertex->v[1], vertex->v[2],
												vertex->n[0], vertex->n[1], vertex->n[2]);
					}
				}
			}
			if (kudu_options_get_int_no(KO_SELECTION_MODE, program.mode_opt_no) == SELECT_FACES) {
				if (!kudu_selection_list_for_each_do(selection_list)) break;
				while ((face = (KuduFace*)kudu_selection_list_next_do()) != NULL) {
					printf("Face: %d,  Edge: %d\n", face->id, face->edge->id);
					printf("Normal Vector: %4.8f, %4.8f, %4.8f\n", face->fn[0], face->fn[1], face->fn[2]);
					printf("Face Vertices:\n");
					if (!kudu_face_for_each_vertex_do(face)) break;
					while ((vertex = kudu_face_for_each_vertex_next_do()) != NULL) {
						printf("Vertex: %d,  ", vertex->id);
					}
					printf("\n\n");
				}
			}
			if (program.sub_mode == PROGRAM_MODE_SKELETON) {
				printf("\nThe following bones selected:\n");
				if (!kudu_selection_list_for_each_do(selected_bones_list)) break;
				while ((bone = (KuduBone*)kudu_selection_list_next_do()) != NULL) {
					printf("%s\n", bone->name);
				}
				printf("\n");
			}
			break;
		default:
			break;
	}

	if (program.uip_mode) kudu_uip_keypress(event->keyval, event->type);

	return FALSE;
}

static gboolean mouse_motion(GtkWidget *widget, GdkEventMotion *event, gpointer window)
{
	static gboolean force_shift, skip_turn = FALSE;
	int X = (GLint)event->x;
	int Y = (GLint)event->y;

	float md, tx, ty, vscroll, hscroll;
	int nx, ny;
	static float opt_v, opt_h;
	KuduBone *current_bone;

	nx = X;
	ny = Y;

	/* Skip every second time this function is called */
	/* This will lower the cpu load on recalculation (i.e. when adjusting a bone)*/
	/*skip_turn = !skip_turn;
	if (skip_turn) return TRUE;*/

	/* Set if mouse position was explicity shifted */
	/* Don't want that to affect anything */
	if (force_shift) {
		force_shift = FALSE;
		return TRUE;
	}

	if (camera->mode != CAMERA_MODE_FIXED) event->state = 0;
	if (!kudu_util_mouse_scroll(nx, ny, &hscroll, &vscroll, &opt_h, &opt_v, event->state)) return TRUE;


	if (program.mouse.dragging) {
		program.mouse.ex = X;
		program.mouse.ey = Y;
	}

	if (kudu_camera_mouse_action(camera, hscroll, vscroll)) kudu_program_view_point_set();


	if (program.uip_mode) {
		kudu_uip_mouse_motion(X, Y, event->state, hscroll, vscroll, opt_h, opt_v);
	} else
	/* Adjust apropriate bone angle if in bone adjustment mode */
	if (program.bone_mode != BONE_MODE_FIXED) {
		if (kudu_bones_edit_selection(selected_bones_list, opt_h, opt_v, hscroll, vscroll, program.bone_mode)) {
			kudu_bone_smart_update_all(program.selected_object->bone);
			kudu_program_skeleton_list_regen();

			kudu_program_update_skin();
		}
	} else if (program.joint_mode != JOINT_MODE_FIXED) {
		if (kudu_joints_edit_selection(selected_bones_list, opt_h, opt_v, program.joint_mode)) {
			kudu_bone_magic_update_all(program.selected_object->bone);
			kudu_program_skeleton_list_regen();
			kudu_program_update_skin();
		}
	} else if (program.edit_mode != EDIT_MODE_NONE) {
		if (kudu_options_get_int_no(KO_SELECTION_MODE, program.mode_opt_no) != SELECT_OBJECTS) {
			kudu_skin_edit_selection(selection_list, hscroll, opt_v, program.edit_mode);
			kudu_program_update_skin();
		} else {
			kudu_skin_edit_object(program.selected_object, hscroll, opt_v, program.edit_mode);
			kudu_program_skeleton_list_regen();
			kudu_program_update_skin();
		}
	} else {
		opt_v = 0;
		opt_h = 0;
	}

	if ((camera->mode != CAMERA_MODE_FIXED) || (program.bone_mode != BONE_MODE_FIXED) ||
	    (program.joint_mode != JOINT_MODE_FIXED) || (program.edit_mode != EDIT_MODE_NONE)) {
		force_shift = TRUE;
		kudu_gui_center_pointer();
	}

	return TRUE;
}

int mouse_scroll(GtkWidget *widget, GdkEventScroll *event, gpointer data)
{
	if (!(kudu_options_enabled(KO_MOUSE_SCROLL_ZOOM))) return FALSE;

	int direction = event->direction;
	float amount, percent = kudu_options_get_float_no(KO_MOUSE_SCROLL_ZOOM, 0);

	/* 1% of distance */
	amount = (kudu_camera_get_distance(camera) / 100);
	/* Scale to percent */
	amount *= percent;

	if (kudu_options_enabled(KO_MOUSE_SCROLL_REVERSE)) amount = -amount;

	switch (direction) {
		case GDK_SCROLL_UP:
			kudu_camera_swing(camera, 0, 0, -amount);
			break;
		case GDK_SCROLL_DOWN:
			kudu_camera_swing(camera, 0, 0, amount);
			break;
	}

	kudu_program_view_point_set();

	return FALSE;
}

static gboolean mouse_button(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
	unsigned int state, button = event->button;
	int X = (int)event->x;
	int Y = (int)event->y;
	int a, do_pick = TRUE, modifiers = 0, cam_tumble[5], cam_track[5], cam_dolly[5], cam_globals[2];
	GtkWidget *menu = NULL;
	KuduBone *current_bone;

	if (event->type == GDK_BUTTON_PRESS) state = BUTTON_DOWN;
	else if (event->type == GDK_BUTTON_RELEASE) state = BUTTON_UP;

	if (event->state & GDK_SHIFT_MASK) modifiers += GDK_SHIFT_MASK;
	if (event->state & GDK_CONTROL_MASK) modifiers += GDK_CONTROL_MASK;
	if (event->state & GDK_MOD1_MASK) modifiers += GDK_MOD1_MASK;

	program.mouse.button = event->button;
	program.mouse.state = state;
	program.mouse.modifiers = modifiers;


	if ((program.mouse.drop_release > 0) && state == BUTTON_UP) {
		program.mouse.drop_release--;
		return TRUE;
	}

	if ((program.mouse.drop_push > 0) && state == BUTTON_DOWN) {
		program.mouse.drop_push--;
		return TRUE;
	}

	a = kudu_camera_determine_mode(camera, &program.mouse);

	if (a > 0) {
		if (a > 1) kudu_program_view_point_set();
		return TRUE;
	}

	switch (button) {
		case MOUSE_RIGHT_BUTTON:
			if (program.bone_mode != BONE_MODE_FIXED) {
				kudu_bones_edit_unanchor(selected_bones_list, program.bone_mode);
				kudu_bone_smart_update_all(program.selected_object->bone);
				kudu_program_bone_mode_set(BONE_MODE_FIXED);

				kudu_program_skeleton_list_regen();
				kudu_program_update_skin();
			} else if (program.joint_mode != JOINT_MODE_FIXED) {
				kudu_joints_edit_unanchor(selected_bones_list, program.joint_mode);
				kudu_bone_smart_update_all(program.selected_object->bone);
				kudu_program_joint_mode_set(JOINT_MODE_FIXED);

				kudu_program_skeleton_list_regen();
				kudu_program_update_skin();
			} else if (program.edit_mode != EDIT_MODE_NONE) {
				kudu_skin_edit_unanchor(selection_list, program.edit_mode);
				kudu_program_edit_mode_set(EDIT_MODE_NONE);
				kudu_program_update_skin();

			} else if ((state == BUTTON_DOWN) && (camera->mode == CAMERA_MODE_FIXED)) {
				if (program.mode == PROGRAM_MODE_EDIT) {
					switch (program.sub_mode) {
						case PROGRAM_MODE_SKELETON:
							if (kudu_options_get_int_no(KO_SELECTION_MODE, program.mode_opt_no) == SELECT_JOINTS)
								menu = kudu_menu_get_submenu(KM_EDIT_JOINTS);
							else	menu = kudu_menu_get_submenu(KM_EDIT_BONES);
							break;
						case PROGRAM_MODE_SKIN:
							menu = kudu_menu_get_submenu(KM_EDIT_SKIN);
							break;
						case PROGRAM_MODE_ATTACH:
							menu = kudu_menu_get_submenu(KM_EDIT_ATTACHMENTS);
							break;
					}
				} else if (program.mode == PROGRAM_MODE_ANIMATION) {
					switch (kudu_options_get_int_no(KO_SELECTION_MODE, program.mode_opt_no)) {
						case SELECT_OBJECTS:
							menu = kudu_menu_get_submenu(KM_EDIT_OBJECT);
							break;
						case SELECT_BONES:
							menu = kudu_menu_get_submenu(KM_EDIT_BONES);
							break;
						case SELECT_POINTS:
							break;
						case SELECT_EDGES:
							break;
						case SELECT_FACES:
							break;
						case SELECT_SHAPES:
							break;
					}
				}

				if (menu != NULL) {
					gtk_menu_popup(GTK_MENU(menu), NULL, NULL, NULL, NULL, event->button, event->time);
				}
			}
			break;

		case MOUSE_LEFT_BUTTON:
			if (state == BUTTON_DOWN) {
				program.mouse.dragging = TRUE;
				program.mouse.sx = X;
				program.mouse.sy = Y;
				program.mouse.ex = X;
				program.mouse.ey = Y;
			} else {
				program.mouse.dragging = FALSE;
				program.mouse.ex = X;
				program.mouse.ey = Y;

				if (program.bone_mode != BONE_MODE_FIXED) {
					kudu_program_bone_mode_set(BONE_MODE_FIXED);
					do_pick = FALSE;
				}

				if (program.joint_mode != JOINT_MODE_FIXED) {
					kudu_program_joint_mode_set(JOINT_MODE_FIXED);
					do_pick = FALSE;
				}

				if (program.edit_mode != EDIT_MODE_NONE) {
					kudu_program_edit_mode_set(EDIT_MODE_NONE);
					do_pick = FALSE;
				}
			}
			break;
	}

	if (!do_pick) return TRUE;

	/* Is this a left mouse button release ? */
	if ((button != MOUSE_LEFT_BUTTON) || (state != BUTTON_UP)) return TRUE;

	/* Do not continue if we are currently editing something or we are in view/playback mode */
	if ((camera->mode != CAMERA_MODE_FIXED) || (program.bone_mode != BONE_MODE_FIXED) ||
	    (program.edit_mode != EDIT_MODE_NONE) || (program.sub_mode == PROGRAM_MODE_VIEW)) return TRUE;

	int viewport[4], selection_type, render_type, fc;
	unsigned int selection_mode;

	/* Retrieve the selection mode */
	selection_mode = kudu_options_get_int_no(KO_SELECTION_MODE, program.mode_opt_no);

	kudu_hits_list_clear();

	/* If we are selecting objects, but the object list is null, then stop */
	if ((selection_mode == SELECT_OBJECTS) && (object == NULL)) return TRUE;
	else if (program.selected_object == NULL) return TRUE;	/* If not we need a valid selected object to continue... */

	switch (selection_mode) {
		case SELECT_BONES:
			if (program.selected_object->bone == NULL) return TRUE;
			break;
		case SELECT_JOINTS:
			if (program.selected_object->joint == NULL) return TRUE;
			break;
		default:
			if (program.selected_object->skin == NULL) return TRUE;
			break;
	}

	glGetIntegerv(GL_VIEWPORT, viewport);

	glSelectBuffer(program.hit.buf_size, program.hit.buffer);
	glRenderMode(GL_SELECT);

	glInitNames();

	selection_type = kudu_camera_set_gl_pick_matrix(camera, program.mouse.sx,
				program.mouse.sy, program.mouse.ex, program.mouse.ey);

	fc = glIsEnabled(GL_CULL_FACE);
	if (selection_type == K_SELECT_PICK) glEnable(GL_CULL_FACE);
	if (selection_type == K_SELECT_RECTANGLE) glDisable(GL_CULL_FACE);

	if ((selection_mode != SELECT_OBJECTS) && (program.mode == PROGRAM_MODE_ANIMATION)) {
		glPushMatrix();
		glTranslatef(program.selected_object->position[0], program.selected_object->position[1], program.selected_object->position[2]);
	}

	if (selection_mode == SELECT_BONES) {
		glPushName(0);
		kudu_draw_all_bones(program.selected_object->bone, K_RENDER_SELECT);
	} else if (selection_mode == SELECT_JOINTS) {
		glPushName(0);
		kudu_draw_all_joints(program.selected_object->joint, K_RENDER_SELECT);
	} else if (selection_mode == SELECT_OBJECTS) {
		kudu_draw_objects(object, K_RENDER_FACES | K_RENDER_SELECT_OBJECTS | K_RENDER_TRANSLATE_POS);
	} else {
		render_type = K_RENDER_SELECT;
		switch(selection_mode) {
			case SELECT_POINTS:
				render_type += K_RENDER_POINTS;
				break;
			case SELECT_EDGES:
				render_type += K_RENDER_EDGES;
				break;
			case SELECT_FACES:
				render_type += K_RENDER_FACES;
				break;
			case SELECT_SHAPES:
				render_type = K_RENDER_SELECT_SHAPES;
				render_type += K_RENDER_FACES;
				break;
		}

			kudu_draw_shapes(program.selected_object->skin,  render_type);
	}

	if ((selection_mode != SELECT_OBJECTS) && (program.mode == PROGRAM_MODE_ANIMATION)) glPopMatrix();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glFlush();

	if ((fc) && (!glIsEnabled(GL_CULL_FACE))) glEnable(GL_CULL_FACE);

	program.hit.hits = glRenderMode(GL_RENDER);
	kudu_selection_hits_process(selection_mode, selection_type, event->state, object, selection_list, selected_bones_list);
	if ((selection_mode == SELECT_BONES) || (selection_mode == SELECT_JOINTS)) kudu_program_skeleton_list_regen();
	else kudu_program_skin_list_regen();

	return TRUE;
}

static gboolean redraw(GtkWidget *widget)
{
	gdk_window_invalidate_rect (widget->window, &widget->allocation, FALSE);
	gdk_window_process_updates (widget->window, FALSE);

	if (program.request_refresh) {
		kudu_program_skin_list_regen();
		kudu_program_skeleton_list_regen();
		program.request_refresh = FALSE;
	}

	usleep(1000);

	return TRUE;
}

static GdkGLConfig *configure_gl(void)
{
	GdkGLConfig *glConfig;

	glConfig = gdk_gl_config_new_by_mode(GDK_GL_MODE_RGBA | GDK_GL_MODE_DOUBLE | GDK_GL_MODE_DEPTH);

	if (glConfig == NULL) {
		g_print("\nError - Unable to initialize OpenGL - quitting\n");
		exit (1);
	}

	return glConfig;
}

gboolean main_window_move(GtkWidget *widget, gpointer data)
{
	GtkWindow *window = GTK_WINDOW(widget);

	if (kudu_options_get_int_no(KO_WINDOW_MAXED, 0)) return FALSE;

	if (kudu_options_enabled(KO_WINDOW_SIZE)) {
		int wsize[2];
		gtk_window_get_size(window, &wsize[0], &wsize[1]);
		kudu_options_set_int(KO_WINDOW_SIZE, 2, wsize);
	}

	if (kudu_options_enabled(KO_WINDOW_POSITION)) {
		int wpos[2];
		gtk_window_get_position(window, &wpos[0], &wpos[1]);
		kudu_options_set_int(KO_WINDOW_POSITION, 2, wpos);
	}

	return FALSE;
}

gboolean main_window_state(GtkWidget *widget, GdkEventWindowState *event, gpointer data)
{
	if (!kudu_options_enabled(KO_WINDOW_MAXED)) return FALSE;

	if (event->new_window_state & GDK_WINDOW_STATE_MAXIMIZED)
		kudu_options_set_int_no(KO_WINDOW_MAXED, 0, 1);
	else	kudu_options_set_int_no(KO_WINDOW_MAXED, 0, 0);

	return FALSE;
}

static GtkWidget *create_main_window(GdkGLConfig *glConfig)
{
	GtkWidget *window, *vbox, *main_menu_bar, *gl_drawing_area, *toolbar, *status_bar;
	GtkToolItem *tool_item;
	GtkAccelGroup *accel_group;


	/* Main application window */
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_drag_dest_set(window, GTK_TARGET_SAME_APP, NULL, 0, 0);
	gtk_window_set_title(GTK_WINDOW(window), "Kudu");
	gtk_container_set_reallocate_redraws(GTK_CONTAINER(window), TRUE);

	g_signal_connect(G_OBJECT(window), "delete_event", G_CALLBACK(kudu_program_exit), window);

	vbox = gtk_vbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(window), vbox);
	gtk_widget_show(vbox);

	/* Accelerator group for main window */
	accel_group = gtk_accel_group_new();
	gtk_window_add_accel_group(GTK_WINDOW(window), accel_group);

	/* Main menu */
	main_menu_bar = kudu_gui_main_menu_bar_build(GTK_WINDOW(window), accel_group, G_CALLBACK(main_menu_action));
	gtk_box_pack_start(GTK_BOX(vbox), main_menu_bar, FALSE, FALSE, 0);
	gtk_widget_show(main_menu_bar);

	/* Toolbar */
/*	toolbar = gtk_toolbar_new();*/
	toolbar = kudu_main_toolbar_build(accel_group, G_CALLBACK(main_toolbar_action));

	gtk_box_pack_start(GTK_BOX(vbox), toolbar, FALSE, FALSE, 0);
	gtk_widget_show(toolbar);

	/* Most important - set up the OpenGL drawing area */
	gl_drawing_area = gtk_drawing_area_new();
	gtk_widget_set_size_request(gl_drawing_area, 320, 240);

	gtk_widget_set_gl_capability(gl_drawing_area, glConfig, NULL, TRUE, GDK_GL_RGBA_TYPE);
	gtk_widget_add_events(gl_drawing_area, GDK_BUTTON1_MOTION_MASK | GDK_BUTTON2_MOTION_MASK |
			GDK_BUTTON_PRESS_MASK | GDK_VISIBILITY_NOTIFY_MASK | GDK_BUTTON_RELEASE_MASK |
			GDK_POINTER_MOTION_MASK | GDK_SCROLL_MASK);

	g_signal_connect(G_OBJECT(gl_drawing_area), "expose_event", G_CALLBACK(display), NULL);
	g_signal_connect(G_OBJECT(gl_drawing_area), "realize", G_CALLBACK(kudu_program_init), NULL);
	g_signal_connect(G_OBJECT(gl_drawing_area), "configure_event", G_CALLBACK(reshape), window);
	g_signal_connect(G_OBJECT(gl_drawing_area), "motion_notify_event", G_CALLBACK(mouse_motion), window);
	g_signal_connect(G_OBJECT(gl_drawing_area), "button_press_event", G_CALLBACK(mouse_button), NULL);
	g_signal_connect(G_OBJECT(gl_drawing_area), "button_release_event", G_CALLBACK(mouse_button), NULL);
	g_signal_connect(G_OBJECT(gl_drawing_area), "scroll_event", G_CALLBACK(mouse_scroll), NULL);

	gtk_box_pack_start(GTK_BOX(vbox), gl_drawing_area, TRUE, TRUE, 0);
	gtk_widget_show(gl_drawing_area);

	g_signal_connect(G_OBJECT(window), "window_state_event", G_CALLBACK(main_window_state), NULL);
	g_signal_connect(G_OBJECT(window), "configure_event", G_CALLBACK(main_window_move), NULL);
	g_signal_connect_after(G_OBJECT(window), "key_press_event", G_CALLBACK(key_press), gl_drawing_area);
	g_signal_connect_after(G_OBJECT(window), "key_release_event", G_CALLBACK(key_press), gl_drawing_area);


/*	status_bar = gtk_statusbar_new();
	gtk_statusbar_push(GTK_STATUSBAR(status_bar), 0, "Kudu - Copyright (C) 2005 Daniel Pekelharing");
	gtk_box_pack_start(GTK_BOX(vbox), status_bar, FALSE, FALSE, 0);
	gtk_widget_show(status_bar);*/

	kudu_stack_list_add_item("main_window", window);
	kudu_stack_list_add_item("main_gl_drawing_area", gl_drawing_area);

	#ifdef SUPPORT_PYTHON
	kudu_script_list_init();
	#endif

	kudu_options_load_accel_map();

	return window;
}

int main(int argc, char** argv)
{
	GtkWidget *window, *gl_drawing_area;
	GdkGLConfig *glConfig;

	gtk_init(&argc, &argv);
	gtk_gl_init(&argc, &argv);

	/* Init stack list and options system */
	kudu_stack_list_init();
	kudu_options_init();

	/* Set default options, this will also load user specific options if user has a config file in their home dir */
	kudu_options_set_defaults();

	/* Parse command line options */
	kudu_options_parse(argc, argv);

	/* Initialize the splash screen... (if enabled) */
	if (kudu_options_enabled(KO_SHOW_SPLASH)) kudu_about_splash_display();

	/* Initialize OpenGL system and create main window */
	kudu_about_splash_message("Creating main window...");
	glConfig = configure_gl();
	window = create_main_window(glConfig);

	kudu_about_splash_message("Configuring...");

	/* Setup initial window size and pos based on options */
	if (kudu_options_enabled(KO_WINDOW_MAXED)) {
		if (kudu_options_get_int_no(KO_WINDOW_MAXED, 0))
			gtk_window_maximize(GTK_WINDOW(window));
	}

	if (kudu_options_enabled(KO_WINDOW_SIZE)) {
		int wsize[2];
		kudu_options_get_int(KO_WINDOW_SIZE, 2, wsize);
		gtk_window_resize(GTK_WINDOW(window), wsize[0], wsize[1]);
	}

	if (kudu_options_enabled(KO_WINDOW_POSITION)) {
		int pos[2];
		kudu_options_get_int(KO_WINDOW_POSITION, 2, pos);
		gtk_window_move(GTK_WINDOW(window), pos[0], pos[1]);
	}

	/* Set splash screen parent to main window - so that it remains on top */
	kudu_about_splash_set_parent(GTK_WINDOW(window));
	gtk_widget_show_all(window);

	/* Close the splash screen */
	kudu_about_splash_close();

	/* Start the idle time funcion */
	gl_drawing_area = kudu_stack_list_get_item("main_gl_drawing_area");
	gtk_idle_add((GSourceFunc)redraw, gl_drawing_area);

	gtk_main();

	return 0;
}


