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
#include "vertices.h"

static int VERTEX_ID = 1;
static KuduVertex *do_vertex = NULL;

int kudu_vertex_force_next_id(KuduObject *object, int id)
{
	if (object == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	object->next_vertex_id = id;

	return TRUE;
}

int kudu_vertex_id_counter_reset(KuduObject *object)
{
	if (object == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	object->next_vertex_id = 1;

	return TRUE;
}

int kudu_vertex_next_id(KuduObject *object)
{
	if (object == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	return object->next_vertex_id;
}

KuduVertex *kudu_vertex_new(KuduObject *object, KuduVertex *previous_vertex)
{
	if (object == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return NULL;
	}

	KuduVertex *vertex, *old_next_vertex;
	int a;

	vertex = (KuduVertex*)malloc(sizeof(KuduVertex));
	if (vertex == NULL) return NULL;

	vertex->id = object->next_vertex_id++;

	if (previous_vertex == NULL) {
		vertex->previous_vertex = NULL;
		vertex->next_vertex = NULL;
	} else {
		vertex->previous_vertex = previous_vertex;
		old_next_vertex = previous_vertex->next_vertex;
		vertex->next_vertex = old_next_vertex;
		previous_vertex->next_vertex = vertex;

		if (old_next_vertex != NULL) old_next_vertex->previous_vertex = vertex;
	}

	vertex->bone = NULL;
	vertex->edge = NULL;

	for (a = 0; a < 3; a++) {
		vertex->v[a] = 0.0;
		vertex->av[a] = 0.0;
		vertex->ov[a] = 0.0;
		vertex->n[a] = 0.0;
		vertex->tv[a] = 0.0;
	}

	vertex->attached_percent = 100;
	vertex->flags = 0;
	vertex->magic_normal = 0;
	vertex->selected = FALSE;

	return vertex;
}

int kudu_vertex_destroy(KuduVertex *vertex)
{
	KuduVertex *previous_vertex = NULL, *next_vertex = NULL;

	if (vertex == NULL) return FALSE;

	previous_vertex = vertex->previous_vertex;
	next_vertex = vertex->next_vertex;

	if (previous_vertex != NULL) previous_vertex->next_vertex = next_vertex;
	if (next_vertex != NULL) next_vertex->previous_vertex = previous_vertex;

	free(vertex);
	vertex = NULL;

	return TRUE;
}

int kudu_vertex_destroy_all(KuduVertex *vertex)
{
	KuduVertex *current_vertex = NULL, *first_vertex = NULL, *destroy_vertex = NULL;

	if (vertex == NULL) return FALSE;

	do {
		if (first_vertex == NULL) first_vertex = vertex;
		else	first_vertex = first_vertex->previous_vertex;
	} while (first_vertex->previous_vertex != NULL);

	do {
		destroy_vertex = current_vertex;
		if (current_vertex == NULL) current_vertex = first_vertex;
		else	current_vertex = current_vertex->next_vertex;

		if (destroy_vertex != NULL) kudu_vertex_destroy(destroy_vertex);
		
	} while (current_vertex->next_vertex != NULL);

	kudu_vertex_destroy(current_vertex);

	vertex = NULL;
	
	return TRUE;
}

KuduVertex *kudu_vertex_new_with_id(KuduObject *object, KuduVertex *previous_vertex, int id)
{
	if (object == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return NULL;
	}

	KuduVertex *vertex;

	vertex = kudu_vertex_new(object, previous_vertex);

	vertex->id = id;

	if (id <= object->next_vertex_id) object->next_vertex_id = id+1;

	return vertex;
}

KuduVertex *kudu_vertex_find_with_id(KuduVertex *vertex, int find_id)
{
	int parse_backwards, found_or_eol;
	KuduVertex *current_vertex;

	if (vertex == NULL) return NULL;

	for (parse_backwards = 0; parse_backwards < 2; parse_backwards++) {
		current_vertex = NULL;

		do {
			if (current_vertex == NULL) current_vertex = vertex;
			else {
				if (!parse_backwards) current_vertex = current_vertex->next_vertex;
				else	current_vertex = current_vertex->previous_vertex;
			}

			if (!parse_backwards) {
				if ((current_vertex->next_vertex != NULL) && (current_vertex->id != find_id))
					found_or_eol = FALSE;
				else	found_or_eol = TRUE;
			} else {
				if ((current_vertex->previous_vertex != NULL) && (current_vertex->id != find_id))
					found_or_eol = FALSE;
				else	found_or_eol = TRUE;
			}

		} while (!found_or_eol);

		if (current_vertex->id == find_id) return current_vertex;
	}

	return NULL;
}

int kudu_vertices_find_with_ids(KuduVertex *vertex, int num, int *ids, KuduVertex **vertex_list)
{
	if (vertex == NULL) return 0;
	if (ids == NULL) return 0;
	if (vertex_list == NULL) return 0;
	if (num == 0) return 0;

	int parse_backwards, eol, num_found = 0, a;
	KuduVertex *current_vertex;

	for (a = 0; a < num; a++) vertex_list[a] = NULL;

	for (parse_backwards = 0; parse_backwards < 2; parse_backwards++) {
		current_vertex = NULL;

		do {
			if (current_vertex == NULL) current_vertex = vertex;
			else {
				if (!parse_backwards) current_vertex = current_vertex->next_vertex;
				else	current_vertex = current_vertex->previous_vertex;
			}

			for (a = 0; a < num; a++) {
				if (current_vertex->id == ids[a]) {
					vertex_list[a] = current_vertex;
					num_found++;
				}
			}

			if (!parse_backwards) {
				if (current_vertex->next_vertex == NULL) eol = TRUE;
				else eol = FALSE;
			} else {
				if (current_vertex->previous_vertex == NULL) eol = TRUE;
				else eol = FALSE;
			}
		} while ((num_found < num) && (!eol));

		if (num_found == num) return num_found;
	}

	return num_found;
}

int kudu_vertex_for_each_do(KuduVertex *vertex)
{
	KuduVertex *first_vertex = NULL;
	if (vertex == NULL) return FALSE;

	do {
		if (first_vertex == NULL) first_vertex = vertex;
		else	first_vertex = first_vertex->previous_vertex;

	} while (vertex->previous_vertex != NULL);

	do_vertex = first_vertex;

	return TRUE;
}

KuduVertex *kudu_vertex_next_do(void)
{
	KuduVertex *return_vertex;

	if (do_vertex == NULL) return NULL;

	return_vertex = do_vertex;

	do_vertex = do_vertex->next_vertex;

	return return_vertex;
}

int kudu_vertex_attach_to_bone(KuduVertex *vertex, KuduBone *bone)
{
	int a, b;
	float x, y, z;

	if (vertex == NULL) return FALSE;
	if (bone == NULL) return FALSE;

	/*for (a = 0; a < 3; a++) vertex->ov[a] = vertex->v[a];*/

	vertex->bone = bone;

	x = (vertex->v[0]);
	y = (vertex->v[1]);
	z = (vertex->v[2]);

	x -= bone->lineX;
	y -= bone->lineY;
	z -= bone->lineZ;

	kudu_math_translate_vertex_inverse(&x, &y, &z, bone->matrix);

	vertex->v[0] = x;
	vertex->v[1] = y;
	vertex->v[2] = z;
	vertex->attached_percent = 100;

	return TRUE;
}

int kudu_vertex_attach_to_bone_by_percent(KuduVertex *vertex, KuduBone *bone, GLubyte percent)
{
	int a, b;
	float x, y, z;
	KuduBone temp_bone;

	if (vertex == NULL) return FALSE;
	if (bone == NULL) return FALSE;

/*	temp_bone = kudu_bone_new_with_id(NULL, 0);
	if (temp_bone == NULL) return FALSE;*/

	if (percent > 100) percent = 100;

	vertex->bone = bone;
	vertex->attached_percent = percent;

	x = (vertex->v[0]);
	y = (vertex->v[1]);
	z = (vertex->v[2]);

	kudu_bone_joint_by_percent(bone, &temp_bone, vertex->attached_percent);

	x -= temp_bone.lineX;
	y -= temp_bone.lineY;
	z -= temp_bone.lineZ;

	kudu_math_translate_vertex_inverse(&x, &y, &z, temp_bone.matrix);

	vertex->v[0] = x;
	vertex->v[1] = y;
	vertex->v[2] = z;

	/*kudu_bone_destroy(temp_bone);*/

	return TRUE;
}


int kudu_vertex_detach(KuduVertex *vertex)
{
	int a;
	float x, y, z;
	KuduBone temp_bone;

	if (vertex == NULL) return FALSE;
	if (vertex->bone == NULL) return FALSE;

	/*temp_bone = kudu_bone_new_with_id(NULL, 0);
	if (temp_bone == NULL) return FALSE;*/

	kudu_bone_joint_by_percent(vertex->bone, &temp_bone, vertex->attached_percent);

	x = vertex->v[0];
	y = vertex->v[1];
	z = vertex->v[2];

	kudu_math_translate_vertex(&x, &y, &z, temp_bone.matrix);

	vertex->v[0] = x + temp_bone.lineX;
	vertex->v[1] = y + temp_bone.lineY;
	vertex->v[2] = z + temp_bone.lineZ;

	vertex->bone = NULL;
	vertex->attached_percent = 100;

	/*kudu_bone_destroy(temp_bone);*/

	return TRUE;
}

int kudu_vertex_update(KuduVertex *vertex)
{
	if (vertex == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	KuduBone *bone, temp_bone;
	int a;

	/* Start with initial vertex position */
	for (a = 0; a < 3; a++) vertex->av[a] = vertex->v[a];

	/* Transform the vertex with its bone matrix */
	if ((bone = vertex->bone) != NULL) {
		if (vertex->attached_percent < 100) {	/* Not 100% attached, calculate a new matrix */
			kudu_bone_joint_by_percent(bone, &temp_bone, vertex->attached_percent);
			kudu_math_translate_vertex(&vertex->av[0], &vertex->av[1], &vertex->av[2], temp_bone.matrix);

			vertex->av[0] += temp_bone.lineX;
			vertex->av[1] += temp_bone.lineY;
			vertex->av[2] += temp_bone.lineZ;
		} else {	/* 100% attached, just use the bone matrix */
			kudu_math_translate_vertex(&vertex->av[0], &vertex->av[1], &vertex->av[2], bone->matrix);

			vertex->av[0] += bone->lineX;
			vertex->av[1] += bone->lineY;
			vertex->av[2] += bone->lineZ;
		}
	}

	/* Add the vertex offset */
	for (a = 0; a < 3; a++) vertex->av[a] += vertex->ov[a];
	return TRUE;
}

int kudu_vertex_update_vertices(KuduVertex *vertex)
{
	int parse_backwards, end_of_list, a;
	KuduVertex *current_vertex;

	if (vertex == NULL) return FALSE;

	/* Parse the list twice first forwards then backwards */
	/* This way we can use any vertex in the list to re-calculate them all */
	for (parse_backwards = 0; parse_backwards < 2; parse_backwards++) {
		current_vertex = NULL;

		do {
			if (current_vertex == NULL) current_vertex = vertex;
			else {
				if (!parse_backwards) current_vertex = current_vertex->next_vertex;
				else	current_vertex = current_vertex->previous_vertex;
			}

			kudu_vertex_update(current_vertex);

			/* Add the vertex offset */
			/*for (a = 0; a < 3; a++) current_vertex->av[a] += current_vertex->ov[a];*/

			if (!parse_backwards) {
				if (current_vertex->next_vertex == NULL) end_of_list = TRUE;
				else	end_of_list = FALSE;
			} else {
				if (current_vertex->previous_vertex == NULL) end_of_list = TRUE;
				else	end_of_list = FALSE;
			}

		} while (!end_of_list);
	}

	return TRUE;
}

/* Smart vertex update only updates a vertex if it's attached bone or its ancestory is selected */
int kudu_vertex_smart_update_vertices(KuduVertex *vertex)
{
	int parse_backwards, end_of_list, a;
	KuduVertex *current_vertex;
	KuduBone *bone;

	if (vertex == NULL) return FALSE;

	/* Parse the list twice first forwards then backwards */
	/* This way we can use any vertex in the list to re-calculate them all */
	for (parse_backwards = 0; parse_backwards < 2; parse_backwards++) {
		current_vertex = NULL;

		do {
			if (current_vertex == NULL) current_vertex = vertex;
			else {
				if (!parse_backwards) current_vertex = current_vertex->next_vertex;
				else	current_vertex = current_vertex->previous_vertex;
			}

			bone = current_vertex->bone;
			if (bone != NULL) {
				if (bone->selected) kudu_vertex_update(current_vertex);
			}

			if (!parse_backwards) {
				if (current_vertex->next_vertex == NULL) end_of_list = TRUE;
				else	end_of_list = FALSE;
			} else {
				if (current_vertex->previous_vertex == NULL) end_of_list = TRUE;
				else	end_of_list = FALSE;
			}

		} while (!end_of_list);
	}

	return TRUE;
}


