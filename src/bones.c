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
	KuduJoint *s_joint, *e_joint;

	int a, b;

	bone = (KuduBone*)malloc(sizeof(KuduBone));
	if (bone == NULL) {
		kudu_error(KE_NO_MEM);
		return NULL;
	}

	bone->id = object->next_bone_id++;

	s_joint = kudu_joint_new(object);
	e_joint = kudu_joint_new(object);
	kudu_joint_incref(s_joint);
	kudu_joint_incref(e_joint);

	for (a = 0; a < 16; a++) {
		bone->matrix[a] = 0.0;
		bone->pmatrix[a] = 0.0;
	}

	/* Local and global rotation matrices (lmatrix & gmatrix) must be initialized to identity matrices */
	kudu_math_matrix_set_identity(bone->pmatrix);

	kudu_math_quat_set_identity(bone->lquat);
	kudu_math_quat_set_identity(bone->gquat);

	kudu_math_quat_set_identity(bone->lpquat);

	sprintf(bone->name, "Bone: %d", bone->id);

	bone->rotation = 0.0;
	bone->length = 1.0;
	bone->plength = 0.0;

	for (a = 0; a < 3; a++) bone->axis[a] = 0.0;

	bone->selected = FALSE;
	bone->magic_touch = 0;

	bone->s_joint = s_joint;
	bone->e_joint = e_joint;
	s_joint->bone = bone;
	e_joint->bone = bone;

	bone->parent = NULL;
	bone->first_child = NULL;
	bone->next_sibling = NULL;
	bone->previous_sibling = NULL;
	bone->object = object;

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

