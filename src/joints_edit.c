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
#include "joints_edit.h"

int kudu_joints_edit_selection(KuduSelectionList *selected_joints_list, float opt_h, float opt_v, int joint_mode)
{
	if (selected_joints_list == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	if (joint_mode == JOINT_MODE_FIXED) return FALSE;

	KuduJoint *current_joint, *joint;
	KuduBone *bone, *bone_b, *bone_a, *pbone;
	float h, v, x, y, z, dp, tquat[4];

	kudu_selection_list_for_each_do(selected_joints_list);

	while ((current_joint = (KuduJoint*)kudu_selection_list_next_do()) != NULL) {
		bone = current_joint->bone;

		if (bone->s_joint == current_joint) {
			bone_b = bone->parent;
			bone_a = bone;
		} else {
			bone_b = bone;
			bone_a = bone->first_child;
		}

		if ((bone->s_joint == current_joint) && (bone->parent != NULL)) bone = bone->parent;
		if (bone->e_joint == current_joint) joint = bone->s_joint;
		else	joint = NULL;

		switch (joint_mode) {
			case JOINT_MODE_MOVEX:
				current_joint->pos[0] = current_joint->temp + (opt_h / 10);
				break;
			case JOINT_MODE_MOVEY:
				current_joint->pos[1] = current_joint->temp + (opt_h / 10);
				break;
			case JOINT_MODE_MOVEZ:
				current_joint->pos[2] = current_joint->temp + (opt_h / 10);
				break;
		}

		/*if (bone_a != NULL) {
			kudu_math_quat_set_identity(bone_a->lquat);
			kudu_math_quat_aim(bone_a->gquat, current_joint->pos, bone_a->e_joint->pos);
			kudu_bone_magic_touch(bone);
		}*/

		if (!kudu_bone_both_joints_selected(bone)) {
			/*kudu_bone_calculate_inverse(bone);*/
			/*kudu_math_angles_between_v(current_joint->pos, joint->pos, &h, &v);

			kudu_math_quat_to_euler(bone->quat, &x, &y, &z);

			kudu_bone_apply_rotation(bone, (h-x), 3, FALSE);
			kudu_bone_apply_rotation(bone, (v-y), 4, FALSE);*/
			//kudu_math_quat_set_identity(bone->lquat);
			kudu_math_quat_aim(bone->lquat, joint->pos, current_joint->pos);

			/*dp = kudu_math_vector_dot_product(joint->pos, current_joint->pos);

			printf("dp: %4.8f   as: %4.4f\n", dp, (float)acos((double)dp));*/

			//kudu_math_quat_multiply(bone->gquat, tquat);

			/*if (bone->parent != NULL) {
				pbone = bone->parent;
				kudu_math_quat_copy(tquat, pbone->quat);
				kudu_math_quat_invert(tquat);
				kudu_math_quat_multiply(tquat, bone->gquat);
				kudu_math_quat_copy(bone->gquat, tquat);
			}*/

			bone->length = kudu_math_distance_between(current_joint->pos, joint->pos);

			kudu_bone_magic_touch(bone);


			/*bone = bone->first_child;

			kudu_math_quat_set_identity(bone->lquat);
			kudu_math_quat_aim(bone->gquat, current_joint->pos, bone->e_joint->pos);
			bone->length = kudu_math_distance_between(current_joint->pos, bone->e_joint->pos);*/
		}
		/*if (joint != NULL) {
			if (joint_mode != JOINT_MODE_MOVEY) bone->hAngle = h;
			bone->vAngle = v;
			kudu_bone_magic_touch(bone);
		}*/
	}

	return TRUE;
}

int kudu_joints_edit_anchor(KuduSelectionList *selection_list, int joint_mode)
{
	if (selection_list == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	KuduJoint *joint;

	if (!kudu_selection_list_for_each_do(selection_list)) return TRUE;

	while ((joint = (KuduJoint*)kudu_selection_list_next_do()) != NULL) {
		switch (joint_mode) {
			case JOINT_MODE_MOVEX:
				joint->temp = joint->pos[0];
				break;
			case JOINT_MODE_MOVEY:
				joint->temp = joint->pos[1];
				break;
			case JOINT_MODE_MOVEZ:
				joint->temp = joint->pos[2];
				break;
		}		
	}

	return TRUE;
}

int kudu_joints_edit_unanchor(KuduSelectionList *selection_list, int joint_mode)
{
	if (selection_list == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	KuduJoint *joint;

	if (!kudu_selection_list_for_each_do(selection_list)) return TRUE;

	while ((joint = (KuduJoint*)kudu_selection_list_next_do()) != NULL) {
		switch (joint_mode) {
			case JOINT_MODE_MOVEX:
				joint->pos[0] = joint->temp;
				break;
			case JOINT_MODE_MOVEY:
				joint->pos[1] = joint->temp;
				break;
			case JOINT_MODE_MOVEZ:
				joint->pos[2] = joint->temp;
				break;
		}		
	}

	return TRUE;
}




