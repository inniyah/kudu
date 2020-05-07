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
#include "frames.h"

KuduFrame *kudu_frame_new(KuduFrame *previous_frame, int no)
{
	KuduFrame *frame, *old_next_frame;

	int a;
	frame = (KuduFrame*)malloc(sizeof(KuduFrame));
	if (frame == NULL) return NULL;

	if (previous_frame == NULL) {
		frame->previous_frame = NULL;
		frame->next_frame = NULL;
	} else {
		frame->previous_frame = previous_frame;
		old_next_frame = previous_frame->next_frame;
		previous_frame->next_frame = frame;
		frame->next_frame = old_next_frame;

		if (old_next_frame != NULL) old_next_frame->previous_frame = frame;
	}

	frame->item_frame = NULL;
	frame->frame = no;
	frame->operation = OPERATION_NONE;
	for (a = 0; a < 4; a++) frame->fdata[a] = 0.0;

	return frame;
}

int kudu_frame_destroy(KuduFrame *frame)
{
	if (frame == NULL) return FALSE;

	KuduFrame *previous_frame, *next_frame;
	KuduItemFrame *item_frame;

	previous_frame = frame->previous_frame;
	next_frame = frame->next_frame;

	if (previous_frame != NULL) previous_frame->next_frame = next_frame;
	if (next_frame != NULL) next_frame->previous_frame = previous_frame;

	if (frame->item_frame != NULL) {
		item_frame = frame->item_frame;
		if (item_frame->first_frame == frame) item_frame->first_frame = next_frame;
		if (item_frame->last_frame == frame) {
			if (previous_frame == NULL) item_frame->last_frame = previous_frame;
			else	item_frame->last_frame = next_frame;
		}
	}

	free(frame);

	return TRUE;
}

KuduItemFrame *kudu_item_frame_new(KuduItemFrame *previous_item)
{
	KuduItemFrame *item, *old_next_item;

	item = (KuduItemFrame*)malloc(sizeof(KuduItemFrame));
	if (item == NULL) return NULL;

	if (previous_item == NULL) {
		item->next_item = NULL;
		item->previous_item = NULL;
	} else {
		item->previous_item = previous_item;
		old_next_item = previous_item->next_item;
		previous_item->next_item = item;
		item->next_item = old_next_item;

		if (old_next_item != NULL) old_next_item->previous_item = item;
	}

	item->object_frame = NULL;
	item->item = NULL;
	item->first_frame = NULL;
	item->last_frame = NULL;

	return item;
}

int kudu_item_frame_destroy(KuduItemFrame *item_frame)
{
	if (item_frame == NULL) return FALSE;

	KuduItemFrame *previous_item, *next_item;
	KuduObjectFrame *obj_frame;

	previous_item = item_frame->previous_item;
	next_item = item_frame->next_item;

	if (previous_item != NULL) previous_item->next_item = next_item;
	if (next_item != NULL) next_item->previous_item = previous_item;

	if (item_frame->object_frame != NULL) {
		obj_frame = item_frame->object_frame;
		if (obj_frame->first_item == item_frame) obj_frame->first_item = next_item;
	}

	free(item_frame);

	return TRUE;
}

KuduObjectFrame *kudu_object_frame_new(KuduObjectFrame *previous_object)
{
	KuduObjectFrame *object, *old_next_object;

	object = (KuduObjectFrame*)malloc(sizeof(KuduObjectFrame));
	if (object == NULL) return NULL;

	if (previous_object == NULL) {
		object->next_object = NULL;
		object->previous_object = NULL;
	} else {
		object->previous_object = previous_object;
		old_next_object = previous_object->next_object;
		previous_object->next_object = object;
		object->next_object = old_next_object;

		if (old_next_object != NULL) old_next_object->previous_object = object;
	}

	object->frame_list = NULL;
	object->object = NULL;
	object->first_item = NULL;

	return object;
}

