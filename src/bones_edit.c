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
#include "bones_edit.h"

int kudu_bones_edit_selection(KuduSelectionList *selected_bones_list, float opt_h, float opt_v, float hscroll, float vscroll, int bone_mode)
{
	if (selected_bones_list == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	if (bone_mode == BONE_MODE_FIXED) return FALSE;

	KuduBone *current_bone;
	KuduJoint *joint;
	int pose;

	/* Are we in editor or animator mode ? */
	/* If in animator mode set: pose = TRUE this is so that the correct quaternions get adjusted */
	if (program.mode == PROGRAM_MODE_EDIT) pose = FALSE;
	else if (program.mode == PROGRAM_MODE_ANIMATION) pose = TRUE;

	kudu_selection_list_for_each_do(selected_bones_list);

	while ((current_bone = (KuduBone*)kudu_selection_list_next_do()) != NULL) {
		switch (bone_mode) {
			case BONE_MODE_ROT_LX:
				/*current_bone->laxis_rot[0] = current_bone->temp + opt_h;
				kudu_math_degrees_clamp(&current_bone->laxis_rot[0]);*/
				current_bone->temp += hscroll;
				kudu_bone_apply_rotation(current_bone, hscroll, 0, pose);
				break;
			case BONE_MODE_ROT_LY:
				/*current_bone->laxis_rot[1] = current_bone->temp + opt_h;
				kudu_math_degrees_clamp(&current_bone->laxis_rot[1]);*/
				current_bone->temp += hscroll;
				kudu_bone_apply_rotation(current_bone, hscroll, 1, pose);
				break;
			case BONE_MODE_ROT_LZ:
				/*current_bone->laxis_rot[2] = current_bone->temp + opt_h;
				kudu_math_degrees_clamp(&current_bone->laxis_rot[2]);*/
				current_bone->temp += hscroll;
				kudu_bone_apply_rotation(current_bone, hscroll, 2, pose);
				break;
			case BONE_MODE_ROT_GX:
				current_bone->temp += hscroll;
				kudu_bone_apply_rotation(current_bone, hscroll, 3, pose);
				break;
			case BONE_MODE_ROT_GY:
				current_bone->temp += hscroll;
				kudu_bone_apply_rotation(current_bone, hscroll, 4, pose);
				break;
			case BONE_MODE_ROT_GZ:
				current_bone->temp += hscroll;
				kudu_bone_apply_rotation(current_bone, hscroll, 5, pose);
				break;
			case BONE_MODE_ROTATE:
				current_bone->rotation = current_bone->temp + opt_h;
				kudu_math_degrees_clamp(&current_bone->rotation);
				break;
			case BONE_MODE_STRETCH:
				if (!pose) {
					current_bone->length = current_bone->temp + (opt_h / 10);
					if (current_bone->length < 0.0) current_bone->length = 0.0;
				} else {
					current_bone->plength = current_bone->temp + (opt_h / 10);
					if (current_bone->plength < 0.0) current_bone->plength = 0.0;
				}
				break;
			case BONE_MODE_MOVEX:
				joint = current_bone->s_joint;
				if (!pose) joint->pos[0] = current_bone->temp + (opt_h / 10);
				else	joint->ppos[0] = current_bone->temp + (opt_h / 10);
				break;
			case BONE_MODE_MOVEY:
				joint = current_bone->s_joint;
				if (!pose) joint->pos[1] = current_bone->temp + (opt_h / 10);
				else	joint->ppos[1] = current_bone->temp + (opt_h / 10);
				break;
			case BONE_MODE_MOVEZ:
				joint = current_bone->s_joint;
				if (!pose) joint->pos[2] = current_bone->temp + (opt_h / 10);
				else	joint->ppos[2] = current_bone->temp + (opt_h / 10);
				break;
		}
	}

	return TRUE;
}

int kudu_bones_edit_anchor(KuduSelectionList *selection_list, int bone_mode)
{
	if (selection_list == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	KuduBone *bone;
	KuduJoint *joint;
	int pose;

	/* Are we in editor or animator mode ? */
	/* If in animator mode set: pose = TRUE this is so that the correct quaternions get adjusted */
	if (program.mode == PROGRAM_MODE_EDIT) pose = FALSE;
	else if (program.mode == PROGRAM_MODE_ANIMATION) pose = TRUE;

	if (!kudu_selection_list_for_each_do(selection_list)) return TRUE;

	while ((bone = (KuduBone*)kudu_selection_list_next_do()) != NULL) {
		switch (bone_mode) {
			case BONE_MODE_ROT_LX:
				/*bone->temp = bone->laxis_rot[0];*/
				bone->temp = 0.0;
				break;
			case BONE_MODE_ROT_LY:
				/*bone->temp = bone->laxis_rot[1];*/
				bone->temp = 0.0;
				break;
			case BONE_MODE_ROT_LZ:
				/*bone->temp = bone->laxis_rot[2];*/
				bone->temp = 0.0;
				break;
			case BONE_MODE_ROT_GX:
				bone->temp = 0.0;
				break;
			case BONE_MODE_ROT_GY:
				bone->temp = 0.0;
				break;
			case BONE_MODE_ROT_GZ:
				bone->temp = 0.0;
				break;
			case BONE_MODE_ROTATE:
				bone->temp = bone->rotation;
				break;
			case BONE_MODE_STRETCH:
				bone->temp = bone->length;
				break;
			case BONE_MODE_MOVEX:
				joint = bone->s_joint;
				if (!pose) bone->temp = joint->pos[0];
				else bone->temp = joint->ppos[0];
				break;
			case BONE_MODE_MOVEY:
				joint = bone->s_joint;
				if (!pose) bone->temp = joint->pos[1];
				else bone->temp = joint->ppos[1];
				break;
			case BONE_MODE_MOVEZ:
				joint = bone->s_joint;
				if (!pose) bone->temp = joint->pos[2];
				else bone->temp = joint->ppos[2];
				break;
		}		
	}

	return TRUE;
}

int kudu_bones_edit_unanchor(KuduSelectionList *selection_list, int bone_mode)
{
	if (selection_list == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	KuduBone *bone;
	KuduJoint *joint;
	int pose;

	/* Are we in editor or animator mode ? */
	/* If in animator mode set: pose = TRUE this is so that the correct quaternions get adjusted */
	if (program.mode == PROGRAM_MODE_EDIT) pose = FALSE;
	else if (program.mode == PROGRAM_MODE_ANIMATION) pose = TRUE;

	if (!kudu_selection_list_for_each_do(selection_list)) return TRUE;

	while ((bone = (KuduBone*)kudu_selection_list_next_do()) != NULL) {
		switch (bone_mode) {
			case BONE_MODE_ROT_LX:
				/*bone->laxis_rot[0] = bone->temp;*/
				kudu_bone_apply_rotation(bone, -bone->temp, 0, pose);
				break;
			case BONE_MODE_ROT_LY:
				/*bone->laxis_rot[1] = bone->temp;*/
				kudu_bone_apply_rotation(bone, -bone->temp, 1, pose);
				break;
			case BONE_MODE_ROT_LZ:
				/*bone->laxis_rot[2] = bone->temp;*/
				kudu_bone_apply_rotation(bone, -bone->temp, 2, pose);
				break;
			case BONE_MODE_ROT_GX:
				kudu_bone_apply_rotation(bone, -bone->temp, 3, pose);
				break;
			case BONE_MODE_ROT_GY:
				kudu_bone_apply_rotation(bone, -bone->temp, 4, pose);
				break;
			case BONE_MODE_ROT_GZ:
				kudu_bone_apply_rotation(bone, -bone->temp, 5, pose);
			case BONE_MODE_ROTATE:
				bone->rotation = bone->temp;
				break;
			case BONE_MODE_STRETCH:
				bone->length = bone->temp;
				break;
			case BONE_MODE_MOVEX:
				joint = bone->s_joint;
				if (!pose) joint->pos[0] = bone->temp;
				else joint->ppos[0] = bone->temp;
				break;
			case BONE_MODE_MOVEY:
				joint = bone->s_joint;
				if (!pose) joint->pos[1] = bone->temp;
				else joint->ppos[1] = bone->temp;
				break;
			case BONE_MODE_MOVEZ:
				joint = bone->s_joint;
				if (!pose) joint->pos[2] = bone->temp;
				else joint->ppos[2] = bone->temp;
				break;
		}		
	}

	return TRUE;
}


int kudu_bones_edit_function(KuduSelectionList *selection_list, BE_Function func, int oper, KuduObject *object)
{
	if ((selection_list == NULL) || (object == NULL)) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}
	if (selection_list->first_item == NULL) return TRUE;

	KuduSelectionList *temp_list;
	KuduSelection *item;
	KuduBone *current_bone, *new_bone;
	int keep = (oper & SL_KEEP_CURRENT), select_new = (oper & SL_SELECT_NEW);

	temp_list = kudu_selection_list_new();
	if (temp_list == NULL) return FALSE;

	kudu_selection_list_for_each_do(selection_list);

	while ((current_bone = (KuduBone*)kudu_selection_list_next_do()) != NULL) {
		switch (func) {
			case ADD_CHILD:
				new_bone = kudu_bone_add_child(current_bone);
				if (select_new) {
					new_bone->selected = TRUE;
					kudu_selection_list_add_item(temp_list, new_bone);
				}
				kudu_bone_update(new_bone);

				if (keep) kudu_selection_list_add_item(temp_list, current_bone);
				else	current_bone->selected = FALSE;
				break;

			case ADD_PARENT:
				if (current_bone->id != BONE_ROOT) {
					new_bone = kudu_bone_add_parent(current_bone);
					if (select_new) {
						new_bone->selected = TRUE;
						kudu_selection_list_add_item(temp_list, new_bone);
					}
					if (current_bone == object->bone) object->bone = new_bone;
					kudu_bone_update(new_bone);
				}

				if (keep) kudu_selection_list_add_item(temp_list, current_bone);
				else	current_bone->selected = FALSE;
				break;

			case DELETE:
				new_bone = current_bone->parent;
				if (current_bone == object->bone) {
					if (current_bone->first_child != NULL) object->bone = current_bone->first_child;
					else	object->bone = current_bone->next_sibling;
				}

				if (current_bone->parent == NULL) {
					if (current_bone->first_child != NULL) new_bone = current_bone->first_child;
					if ((new_bone != NULL) && (new_bone->previous_sibling == NULL) && (new_bone->next_sibling == NULL)) {
					}
				}
				kudu_bone_destroy(current_bone);
				if (new_bone != NULL) kudu_bone_update(new_bone);
				break;
		}
	}

	item = temp_list->first_item;
	temp_list->first_item = selection_list->first_item;
	selection_list->first_item = item;
	selection_list->selected_items = temp_list->selected_items;

	kudu_selection_list_destroy(temp_list);

	return TRUE;
}


