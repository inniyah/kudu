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
#include "wingedge.h"

static KuduVertex *gve_vertex = NULL;
static KuduShape *do_shape = NULL;
static KuduFace *do_face = NULL, *face_vert = NULL, *gfe_face = NULL;
static KuduEdge *do_edge = NULL, *edge_vert = NULL, *gfe_edge = NULL, *gve_edge = NULL;

int kudu_edge_force_next_id(KuduObject *object, int id)
{
	if (object == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	object->next_edge_id = id;

	return TRUE;
}

int kudu_edge_next_id(KuduObject *object)
{
	if (object == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	return object->next_edge_id;
}

int kudu_face_force_next_id(KuduObject *object, int id)
{
	if (object == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	object->next_face_id = id;
	return TRUE;
}

int kudu_face_next_id(KuduObject *object)
{
	if (object == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	return object->next_face_id;
}

int kudu_shape_force_next_id(KuduObject *object, int id)
{
	if (object == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	object->next_shape_id = id;
	return TRUE;
}

int kudu_shape_next_id(KuduObject *object)
{
	if (object == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	return object->next_shape_id;
}

KuduEdge *kudu_edge_new(KuduObject *object, KuduEdge *previous_edge)
{
	if (object == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return NULL;
	}

	KuduEdge *edge = NULL, *old_next_edge;

	edge = (KuduEdge*)malloc(sizeof(KuduEdge));
	if (edge == NULL) return NULL;

	if (previous_edge == NULL) {
		edge->next_edge = NULL;
		edge->previous_edge = NULL;
		edge->shape = NULL;
	} else {
		edge->previous_edge = previous_edge;
		old_next_edge = previous_edge->next_edge;
		edge->next_edge = old_next_edge;
		previous_edge->next_edge = edge;

		if (old_next_edge != NULL) old_next_edge->previous_edge = edge;
		edge->shape = previous_edge->shape;
	}

	edge->id = object->next_edge_id++;
	edge->selected = FALSE;
	edge->hard = FALSE;
	edge->left_face = NULL;
	edge->right_face = NULL;
	edge->start_vertex = NULL;
	edge->end_vertex = NULL;
	edge->left_succ = NULL;
	edge->left_pred = NULL;
	edge->right_succ = NULL;
	edge->right_pred = NULL;

	return edge;
}

KuduEdge *kudu_edge_new_with_id(KuduObject *object, KuduEdge *previous_edge, int id)
{
	KuduEdge *edge;

	edge = kudu_edge_new(object, previous_edge);

	edge->id = id;

	if (id <= object->next_edge_id) object->next_edge_id = id+1;

	return edge;
}

KuduFace *kudu_face_new(KuduObject *object, KuduFace *previous_face)
{
	if (object == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	KuduFace *face = NULL, *old_next_face;

	face = (KuduFace*)malloc(sizeof(KuduFace));
	if (face == NULL) return NULL;

	if (previous_face == NULL) {
		face->next_face = NULL;
		face->previous_face = NULL;
	} else {
		face->previous_face = previous_face;
		old_next_face = previous_face->next_face;
		face->next_face = old_next_face;
		previous_face->next_face = face;

		if (old_next_face != NULL) old_next_face->previous_face = face;
	}

	face->id = object->next_face_id++;
	face->selected = FALSE;
	face->edge = NULL;
	face->material = NULL;

	return face;
}

KuduFace *kudu_face_new_with_id(KuduObject *object, KuduFace *previous_face, int id)
{
	if (object == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	KuduFace *face = kudu_face_new(object, previous_face);

	face->id = id;

	if (id <= object->next_face_id) object->next_face_id = id+1;

	return face;
}

KuduShape *kudu_shape_new(KuduObject *object, KuduShape *previous_shape)
{
	if (object == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	KuduShape *shape = NULL, *old_next_shape;

	shape = (KuduShape*)malloc(sizeof(KuduShape));
	if (shape == NULL) return NULL;

	if (previous_shape == NULL) {
		shape->next_shape = NULL;
		shape->previous_shape = NULL;
	} else {
		shape->previous_shape = previous_shape;
		old_next_shape = previous_shape->next_shape;
		shape->next_shape = old_next_shape;
		previous_shape->next_shape = shape;

		if (old_next_shape != NULL) old_next_shape->previous_shape = shape;
	}

	shape->id = object->next_shape_id;
	shape->selected = FALSE;

	shape->face = NULL;
	shape->edge = NULL;
	shape->vertex = NULL;

	return shape;
}

KuduShape *kudu_shape_new_with_id(KuduObject *object, KuduShape *previous_shape, int id)
{
	if (object == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	KuduShape *shape = kudu_shape_new(object, previous_shape);

	shape->id = id;

	if (id <= object->next_shape_id) object->next_shape_id = id+1;

	return shape;
}

int kudu_edge_vertices_set(KuduEdge *edge, KuduVertex *start_vertex, KuduVertex *end_vertex)
{
	if (edge == NULL) return FALSE;

	if (start_vertex != NULL) {
		edge->start_vertex = start_vertex;
		if (start_vertex->edge == NULL) start_vertex->edge = edge;
	}

	if (end_vertex != NULL) {
		edge->end_vertex = end_vertex;
		if (end_vertex->edge == NULL) end_vertex->edge = edge;
	}

	return TRUE;
}

int kudu_edge_faces_set(KuduEdge *edge, KuduFace *left_face, KuduFace *right_face)
{
	if (edge == NULL) return FALSE;

	if (left_face != NULL) {
		edge->left_face = left_face;
		if (left_face->edge == NULL) left_face->edge = edge;
	}

	if (right_face != NULL) {
		edge->right_face = right_face;
		if (right_face->edge == NULL) right_face->edge = edge;
	}

	return TRUE;
}

int kudu_edge_wings_set(KuduEdge *edge, KuduEdge *left_succ, KuduEdge *left_pred, KuduEdge *right_succ, KuduEdge *right_pred)
{
	if (edge == NULL) return FALSE;

	if (left_succ != NULL) {
		edge->left_succ = left_succ;
/*		if ((left_succ->left_face == edge->left_face) || (left_succ->right_face == edge->right_face))
			left_succ->left_pred = edge;
		else if ((left_succ->left_face == edge->right_face) || (left_succ->right_face == edge->left_face))
			left_succ->right_pred = edge;*/
	}

	if (left_pred != NULL) {
		edge->left_pred = left_pred;
/*		if ((left_pred->left_face == edge->left_face) || (left_pred->right_face == edge->right_face))
			left_pred->left_succ = edge;
		else if ((left_pred->left_face == edge->right_face) || (left_pred->right_face == edge->left_face))
			left_pred->right_succ = edge;*/
	}

	if (right_succ != NULL) {
		edge->right_succ = right_succ;
/*		if ((right_succ->left_face == edge->left_face) || (right_succ->right_face == edge->right_face))
			right_succ->right_pred = edge;
		else if ((right_succ->left_face == edge->right_face) || (right_succ->right_face == edge->left_face))
			right_succ->right_pred = edge;*/
	}

	if (right_pred != NULL) {
		edge->right_pred = right_pred;
/*		if ((right_pred->left_face == edge->left_face) || (right_pred->right_face == edge->right_face))
			right_pred->left_succ = edge;
		else if ((right_pred->left_face == edge->right_face) || (right_pred->right_face == edge->left_face))
			right_pred->right_succ = edge;*/
	}

	return TRUE;
}

KuduEdge *kudu_edge_find_with_vertex_ids(KuduEdge *edge, int id1, int id2)
{
	if (edge == NULL) return NULL;

	KuduEdge *current_edge;
	KuduVertex *s_vertex, *e_vertex;
	int found, eol, parse_backwards;

	for (parse_backwards = 0; parse_backwards < 2; parse_backwards++) {
		current_edge = NULL;

		do {
			if (current_edge == NULL) current_edge = edge;
			else {
				if (!parse_backwards) current_edge = current_edge->next_edge;
				else	current_edge = current_edge->previous_edge;
			}

			s_vertex = current_edge->start_vertex;
			e_vertex = current_edge->end_vertex;
			if ((s_vertex != NULL) && (e_vertex != NULL)) {
				if (((s_vertex->id == id1) || (s_vertex->id == id2)) &&
				    ((e_vertex->id == id1) || (e_vertex->id == id2))) found = TRUE;
				else	found = FALSE;
			} else	found = FALSE;

			if (!parse_backwards) {
				if (current_edge->next_edge != NULL) eol = FALSE;
				else	eol = TRUE;
			} else {
				if (current_edge->previous_edge != NULL) eol = FALSE;
				else	eol = TRUE;
			}

		} while ((!found) && (!eol));

		if (found) return current_edge;
	}

	return NULL;
}

KuduFace *kudu_face_find_with_id(KuduFace *face, int id)
{
	if (face == NULL) return NULL;

	KuduFace *current_face;
	int parse_backwards, found, eol;

	for (parse_backwards = 0; parse_backwards < 2; parse_backwards++) {
		current_face = NULL;

		do {
			if (current_face == NULL) current_face = face;
			else {
				if (!parse_backwards) current_face = current_face->next_face;
				else	current_face = current_face->previous_face;
			}

			if (current_face->id == id) found = TRUE;
			else	found = FALSE;

			if (!parse_backwards) {
				if (current_face->next_face == NULL) eol = TRUE;
				else eol = FALSE;
			} else {
				if (current_face->previous_face == NULL) eol = TRUE;
				else eol = FALSE;
			}
		} while ((!found) && (!eol));

		if (found) return current_face;
	}

	return NULL;
}

int kudu_faces_find_with_ids(KuduFace *face, int num, int *ids, KuduFace **face_list)
{
	if (face == NULL) return 0;
	if (ids == NULL) return 0;
	if (face_list == NULL) return 0;
	if (num == 0) return 0;

	KuduFace *current_face;
	int parse_backwards, num_found = 0, eol, a;

	for (a = 0; a < num; a++) face_list[a] = NULL;

	for (parse_backwards = 0; parse_backwards < 2; parse_backwards++) {
		current_face = NULL;

		do {
			if (current_face == NULL) current_face = face;
			else {
				if (!parse_backwards) current_face = current_face->next_face;
				else	current_face = current_face->previous_face;
			}

			for (a = 0; a < num; a++) {
				if (current_face->id == ids[a]) {
					face_list[a] = current_face;
					num_found++;
				}
			}

			if (!parse_backwards) {
				if (current_face->next_face == NULL) eol = TRUE;
				else eol = FALSE;
			} else {
				if (current_face->previous_face == NULL) eol = TRUE;
				else eol = FALSE;
			}
		} while ((num_found < num) && (!eol));

		if (num_found == num) return num_found;
	}

	return num_found;
}

int kudu_face_destroy(KuduFace* face)
{
	if (face == NULL) return FALSE;
	KuduFace *previous_face, *next_face;
	KuduEdge *edge;

	previous_face = face->previous_face;
	next_face = face->next_face;
	edge = face->edge;

	if (previous_face != NULL) previous_face->next_face = next_face;
	if (next_face != NULL) next_face->previous_face = previous_face;

	if (edge != NULL) {
		if (edge->left_face == face) edge->left_face = edge->right_face;
		else if (edge->right_face == face) edge->right_face = edge->left_face;
	}

	face->material = NULL;
	face->edge = NULL;

	free(face);

	return TRUE;
}

int kudu_edge_destroy(KuduEdge* edge)
{
	if (edge == NULL) return FALSE;

	KuduEdge *previous_edge, *next_edge;
	KuduEdge *left_succ, *left_pred, *right_succ, *right_pred;
	KuduFace *left_face, *right_face;
	KuduVertex *start_vertex, *end_vertex;

	next_edge = edge->next_edge;
	previous_edge = edge->previous_edge;
	left_pred = edge->left_pred;
	left_succ = edge->left_succ;
	right_pred = edge->right_pred;
	right_succ = edge->right_succ;
	left_face = edge->left_face;
	right_face = edge->right_face;
	start_vertex = edge->start_vertex;
	end_vertex = edge->end_vertex;

	if (previous_edge != NULL) previous_edge->next_edge = next_edge;
	if (next_edge != NULL) next_edge->previous_edge = previous_edge;

	if (start_vertex != NULL)
		if (start_vertex->edge == edge) start_vertex->edge = NULL;
	if (end_vertex != NULL)
		if (end_vertex->edge == edge) end_vertex->edge = NULL;
	if (left_face != NULL)
		if (left_face->edge == edge) left_face->edge = NULL;
	if (right_face != NULL)
		if (right_face->edge == edge) right_face->edge = NULL;


	edge->left_face = NULL;
	edge->right_face = NULL;
	edge->start_vertex = NULL;
	edge->end_vertex = NULL;

	free(edge);

	return TRUE;
}

int kudu_face_destroy_all(KuduFace *face)
{
	if (face == NULL) return FALSE;

	KuduFace *first_face = NULL, *current_face = NULL, *destroy_face = NULL;

	do {
		if (first_face == NULL) first_face = face;
		else	first_face = first_face->next_face;
	} while (first_face->previous_face != NULL);

	do {
		destroy_face = current_face;
		if (current_face == NULL) current_face = first_face;
		else	current_face = current_face->next_face;

		kudu_face_destroy(destroy_face);

	} while (current_face->next_face != NULL);

	kudu_face_destroy(current_face);

	return TRUE;	
}

int kudu_edge_destroy_all(KuduEdge *edge)
{
	if (edge == NULL) return FALSE;

	KuduEdge *first_edge = NULL, *current_edge = NULL, *destroy_edge = NULL;

	do {
		if (first_edge == NULL) first_edge = edge;
		else	first_edge = first_edge->previous_edge;
	} while (first_edge->previous_edge != NULL);


	do {
		destroy_edge = current_edge;
		if (current_edge == NULL) current_edge = first_edge;
		else	current_edge = current_edge->next_edge;

		kudu_edge_destroy(destroy_edge);

	} while (current_edge->next_edge != NULL);

	kudu_edge_destroy(current_edge);

	return TRUE;
}

int kudu_shape_destroy(KuduShape *shape)
{
	if (shape == NULL) return FALSE;

	KuduShape *next_shape, *previous_shape;

	next_shape = shape->next_shape;
	previous_shape = shape->previous_shape;

	kudu_vertex_destroy_all(shape->vertex);
	kudu_face_destroy_all(shape->face);
	kudu_edge_destroy_all(shape->edge);

	if (previous_shape != NULL) previous_shape->next_shape = next_shape;
	if (next_shape != NULL) next_shape->previous_shape = previous_shape;

	free(shape);

	return TRUE;
}

int kudu_shape_destroy_all(KuduShape *shape)
{
	if (shape == NULL) return FALSE;

	KuduShape *first_shape = NULL, *current_shape = NULL, *destroy_shape = NULL;

	do {
		if (first_shape == NULL) first_shape = shape;
		else	first_shape = first_shape->previous_shape;
	} while (first_shape->previous_shape != NULL);

	do {
		destroy_shape = current_shape;
		if (current_shape == NULL) current_shape = first_shape;
		else	current_shape = current_shape->next_shape;

		kudu_shape_destroy(destroy_shape);

	} while (current_shape->next_shape != NULL);

	kudu_shape_destroy(current_shape);

	return TRUE;
}

KuduShape *kudu_shape_find_with_id(KuduShape *shape, int id)
{
	if (shape == NULL) return NULL;

	KuduShape *current_shape;
	int parse_backwards, found, eol;

	for (parse_backwards = 0; parse_backwards < 2; parse_backwards++) {
		current_shape = NULL;
		eol = FALSE;

		do {
			if (current_shape == NULL) current_shape = shape;
			else {
				if (!parse_backwards) current_shape = current_shape->next_shape;
				else	current_shape = current_shape->previous_shape;
			}

			if (current_shape->id == id) found = TRUE;
			else	found = FALSE;

			if (!parse_backwards) {
				if (current_shape->next_shape == NULL) eol = TRUE;
			} else {
				if (current_shape->previous_shape == NULL) eol = TRUE;
			}
		} while ((!found) && (!eol));

		if (found) return current_shape;
	}

	return NULL;
}

KuduEdge *kudu_edge_find_with_id(KuduEdge *edge, int id)
{
	if (edge == NULL) return NULL;

	KuduEdge *current_edge;
	int parse_backwards, found, eol;

	for (parse_backwards = 0; parse_backwards < 2; parse_backwards++) {
		current_edge = NULL;

		do {
			if (current_edge == NULL) current_edge = edge;
			else {
				if (!parse_backwards) current_edge = current_edge->next_edge;
				else	current_edge = current_edge->previous_edge;
			}

			if (current_edge->id == id) found = TRUE;
			else	found = FALSE;

			if (!parse_backwards) {
				if (current_edge->next_edge == NULL) eol = TRUE;
				else eol = FALSE;
			} else {
				if (current_edge->previous_edge == NULL) eol = TRUE;
				else eol = FALSE;
			}
		} while ((!found) && (!eol));

		if (found) return current_edge;
	}

	return NULL;
}

int kudu_edges_find_with_ids(KuduEdge *edge, int num, int *ids, KuduEdge **edge_list)
{
	if (edge == NULL) return 0;
	if (ids == NULL) return 0;
	if (edge_list == NULL) return 0;
	if (num == 0) return 0;

	KuduEdge *current_edge;
	int parse_backwards, num_found = 0, eol, a;
	for (a = 0; a < num; a++) edge_list[a] = NULL;

	for (parse_backwards = 0; parse_backwards < 2; parse_backwards++) {
		current_edge = NULL;

		do {
			if (current_edge == NULL) current_edge = edge;
			else {
				if (!parse_backwards) current_edge = current_edge->next_edge;
				else	current_edge = current_edge->previous_edge;
			}

			for (a = 0; a < num; a++) {
				if (current_edge->id == ids[a]) {
					edge_list[a] = current_edge;
					num_found++;
				}
			}

			if (!parse_backwards) {
				if (current_edge->next_edge == NULL) eol = TRUE;
				else eol = FALSE;
			} else {
				if (current_edge->previous_edge == NULL) eol = TRUE;
				else eol = FALSE;
			}
		} while ((num_found < num) && (!eol));

		if (num_found == num) return num_found;
	}

	return num_found;
}

int kudu_shape_for_each_do(KuduShape *shape)
{
	if (shape == NULL) return FALSE;
	KuduShape *first_shape = NULL;

	do {
		if (first_shape == NULL) first_shape = shape;
		else	first_shape = first_shape->previous_shape;
	} while (first_shape->previous_shape != NULL);

	do_shape = first_shape;

	return TRUE;
}

KuduShape *kudu_shape_next_do(void)
{
	KuduShape *return_shape;

	if (do_shape == NULL) return NULL;

	return_shape = do_shape;

	do_shape = do_shape->next_shape;

	return return_shape;
}

int kudu_face_for_each_do(KuduFace *face)
{
	if (face == NULL) return FALSE;
	KuduFace *first_face = NULL;

	do {
		if (first_face == NULL) first_face = face;
		else	first_face = first_face->previous_face;
	 } while (first_face->previous_face != NULL);

	do_face = first_face;

	return TRUE;
}

KuduFace *kudu_face_next_do(void)
{
	if (do_face == NULL) return NULL;

	KuduFace *return_face = do_face;
	do_face = do_face->next_face;

	return return_face;
}

int kudu_edge_for_each_do(KuduEdge *edge)
{
	if (edge == NULL) return FALSE;
	KuduEdge *first_edge = NULL;

	do {
		if (first_edge == NULL) first_edge = edge;
		else	first_edge = first_edge->previous_edge;
	 } while (first_edge->previous_edge != NULL);

	do_edge = first_edge;

	return TRUE;
}

KuduEdge *kudu_edge_next_do(void)
{
	if (do_edge == NULL) return NULL;

	KuduEdge *return_edge = do_edge;
	do_edge = do_edge->next_edge;

	return return_edge;
}

int kudu_face_for_each_vertex_do(KuduFace *face)
{
	if (face == NULL) return FALSE;
	if (face->edge == NULL) return FALSE;

	face_vert = face;
	edge_vert = face->edge;

	return TRUE;
}

KuduVertex *kudu_face_for_each_vertex_next_do(void)
{
	if (face_vert == NULL) return NULL;
	if (edge_vert == NULL) return NULL;

	KuduVertex *vertex;
	static KuduVertex *previous_vertex;

	if (edge_vert->right_face == face_vert) {
		vertex = edge_vert->start_vertex;
		edge_vert = edge_vert->right_succ;
	} else
	if (edge_vert->left_face == face_vert) {
		vertex = edge_vert->end_vertex;
		edge_vert = edge_vert->left_succ;
	}

	if (previous_vertex == vertex) edge_vert = NULL;

	if (edge_vert == face_vert->edge) edge_vert = NULL;

	previous_vertex = vertex;

	return vertex;
}

int kudu_face_for_each_edge_do(KuduFace* face)
{
	if (face == NULL) return FALSE;
	if (face->edge == NULL) return FALSE;

	gfe_face = face;
	gfe_edge = face->edge;

	return TRUE;
}

KuduEdge *kudu_face_for_each_edge_next_do(void)
{
	if (gfe_face == NULL) return NULL;
	if (gfe_edge == NULL) return NULL;

	if (gfe_edge->right_face == gfe_face) {
		gfe_edge = gfe_edge->right_succ;
	} else
	if (gfe_edge->left_face == gfe_face) {
		gfe_edge = gfe_edge->left_succ;
	}

	static KuduEdge *previous_edge;
	KuduEdge *edge = gfe_edge;

	if (previous_edge == edge) gfe_edge = NULL;
	if (gfe_edge == gfe_face->edge) gfe_edge = NULL;

	previous_edge = edge;

	return edge;
}

int kudu_vertex_for_each_edge_do(KuduVertex *vertex)
{
	if (vertex == NULL) return FALSE;
	if (vertex->edge == NULL) return FALSE;

	gve_vertex = vertex;
	gve_edge = vertex->edge;

	return TRUE;
}

KuduEdge *kudu_vertex_for_each_edge_next_do(void)
{
	if (gve_vertex == NULL) return NULL;
	if (gve_edge == NULL) return NULL;

	if (gve_edge->start_vertex == gve_vertex) {
		gve_edge = gve_edge->left_succ;
	} else
	if (gve_edge->end_vertex == gve_vertex) {
		gve_edge = gve_edge->right_succ;
	}

	KuduEdge *edge = gve_edge;
	if (gve_edge == gve_vertex->edge) gve_edge = NULL;

	return edge;
}


int kudu_shape_normals_calculate(KuduShape *shape)
{
	if (shape == NULL) return FALSE;
	if (shape->face == NULL) return FALSE;

	KuduFace *current_face;
	KuduEdge *current_edge, *bnv_edge;
	KuduVertex *current_vertex, *vertex[3];
	unsigned char magic_normal = 1;
	int parse_backwards, eol, bnv, c, a, b;
	GLfloat v[3][3], nv[3];

	if (shape->vertex != NULL) magic_normal = shape->vertex->magic_normal;
	if (magic_normal < 255) magic_normal++;
	else	magic_normal = 0;

	for (parse_backwards = 0; parse_backwards < 2; parse_backwards++) {
		current_face = NULL;

		do {
			if (current_face == NULL) current_face = shape->face;
			else {
				if (!parse_backwards) current_face = current_face->next_face;
				else	current_face = current_face->previous_face;
			}


			current_edge = current_face->edge;
			if (current_edge == NULL) goto eol_test;
			c = 0;
			bnv = 0; /* Bad normal vertices */

			do {
				if (current_edge->right_face == current_face) {
					current_vertex = current_edge->start_vertex;
					current_edge = current_edge->right_succ;
				} else
				if (current_edge->left_face == current_face) {
					current_vertex = current_edge->end_vertex;
					current_edge = current_edge->left_succ;
				} else break;

				if (current_vertex != NULL) {
					/* add this vertex to the vertex list for "flat" normal calculation */
					if (c < 3) {
						vertex[c] = current_vertex;
						for (a = 0; a < 3; a++) v[c][a] = current_vertex->av[a];
						c++;
					} else if (c > 3) { /* "flat" normal already calculated, "append" to vertex */
						if (current_vertex->magic_normal != magic_normal) {
							for (a = 0; a < 3; a++) current_vertex->n[a] = nv[a];
							current_vertex->magic_normal = magic_normal;
						} else {
							for (b = 0; b < 3; b++) current_vertex->n[b] += nv[b];
							kudu_math_vertex_normalize(current_vertex->n);
						}
					}
				}

				/* 3 vertices accumalted caluclate "flat" normal vector from these */
				if (c == 3) {
					if (!kudu_math_normal_vector(vertex[0]->av, vertex[1]->av, vertex[2]->av, nv)) {
						c = 2;
						bnv++;	/* Bad normal vertices */
						if (bnv == 1) bnv_edge = current_edge;
						continue;
					}

					for (a = 0; a < 3; a++) current_face->fn[a] = nv[a];
					c++;

					/* "Append" flat normal vector to each of these vertices */
					for (a = 0; a < 3; a++) {
						if ((bnv) && (a == 2)) break;
						if (vertex[a]->magic_normal != magic_normal) {
							for (b = 0; b < 3; b++) vertex[a]->n[b] = nv[b];
							vertex[a]->magic_normal = magic_normal;
						} else {
							for (b = 0; b < 3; b++) vertex[a]->n[b] += nv[b];
							kudu_math_vertex_normalize(vertex[a]->n);
						}
					}

					if (bnv) current_edge = bnv_edge;
				}

			} while ((current_edge != current_face->edge) && (current_edge != NULL));

			eol_test:
			if (!parse_backwards) {
				if (current_face->next_face == NULL) eol = TRUE;
				else eol = FALSE;
			} else {
				if (current_face->previous_face == NULL) eol = TRUE;
				else	eol = FALSE;
			}

		} while (!eol);

	}

	return TRUE;
}

int kudu_diag_print_edge(KuduEdge *edge)
{
	if (edge == NULL) return FALSE;
	KuduEdge *ls, *lp, *rs, *rp;
	KuduFace *lf, *rf;
	KuduVertex *sv, *ev;

	ls = edge->left_succ;
	lp = edge->left_pred;
	rs = edge->right_succ;
	rp = edge->right_pred;
	rf = edge->right_face;
	lf = edge->left_face;
	sv = edge->start_vertex;
	ev = edge->end_vertex;

	printf("Edge: %d,  ", edge->id);
	if (lf != NULL) printf("LF: %d, ", lf->id); else printf("LF: !NULL!, ");
	if (rf != NULL) printf("RF: %d, ", rf->id); else printf("RF: !NULL!, ");
	if (sv != NULL) printf("SV: %d, ", sv->id); else printf("SV: !NULL!, ");
	if (ev != NULL) printf("EV: %d, ", ev->id); else printf("EV: !NULL!, ");
	if (ls != NULL) printf("LS: %d, ", ls->id); else printf("LS: !NULL!, ");
	if (rs != NULL) printf("RS: %d, ", rs->id); else printf("RS: !NULL!, ");
	if (lp != NULL) printf("LP: %d, ", lp->id); else printf("LP: !NULL!, ");
	if (rp != NULL) printf("RP: %d, ", rp->id); else printf("RP: !NULL!, ");
	printf("\n");

	return TRUE;
}

int kudu_shape_select(KuduShape *shape, int select_opts)
{


}

int kudu_shape_num_vertices(KuduShape *shape)
{
	if (shape == NULL) return 0;
	if (shape->vertex == NULL) return 0;

	KuduVertex *vertex = NULL;
	int count = 0;

	do {
		if (vertex == NULL) vertex = shape->vertex;
		else	vertex = vertex->next_vertex;
		count++;

	} while (vertex->next_vertex != NULL);

	return count;
}

int kudu_shape_num_edges(KuduShape *shape)
{
	if (shape == NULL) return 0;
	if (shape->edge == NULL) return 0;

	KuduEdge *edge = NULL;
	int count = 0;

	do {
		if (edge == NULL) edge = shape->edge;
		else	edge = edge->next_edge;
		count++;

	} while (edge->next_edge != NULL);

	return count;
}

int kudu_shape_num_faces(KuduShape *shape)
{
	if (shape == NULL) return 0;
	if (shape->face == NULL) return 0;

	KuduFace *face = NULL;
	int count = 0;

	do {
		if (face == NULL) face = shape->face;
		else	face = face->next_face;
		count++;

	} while (face->next_face != NULL);

	return count;
}