int kudu_object_frame_destroy(KuduObjectFrame *obj_frame)
{
	if (obj_frame == NULL) return FALSE;

	KuduObjectFrame *previous_object, *next_object;
	KuduFrameList *frame_list;

	previous_object = obj_frame->previous_object;
	next_object = obj_frame->next_object;

	if (previous_object != NULL) previous_object->next_object = next_object;
	if (next_object != NULL) next_object->previous_object = previous_object;

	if (obj_frame->frame_list != NULL) {
		frame_list = obj_frame->frame_list;
		if (frame_list->first_object == obj_frame) frame_list->first_object = next_object;
	}

	free(obj_frame);

	return TRUE;
}

KuduFrameList *kudu_frame_list_new(void)
{
	KuduFrameList *frame_list;

	frame_list = (KuduFrameList*)malloc(sizeof(KuduFrameList));
	if (frame_list == NULL) return NULL;

	frame_list->max_frame = 0;
	frame_list->object_cnt = 0;
	frame_list->current_frame = 0;
	frame_list->first_object = NULL;

	return frame_list;
}

int kudu_frame_list_destroy(KuduFrameList *frame_list)
{
	if (frame_list == NULL) return FALSE;

	KuduObjectFrame *obj_frame = NULL, *destroy_object;
	KuduItemFrame *item_frame, *destroy_item;
	KuduFrame *frame, *destroy_frame;

	if (frame_list->first_object == NULL) {
		free(frame_list);
		return TRUE;
	}

	do {
		destroy_object = obj_frame;
		if (obj_frame == NULL) obj_frame = frame_list->first_object;
		else	obj_frame = obj_frame->next_object;
		if (destroy_object != NULL) kudu_object_frame_destroy(destroy_object);
		if (obj_frame->first_item == NULL) continue;

		item_frame = NULL;
		do {
			destroy_item = item_frame;
			if (item_frame == NULL) item_frame = obj_frame->first_item;
			else	item_frame = item_frame->next_item;
			if (destroy_item != NULL) kudu_item_frame_destroy(destroy_item);
			if (item_frame->first_frame == NULL) continue;

			frame = NULL;
			do {
				destroy_frame = frame;
				if (frame == NULL) frame = item_frame->first_frame;
				else	frame = frame->next_frame;
				if (destroy_frame != NULL) kudu_frame_destroy(destroy_frame);
			} while (frame->next_frame != NULL);
			kudu_frame_destroy(frame);

		} while (item_frame->next_item != NULL);
		kudu_item_frame_destroy(item_frame);

	} while (obj_frame->next_object != NULL);
	kudu_object_frame_destroy(obj_frame);

	free(frame_list);

	return TRUE;
}

KuduObjectFrame *kudu_frame_list_find_object_frame(KuduFrameList *frame_list, KuduObject *object)
{
	if (frame_list == NULL) return NULL;
	if (object == NULL) return NULL;
	if (frame_list->first_object == NULL) return NULL;

	KuduObjectFrame *current_object = NULL;

	do {
		if (current_object == NULL) current_object = frame_list->first_object;
		else	current_object = current_object->next_object;

	} while ((current_object->next_object != NULL) && (current_object->object != object));

	if (current_object->object == object) return current_object;

	return NULL;
}

KuduObjectFrame *kudu_frame_list_find_or_add_object_frame(KuduFrameList *frame_list, KuduObject *object)
{
	if (frame_list == NULL) return NULL;
	if (object == NULL) return NULL;

	KuduObjectFrame *obj_frame, *current_object = NULL;

	if (frame_list->first_object != NULL) {
		do {
			if (current_object == NULL) current_object = frame_list->first_object;
			else	current_object = current_object->next_object;

			if (current_object->object == object) return current_object;
		} while (current_object->next_object != NULL);
		obj_frame = kudu_object_frame_new(current_object);
		frame_list->object_cnt++;
	} else {
	 	obj_frame = kudu_object_frame_new(frame_list->first_object);
		if (obj_frame == NULL) return NULL;
		frame_list->first_object = obj_frame;
		frame_list->object_cnt++;
	}

	obj_frame->object = object;
	obj_frame->frame_list = frame_list;

	return obj_frame;
}

