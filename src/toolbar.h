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
#ifndef TOOLBAR_H_INCLUDED
#define TOOLBAR_H_INCLUDED

#include "globals.h"
#include "options.h"

typedef enum {
	KT_FILE_OPEN,
	KT_FILE_SAVE,

	KT_SELECT_OBJECTS,
	KT_SELECT_BONES,
	KT_SELECT_JOINTS,
	KT_SELECT_VERTICES,
	KT_SELECT_EDGES,
	KT_SELECT_FACES,
	KT_SELECT_SHAPES,

	KT_NUM_ITEMS,
} K_MainToolbar;

typedef struct _KuduToolbarItem {
	GtkToolItem *tool_item;
	gulong signal;
} KuduToolbarItem;



gulong kudu_toolbar_last_signal_id(void);
int kudu_toolbar_set_path(char *);
int kudu_toolbar_start_new(char*, GtkAccelGroup*, GCallback);
GtkWidget *kudu_toolbar_end(void);
int kudu_toolbar_item_set_accelerator(GtkToolItem*, char*);
void kudu_toolbar_new_radio_group(void);
GtkToolItem *kudu_toolbar_add_radio_button(unsigned int, char*, char*, char*);
GtkToolItem *kudu_toolbar_add_stock_item(unsigned int, char*, char*, char*);
GtkToolItem *kudu_toolbar_add_seperator(int, int);

int kudu_toolbar_store_init(void);
int kudu_toolbar_store_item(K_MainToolbar, GtkToolItem*);
GtkToolItem *kudu_toolbar_get_item(K_MainToolbar);
int kudu_toolbar_item_disable(K_MainToolbar);
int kudu_toolbar_item_enable(K_MainToolbar);
int kudu_toolbar_item_activate(K_MainToolbar);
int kudu_toolbar_item_activate_no_call(K_MainToolbar);
int kudu_toolbar_toggle_item_set_active_no_call(K_MainToolbar, int);

GtkWidget *kudu_main_toolbar_build(GtkAccelGroup*, GCallback);


#endif /* TOOLBAR_H_INCLUDED */

