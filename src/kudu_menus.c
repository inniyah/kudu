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
#include "kudu_menus.h"

static KuduMenu *k_menu = NULL;

int kudu_menu_store_init(void)
{
	if (k_menu != NULL) return FALSE;

	int a;

	k_menu = malloc((sizeof(KuduMenu)*KM_NUM_ITEMS));
	if (k_menu == NULL) return FALSE;

	for (a = 0; a < KM_NUM_ITEMS; a++) {
		k_menu[a].menu_item = NULL;
	}

	return TRUE;
}

int kudu_menu_store_item(K_MainMenu item, GtkWidget *widget)
{
	if ((k_menu == NULL) || (item > KM_NUM_ITEMS)) return FALSE;

	KuduMenu *menu = &k_menu[item];

	menu->menu_item = widget;
	menu->signal = kudu_menu_last_signal_id();

	return TRUE;
}

GtkWidget *kudu_menu_get_item(K_MainMenu item)
{
	if ((k_menu == NULL) || (item > KM_NUM_ITEMS)) return NULL;

	KuduMenu *menu = &k_menu[item];

	return menu->menu_item;
}

GtkWidget *kudu_menu_get_submenu(K_MainMenu item)
{
	if ((k_menu == NULL) || (item > KM_NUM_ITEMS)) return NULL;

	GtkWidget *m_item = k_menu[item].menu_item;
	GtkWidget *sub;

	if (m_item != NULL) sub = gtk_menu_item_get_submenu(GTK_MENU_ITEM(m_item));

	return sub;
}

int kudu_menu_item_set_label(K_MainMenu item, char *label)
{
	if ((k_menu == NULL) || (item > KM_NUM_ITEMS)) return FALSE;

	KuduMenu *menu = &k_menu[item];

	if (menu->menu_item != NULL) gtk_label_set_text(GTK_LABEL(gtk_bin_get_child(GTK_BIN(menu->menu_item))), label);

	return TRUE;
}

int kudu_menu_item_set_visible(K_MainMenu item, int visible)
{
	if ((k_menu == NULL) || (item > KM_NUM_ITEMS)) return FALSE;

	KuduMenu *menu = &k_menu[item];

	if (visible) gtk_widget_show(menu->menu_item);
	else	gtk_widget_hide(menu->menu_item);

	return TRUE;
}

int kudu_menu_item_activate(K_MainMenu item)
{
	if ((k_menu == NULL) || (item > KM_NUM_ITEMS)) return FALSE;

	KuduMenu *menu = &k_menu[item];

	gtk_widget_activate(menu->menu_item);

	return TRUE;
}

int kudu_menu_item_activate_no_call(K_MainMenu item)
{
	if ((k_menu == NULL) || (item > KM_NUM_ITEMS)) return FALSE;

	KuduMenu *menu = &k_menu[item];

	if (menu->menu_item != NULL) {
		g_signal_handler_block(menu->menu_item, menu->signal);
		gtk_widget_activate(menu->menu_item);
		g_signal_handler_unblock(menu->menu_item, menu->signal);
	}

	return TRUE;
}

int kudu_menu_check_item_set_state(K_MainMenu item, int state)
{
	if ((k_menu == NULL) || (item > KM_NUM_ITEMS)) return FALSE;

	KuduMenu *menu = &k_menu[item];

	if (menu->menu_item != NULL) gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(menu->menu_item), (gboolean)state);

	return TRUE;
}

int kudu_menu_check_item_set_state_no_call(K_MainMenu item, int state)
{
	if ((k_menu == NULL) || (item > KM_NUM_ITEMS)) return FALSE;

	KuduMenu *menu = &k_menu[item];

	if (menu->menu_item != NULL) {
		g_signal_handler_block(menu->menu_item, menu->signal);
		gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(menu->menu_item), (gboolean)state);
		g_signal_handler_unblock(menu->menu_item, menu->signal);
	}

	return TRUE;
}

int kudu_menu_item_set_sensitive(K_MainMenu item, int sensitive)
{
	if ((k_menu == NULL) || (item > KM_NUM_ITEMS)) return FALSE;

	KuduMenu *menu = &k_menu[item];

	if (menu->menu_item != NULL) gtk_widget_set_sensitive(menu->menu_item, sensitive);

	return TRUE;
}