KuduItemFrame *kudu_object_frame_find_item_frame(KuduObjectFrame *obj_frame, void *item)
{
	if (obj_frame == NULL) return NULL;
	if (item == NULL) return NULL;
	if (obj_frame->first_item == NULL) return NULL;

	KuduItemFrame *current_item = NULL;

	do {
		if (current_item == NULL) current_item = obj_frame->first_item;
		else	current_item = current_item->next_item;
	} while ((current_item->next_item != NULL) && (current_item->item != item));

	if (current_item->item == item) return current_item;

	return NULL;
}

KuduItemFrame *kudu_object_frame_find_or_add_item_frame(KuduObjectFrame *obj_frame, void *item)
{
	if (obj_frame == NULL) return NULL;
	if (item == NULL) return NULL;

	KuduItemFrame *item_frame, *current_item = NULL;

	if (obj_frame->first_item != NULL) {
		do {
			if (current_item == NULL) current_item = obj_frame->first_item;
			else	current_item = current_item->next_item;

			if (current_item->item == item) return current_item;
		} while (current_item->next_item != NULL);
		item_frame = kudu_item_frame_new(current_item);
	} else {
		item_frame = kudu_item_frame_new(obj_frame->first_item);
		if (item_frame == NULL) return NULL;
		obj_frame->first_item = item_frame;
	}

	item_frame->item = item;
	item_frame->object_frame = obj_frame;

	return item_frame;
}

KuduFrame *kudu_item_frame_find_frame(KuduItemFrame *item_frame, int frame_no)
{
	if (item_frame == NULL) return NULL;
	if (frame_no < 0) return NULL;
	if (item_frame->first_frame == NULL) return NULL;

	KuduFrame *current_frame = NULL;

	do {
		if (current_frame == NULL) current_frame = item_frame->first_frame;
		else	current_frame = current_frame->next_frame;

	} while ((current_frame->next_frame != NULL) && (current_frame->frame != frame_no));

	if (current_frame->frame == frame_no) return current_frame;

	return NULL;
}

KuduFrame *kudu_item_frame_find_or_add_frame(KuduItemFrame *item_frame, int frame_no)
{
	if (item_frame == NULL) return NULL;
	if (frame_no < 0) return NULL;

	int found = FALSE;
	KuduFrame *frame, *current_frame = NULL, *previous_frame;

	if (item_frame->first_frame == NULL) {
		frame = kudu_frame_new(NULL, frame_no);
		if (frame == NULL) return NULL;
		item_frame->first_frame = frame;
	} else {
		do {
			previous_frame = current_frame;
			if (current_frame == NULL) current_frame = item_frame->first_frame;
			else	current_frame = current_frame->next_frame;

			if (current_frame->frame == frame_no) return current_frame; /* Frame already exists */
			if (current_frame->frame > frame_no) found = TRUE;
		} while ((!found) && (current_frame->next_frame != NULL));

		if (found) {	/* A frame of higher value than the new one to be already exists */
			frame = kudu_frame_new(previous_frame, frame_no);
			if (frame == NULL) return NULL;
			if (previous_frame == NULL) {	/* We need to insert new frame before the current "first" frame */
				current_frame = item_frame->first_frame;
				item_frame->first_frame = frame;
				current_frame->previous_frame = frame;
			}
		} else {	/* There are no frames with a value higher than the new one to be - add it to the end */
			frame = kudu_frame_new(current_frame, frame_no);
			if (frame == NULL) return NULL;
		}

	}

	frame->item_frame = item_frame;

	return frame;
}

