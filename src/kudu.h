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
#ifndef KUDU_H_INCLUDED
#define KUDU_H_INCLUDED

#include "globals.h"
#include "camera.h"
#include "mathematics.h"
#include "graphics.h"
#include "font.h"
#include "joints.h"
#include "bones.h"
#include "utils.h"
#include "vertices.h"
#include "rwxobject.h"
#include "objobject.h"
#include "wingedge.h"
#include "kudu_menus.h"
#include "object.h"
#include "materials.h"
#include "gui.h"
#include "selection.h"
#include "gui_materials.h"
#include "gui_vertex.h"
#include "stack.h"
#include "options.h"
#include "gui_bone.h"
#include "image.h"
#include "bones_edit.h"
#include "skin_edit.h"
#include "gui_object.h"
#include "frames.h"
#include "sequence.h"
#include "toolbar.h"
#include "gui_scripts.h"
#include "scripts.h"
#include "image.h"
#include "texture.h"
#include "joints_edit.h"
#include "uiplugin.h"
#include "about.h"
#include "gui_options.h"



/* Function Declarations */
void kudu_program_init_objects(void);
int kudu_program_view_point_set(void);
int kudu_program_set_sub_mode(int);
int kudu_program_bone_mode_set(int);
static gboolean display(GtkWidget*, GdkEventExpose*, gpointer);
gint kudu_program_exit(GtkWidget*, gpointer);
void kudu_program_request_save_kudu_object(void);
static void main_menu_action(GtkWidget*, gpointer);
void kudu_program_edit_mode_set(int);
int kudu_program_selection_mode_set(int, int);
int kudu_program_skin_list_regen(void);
int kudu_program_skeleton_list_regen(void);
int kudu_program_skeleton_calculate(void);
int kudu_program_update_skin(void);
int kudu_program_objects_update(void);

int kudu_graphics_init_bone_lists(void);
int kudu_draw_mouse_rectangle(int sx, int sy, int ex, int ey);
int kudu_about_dialog_run(void);
int kudu_gui_object_edit(KuduObject *object);
int kudu_gui_vertex_properties_reset(KuduSelectionList *selected_items);
int kudu_gui_show_cursor(void);
int kudu_camera_ortho(KuduCamera *camera, int on);
int kudu_menu_item_set_visible(K_MainMenu item, int visible);
int kudu_stack_list_destroy(void);

#endif /* KUDU_H_INCLUDED */


