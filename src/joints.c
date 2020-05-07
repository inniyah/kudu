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
#include "joints.h"

KuduJoint *kudu_joint_new(KuduObject *object)
{
	if (object == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return NULL;
	}

	KuduJoint *joint, *old_next_joint;
	int a;

	joint = (KuduJoint*)malloc(sizeof(KuduJoint));
	if (joint == NULL) {
		kudu_error(KE_NO_MEM);
		return NULL;
	}

	old_next_joint = object->joint;
	object->joint = joint;
	joint->previous_joint = NULL;
	joint->next_joint = old_next_joint;
	if (old_next_joint != NULL) old_next_joint->previous_joint = joint;
	joint->object = object;

	joint->id = object->next_joint_id;
	object->next_joint_id++;
	joint->num_bones = 0;
	joint->selected = FALSE;
	joint->zlen = 0.0;
	for (a = 0; a < 3; a++) {
		joint->pos[a] = 0.0;
		joint->ppos[a] = 0.0;
	}

	return joint;
}

int kudu_joint_destroy(KuduJoint *joint)
{
	if (joint == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	KuduJoint *previous_joint, *next_joint;
	KuduObject *object = joint->object;

	previous_joint = joint->previous_joint;
	next_joint = joint->next_joint;

	if (previous_joint != NULL) previous_joint->next_joint = next_joint;
	if (next_joint != NULL) next_joint->previous_joint = previous_joint;

	if (joint == object->joint) {
		if (next_joint != NULL) object->joint = next_joint;
		else object->joint = NULL;
	}

	free(joint);

	return TRUE;
}

/* Copy important values from ijoint to ojoint */
int kudu_joint_clone_values(KuduJoint *ojoint, KuduJoint *ijoint)
{
	if ((ojoint == NULL) || (ijoint == NULL)) {
		kudu_error(KE_OBJECT_INVALID);
			return FALSE;
	}

	int a;

	for (a = 0; a < 3; a++) ojoint->pos[a] = ijoint->pos[a];

	return TRUE;
}

/*KuduJointList *kudu_joint_list_new(void)
{
	KuduJointList *joint_list;

	joint_list = (KuduJointList*)malloc(sizeof(KuduJointList));

	int a;
*/
	/*joint_list->num_joints = 0;
	joint_list->num_avail = 0;
	joint_list->joint = NULL;*/

	/* Create an array large enough to hold 500 joints */
	/* If more than that are needed the array will be automatically resized */
/*	joint_list->joint = (KuduJoint*)malloc(sizeof(KuduJoint) * 500);
	joint_list->num_joints = 500;
	joint_list->num_avail = 500;

	for (a = 0; a < 499; a++) {
		joint_list->joint[a].avail = TRUE;
	}

	return joint_list;
}

int kudu_joint_list_destroy(KuduJointList *joint_list)
{
	if (joint_list == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	if (joint_list->joint != NULL) free(joint_list->joint);

	free(joint_list);

	return TRUE;
}

int kudu_joint_list_add_joint(KuduJointList *joint_list)
{
	if (joint_list == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	int a, jid = -1;

	if (joint_list->joint != NULL) {
		if (joint_list->num_avail > 0) {
			a = 0;
			while ((a < joint_list->num_joints) && (!joint_list->joint[a].avail)) {
				a++;
			}

			if (a < joint_list->num_joints) {
				jid = a;
				joint_list->num_avail--;
			}
		}
	}

	if (jid < 0) {
		joint_list->num_joints++;
		if (joint_list->joint == NULL) joint_list->joint = (KuduJoint*)malloc(sizeof(KuduJoint) * joint_list->num_joints);
		else	joint_list->joint = (KuduJoint*)realloc(joint_list->joint, sizeof(KuduJoint) * joint_list->num_joints);
		jid = joint_list->num_joints-1;
	}

	joint_list->joint[jid].avail = FALSE;
	for (a = 0; a < 3; a++) joint_list->joint[jid].pos[a] = 0.0;

	return jid;
}*/

/* Increase the bone count on given joint by one */
int kudu_joint_incref(KuduJoint *joint)
{
	if (joint == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	joint->num_bones++;

	return joint->num_bones;
}

/* Decrease the bone count on given joint by one, and mark for availability if applicable */
int kudu_joint_decref(KuduJoint *joint)
{
	if (joint == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	joint->num_bones--;
	if (joint->num_bones <= 0) kudu_joint_destroy(joint);

	return TRUE;
}

/* Return a pointer to the joint specified by jid */
/* Care must be taken when using this function, adding a new joint after having aquired
   a pointer could result in the pointer becoming invalid */
/*KuduJoint *kudu_joint_list_get_joint(KuduJointList *joint_list, int jid)
{
	if (joint_list == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return NULL;
	}

	if ((jid > joint_list->num_joints) || (joint_list->joint == NULL)) return NULL;
*/
	/* If this joint has actually been erased return NULL */
/*	if (joint_list->joint[jid].avail) return NULL;

	return &joint_list->joint[jid];
}*/






