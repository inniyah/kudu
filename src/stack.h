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
#ifndef STACK_H_INCLUDED
#define STACK_H_INCLUDED

#include "globals.h"

#define STACK_ADD(name, data) (kudu_stack_list_add_item(name, data))

/* Gtk Widget operation macros */
#define STACK_GET(name) (kudu_stack_list_get_item(name))
#define STACK_DESTROY(name) (kudu_stack_list_item_destroy(name))
#define STACK_GET_WIDGET(name) (GTK_WIDGET(kudu_stack_list_get_item(name)))
#define STACK_HIDE_WIDGET(name) (kudu_stack_list_widget_hide(name))
#define STACK_SHOW_WIDGET(name) (kudu_stack_list_widget_show(name))
#define STACK_CHECK_ACTIVE(name, c) (kudu_stack_list_widget_check_menu_item_active(name, c))
#define STACK_WIDGET_ACTIVATE(name) (gtk_widget_activate(STACK_GET_WIDGET(name)))
#define STACK_MENU_SET_LABEL(name, label) (gtk_label_set_text(GTK_LABEL(gtk_bin_get_child(GTK_BIN(STACK_GET_WIDGET(name)))), label))
#define STACK_WIDGET_DISABLE(name) (gtk_widget_set_sensitive(STACK_GET_WIDGET(name), FALSE))
#define STACK_WIDGET_ENABLE(name) (gtk_widget_set_sensitive(STACK_GET_WIDGET(name), TRUE))

typedef struct _K_STACK_ITEM {
	char *name;
	void *data;
	struct _K_STACK_ITEM *next_item;
	struct _K_STACK_ITEM *previous_item;
	} K_STACK_ITEM;

typedef struct _K_STACK_LIST {
	int num_items;
	K_STACK_ITEM *first_item;
	} K_STACK_LIST;

int kudu_stack_list_ready(void);
int kudu_stack_list_init(void);
K_STACK_ITEM *kudu_stack_item_new(K_STACK_ITEM*, char*);
K_STACK_ITEM *kudu_stack_list_find_item(char*);
int kudu_stack_list_item_exist(char*);
int kudu_stack_list_add_item(char*, void*);
int kudu_stack_list_set_item(char*, void*);
void *kudu_stack_list_get_item(char*);
int kudu_stack_list_item_destroy(char*);
int kudu_stack_list_widget_show(char*);
int kudu_stack_list_widget_hide(char*);
int kudu_stack_list_widget_check_menu_item_active(char*, int);


#endif /* STACK_H_INCLUDED */
