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
#ifndef BONES_H_INCLUDED
#define BONES_H_INCLUDED

#include "globals.h"
#include "utils.h"
#include "mathematics.h"
#include "joints.h"

#define NOKE_NULL -1
#define NOKE_ROOT 0
#define BONE_NULL -1
#define BONE_ROOT 0

#define CO_CALCULATE_ALL 1
#define CO_CALCULATE_MATRICES 2
#define CO_CALCULATE_XYZ 4
#define CO_CALCULATE_PS 8

#define SKELETON_CALCULATE_ALL 1
#define SKELETON_CALCULATE_MATRICES 2
#define SKELETON_CALCULATE_POSITIONS 4
#define SKELETON_CALCULATE_PS 8

int kudu_bone_force_next_id(KuduObject*, int);
int kudu_bone_id_counter_reset(KuduObject*);
int kudu_bone_next_id(KuduObject*);
int kudu_bone_is_parent(KuduBone*);
KuduBone *kudu_bone_new(KuduObject*);
KuduBone *kudu_bone_new_with_id(KuduObject*, int);
int kudu_bone_set_joints(KuduBone*, KuduJoint*, KuduJoint*);
KuduBone *kudu_bone_add_child(KuduBone*);
KuduBone *kudu_bone_add_parent(KuduBone*);
KuduBone *kudu_bone_add_sibling(KuduBone*);
int kudu_bone_insert_parent(KuduBone*, KuduBone*);
int kudu_bone_adopt_child(KuduBone*, KuduBone*);
int kudu_bone_adopt_sibling(KuduBone*, KuduBone*);
int kudu_bone_unlink(KuduBone*);
KuduBone *kudu_bone_find_child_with_id(KuduBone*, int);
KuduBone *kudu_bone_find_with_id(KuduBone*, int);
int kudu_bone_copy_values(KuduBone*, KuduBone*);
int kudu_bone_destroy(KuduBone*);
int kudu_bone_destroy_with_children(KuduBone*);
int kudu_bone_update(KuduBone*);
int kudu_bone_update_all(KuduBone*);
int kudu_bone_smart_update(KuduBone*);
int kudu_bone_smart_update_all(KuduBone*);
int kudu_bone_magic_touch(KuduBone*);
int kudu_bone_magic_update(KuduBone*);
int kudu_bone_magic_update_all(KuduBone*);

int kudu_bone_num_children(KuduBone*);

int kudu_bone_both_joints_selected(KuduBone*);
void kudu_bone_calculate_pos_end(KuduBone*);
int kudu_bone_calculate_inverse(KuduBone*);
int kudu_bone_joint_by_percent(KuduBone*, KuduBone*, unsigned char);
void kudu_bone_calculate_matrix(KuduBone*, int);
int kudu_bone_apply_rotation(KuduBone*, float, int, int);

#endif /* BONES_H_INCLUDED */
