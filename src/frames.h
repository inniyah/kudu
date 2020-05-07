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
#ifndef FRAMES_H_INCLUDED
#define FRAMES_H_INCLUDED

#include "globals.h"
#include "object.h"
#include "bones.h"

KuduFrame *kudu_frame_new(KuduFrame*, int);
int kudu_frame_destroy(KuduFrame*);
KuduItemFrame *kudu_item_frame_new(KuduItemFrame*);
int kudu_item_frame_destroy(KuduItemFrame*);
KuduObjectFrame *kudu_object_frame_new(KuduObjectFrame*);
int kudu_object_frame_destroy(KuduObjectFrame*);
KuduFrameList *kudu_frame_list_new(void);
int kudu_frame_list_destroy(KuduFrameList*);
KuduObjectFrame *kudu_frame_list_find_object_frame(KuduFrameList*, KuduObject*);
KuduObjectFrame *kudu_frame_list_find_or_add_object_frame(KuduFrameList*, KuduObject*);
KuduItemFrame *kudu_object_frame_find_item_frame(KuduObjectFrame*, void*);
KuduItemFrame *kudu_object_frame_find_or_add_item_frame(KuduObjectFrame*, void*);
KuduFrame *kudu_item_frame_find_frame(KuduItemFrame*, int);
KuduFrame *kudu_item_frame_find_or_add_frame(KuduItemFrame*, int);
int kudu_frame_list_set_frame_operation(KuduFrameList*, KuduObject*, void*, int, K_FRAME_OPERATION);
int kudu_frame_list_clear_frame(KuduFrameList*, KuduObject*, void*, int);
int kudu_frame_perform_operation(KuduItemFrame*, KuduFrame*);
int kudu_frame_perform_average_operation(KuduItemFrame*, KuduFrame*, KuduFrame*, int);
int kudu_frame_list_goto_frame(KuduFrameList*, int);

#endif /* FRAMES_H_INCLUDED */