int kudu_frame_list_set_frame_operation(KuduFrameList *frame_list, KuduObject *object, void *item, int frame_no,
				       K_FRAME_OPERATION frame_op)
{
	if (frame_list == NULL) return FALSE;
	if (object == NULL) return FALSE;
	if (item == NULL) return FALSE;

	KuduObjectFrame *obj_frame;
	KuduItemFrame *item_frame;
	KuduFrame *frame;
	KuduBone *bone;
	KuduVertex *vertex;
	KuduObject *sobject;
	int a;

	obj_frame = kudu_frame_list_find_or_add_object_frame(frame_list, object);
	if (obj_frame == NULL) return FALSE;

	item_frame = kudu_object_frame_find_or_add_item_frame(obj_frame, item);
	if (item_frame == NULL) return FALSE;

	frame = kudu_item_frame_find_or_add_frame(item_frame, frame_no);
	if (frame == NULL) return FALSE;

	switch (frame_op) {
		case OPERATION_NONE:
			break;
		case OPERATION_BONE_SET:
			bone = (KuduBone*)item;
			frame->operation = OPERATION_BONE_SET;
			/*frame->fdata[0] = bone->hAngle;
			frame->fdata[1] = bone->vAngle;
			frame->fdata[2] = bone->rAngle;
			frame->fdata[3] = bone->length;*/
			break;
		case OPERATION_VERTEX_SET:
			vertex = (KuduVertex*)item;
			frame->operation = OPERATION_VERTEX_SET;
			for (a = 0; a < 3; a++) frame->fdata[a] = vertex->ov[a];
			break;
		case OPERATION_OBJECT_SET:
			sobject = (KuduObject*)item;
			frame->operation = OPERATION_OBJECT_SET;
			for (a = 0; a < 3; a++) frame->fdata[a] = sobject->position[a];
			break;
	}

	if (frame_no > frame_list->max_frame) frame_list->max_frame = frame_no;
	return TRUE;
}

int kudu_frame_list_clear_frame(KuduFrameList *frame_list, KuduObject *object, void *item, int frame_no)
{
	if (frame_list == NULL) return FALSE;
	if (object == NULL) return FALSE;
	if (item == NULL) return FALSE;

	KuduObjectFrame *obj_frame;
	KuduItemFrame *item_frame;
	KuduFrame *frame;

	obj_frame = kudu_frame_list_find_object_frame(frame_list, object);
	if (obj_frame == NULL) return FALSE;

	item_frame = kudu_object_frame_find_item_frame(obj_frame, item);
	if (item_frame == NULL) return FALSE;

	frame = kudu_item_frame_find_frame(item_frame, frame_no);
	if (frame == NULL) return FALSE;

	return kudu_frame_destroy(frame);
}

int kudu_frame_perform_operation(KuduItemFrame *item_frame, KuduFrame *frame)
{
	if (item_frame == NULL) return FALSE;
	if (item_frame->item == NULL) return FALSE;
	if (frame == NULL) return FALSE;

	KuduBone *bone;
	KuduVertex *vertex;
	KuduObject *object;
	int a;

	switch (frame->operation) {
		case OPERATION_NONE:
			break;
		case OPERATION_BONE_SET:
			bone = (KuduBone*)item_frame->item;
			/*bone->hAngle = frame->fdata[0];
			bone->vAngle = frame->fdata[1];
			bone->rAngle = frame->fdata[2];
			bone->length = frame->fdata[3];*/
			kudu_bone_magic_touch(bone);
			break;
		case OPERATION_VERTEX_SET:
			vertex = (KuduVertex*)item_frame->item;
			for (a = 0; a < 3; a++) vertex->ov[a] = frame->fdata[a];
			break;
		case OPERATION_OBJECT_SET:
			object = (KuduObject*)item_frame->item;
			for (a = 0; a < 3; a++) object->position[a] = frame->fdata[a];
			break;
	}

	return TRUE;
}

