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
#ifndef GUI_OPTIONS_H_INCLUDED
#define GUI_OPTIONS_H_INCLUDED

#include "globals.h"
#include "options.h"

typedef enum {
	NAVI_PROFILE_MIRAI,
	NAVI_PROFILE_NENDO,
	NAVI_PROFILE_MAYA,
	NAVI_PROFILE_3DS_MAX,
	NAVI_PROFILE_BLENDER,
	NAVI_PROFILE_MOTIONBUILDER,
	NAVI_NUM_PROFILES,
	NAVI_PROFILE_CUSTOM,
} K_NavProfiles;

void kudu_gui_options_checkbox_callback(GtkWidget*, gpointer);
GtkWidget *kudu_gui_options_new_checkbox(char*, KuduOption, int);

void kudu_gui_options_navigation_dialog(GtkWidget*, gpointer);
void kudu_gui_options_dialog_show(void);

#endif /* GUI_OPTIONS_H_INCLUDED */

