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
#ifndef SCRIPTS_H_INCLUDED
#define SCRIPTS_H_INCLUDED

#include "globals.h"
#include "objectsmodule.h"
#include "guimodule.h"
#include "kudu_menus.h"

KuduScript *kudu_script_new(KuduScript*);
int kudu_script_set_filename(KuduScript*, char*);
int kudu_script_set_author(KuduScript*, char*);
int kudu_script_set_name(KuduScript*, char*);
int kudu_script_set_menu(KuduScript*, char*);

int kudu_script_process(char*, char*);
int kudu_script_list_init(void);
int kudu_script_list_for_each_do(void);
KuduScript *kudu_script_list_next_do(void);

void kudu_script_init_objects(KuduObject**, KuduFrameList**, KuduSelectionList**, KuduSelectionList**);
int kudu_script_run(KuduScript*);

#endif /* SCRIPTS_H_INCLUDED */