int kudu_frame_perform_average_operation(KuduItemFrame *item_frame, KuduFrame *before, KuduFrame *after, int near_frame)
{
	if (item_frame == NULL) return FALSE;
	if (before == NULL) return FALSE;
	if (after == NULL) return FALSE;

	if (before->operation != after->operation) return FALSE;

	KuduBone *bone;
	KuduVertex *vertex;
	KuduObject *object;
	int frame_diff, a, frames;
	float tmp[4];

	frame_diff = (after->frame - before->frame);
	frames = (near_frame - before->frame);

	switch (before->operation) {
		case OPERATION_NONE:
			break;
		case OPERATION_BONE_SET:
			bone = (KuduBone*)item_frame->item;
			for (a = 0; a < 4; a++) {
				tmp[a] = (after->fdata[a] - before->fdata[a]);
				if (a < 4) {
					if (tmp[a] > 180) tmp[a] -= 360;
					else if (tmp[a] < -180) tmp[a] += 360;
				}

				if (tmp[a] != 0.0) {
					tmp[a] /= frame_diff;
					tmp[a] *= frames;
				}
			}

			/*bone->hAngle = before->fdata[0] + tmp[0];
			bone->vAngle = before->fdata[1] + tmp[1];
			bone->rAngle = before->fdata[2] + tmp[2];
			bone->length = before->fdata[3] + tmp[3];
			kudu_math_degrees_clamp(&bone->hAngle);
			kudu_math_degrees_clamp(&bone->vAngle);
			kudu_math_degrees_clamp(&bone->rAngle);*/
			kudu_bone_magic_touch(bone);
			break;
		case OPERATION_VERTEX_SET:
			vertex = (KuduVertex*)item_frame->item;
			for (a = 0; a < 3; a++) {
				tmp[a] = (after->fdata[a] - before->fdata[a]);
				if (tmp[a] != 0.0) {
					tmp[a] /= frame_diff;
					tmp[a] *= frames;
				}
			}

			for (a = 0; a < 3; a++) vertex->ov[a] = before->fdata[a] + tmp[a];
			break;
		case OPERATION_OBJECT_SET:
			object = (KuduObject*)item_frame->item;
			for (a = 0; a < 3; a++) {
				tmp[a] = (after->fdata[a] - before->fdata[a]);
				if (tmp[a] != 0.0) {
					tmp[a] /= frame_diff;
					tmp[a] *= frames;
				}
			}

			for (a = 0; a < 3; a++) object->position[a] = before->fdata[a] + tmp[a];
			break;
	}

	return TRUE;
}

int kudu_frame_list_goto_frame(KuduFrameList *frame_list, int frame_no)
{
	if (frame_list == NULL) return FALSE;
	if (frame_no == frame_list->current_frame) return TRUE;
	if (frame_no < 0) frame_no = 0;
	frame_list->current_frame = frame_no;
	if (frame_list->first_object == NULL) return TRUE;

	KuduObjectFrame *obj_frame = NULL;
	KuduItemFrame *item_frame;
	KuduFrame *frame, *before, *ff;
	KuduBone *bone;
	KuduVertex *vertex;
	int found, past, eol;


	do {
		if (obj_frame == NULL) obj_frame = frame_list->first_object;
		else	obj_frame = obj_frame->next_object;
		if (obj_frame->first_item != NULL) {

			item_frame = NULL;
			do {
				if (item_frame == NULL) item_frame = obj_frame->first_item;
				else	item_frame = item_frame->next_item;
				if (item_frame->first_frame != NULL) {

					if (item_frame->last_frame != NULL) {
						ff = item_frame->last_frame;
						if (ff->frame > frame_no) ff = item_frame->first_frame;
					} else	ff = item_frame->first_frame;

					frame = NULL;
					found = FALSE;
					past = FALSE;
					eol = FALSE;
					do {
						before = frame;
						if (frame == NULL) frame = ff;
						else	frame = frame->next_frame;

						if (frame->frame == frame_no) found = TRUE;	/* Exact frame found */
						if (frame->frame > frame_no) past = TRUE;	/* frame not found but "higher" exists */
						if (frame->next_frame == NULL) eol = TRUE;	/* Highest frame found is "lower" */
					} while ((!found) && (!past) && (!eol));

					if (found) { /* Exact frame has been found */
						item_frame->last_frame = frame;
						kudu_frame_perform_operation(item_frame, frame);
					} else if (past) { /* A "lower" and a "greater" frame have been found */
						kudu_frame_perform_average_operation(item_frame, before, frame, frame_no);
						item_frame->last_frame = before;
					} else if (eol) { /* Only a "lower" frame has been found */
						item_frame->last_frame = frame;
						kudu_frame_perform_operation(item_frame, frame);
					}
				}
			} while (item_frame->next_item != NULL);
		}
	} while (obj_frame->next_object != NULL);

}

int kudu_frame_list_save_to_file(KuduFrameList *frame_list, char *filename)
{
	if (frame_list == NULL) return FALSE;
	if (filename == NULL) return FALSE;




}

