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
#ifndef MENUS_H_INCLUDED
#define MENUS_H_INLCUDED

#include "globals.h"
#include "stack.h"

/*#define MENU_ALL 15
#define MENU1 1
#define MENU2 2
#define MENU3 4
#define MENU4 8*/

#define NEW_ITEM(shell, label, value, key, mods) (kudu_gui_menu_item_new(shell, label, value, key, mods))
#define NEW_CHECK(shell, label, value, key, mods) (kudu_gui_check_menu_item_new(shell, label, value, key, mods))
#define NEW_RADIO(g, shell, label, value, key, mods) (kudu_gui_radio_menu_item_new(g, shell, label, value, key, mods))
#define NEW_SEPARATOR(menu) (kudu_gui_menu_separator_new(menu))
#define NEW_SUB(menu, label) (kudu_gui_sub_menu_new(menu, label))
#define NEW_TEAROFF(menu) (kudu_gui_menu_tearoff_new(menu))
#define NEW_MENU(bar, label) (kudu_gui_menu_new(bar, label))

gulong kudu_menu_last_signal_id(void);
int kudu_menu_start_new(GtkAccelGroup*, GtkWidget*, char*, GCallback);
GtkWidget *kudu_menu_end(void);
GtkWidget *kudu_menu_push(char*);
int kudu_menu_push_fake(char*);
int kudu_menu_pop(void);
int kudu_menu_set_accelerator(GtkWidget*, char*);
GtkWidget *kudu_menu_add_item(char*, int, char*);
GtkWidget *kudu_menu_add_check(char*, int, char*);
int kudu_menu_new_radio_group(void);
GtkWidget *kudu_menu_add_radio(char*, int, char*);
GtkWidget *kudu_menu_add_tearoff(void);
GtkWidget *kudu_menu_add_separator(void);


GSList *kudu_gui_menu_get_last_radio_item_group(void);
int kudu_gui_menu_set_callback(GCallback);
int kudu_gui_menu_set_accel_group(GtkAccelGroup*);
GtkWidget *kudu_gui_menu_item_new(GtkWidget*, char*, int, guint,	GdkModifierType);
GtkWidget *kudu_gui_check_menu_item_new(GtkWidget*, char*, int, guint, GdkModifierType);
GtkWidget *kudu_gui_radio_menu_item_new(int, GtkWidget*, char*, int, guint, GdkModifierType);
GtkSeparatorMenuItem *kudu_gui_menu_separator_new(GtkWidget*);
GtkTearoffMenuItem *kudu_gui_menu_tearoff_new(GtkWidget*);
GtkWidget *kudu_gui_menu_new(GtkWidget*, char*);
GtkWidget *kudu_gui_sub_menu_new(GtkWidget*, char*);
GtkWidget *kudu_gui_sub_menu_get_item(void);


#endif /* MENUS_H_INCLUDED */
