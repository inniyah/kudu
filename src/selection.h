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
#ifndef SELECTION_H_INCLUDED
#define SELECTION_H_INCLUDED

#include "globals.h"
#include "vertices.h"
#include "wingedge.h"
#include "object.h"

#define SL_KEEP_CURRENT 1
#define SL_SELECT_NEW 2

#define HL_DEFAULT_NUM 2048;
#define HL_DEFAULT_INC 128;

typedef struct {
	void *data;
} KuduHit;

typedef struct {
	unsigned int cpos;
	unsigned int num_records;
	KuduHit *record;
} KuduHitsList;

int kudu_hits_list_init(void);
unsigned int kudu_hits_list_push_item(void*);
void *kudu_hits_list_get_item(unsigned int);
int kudu_hits_list_clear(void);
int kudu_hits_list_destroy(void);


KuduSelection *kudu_selection_new(KuduSelection*);
int kudu_selection_destroy(KuduSelection*);
KuduSelectionList *kudu_selection_list_new(void);
KuduSelectionList *kudu_selection_list_new_from_copy(KuduSelectionList*);
int kudu_selection_list_destroy(KuduSelectionList*);
int kudu_selection_list_set_type(KuduSelectionList*, K_SELECTION_TYPE, int);
int kudu_selection_list_add_item(KuduSelectionList*, void*);
int kudu_selection_list_item_added(KuduSelectionList*, void*);
int kudu_selection_list_remove_item(KuduSelectionList*, void*);
int kudu_selection_list_remove_all_items(KuduSelectionList*);
int kudu_selection_list_for_each_do(KuduSelectionList*);
void *kudu_selection_list_next_do(void);
void *kudu_selection_list_get_first_item(KuduSelectionList*);
int kudu_selection_list_anything_selected(KuduSelectionList*);
int kudu_selection_list_get_center_point(KuduSelectionList*, float*);
int kudu_selection_list_select(KuduSelectionList*, KuduObject*, int, K_SELECTION_TYPE);
int kudu_selection_list_select_bones(KuduSelectionList*, KuduBone*, K_SELECTION_TYPE);
int kudu_selection_list_select_joints(KuduSelectionList*, KuduJoint*, K_SELECTION_TYPE);
int kudu_selection_list_pick_bone(KuduSelectionList*, KuduBone*, int, unsigned int);
int kudu_selection_list_pick_joint(KuduSelectionList*, KuduJoint*, int, unsigned int);
void kudu_selection_set_object_selector(int(*)(KuduObject*));
int kudu_selection_list_draw_info(KuduSelectionList*, int, int);
int kudu_selection_list_bones_are_parents(KuduSelectionList*);
int kudu_selection_list_bones_select_more(KuduSelectionList*, K_SELECTION_TYPE, int);
int kudu_selection_hits_process(int, int, unsigned int, KuduObject*, KuduSelectionList*, KuduSelectionList*);

#endif /* SELECTION_H_INCLUDED */