/* Macros to simply creation and storing of menus and items */
#define KM_MENU(label, action) (kudu_menu_store_item(action, kudu_menu_push(label)))
#define KM_ITEM(label, action, accel) (kudu_menu_store_item(action, kudu_menu_add_item(label, action, accel)))
#define KM_CHECK(label, action, accel) (kudu_menu_store_item(action, kudu_menu_add_check(label, action, accel)))
#define KM_RADIO(label, action, accel) (kudu_menu_store_item(action, kudu_menu_add_radio(label, action, accel)))
#define KM_SEPARATOR(action) (kudu_menu_store_item(action, kudu_menu_add_separator()))

GtkWidget *kudu_gui_main_menu_bar_build(GtkWindow *window, GtkAccelGroup *accel_group, GCallback menu_action)
{
	GtkWidget *menu_bar, *menu, *menu_item, *sub_menu, *sub_menu2;
	int a;

	kudu_menu_store_init();

	menu_bar = gtk_menu_bar_new();

	kudu_menu_start_new(accel_group, menu_bar, "main", menu_action);

	/* File menu */
	kudu_menu_push("_File");
	KM_ITEM("New", KM_FILE_NEW, "<ctrl>N");
	KM_ITEM("Open", KM_FILE_OPEN, "<ctrl>O");
	KM_ITEM("Save", KM_FILE_SAVE, "<ctrl>S");
	KM_ITEM("Save as", KM_FILE_SAVE_AS, NULL);
	kudu_menu_add_separator();
	KM_ITEM("Close", KM_FILE_CLOSE, "<ctrl>C");
	kudu_menu_add_separator();
		KM_MENU("Import", KM_FILE_IMPORT);
		KM_ITEM("Renderware (.rwx)", KM_FILE_IMPORT_RWX, NULL);
		KM_ITEM("Wavefront (.obj)", KM_FILE_IMPORT_OBJ, NULL);
		kudu_menu_pop();
	kudu_menu_add_separator();
	KM_ITEM("Exit Kudu", KM_FILE_EXIT, "<ctrl>q");
	kudu_menu_pop();


	/* Edit menu */
	KM_MENU("_Edit", KM_EDIT);

		KM_MENU("Bones", KM_EDIT_BONES);
		KM_ITEM("New root bone", KM_EDIT_BONES_NEW_ROOT, NULL);
		KM_SEPARATOR(KM_EDIT_BONES_SEP1);
		KM_ITEM("Add Child", KM_EDIT_BONES_ADD_CHILD, "A");
		KM_ITEM("Insert Parent", KM_EDIT_BONES_INSERT_PARENT, "I");
		KM_ITEM("Delete", KM_EDIT_BONES_DELETE, "D");
		KM_SEPARATOR(KM_EDIT_BONES_SEP2);
		KM_ITEM("Rotate about local X", KM_EDIT_BONES_ROT_LX, "1");
		KM_ITEM("Rotate about local Y", KM_EDIT_BONES_ROT_LY, "2");
		KM_ITEM("Rotate about local Z", KM_EDIT_BONES_ROT_LZ, "3");
		kudu_menu_add_separator();
		KM_ITEM("Rotate about global X", KM_EDIT_BONES_ROT_GX, NULL);
		KM_ITEM("Rotate about global Y", KM_EDIT_BONES_ROT_GY, NULL);
		KM_ITEM("Rotate about global Z", KM_EDIT_BONES_ROT_GZ, NULL);
		kudu_menu_add_separator();
		KM_ITEM("Rotate Bone", KM_EDIT_BONES_ROTATE, "4");
		KM_ITEM("Stretch Bone", KM_EDIT_BONES_STRETCH, "5");
		kudu_menu_add_separator();
		KM_ITEM("Move X", KM_EDIT_BONES_MOVE_X, NULL);
		KM_ITEM("Move Y", KM_EDIT_BONES_MOVE_Y, NULL);
		KM_ITEM("Move Z", KM_EDIT_BONES_MOVE_Z, NULL);
		kudu_menu_add_separator();
		KM_ITEM("Properties Dialog", KM_EDIT_BONES_PROPERTIES, "<ctr>P");
		kudu_menu_pop();

		KM_MENU("Joints", KM_EDIT_JOINTS);
		KM_ITEM("Move X", KM_EDIT_JOINTS_MOVE_X, NULL);
		KM_ITEM("Move Y", KM_EDIT_JOINTS_MOVE_Y, NULL);
		KM_ITEM("Move Z", KM_EDIT_JOINTS_MOVE_Z, NULL);
		kudu_menu_pop();

		KM_MENU("Attachments", KM_EDIT_ATTACHMENTS);
		KM_ITEM("Attach...", KM_EDIT_ATTACHMENTS_ATTACH, NULL);
		KM_ITEM("Reset", KM_EDIT_ATTACHMENTS_CLEAR, NULL);
	/*	KM_NEW_I(sub_menu, "Attach to Current Bone", 452, GDK_A, 0);*/
		kudu_menu_pop();


/*		NEW_ITEM(sub_menu, "Y Axis", 701, 0, 0);
		NEW_ITEM(sub_menu, "Z Axis", 702, 0, 0);

	STACK_ADD("pick_material", NEW_ITEM(menu, "Material...", 750, 0, 0));

	STACK_ADD("edge_hardness", sub_menu = NEW_SUB(menu, "Hardness"));
		NEW_ITEM(sub_menu, "Hard", 750, 0, 0);
		NEW_ITEM(sub_menu, "Soft", 750, 0, 0);*/

		KM_MENU("Skin", KM_EDIT_SKIN);
		KM_ITEM("Move X", KM_EDIT_SKIN_MOVE_X, NULL);
		KM_ITEM("Move Y", KM_EDIT_SKIN_MOVE_Y, NULL);
		KM_ITEM("Move Z", KM_EDIT_SKIN_MOVE_Z, NULL);
			KM_MENU("Hardness", KM_EDIT_SKIN_EDGE_HARDNESS);
			KM_ITEM("Hard", KM_EDIT_SKIN_EDGE_HARDNESS_HARD, NULL);
			KM_ITEM("Soft", KM_EDIT_SKIN_EDGE_HARDNESS_SOFT, NULL);
			kudu_menu_pop();

		KM_ITEM("Material...", KM_EDIT_SKIN_MATERIAL, NULL);
		kudu_menu_pop();

		KM_MENU("Object", KM_EDIT_OBJECT);
		KM_ITEM("Properties...", KM_EDIT_OBJECT_PROPERTIES, NULL);
		kudu_menu_add_separator();
		KM_ITEM("Move X", KM_EDIT_OBJECT_MOVE_X, "5");
		KM_ITEM("Move Y", KM_EDIT_OBJECT_MOVE_Y, "6");
		KM_ITEM("Move Z", KM_EDIT_OBJECT_MOVE_Z, "7");
		KM_SEPARATOR(KM_EDIT_OBJECT_SEP1);
		KM_MENU("Current", KM_EDIT_OBJECT_CURRENT);
		kudu_menu_pop();
		kudu_menu_pop();


	KM_ITEM("Materials...", KM_EDIT_MATERIALS, "<ctrl>M");
	KM_ITEM("Scripts...", KM_EDIT_SCRIPTS, NULL);

	kudu_menu_pop();


	/* Frames menu */
	KM_MENU("_Frames", KM_FRAMES);
		kudu_menu_push("Insert Keyframe");
			kudu_menu_push("Everything");
			KM_ITEM("World", KM_FRAMES_KEY_ALL_WORLD, "<alt><shift>W");
			KM_ITEM("Selected Object", KM_FRAMES_KEY_ALL_OBJECT, "<alt><shift>O");
			kudu_menu_pop();

			kudu_menu_push("Bones");
			KM_ITEM("World", KM_FRAMES_KEY_BONES_WORLD, "W");
			KM_ITEM("Selected Object", KM_FRAMES_KEY_BONES_OBJECT, "O");
			KM_ITEM("Selected Bone(s)", KM_FRAMES_KEY_BONES_SELECTED, "B");
			kudu_menu_pop();

			kudu_menu_push("Vertices");
			KM_ITEM("World", KM_FRAMES_KEY_VERTICES_WORLD, "<alt>W");
			KM_ITEM("Selected Object", KM_FRAMES_KEY_VERTICES_OBJECT, "<alt>O");
			KM_ITEM("Selected Vertice(s)", KM_FRAMES_KEY_VERTICES_SELECTED, "<alt>V");
			kudu_menu_pop();

			kudu_menu_push("Positions");
			KM_ITEM("World", KM_FRAMES_KEY_POSITIONS_WORLD, "<shift>W");
			KM_ITEM("Selected Object", KM_FRAMES_KEY_POSITIONS_OBJECT, "<shift>O");
			kudu_menu_pop();
		kudu_menu_pop();

		kudu_menu_push("Increment");
			KM_ITEM("One Frame", KM_FRAMES_INCR_1F, "equal");
			KM_ITEM("One Second", KM_FRAMES_INCR_1S, "bracketright");
			KM_ITEM("One Minute", KM_FRAMES_INCR_1M, "apostrophe");
		kudu_menu_pop();

		kudu_menu_push("Increment Non-Destructive");
			KM_ITEM("One Frame", KM_FRAMES_INCR_NOND_1F, "<shift>equal");
			KM_ITEM("One Second", KM_FRAMES_INCR_NOND_1S, "<shift>bracketright");
			KM_ITEM("One Minute", KM_FRAMES_INCR_NOND_1M, "<shift>apostrophe");
		kudu_menu_pop();

		kudu_menu_push("Decrement");
			KM_ITEM("One Frame", KM_FRAMES_DECR_1F, "minus");
			KM_ITEM("One Second", KM_FRAMES_DECR_1S, "bracketleft");
			KM_ITEM("One Minute", KM_FRAMES_DECR_1M, "semicolon");
		kudu_menu_pop();

		kudu_menu_push("Decrement Non-Destructive");
			KM_ITEM("One Frame", KM_FRAMES_DECR_NOND_1F, "<shift>minus");
			KM_ITEM("One Second", KM_FRAMES_DECR_NOND_1S, "<shift>bracketleft");
			KM_ITEM("One Minute", KM_FRAMES_DECR_NOND_1M, "<shift>semicolon");
		kudu_menu_pop();
	kudu_menu_pop();

	/* Select menu */
	KM_MENU("_Select", KM_SELECT);
	KM_ITEM("Next Bone (Sibling)", KM_SELECT_BONES_NEXT, "period");
	KM_ITEM("Previous Bone (Sibling)", KM_SELECT_BONES_PREVIOUS, "comma");
	KM_ITEM("First Child", KM_SELECT_BONES_FIRST_CHILD, "<shift>slash");
	KM_ITEM("All Children", KM_SELECT_BONES_ALL_CHILDREN, "slash");
	KM_ITEM("Parent", KM_SELECT_BONES_PARENT, "M");
	kudu_menu_add_separator();
	KM_ITEM("All", KM_SELECT_ALL, "<ctrl>A");
	KM_ITEM("None", KM_SELECT_NONE, "space");
	KM_ITEM("Inverse", KM_SELECT_INVERSE, "<ctrl>I");
	kudu_menu_pop();


	/* View menu */
	kudu_menu_push("_View");
		kudu_menu_push("Look Down");
		KM_ITEM("-X Axis", KM_VIEW_LOOK_XM, "<shift>X");
		KM_ITEM("+X Axis", KM_VIEW_LOOK_XP, "X");
		KM_ITEM("-Z Axis", KM_VIEW_LOOK_ZM, "<shift>Z");
		KM_ITEM("+Z Axis", KM_VIEW_LOOK_ZP, "Z");
		KM_ITEM("-Y Axis", KM_VIEW_LOOK_XM, "<shift>Y");
		KM_ITEM("+Y Axis", KM_VIEW_LOOK_XP, "Y");
		kudu_menu_pop();

		kudu_menu_push("Focus");
		KM_ITEM("On Origin", KM_VIEW_FOCUS_ORIGIN, "<ctrl>F");
		KM_ITEM("On Selection", KM_VIEW_FOCUS_SELECTION, "F");
		kudu_menu_pop();

	KM_CHECK("Orthographic View", KM_VIEW_ORTHOGRAPHIC, "F5");
	kudu_menu_pop();

	/* Playback Menu */
	KM_MENU("Play_back", KM_PLAYBACK);
	KM_ITEM("Play", KM_PLAYBACK_PLAY, "<ctrl>p");
	KM_ITEM("Pause", KM_PLAYBACK_PAUSE, "<alt>p");
	KM_ITEM("Stop", KM_PLAYBACK_STOP, "<shift>p");
	KM_CHECK("Loop around", KM_PLAYBACK_LOOP, "<ctrl>l");
	KM_CHECK("Use scene camera", KM_PLAYBACK_USE_CAMERA, "<ctrl>c");
	kudu_menu_pop();


	/* Program menu */
	kudu_menu_push("_Program");
		kudu_menu_push("Mode");
		kudu_menu_new_radio_group();
		KM_RADIO("Editor Mode", KM_PROGRAM_MODE_EDIT, "F6");
		KM_RADIO("Animation Mode", KM_PROGRAM_MODE_ANIMATION, "F7");
		kudu_menu_pop();

		kudu_menu_push("Sub Mode");
		kudu_menu_new_radio_group();
		KM_RADIO("Skeleton Mode", KM_PROGRAM_SUB_MODE_1, "F1");
		KM_RADIO("Skin Mode", KM_PROGRAM_SUB_MODE_2, "F2");
		KM_RADIO("Attach Mode", KM_PROGRAM_SUB_MODE_3, "F3");
		KM_RADIO("View Mode", KM_PROGRAM_SUB_MODE_4, "F4");
		kudu_menu_pop();

		kudu_menu_push("Skeleton Detail");
		KM_CHECK("Show Bones", KM_PROGRAM_BONES_SHOW, NULL);
		KM_CHECK("Show Joints", KM_PROGRAM_BONES_SHOW_JOINTS, NULL);
		kudu_menu_add_separator();
		kudu_menu_new_radio_group();
		KM_RADIO("Show All Bone Names", KM_PROGRAM_BONES_SHOW_NAMES, NULL);
		KM_RADIO("Show Selected Bone Names", KM_PROGRAM_BONES_SHOW_SELECTED_NAMES, NULL);
		KM_RADIO("Hide Bone Names", KM_PROGRAM_BONES_HIDE_NAMES, NULL);
		kudu_menu_add_separator();
		kudu_menu_new_radio_group();
		KM_RADIO("Show All Bone Axes", KM_PROGRAM_BONES_SHOW_AXES, NULL);
		KM_RADIO("Show Selected Bone Axes", KM_PROGRAM_BONES_SHOW_SELECTED_AXES, NULL);
		KM_RADIO("Hide Bone Axes", KM_PROGRAM_BONES_HIDE_AXES, NULL);
		kudu_menu_pop();

		kudu_menu_push("Skin Detail");
		KM_CHECK("Show Skin", KM_PROGRAM_SKIN_SHOW, NULL);
		KM_CHECK("Smooth Shading", KM_PROGRAM_SKIN_SMOOTH, NULL);
		KM_CHECK("Lighting", KM_PROGRAM_SKIN_LIT, NULL);
		KM_CHECK("Real Colours", KM_PROGRAM_SKIN_REAL_COLOURS, NULL);
		KM_CHECK("Textured", KM_PROGRAM_SKIN_TEXTURED, NULL);
		kudu_menu_add_separator();
		kudu_menu_new_radio_group();
		KM_RADIO("Vertex Skin", KM_PROGRAM_SKIN_VERTEX, NULL);
		KM_RADIO("Edge Skin", KM_PROGRAM_SKIN_EDGE, NULL);
		KM_RADIO("Face Skin", KM_PROGRAM_SKIN_FACE, NULL);
		kudu_menu_pop();
	kudu_menu_pop();


	/* Help menu */
	kudu_menu_push("_Help");
	KM_ITEM("About Kudu", KM_HELP_ABOUT, NULL);
	kudu_menu_pop();

	kudu_menu_end();

	return menu_bar;
}

