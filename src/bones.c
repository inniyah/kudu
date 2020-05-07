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
#include "bones.h"

int kudu_bone_force_next_id(KuduObject *object, int id)
{
	if (object == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	object->next_bone_id = id;
	return TRUE;
}

int kudu_bone_next_id(KuduObject *object)
{
	if (object == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	return object->next_bone_id;
}

int kudu_bone_id_counter_reset(KuduObject *object)
{
	if (object == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	object->next_bone_id = 1;
	return TRUE;
}

int kudu_bone_is_parent(KuduBone *bone)
{
	if (bone == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	if (bone->parent == NULL) return TRUE;

	return FALSE;
}

KuduBone *kudu_bone_new(KuduObject *object)
{
	if (object == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return NULL;
	}

	KuduBone *bone;
	int a, b;

	bone = (KuduBone*)malloc(sizeof(KuduBone));
	if (bone == NULL) {
		kudu_error(KE_NO_MEM);
		return NULL;
	}

	bone->id = object->next_bone_id++;

	for (a = 0; a < 4; a++)
		for (b = 0; b < 4; b++) bone->matrix[a][b] = 0.0;

	sprintf(bone->name, "Bone: %d", bone->id);
	bone->hAngle = 0.0;
	bone->vAngle = 0.0;
	bone->rAngle = 0.0;
	bone->length = 1.0;
	bone->posX = 0.0;
	bone->posY = 0.0;
	bone->posZ = 0.0;
	bone->selected = FALSE;
	bone->magic_touch = 0;

	bone->parent = NULL;
	bone->first_child = NULL;
	bone->next_sibling = NULL;
	bone->previous_sibling = NULL;

	return bone;
}

KuduBone *kudu_bone_new_with_id(KuduObject *object, int id)
{
	if (object == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return NULL;
	}

	KuduBone *bone;

	bone = kudu_bone_new(object);

	bone->id = id;

	if (id >= object->next_bone_id) object->next_bone_id = id+1;

	return bone;
}

void kudu_bone_calculate_pos_end(KuduBone *bone)
{
	if (bone == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return;
	}

	float x, y, z;

	x = 0.0;
	y = 0.0;
	z = bone->length;

	bone->lineX = bone->posX + (x * (bone->matrix[0][0])) +
		(y * (bone->matrix[1][0])) +
		(z * (bone->matrix[2][0])) +
		(bone->matrix[3][0]);

	bone->lineY = bone->posY + (x * (bone->matrix[0][1])) +
		(y * (bone->matrix[1][1])) +
		(z * (bone->matrix[2][1])) +
		(bone->matrix[3][1]);

	bone->lineZ = bone->posZ + (x * (bone->matrix[0][2])) +
		(y * (bone->matrix[1][2])) +
		(z * (bone->matrix[2][2])) +
		(bone->matrix[3][2]);

}

KuduBone *kudu_bone_add_child(KuduObject *object, KuduBone *parent_bone)
{
	if ((parent_bone == NULL) || (object == NULL)) {
		kudu_error(KE_OBJECT_INVALID);
		return NULL;
	}

	KuduBone *bone, *next_bone;


	bone = kudu_bone_new(object);
	if (bone == NULL) return NULL;

	bone->parent = parent_bone;

	if (parent_bone->first_child == NULL) {
		parent_bone->first_child = bone;
	} else {
		next_bone = NULL;

		do {
			if (next_bone == NULL) next_bone = parent_bone->first_child;
			else next_bone = next_bone->next_sibling;
		} while (next_bone->next_sibling != NULL);

		next_bone->next_sibling = bone;
		bone->previous_sibling = next_bone;
	}

	bone->length = parent_bone->length;

	return bone;
}

KuduBone *kudu_bone_add_parent(KuduObject *object, KuduBone *bone)
{
	if ((bone == NULL) || (object == NULL)) {
		kudu_error(KE_OBJECT_INVALID);
		return NULL;
	}

	KuduBone *parent_bone, *old_parent, *previous_sibling, *next_sibling;

	parent_bone = kudu_bone_new(object);
	if (parent_bone == NULL) return NULL;

	old_parent = bone->parent;
	previous_sibling = bone->previous_sibling;
	next_sibling = bone->next_sibling;

	parent_bone->parent = old_parent;
	parent_bone->previous_sibling = previous_sibling;
	parent_bone->next_sibling = next_sibling;
	parent_bone->first_child = bone;

	if (old_parent != NULL) {
		parent_bone->length = old_parent->length;
		/* Is this the "first_bone" of it's parent ?? */
		if (old_parent->first_child == bone) old_parent->first_child = parent_bone;
	} else { /* OK we are inserting this bone before a current root bone, so we must inherit its position */
		parent_bone->posX = bone->posX;
		parent_bone->posY = bone->posY;
		parent_bone->posZ = bone->posZ;
	}

	if (previous_sibling != NULL) previous_sibling->next_sibling = parent_bone;
	if (next_sibling != NULL) next_sibling->previous_sibling = parent_bone;

	bone->parent = parent_bone;
	bone->previous_sibling = NULL;
	bone->next_sibling = NULL;

	return parent_bone;
}

KuduBone *kudu_bone_add_sibling(KuduObject *object, KuduBone *previous_sibling)
{
	if ((previous_sibling == NULL) || (object == NULL)) {
		kudu_error(KE_OBJECT_INVALID);
		return NULL;
	}

	KuduBone *bone, *old_next_sibling;

	bone = kudu_bone_new(object);
	if (bone == NULL) return NULL;

	old_next_sibling = previous_sibling->next_sibling;

	bone->parent = previous_sibling->parent;
	bone->previous_sibling = previous_sibling;
	bone->next_sibling = old_next_sibling;
	previous_sibling->next_sibling = bone;

	if (old_next_sibling != NULL) old_next_sibling->previous_sibling = bone;

	return bone;
}

int kudu_bone_insert_parent(KuduBone *bone, KuduBone *parent_bone)
{
	if ((bone == NULL) || (parent_bone == NULL)) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	KuduBone *old_parent, *previous_sibling, *next_sibling;

	old_parent = bone->parent;
	previous_sibling = bone->previous_sibling;
	next_sibling = bone->next_sibling;

	parent_bone->parent = old_parent;
	parent_bone->previous_sibling = previous_sibling;
	parent_bone->next_sibling = next_sibling;
	parent_bone->first_child = bone;

	if (old_parent != NULL) {
		/* Is this the "first_bone" of it's parent ?? */
		if (old_parent->first_child == bone) old_parent->first_child = parent_bone;
	}

	if (previous_sibling != NULL) previous_sibling->next_sibling = parent_bone;
	if (next_sibling != NULL) next_sibling->previous_sibling = parent_bone;

	bone->parent = parent_bone;
	bone->previous_sibling = NULL;
	bone->next_sibling = NULL;

	return TRUE;
}

int kudu_bone_adopt_child(KuduBone *parent_bone, KuduBone *bone)
{
	if ((parent_bone == NULL) || (bone == NULL)) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	KuduBone *next_bone;

	bone->parent = parent_bone;

	if (parent_bone->first_child == NULL) {
		parent_bone->first_child = bone;
	} else {
		next_bone = NULL;

		do {
			if (next_bone == NULL) next_bone = parent_bone->first_child;
			else next_bone = next_bone->next_sibling;
		} while (next_bone->next_sibling != NULL);

		next_bone->next_sibling = bone;
		bone->previous_sibling = next_bone;
	}

	return TRUE;
}

int kudu_bone_adopt_sibling(KuduBone *bone, KuduBone *sibling)
{
	if ((bone == NULL) || (sibling == NULL)) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	KuduBone *old_next_sibling;

	old_next_sibling = bone->next_sibling;

	sibling->parent = bone->parent;
	sibling->previous_sibling = bone;
	sibling->next_sibling = old_next_sibling;
	bone->next_sibling = sibling;

	if (old_next_sibling != NULL) old_next_sibling->previous_sibling = sibling;

	return TRUE;
}

/* "Unlinks" a bone - removes it and all its children from a skeleton without destroying them */
/* Sort of like "cutting" a directory in a gui browser */
int kudu_bone_unlink(KuduBone *bone)
{
	if (bone == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	KuduBone *parent, *previous_sibling, *next_sibling, *first_child, *next_child, *last_child;

	parent = bone->parent;
	previous_sibling = bone->previous_sibling;
	next_sibling = bone->next_sibling;

	if (parent != NULL) {	/* This is not a root bone */
		if (parent->first_child == bone) {	/* This bone is the first child of its parent */
			/* Set this bones next_sibling to be the new first_child of this parent bone */
			parent->first_child = next_sibling;
			if (next_sibling != NULL) next_sibling->previous_sibling = NULL;
		} else {	/* This bone is not the first child of its parent */
			/* "Pull" it out from between its previous and next siblings */
			if (previous_sibling != NULL) previous_sibling->next_sibling = next_sibling;
			if (next_sibling != NULL) next_sibling->previous_sibling = previous_sibling;
		}
	} else {	/* This is a root bone */
		/* "Pull" it out from between its previous and next siblings */
		if (previous_sibling != NULL) previous_sibling->next_sibling = next_sibling;
		if (next_sibling != NULL) next_sibling->previous_sibling = previous_sibling;
	}

	bone->parent = NULL;
	bone->next_sibling = NULL;
	bone->previous_sibling = NULL;

	return TRUE;
}


int kudu_bone_destroy(KuduBone *bone)
{
	if (bone == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	KuduBone *parent, *previous_sibling, *next_sibling, *first_child, *next_child, *last_child;

	parent = bone->parent;
	previous_sibling = bone->previous_sibling;
	next_sibling = bone->next_sibling;
	first_child = bone->first_child;

	/* Reparent all this bones children */
	if (first_child != NULL) {
		next_child = NULL;
		do {
			if (next_child == NULL) next_child = first_child;
			else	next_child = next_child->next_sibling;
			next_child->parent = parent;

		} while (next_child->next_sibling != NULL);
		last_child = next_child;
	} else	last_child = NULL;

	if (parent != NULL) {	/* This is not a root bone */
		if (bone == parent->first_child) {	/* This bone is the "first_child" of it's parent */
			if (first_child != NULL) {	/* This bone has at least one child */
				parent->first_child = first_child;
				last_child->next_sibling = next_sibling;
				if (next_sibling != NULL) next_sibling->previous_sibling = last_child;

			} else {			/* This bone has no children */
				parent->first_child = next_sibling;
				if (next_sibling != NULL) next_sibling->previous_sibling = NULL;
			}
		} else if (previous_sibling != NULL) {	/* Ok this bone is _not_ the "first_child" of it's parent */
			if (first_child != NULL) {	/* This bone has at least one child */
				previous_sibling->next_sibling = first_child;
				first_child->previous_sibling = previous_sibling;

				last_child->next_sibling = next_sibling;
				if (next_sibling != NULL) next_sibling->previous_sibling = last_child;
			} else {			/* This bone has no children */
				previous_sibling->next_sibling = next_sibling;
				if (next_sibling != NULL) next_sibling->previous_sibling = previous_sibling;
			}
		}
	} else {	/* This is a root bone */
		if (first_child != NULL) {	/* This bone has at least one child */
			if (previous_sibling != NULL) previous_sibling->next_sibling = first_child;
			first_child->previous_sibling = previous_sibling;

			if (next_sibling != NULL) next_sibling->previous_sibling = last_child;
			last_child->next_sibling = next_sibling;
		} else {			/* This bone has no children */
			if (previous_sibling != NULL) previous_sibling->next_sibling = next_sibling;
			if (next_sibling != NULL) next_sibling->previous_sibling = previous_sibling;
		}
	}

	free(bone);
	bone = NULL;

	return TRUE;
}

int kudu_bone_destroy_with_children(KuduBone *bone)
{
	if (bone == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	int gone_back;
	KuduBone *current_bone, *destroy_bone;

	current_bone = bone;
	gone_back = FALSE;

	do {
		if ((current_bone->first_child != NULL) && (!gone_back)) current_bone = current_bone->first_child;
		else	if ((current_bone->next_sibling != NULL) && (current_bone != bone)) {
				current_bone = current_bone->next_sibling;
				gone_back = FALSE;
			} else {
				if (current_bone->parent != NULL) {
					destroy_bone = current_bone;
					current_bone = current_bone->parent;

					kudu_bone_destroy(destroy_bone);
				}
					
				gone_back = TRUE;
			}

	} while (current_bone->id != bone->id);

	kudu_bone_destroy(bone);

	return TRUE;
}

int kudu_bone_copy_values(KuduBone* copy_to, KuduBone *copy_from)
{
	if ((copy_to == NULL) || (copy_from == NULL)) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	copy_to->hAngle = copy_from->hAngle;
	copy_to->vAngle = copy_from->vAngle;
	copy_to->rAngle = copy_from->rAngle;
	copy_to->length = copy_from->length;
	copy_to->id = copy_from->id;

	return TRUE;
}

/* Update a bone and all its children */
int kudu_bone_update(KuduBone *bone)
{
	if (bone == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	KuduBone *current_bone;
	int gone_back;

	if (bone->parent != NULL) {
		kudu_bone_calculate_matrix(bone, bone->parent);
		kudu_bone_calculate_pos_start(bone, bone->parent);
	} else kudu_bone_calculate_matrix_root(bone);

	kudu_bone_calculate_pos_end(bone);

	if (bone->first_child == NULL) return TRUE;
	current_bone = bone->first_child;

	gone_back = FALSE;

	do {
		if (!gone_back) {
			kudu_bone_calculate_matrix(current_bone, current_bone->parent);
			kudu_bone_calculate_pos_start(current_bone, current_bone->parent);
			kudu_bone_calculate_pos_end(current_bone);
		}

		if ((current_bone->first_child != NULL) && (!gone_back)) current_bone = current_bone->first_child;
		else	if ((current_bone->next_sibling != NULL) && (current_bone != bone)) {
				current_bone = current_bone->next_sibling;
				gone_back = FALSE;
			} else {
				if (current_bone->parent != NULL) current_bone = current_bone->parent;
				gone_back = TRUE;
			}

	} while (current_bone != bone);

	return TRUE;
}

/* Update a bone and its siblings and all their children */
int kudu_bone_update_all(KuduBone *root)
{
	if (root == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	KuduBone *bone = NULL;

	do {
		if (bone == NULL) bone = root;
		else	bone = bone->next_sibling;

		kudu_bone_update(bone);

	} while (bone->next_sibling != NULL);

	return TRUE;
}

/* Only updates selected bones and their children. Scans through this bone and its children */
int kudu_bone_smart_update(KuduBone *bone)
{
	if (bone == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	KuduBone *current_bone;
	int gone_back = FALSE, updated = FALSE;

	if (bone->selected) {
		kudu_bone_update(bone);
		return TRUE;
	}

	if (bone->first_child == NULL) return TRUE;
	current_bone = bone->first_child;

	do {
		if (!gone_back) {
			if (current_bone->selected) {
				kudu_bone_update(current_bone);
				updated = TRUE;
			} else updated = FALSE;
		}


		if ((current_bone->first_child != NULL) && (!gone_back) && (!updated)) current_bone = current_bone->first_child;
		else	if ((current_bone->next_sibling != NULL) && (current_bone != bone)) {
				current_bone = current_bone->next_sibling;
				gone_back = FALSE;
			} else {
				if (current_bone->parent != NULL) current_bone = current_bone->parent;
				gone_back = TRUE;
			}

	} while (current_bone != bone);

	return TRUE;
}

/* Only update selected bones and their children. Scans through this bone and siblings and their children */
int kudu_bone_smart_update_all(KuduBone *root)
{
	if (root == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	KuduBone *bone = NULL;

	do {
		if (bone == NULL) bone = root;
		else	bone = bone->next_sibling;

		if (bone->selected) kudu_bone_update(bone);
		else kudu_bone_smart_update(bone);

	} while (bone->next_sibling != NULL);

	return TRUE;
}

static int bone_magic_touch = 1;

int kudu_bone_magic_touch(KuduBone *bone)
{
	if (bone == NULL) return FALSE;

	bone->magic_touch = bone_magic_touch;

	return TRUE;
}

/* Only updates "magic touched" bones and their children. Scans through this bone and its children */
int kudu_bone_magic_update(KuduBone *bone)
{
	if (bone == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	KuduBone *current_bone;
	int gone_back = FALSE, updated = FALSE;

	if (bone->magic_touch == bone_magic_touch) {
		kudu_bone_update(bone);
		return TRUE;
	}

	if (bone->first_child == NULL) return TRUE;
	current_bone = bone->first_child;

	do {
		if (!gone_back) {
			if (current_bone->magic_touch == bone_magic_touch) {
				kudu_bone_update(current_bone);
				updated = TRUE;
			} else updated = FALSE;
		}


		if ((current_bone->first_child != NULL) && (!gone_back) && (!updated)) current_bone = current_bone->first_child;
		else	if ((current_bone->next_sibling != NULL) && (current_bone != bone)) {
				current_bone = current_bone->next_sibling;
				gone_back = FALSE;
			} else {
				if (current_bone->parent != NULL) current_bone = current_bone->parent;
				gone_back = TRUE;
			}

	} while (current_bone != bone);

	bone_magic_touch++;
	if (bone_magic_touch == MAXINT) bone_magic_touch = 1;

	return TRUE;
}

/* Only updates "magic touched" bones and their children. Scans through this bone and siblings and their children */
int kudu_bone_magic_update_all(KuduBone *root)
{
	if (root == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	KuduBone *bone = NULL;
	int mt = bone_magic_touch;

	do {
		if (bone == NULL) bone = root;
		else	bone = bone->next_sibling;

		if (bone->magic_touch == bone_magic_touch) kudu_bone_update(bone);
		else {
			kudu_bone_magic_update(bone);
			bone_magic_touch = mt;
		}

	} while (bone->next_sibling != NULL);


	bone_magic_touch++;
	if (bone_magic_touch == MAXINT) bone_magic_touch = 1;

	return TRUE;
}

KuduBone *kudu_bone_find_child_with_id(KuduBone *bone, int child_id)
{
	if (bone == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return NULL;
	}
	int gone_back = FALSE;
	KuduBone *current_bone;

	current_bone = bone;

	do {
		if ((current_bone->first_child != NULL) && (!gone_back)) current_bone = current_bone->first_child;
		else	if ((current_bone->next_sibling != NULL) && (current_bone != bone)) {
				current_bone = current_bone->next_sibling;
				gone_back = FALSE;
			} else {
				if (current_bone->parent != NULL) current_bone = current_bone->parent;
				gone_back = TRUE;
			}

	} while ((current_bone->id != bone->id) && (current_bone->id != child_id));

	if (current_bone->id == child_id) return current_bone;

	return NULL;
}

KuduBone *kudu_bone_find_with_id(KuduBone *bone, int id)
{
	if (bone == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return NULL;
	}

	KuduBone *current_bone = NULL, *find_bone = NULL;

	do {
		if (current_bone == NULL) current_bone = bone;
		else	current_bone = current_bone->next_sibling;

		if (current_bone->id == id) return current_bone;

		find_bone = kudu_bone_find_child_with_id(current_bone, id);

		if (find_bone != NULL) return find_bone;

	} while (current_bone->next_sibling != NULL);

	return NULL;
}

int kudu_bone_num_children(KuduBone *bone)
{
	if (bone == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return 0;
	}

	KuduBone *current_bone;
	int count = 0, gone_back = FALSE;

	current_bone = bone;

	do {
		if (!gone_back) count++;

		if ((current_bone->first_child != NULL) && (!gone_back)) current_bone = current_bone->first_child;
		else	if (current_bone->next_sibling != NULL) {
				if (current_bone != bone) {
					current_bone = current_bone->next_sibling;
					gone_back = FALSE;
				}
			} else {
				if (current_bone->parent != NULL) current_bone = current_bone->parent;
				gone_back = TRUE;
			}
	} while (current_bone != bone);

	return count;
}

/*int kudu_bone_has_angle_shifted(KuduBone* bone)
{
	if (bone == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	int shift = GL_FALSE;

	shift = kudu_util_int_bit_flag_get(bone->ps, NOKE_ANGLE_SHIFTED);
	if (shift) kudu_util_int_bit_flag_unset(&bone->ps, NOKE_ANGLE_SHIFTED);

	return shift;
}

int kudu_bone_has_position_shifted(KuduBone* bone)
{
	if (bone == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	int shift;

	shift = kudu_util_int_bit_flag_get(bone->ps, NOKE_POSITION_SHIFTED);
	if (shift) kudu_util_int_bit_flag_unset(&bone->ps, NOKE_POSITION_SHIFTED);

	return shift;
}

void kudu_bone_angle_has_shifted(KuduBone* bone)
{
	if (bone == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return;
	}

	kudu_util_int_bit_flag_set(&bone->ps, NOKE_ANGLE_SHIFTED);
}

void kudu_bone_position_has_shifted(KuduBone* bone)
{
	if (bone == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return;
	}

	kudu_util_int_bit_flag_set(&bone->ps, NOKE_POSITION_SHIFTED);
}*/

int kudu_bone_joint_by_percent(KuduBone *bone, KuduBone *fake_bone, GLubyte percent)
{
	if ((bone == NULL) || (fake_bone == NULL)) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	KuduBone *parent;

	float hA, vA, rA, inP;
	int root = FALSE;

	parent = bone->parent;
	if (parent == NULL) {
		root = TRUE;
	}

	hA = bone->hAngle;
	vA = bone->vAngle;
	rA = bone->rAngle;

	fake_bone->hAngle = ((hA / 100) * percent);
	fake_bone->vAngle = ((vA / 100) * percent);
	fake_bone->rAngle = ((rA / 100) * percent);

	inP = 360.0;
	inP /= 100;
	inP *= (100 - percent);

	if (bone->hAngle > 180) fake_bone->hAngle += inP;
	if (bone->vAngle > 180) fake_bone->vAngle += inP;
	if (bone->rAngle > 180) fake_bone->rAngle += inP;
	fake_bone->length = bone->length;

	kudu_math_degrees_clamp(&fake_bone->hAngle);
	kudu_math_degrees_clamp(&fake_bone->vAngle);
	kudu_math_degrees_clamp(&fake_bone->rAngle);

	if (!root) {
		kudu_bone_calculate_matrix(fake_bone, parent);
		kudu_bone_calculate_pos_start(fake_bone, parent);
	} else {
		fake_bone->posX = bone->posX;
		fake_bone->posY = bone->posY;
		fake_bone->posZ = bone->posZ;
		kudu_bone_calculate_matrix_root(fake_bone);
	}
	kudu_bone_calculate_pos_end(fake_bone);


	return 0;
}

void kudu_bone_calculate_matrix_root(KuduBone *bone)
{
	if (bone == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return;
	}

	float angle;
	float vMatrix[4][4], hMatrix[4][4], rMatrix[4][4];
	float cosA, sinA;

	angle = (bone->vAngle * RADDEG);
	cosA = cos((GLdouble)(angle));
	sinA = sin((GLdouble)(angle));

	vMatrix[0][0] = 1.0;
	vMatrix[0][1] = 0.0;
	vMatrix[0][2] = 0.0;
	vMatrix[0][3] = 0.0;
	
	vMatrix[1][0] = 0.0;
	vMatrix[1][1] = cosA;
	vMatrix[1][2] = -sinA;
	vMatrix[1][3] = 0.0;
	
	vMatrix[2][0] = 0.0;
	vMatrix[2][1] = sinA;
	vMatrix[2][2] = cosA;
	vMatrix[2][3] = 0.0;

	vMatrix[3][0] = 0.0;
	vMatrix[3][1] = 0.0;
	vMatrix[3][2] = 0.0;
	vMatrix[3][3] = 1.0;

	angle = (bone->hAngle * RADDEG);
	cosA = cos((GLdouble)(angle));
	sinA = sin((GLdouble)(angle));

	hMatrix[0][0] = cosA;
	hMatrix[0][1] = 0.0;
	hMatrix[0][2] = sinA;
	hMatrix[0][3] = 0.0;

	hMatrix[1][0] = 0.0;
	hMatrix[1][1] = 1.0;
	hMatrix[1][2] = 0.0;
	hMatrix[1][3] = 0.0;

	hMatrix[2][0] = -sinA;
	hMatrix[2][1] = 0.0;
	hMatrix[2][2] = cosA;
	hMatrix[2][3] = 0.0;

	hMatrix[3][0] = 0.0;
	hMatrix[3][1] = 0.0;
	hMatrix[3][2] = 0.0;
	hMatrix[3][3] = 1.0;

	angle = (bone->rAngle * RADDEG);
	cosA = cos((GLdouble)(angle));
	sinA = sin((GLdouble)(angle));

	rMatrix[0][0] = cosA;
	rMatrix[0][1] = -sinA;
	rMatrix[0][2] = 0.0;
	rMatrix[0][3] = 0.0;

	rMatrix[1][0] = sinA;
	rMatrix[1][1] = cosA;
	rMatrix[1][2] = 0.0;
	rMatrix[1][3] = 0.0;

	rMatrix[2][0] = 0.0;
	rMatrix[2][1] = 0.0;
	rMatrix[2][2] = 1.0;
	rMatrix[2][3] = 0.0;

	rMatrix[3][0] = 0.0;
	rMatrix[3][1] = 0.0;
	rMatrix[3][2] = 0.0;
	rMatrix[3][3] = 1.0;

	/* r,v,h */

	kudu_math_matrix_copy(bone->matrix, rMatrix);
	kudu_math_matrix_multiply(bone->matrix, vMatrix);
	kudu_math_matrix_multiply(bone->matrix, hMatrix);
	
}

void kudu_bone_calculate_matrix(KuduBone *bone, KuduBone *parent_bone)
{
	if ((bone == NULL) || (parent_bone == NULL)) {
		kudu_error(KE_OBJECT_INVALID);
		return;
	}

	float angle;
	float vMatrix[4][4], hMatrix[4][4], rMatrix[4][4];
	float cosA, sinA;

	angle = (bone->vAngle * RADDEG);
	cosA = (GLfloat)cos((GLdouble)angle);
	sinA = (GLfloat)sin((GLdouble)angle);

	vMatrix[0][0] = 1.0;
	vMatrix[0][1] = 0.0;
	vMatrix[0][2] = 0.0;
	vMatrix[0][3] = 0.0;

	vMatrix[1][0] = 0.0;
	vMatrix[1][1] = cosA;
	vMatrix[1][2] = -sinA;
	vMatrix[1][3] = 0.0;
	
	vMatrix[2][0] = 0.0;
	vMatrix[2][1] = sinA;
	vMatrix[2][2] = cosA;
	vMatrix[2][3] = 0.0;

	vMatrix[3][0] = 0.0;
	vMatrix[3][1] = 0.0;
	vMatrix[3][2] = 0.0;
	vMatrix[3][3] = 1.0;


	angle = (bone->hAngle * RADDEG);
	cosA = (GLfloat)cos((GLdouble)angle);
	sinA = (GLfloat)sin((GLdouble)angle);

	hMatrix[0][0] = cosA;
	hMatrix[0][1] = 0.0;
	hMatrix[0][2] = sinA;
	hMatrix[0][3] = 0.0;

	hMatrix[1][0] = 0.0;
	hMatrix[1][1] = 1.0;
	hMatrix[1][2] = 0.0;
	hMatrix[1][3] = 0.0;

	hMatrix[2][0] = -sinA;
	hMatrix[2][1] = 0.0;
	hMatrix[2][2] = cosA;
	hMatrix[2][3] = 0.0;

	hMatrix[3][0] = 0.0;
	hMatrix[3][1] = 0.0;
	hMatrix[3][2] = 0.0;
	hMatrix[3][3] = 1.0;


	angle = (bone->rAngle * RADDEG);
	cosA = (GLfloat)cos((GLdouble)angle);
	sinA = (GLfloat)sin((GLdouble)angle);

	rMatrix[0][0] = cosA;
	rMatrix[0][1] = -sinA;
	rMatrix[0][2] = 0.0;
	rMatrix[0][3] = 0.0;

	rMatrix[1][0] = sinA;
	rMatrix[1][1] = cosA;
	rMatrix[1][2] = 0.0;
	rMatrix[1][3] = 0.0;

	rMatrix[2][0] = 0.0;
	rMatrix[2][1] = 0.0;
	rMatrix[2][2] = 1.0;
	rMatrix[2][3] = 0.0;

	rMatrix[3][0] = 0.0;
	rMatrix[3][1] = 0.0;
	rMatrix[3][2] = 0.0;
	rMatrix[3][3] = 1.0;

	/* r, v, h */
	/*kudu_math_matrix_multiply(hMatrix, vMatrix);*/

	kudu_math_matrix_copy(bone->matrix, rMatrix);
	kudu_math_matrix_multiply(bone->matrix, vMatrix);
	kudu_math_matrix_multiply(bone->matrix, hMatrix);
	kudu_math_matrix_multiply(bone->matrix, parent_bone->matrix);

}

void kudu_bone_calculate_pos_start(KuduBone *bone, KuduBone *parent_bone)
{
	if ((bone == NULL) || (parent_bone == NULL)) {
		kudu_error(KE_OBJECT_INVALID);
		return;
	}

	bone->posX = parent_bone->lineX;
	bone->posY = parent_bone->lineY;
	bone->posZ = parent_bone->lineZ;
}