int kudu_bone_set_joints(KuduBone *bone, KuduJoint* s_joint, KuduJoint *e_joint)
{
	if (bone == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	int rv = 0;
	/*KuduObject *object = bone->object;*/
	/*KuduJointList *joint_list = object->joint_list;*/

	if (bone->s_joint != s_joint) {
		if (bone->s_joint != NULL) kudu_joint_decref(bone->s_joint);

		if (s_joint != NULL) {
			if (kudu_joint_incref(s_joint)) {
				bone->s_joint = s_joint;
				if (s_joint->bone == NULL) s_joint->bone = bone;
				rv++;
			} else bone->s_joint = NULL;
		} else {
			bone->s_joint = NULL;
			rv++;
		}
	} else rv++;

	if (bone->e_joint != e_joint) {
		if (bone->e_joint != NULL) kudu_joint_decref(bone->e_joint);

		if (e_joint != NULL) {
			if (kudu_joint_incref(e_joint)) {
				bone->e_joint = e_joint;
				e_joint->bone = bone;
				rv++;
			} else bone->e_joint = NULL;
		} else {
			bone->e_joint = NULL;
			rv++;
		}
	} else rv++;

	return rv;
}

KuduBone *kudu_bone_add_child(KuduBone *parent_bone)
{
	if (parent_bone == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return NULL;
	}

	KuduBone *bone, *next_bone;
	KuduObject *object = parent_bone->object;
	/*int s_jid, e_jid;*/
	KuduJoint *s_joint, *e_joint;

	bone = kudu_bone_new(object);
	if (bone == NULL) return NULL;

	bone->parent = parent_bone;
	s_joint = parent_bone->e_joint;
	e_joint = bone->e_joint;

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
	kudu_bone_set_joints(bone, s_joint, e_joint);

	return bone;
}

KuduBone *kudu_bone_add_parent(KuduBone *bone)
{
	if (bone == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return NULL;
	}

	KuduBone *parent_bone, *old_parent, *previous_sibling, *next_sibling;
	KuduObject *object = bone->object;
	KuduJoint *s_joint, *e_joint, *p_joint;
	int a;
	/*int s_jid, e_jid;*/

	parent_bone = kudu_bone_new(object);
	if (parent_bone == NULL) return NULL;

	/* parent_bone must claim bone's s_joint as it's own s_joint and keep it's own e_joint */
	s_joint = bone->s_joint;
	e_joint = parent_bone->e_joint;
	kudu_bone_set_joints(parent_bone, s_joint, e_joint);

	/* bone should aquire parent_bone's e_joint as it's s_joint and keep it's own e_joint */
	kudu_bone_set_joints(bone, e_joint, bone->e_joint);

	old_parent = bone->parent;
	previous_sibling = bone->previous_sibling;
	next_sibling = bone->next_sibling;

	parent_bone->parent = old_parent;
	parent_bone->previous_sibling = previous_sibling;
	parent_bone->next_sibling = next_sibling;
	parent_bone->first_child = bone;

	if (old_parent != NULL) {
		parent_bone->length = old_parent->length;
		s_joint = old_parent->e_joint;
		/* Is this the "first_bone" of it's parent ?? */
		if (old_parent->first_child == bone) old_parent->first_child = parent_bone;
	} else { /* OK we are inserting this bone before a current root bone, so we must inherit its position */
	/*	joint = kudu_bone_get_s_joint(parent_bone);*/
	/*	p_joint = kudu_bone_get_s_joint(bone);*/
		p_joint = bone->s_joint;
		for (a = 0; a < 3; a++) s_joint->pos[a] = p_joint->pos[a];
		/*parent_bone->posX = bone->posX;
		parent_bone->posY = bone->posY;
		parent_bone->posZ = bone->posZ;*/
	}

	if (previous_sibling != NULL) previous_sibling->next_sibling = parent_bone;
	if (next_sibling != NULL) next_sibling->previous_sibling = parent_bone;

	bone->parent = parent_bone;
	bone->previous_sibling = NULL;
	bone->next_sibling = NULL;

	return parent_bone;
}

KuduBone *kudu_bone_add_sibling(KuduBone *previous_sibling)
{
	if (previous_sibling == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return NULL;
	}

	KuduBone *bone, *old_next_sibling;
	KuduObject *object = previous_sibling->object;
	/*int s_jid, e_jid;*/
	KuduJoint *s_joint, *e_joint;

	bone = kudu_bone_new(object);
	if (bone == NULL) return NULL;
	if (previous_sibling->parent == NULL) s_joint = bone->s_joint;
	else	s_joint = previous_sibling->s_joint;

	e_joint = bone->e_joint;

	old_next_sibling = previous_sibling->next_sibling;

	bone->parent = previous_sibling->parent;
	bone->previous_sibling = previous_sibling;
	bone->next_sibling = old_next_sibling;
	previous_sibling->next_sibling = bone;

	if (old_next_sibling != NULL) old_next_sibling->previous_sibling = bone;

	kudu_bone_set_joints(bone, s_joint, e_joint);

	return bone;
}

int kudu_bone_insert_parent(KuduBone *bone, KuduBone *parent_bone)
{
	if ((bone == NULL) || (parent_bone == NULL)) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	KuduBone *old_parent, *previous_sibling, *next_sibling;
	/*KuduObject *object = bone->object;*/
	KuduJoint *s_joint, *e_joint;
	/*int s_jid, e_jid;*/

	s_joint = parent_bone->s_joint;
	e_joint = bone->s_joint;

	old_parent = bone->parent;
	previous_sibling = bone->previous_sibling;
	next_sibling = bone->next_sibling;

	parent_bone->parent = old_parent;
	parent_bone->previous_sibling = previous_sibling;
	parent_bone->next_sibling = next_sibling;
	parent_bone->first_child = bone;

	if (old_parent != NULL) {
		s_joint = old_parent->e_joint;
		/* Is this the "first_bone" of it's parent ?? */
		if (old_parent->first_child == bone) old_parent->first_child = parent_bone;
	}

	if (previous_sibling != NULL) previous_sibling->next_sibling = parent_bone;
	if (next_sibling != NULL) next_sibling->previous_sibling = parent_bone;

	bone->parent = parent_bone;
	bone->previous_sibling = NULL;
	bone->next_sibling = NULL;

	kudu_bone_set_joints(parent_bone, s_joint, e_joint);

	return TRUE;
}

int kudu_bone_adopt_child(KuduBone *parent_bone, KuduBone *bone)
{
	if ((parent_bone == NULL) || (bone == NULL)) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	KuduBone *next_bone;
	/*KuduObject *object = parent_bone->object;*/
	/*int s_jid, e_jid;*/
	KuduJoint *s_joint, *e_joint;
	e_joint = bone->e_joint;
	s_joint = parent_bone->e_joint;

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

	kudu_bone_set_joints(bone, s_joint, e_joint);

	return TRUE;
}

int kudu_bone_adopt_sibling(KuduBone *bone, KuduBone *sibling)
{
	if ((bone == NULL) || (sibling == NULL)) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	KuduBone *old_next_sibling;
	KuduJoint *s_joint, *e_joint;

	/* If bone is a root bone then sibling should keep it's own joints, otherwise inherit bone's s_joint */
	if (bone->parent != NULL) {
		s_joint = bone->s_joint;
		e_joint = sibling->e_joint;
		kudu_bone_set_joints(sibling, s_joint, e_joint);
	}


	old_next_sibling = bone->next_sibling;

	sibling->parent = bone->parent;
	sibling->previous_sibling = bone;
	sibling->next_sibling = old_next_sibling;
	bone->next_sibling = sibling;

	if (old_next_sibling != NULL) old_next_sibling->previous_sibling = sibling;


	return TRUE;
}

/* "Unlinks" a bone - removes it and all its children from a skeleton without destroying them */
/* Sort of like "cutting" a directory in a gui file browser */
int kudu_bone_unlink(KuduBone *bone)
{
	if (bone == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	KuduBone *parent, *previous_sibling, *next_sibling, *first_child, *next_child, *last_child;
	KuduObject *object = bone->object;
	/*int s_jid, e_jid;*/
	KuduJoint *s_joint, *e_joint;

	parent = bone->parent;
	previous_sibling = bone->previous_sibling;
	next_sibling = bone->next_sibling;

	e_joint = bone->e_joint;
	s_joint = kudu_joint_new(object);

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

	kudu_bone_set_joints(bone, s_joint, e_joint);

	return TRUE;
}


int kudu_bone_destroy(KuduBone *bone)
{
	if (bone == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	KuduBone *parent, *previous_sibling, *next_sibling, *first_child, *next_child, *last_child;
	KuduObject *object = bone->object;
	KuduJoint *s_joint, *e_joint, *joint;

	parent = bone->parent;
	previous_sibling = bone->previous_sibling;
	next_sibling = bone->next_sibling;
	first_child = bone->first_child;
	s_joint = bone->s_joint;
	e_joint = bone->e_joint;

	/* Reparent all this bones children */
	if (first_child != NULL) {
		next_child = NULL;
		do {
			if (next_child == NULL) next_child = first_child;
			else	next_child = next_child->next_sibling;
			next_child->parent = parent;

			/* Children should inherit their parent's s_joint whilst keeping their own e_joint
			   however if child has no "grandparent" then it should keep it's own s_joint as well */
			if (parent != NULL) kudu_bone_set_joints(next_child, s_joint, next_child->e_joint);
			else {
				/* In the case that a root bone has been deleted which has more than one child
				   each child (excluding the first) must get a new s_joint, as they are each
				   becoming a new root bone */
				if (next_child->previous_sibling != NULL) {
					joint = kudu_joint_new(object);
					/* Make sure position stays the same */
					kudu_joint_clone_values(joint, e_joint);
					kudu_bone_set_joints(next_child, joint, next_child->e_joint);
				}
			}

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

	kudu_bone_set_joints(bone, NULL, NULL);

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
	int gone_back, pose;

	/* Are we in editor or animator mode ? */
	/* If in animator mode set: pose = TRUE this is so that the correct quaternions get adjusted */
	if (program.mode == PROGRAM_MODE_EDIT) pose = FALSE;
	else if (program.mode == PROGRAM_MODE_ANIMATION) pose = TRUE;

	kudu_bone_calculate_matrix(bone, pose);
	kudu_bone_calculate_pos_end(bone);

	if (bone->first_child == NULL) return TRUE;
	current_bone = bone->first_child;

	gone_back = FALSE;

	do {
		if (!gone_back) {
			kudu_bone_calculate_matrix(current_bone, pose);
			/*kudu_bone_calculate_pos_start(current_bone, current_bone->parent);*/
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

	/* New method: since introducing joints we can simply check the num_bones value -1
	   of a bones e_joint to find it's children count */
	KuduJoint *joint = bone->e_joint;
	return joint->num_bones;

	/* Old method was to walk the child child list and keep count ... */
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


/* Calculate position of bones end joint */
int kudu_bone_both_joints_selected(KuduBone *bone)
{
	if (bone == NULL) return FALSE;

	KuduJoint *s_joint = bone->s_joint;
	KuduJoint *e_joint = bone->e_joint;

	if ((s_joint->selected) && (e_joint->selected)) return TRUE;

	return FALSE;
}

/* Calculate the e_joint pos of a bone */
void kudu_bone_calculate_pos_end(KuduBone *bone)
{
	if (bone == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return;
	}

	int a;
	float len, plen;
	KuduJoint *s_joint, *e_joint;

	len = bone->length;
	plen = len + bone->plength;

	s_joint = bone->s_joint;
	e_joint = bone->e_joint;

	e_joint->zlen = s_joint->zlen + bone->length;

	/*e_joint->pos[0] = 0.0;
	e_joint->pos[0] = 0.0;
	e_joint->pos[0] = 0.0; */ /*bone->length - e_joint->zlen;*/

	for (a = 0; a < 3; a++) {
		e_joint->pos[a] = s_joint->pos[a] + (len * (bone->matrix[8+a])) + (bone->matrix[12+a]);
		/*if (program.mode == PROGRAM_MODE_ANIMATION) e_joint->ppos[a] = s_joint->ppos[a];*/

		if (program.mode == PROGRAM_MODE_ANIMATION)
			if (bone->parent != NULL) e_joint->ppos[a] = s_joint->ppos[a] + (plen * (bone->pmatrix[8+a])) + (bone->pmatrix[12+a]);
			else  e_joint->ppos[a] = s_joint->ppos[a] + s_joint->pos[a] + (plen * (bone->pmatrix[8+a])) + (bone->pmatrix[12+a]);

		/*	e_joint->ppos[a] = s_joint->ppos[a] + (len * (bone->pmatrix[8+a])) + (bone->pmatrix[12+a]);*/
	}
}

/* Attempt to calculate the bone's axis from the position of it's joints */
/* *EXPERIMENTAL* not working */
int kudu_bone_calculate_inverse(KuduBone *bone)
{
	if (bone == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	KuduJoint *s_joint = bone->s_joint;
	KuduJoint *e_joint = bone->e_joint;
	float h,v;

	bone->length = kudu_math_distance_between(e_joint->pos, s_joint->pos);
	kudu_math_angles_between_v(e_joint->pos, s_joint->pos, &h, &v);
	/*kudu_bone_magic_touch(bone);*/

	return TRUE;
}


int kudu_bone_joint_by_percent(KuduBone *bone, KuduBone *fake_bone, unsigned char percent)
{
	if ((bone == NULL) || (fake_bone == NULL)) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	KuduBone *parent;
	KuduJoint *joint, *f_joint;

	float hA, vA, rA, inP;
	int root = FALSE, a;

	parent = bone->parent;
	if (parent == NULL) {
		root = TRUE;
	}

	/*hA = bone->hAngle;
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
	kudu_math_degrees_clamp(&fake_bone->rAngle);*/

	if (!root) {
		kudu_bone_calculate_matrix(fake_bone, TRUE);
		/*kudu_bone_calculate_pos_start(fake_bone, parent);*/
	} else {
		joint = bone->s_joint;
		f_joint = fake_bone->s_joint;
		for (a = 0; a < 3; a++) f_joint->pos[a] = joint->pos[a];
		/*fake_bone->posX = bone->posX;
		fake_bone->posY = bone->posY;
		fake_bone->posZ = bone->posZ;*/
		kudu_bone_calculate_matrix(fake_bone, TRUE);
	}
	kudu_bone_calculate_pos_end(fake_bone);


	return 0;
}

/* Recalculate the matrices and quaternions of a bone */
void kudu_bone_calculate_matrix(KuduBone *bone, int update_pose)
{
	if (bone == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return;
	}

	float tmat[16], rmat[16], len = 0.0;
	KuduJoint *joint = bone->s_joint;
	KuduBone *parent_bone = bone->parent;

	kudu_math_quat_copy(bone->quat, bone->lquat);

	/*if (update_pose) kudu_math_quat_multiply(bone->quat, bone->lpquat);*/

	if (parent_bone != NULL) {
		kudu_math_quat_multiply(bone->quat, parent_bone->quat);
		len = parent_bone->length + parent_bone->plength;
	}

	kudu_math_quat_multiply(bone->quat, bone->gquat);

	kudu_math_quat_to_matrix(bone->quat, bone->matrix);


	/* Calculate pose matrix - a whole different cup of tea */
	if (update_pose) {

		/* Local rotation */
		kudu_math_quat_copy(bone->pquat, bone->lquat);

		/* Local pose rotation */
		kudu_math_quat_multiply(bone->pquat, bone->lpquat);

		/* Parent pose quat */
		if (parent_bone != NULL) {
			kudu_math_quat_multiply(bone->pquat, parent_bone->pquat);
		}

		/* Global rotation */
		kudu_math_quat_multiply(bone->pquat, bone->gquat);

		/*kudu_math_quat_copy(bone->pquat, bone->lpquat);*/
		kudu_math_quat_to_matrix(bone->pquat, bone->pmatrix);


		/*if (parent_bone == NULL) {
			kudu_math_matrix_set_translation(tmat, joint->ppos[0], joint->ppos[1], joint->ppos[2]);
			kudu_math_matrix_copy(bone->pmatrix, tmat);
			kudu_math_matrix_multiply(bone->pmatrix, rmat);
		} else kudu_math_matrix_copy(bone->pmatrix, rmat);*/

	/*	kudu_math_matrix_set_translation(tmat, 0.0, 0.0, bone->plength);
		kudu_math_matrix_copy(bone->pmatrix, tmat);

		kudu_math_matrix_multiply(bone->pmatrix, rmat);*/

		/*kudu_math_matrix_set_translation(tmat, len, len, len);*/
		/*kudu_math_matrix_set_translation(tmat, joint->pos[0], joint->pos[1], joint->pos[2]);

		kudu_math_matrix_multiply(bone->pmatrix, bone->matrix);*/

		/*if (parent_bone != NULL) kudu_math_matrix_multiply(bone->pmatrix, parent_bone->pmatrix);*/
		/*else {*/
			/*kudu_math_matrix_set_translation(tmat, joint->pos[0], joint->pos[1], joint->pos[2]);
			kudu_math_matrix_multiply(bone->pmatrix, tmat);*/
		/*}*/
	}
}

int kudu_bone_apply_rotation(KuduBone *bone, float angle, int axis, int pose)
{
	if (bone == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	if (!pose) {	/* Are we applying a rotation to the base bone pos or the actual pose postion ? */
		if (axis < 3) {
			kudu_math_quat_apply_rotation(bone->lquat, axis, angle);
		} else if ((axis > 2) && (axis < 6)) {
			kudu_math_quat_apply_rotation(bone->gquat, axis-3, angle);
		}

	} else {

		if (axis < 3) {
			kudu_math_quat_apply_rotation(bone->lpquat, axis, angle);
		}
	}

	return TRUE;
}