static GSList *objects_list = NULL;

int kudu_gui_objects_menu_add(KuduObject *object, GCallback callback)
{
	if (object == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	char kname[32];
	GtkWidget *menu = gtk_menu_item_get_submenu(GTK_MENU_ITEM(kudu_menu_get_item(KM_EDIT_OBJECT_CURRENT)));
	GtkWidget *menu_item;

	if (menu == NULL) return FALSE;

	sprintf(kname, "_object_%d", object->id);

	menu_item = gtk_radio_menu_item_new_with_label(objects_list, object->name);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);

	objects_list = gtk_radio_menu_item_get_group(GTK_RADIO_MENU_ITEM(menu_item));

	g_signal_connect(G_OBJECT(menu_item), "activate", callback, object);

	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(menu_item), TRUE);

	gtk_widget_show(menu_item);
	
	STACK_ADD(kname, menu_item);

	return TRUE;
}

int kudu_gui_objects_menu_remove(KuduObject *object)
{
	if (object == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	char kname[32];
	unsigned int len;
	GtkWidget *menu_item = NULL, *twidget;
	GSList *list, *tlist;

	sprintf(kname, "_object_%d", object->id);

	menu_item = STACK_GET_WIDGET(kname);

	if (objects_list != NULL) {
		if (objects_list->data == (gpointer)menu_item) {
			objects_list = objects_list->next;
		}
	}

	if (menu_item != NULL) gtk_widget_destroy(menu_item);
	STACK_DESTROY(kname);

	return TRUE;
}

int kudu_gui_objects_menu_select(KuduObject *object)
{
	if (object == NULL) return FALSE;

	char kname[32];
	unsigned int len;
	GtkWidget *menu_item = NULL, *twidget;
	GSList *list, *tlist;

	sprintf(kname, "_object_%d", object->id);

	menu_item = STACK_GET_WIDGET(kname);

	if (menu_item == NULL) return FALSE;

	if (!gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(menu_item)))
		gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(menu_item), TRUE);

	return TRUE;
}


