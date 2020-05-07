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
#ifndef KUDU_MENUS_H_INCLUDED
#define KUDU_MENUS_H_INCLUDED

#include "globals.h"
#include "menus.h"
#include "stack.h"

typedef enum {
	/* File menu */
	KM_FILE_NEW,
	KM_FILE_OPEN,
	KM_FILE_SAVE,
	KM_FILE_SAVE_AS,
	KM_FILE_CLOSE,
	KM_FILE_EXIT,
	KM_FILE_IMPORT,
	KM_FILE_IMPORT_RWX,
	KM_FILE_IMPORT_OBJ,

	/* Edit menu */
	KM_EDIT,
	KM_EDIT_BONES,
	KM_EDIT_BONES_NEW_ROOT,
	KM_EDIT_BONES_SEP1,
	KM_EDIT_BONES_ADD_CHILD,
	KM_EDIT_BONES_INSERT_PARENT,
	KM_EDIT_BONES_DELETE,
	KM_EDIT_BONES_SEP2,
	/*KM_EDIT_BONES_HANGLE,
	KM_EDIT_BONES_VANGLE,*/
	KM_EDIT_BONES_ROTATE,
	KM_EDIT_BONES_STRETCH,
	KM_EDIT_BONES_ROT_LX,
	KM_EDIT_BONES_ROT_LY,
	KM_EDIT_BONES_ROT_LZ,
	KM_EDIT_BONES_ROT_GX,
	KM_EDIT_BONES_ROT_GY,
	KM_EDIT_BONES_ROT_GZ,
	KM_EDIT_BONES_MOVE_X,
	KM_EDIT_BONES_MOVE_Y,
	KM_EDIT_BONES_MOVE_Z,
	KM_EDIT_BONES_PROPERTIES,

	KM_EDIT_JOINTS,
	KM_EDIT_JOINTS_MOVE_X,
	KM_EDIT_JOINTS_MOVE_Y,
	KM_EDIT_JOINTS_MOVE_Z,

	KM_EDIT_ATTACHMENTS,
	KM_EDIT_ATTACHMENTS_ATTACH,
	KM_EDIT_ATTACHMENTS_CLEAR,

	KM_EDIT_SKIN,
	KM_EDIT_SKIN_MOVE_X,
	KM_EDIT_SKIN_MOVE_Y,
	KM_EDIT_SKIN_MOVE_Z,
	KM_EDIT_SKIN_MATERIAL,
	KM_EDIT_SKIN_EDGE_HARDNESS,
	KM_EDIT_SKIN_EDGE_HARDNESS_HARD,
	KM_EDIT_SKIN_EDGE_HARDNESS_SOFT,

	KM_EDIT_OBJECT,
	KM_EDIT_OBJECT_PROPERTIES,
	KM_EDIT_OBJECT_MOVE_X,
	KM_EDIT_OBJECT_MOVE_Y,
	KM_EDIT_OBJECT_MOVE_Z,
	KM_EDIT_OBJECT_SEP1,
	KM_EDIT_OBJECT_CURRENT,

	KM_EDIT_MATERIALS,
	KM_EDIT_SCRIPTS,
	KM_EDIT_PREFERENCES,

	/* Frames menu */
	KM_FRAMES,
	KM_FRAMES_KEY_ALL_WORLD,
	KM_FRAMES_KEY_ALL_OBJECT,
	KM_FRAMES_KEY_BONES_WORLD,
	KM_FRAMES_KEY_BONES_OBJECT,
	KM_FRAMES_KEY_BONES_SELECTED,
	KM_FRAMES_KEY_VERTICES_WORLD,
	KM_FRAMES_KEY_VERTICES_OBJECT,
	KM_FRAMES_KEY_VERTICES_SELECTED,
	KM_FRAMES_KEY_POSITIONS_WORLD,
	KM_FRAMES_KEY_POSITIONS_OBJECT,
	KM_FRAMES_INCR_1F,
	KM_FRAMES_INCR_1S,
	KM_FRAMES_INCR_1M,
	KM_FRAMES_INCR_NOND_1F,
	KM_FRAMES_INCR_NOND_1S,
	KM_FRAMES_INCR_NOND_1M,
	KM_FRAMES_DECR_1F,
	KM_FRAMES_DECR_1S,
	KM_FRAMES_DECR_1M,
	KM_FRAMES_DECR_NOND_1F,
	KM_FRAMES_DECR_NOND_1S,
	KM_FRAMES_DECR_NOND_1M,

	/* Select menu */
	KM_SELECT,
	KM_SELECT_BONES_NEXT,
	KM_SELECT_BONES_PREVIOUS,
	KM_SELECT_BONES_FIRST_CHILD,
	KM_SELECT_BONES_ALL_CHILDREN,
	KM_SELECT_BONES_PARENT,
	KM_SELECT_ALL,
	KM_SELECT_NONE,
	KM_SELECT_INVERSE,

	/* View menu */
	KM_VIEW_LOOK_XM,
	KM_VIEW_LOOK_XP,
	KM_VIEW_LOOK_ZM,
	KM_VIEW_LOOK_ZP,
	KM_VIEW_LOOK_YM,
	KM_VIEW_LOOK_YP,
	KM_VIEW_FOCUS_ORIGIN,
	KM_VIEW_FOCUS_SELECTION,
	KM_VIEW_ORTHOGRAPHIC,

	KM_VIEW_BONES_SHOW,
	KM_VIEW_BONES_SHOW_JOINTS,
	KM_VIEW_BONES_SHOW_NAMES,
	KM_VIEW_BONES_SHOW_SELECTED_NAMES,
	KM_VIEW_BONES_HIDE_NAMES,
	KM_VIEW_BONES_SHOW_AXES,
	KM_VIEW_BONES_SHOW_SELECTED_AXES,
	KM_VIEW_BONES_HIDE_AXES,
	KM_VIEW_SKIN_SHOW,
	KM_VIEW_SKIN_SMOOTH,
	KM_VIEW_SKIN_LIT,
	KM_VIEW_SKIN_REAL_COLOURS,
	KM_VIEW_SKIN_TEXTURED,
	KM_VIEW_SKIN_VERTEX,
	KM_VIEW_SKIN_EDGE,
	KM_VIEW_SKIN_FACE,


	/* Playback menu */
	KM_PLAYBACK,
	KM_PLAYBACK_PLAY,
	KM_PLAYBACK_PAUSE,
	KM_PLAYBACK_STOP,
	KM_PLAYBACK_LOOP,
	KM_PLAYBACK_USE_CAMERA,

	/* Program menu */
	KM_PROGRAM_MODE_EDIT,
	KM_PROGRAM_MODE_ANIMATION,
	KM_PROGRAM_SUB_MODE_1,
	KM_PROGRAM_SUB_MODE_2,
	KM_PROGRAM_SUB_MODE_3,
	KM_PROGRAM_SUB_MODE_4,


	/* Help menu */
	KM_HELP_ABOUT,


	/* Total number of menu items */
	KM_NUM_ITEMS,
} K_MainMenu;

typedef struct _KuduMenu {
	GtkWidget *menu_item;
	gulong signal;
} KuduMenu;

GtkWidget *kudu_menu_get_submenu(K_MainMenu);
int kudu_menu_item_set_label(K_MainMenu, char*);
int kudu_menu_item_activate_no_call(K_MainMenu);
int kudu_menu_item_activate(K_MainMenu);
int kudu_menu_check_item_set_state(K_MainMenu, int);
int kudu_menu_check_item_set_state_no_call(K_MainMenu, int);

GtkWidget *kudu_gui_main_menu_bar_build(GtkWindow*, GtkAccelGroup*, GCallback);
int kudu_gui_objects_menu_add(KuduObject*, GCallback);
int kudu_gui_objects_menu_remove(KuduObject*);
int kudu_gui_objects_menu_select(KuduObject*);

#endif	/* KUDU_MENUS_H_INCLUDED */
