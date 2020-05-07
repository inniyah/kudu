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
#ifndef GUI_H_INCLUDED
#define GUI_H_INCLUDED

#include "globals.h"
#include "options.h"
#include "stack.h"

int kudu_gui_warp_pointer(GtkWidget *, gint, gint);
int kudu_gui_center_pointer(void);
int kudu_gui_save_pointer(void);
int kudu_gui_restore_pointer(void);
GdkCursor *kudu_gui_blank_cursor(void);
int kudu_gui_hide_cursor(void);
GtkWidget *kudu_gui_open_dialog_callback(char*, char*, char*, GCallback);
GtkWidget *kudu_gui_save_dialog_callback(char*, char*, char*, char*, GCallback);
int kudu_gui_message(char*, int);
int kudu_gui_confirm(char*, int);
GtkWidget *kudu_gui_open_dialog(char*, char*, char*);
GtkWidget *kudu_gui_save_dialog(char*, char*, char*, char*);
int kudu_gui_toolbar_add_separator(GtkWidget*, int, int, int);
GtkToolItem *kudu_gui_toolbar_add_radio_button(GtkWidget*, int, int, char*, char*);

#endif /* GUI_H_INCLUDED */
