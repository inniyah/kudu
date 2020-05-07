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
#include "skin_edit.h"

/* The vertex_magic_touch var keeps track of whether a vertex has been edited yet */
/* for example when editing two adjacent faces, they will be sharing two vertices */
static int vertex_magic_touch = 0;

int kudu_skin_edit_vertex(KuduVertex *vertex, float opt_h, int edit_mode)
{
	if (vertex == NULL) return FALSE;
	if (vertex->magic_touch == vertex_magic_touch) return TRUE;
	vertex->magic_touch = vertex_magic_touch;

	switch (edit_mode) {
		case EDIT_MODE_SHIFT_X:
			vertex->v[0] += (opt_h / 10);
			break;
		case EDIT_MODE_SHIFT_Y:
			vertex->v[1] += (opt_h / 10);
			break;
		case EDIT_MODE_SHIFT_Z:
			vertex->v[2] += (opt_h / 10);
			break;
	}

	return TRUE;
}

int kudu_skin_edit_edge(KuduEdge *edge, float opt_h, int edit_mode)
{
	if (edge == NULL) return FALSE;

	kudu_skin_edit_vertex(edge->start_vertex, opt_h, edit_mode);
	kudu_skin_edit_vertex(edge->end_vertex, opt_h, edit_mode);

	return TRUE;
}

int kudu_skin_edit_face(KuduFace *face, float opt_h, int edit_mode)
{
	if (face == NULL) return FALSE;

	KuduEdge *edge;

	if (!kudu_face_for_each_edge_do(face)) return FALSE;

	while ((edge = kudu_face_for_each_edge_next_do()) != NULL) {
		kudu_skin_edit_vertex(edge->start_vertex, opt_h, edit_mode);
		kudu_skin_edit_vertex(edge->end_vertex, opt_h, edit_mode);
	}

	return TRUE;
}

int kudu_skin_edit_shape(KuduShape *shape, float opt_h, int edit_mode)
{
	if (shape == NULL) return FALSE;

	KuduVertex *current_vertex = NULL;

	if (shape->vertex == NULL) return FALSE;

	do {
		if (current_vertex == NULL) current_vertex = shape->vertex;
		else current_vertex = current_vertex->next_vertex;

		kudu_skin_edit_vertex(current_vertex, opt_h, edit_mode);
	} while (current_vertex->next_vertex != NULL);

	return TRUE;
}

static float anchor_h = 0.0, anchor_v = 0.0;
int kudu_skin_edit_anchor(void)
{
	anchor_h = 0.0;
	anchor_v = 0.0;

	return TRUE;
}

int kudu_skin_edit_selection(KuduSelectionList *selection_list, float opt_h, float opt_v, int edit_mode)
{
	if (selection_list == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	if (edit_mode == EDIT_MODE_NONE) return FALSE;

	void *item;

	if (!kudu_selection_list_for_each_do(selection_list)) return FALSE;

	vertex_magic_touch++;
	/* rather unlikely that we reach the maximum int... */
	if (vertex_magic_touch == MAXINT) vertex_magic_touch = 0;

	anchor_h += opt_h;
	anchor_v += opt_v;

	while ((item = (void*)kudu_selection_list_next_do()) != NULL) {
		switch (selection_list->type) {
			case SELECT_POINTS:
				kudu_skin_edit_vertex((KuduVertex*)item, opt_h, edit_mode);
				break;
			case SELECT_EDGES:
				kudu_skin_edit_edge((KuduEdge*)item, opt_h, edit_mode);
				break;
			case SELECT_FACES:
				kudu_skin_edit_face((KuduFace*)item, opt_h, edit_mode);
				break;
			case SELECT_SHAPES:
				kudu_skin_edit_shape((KuduShape*)item, opt_h, edit_mode);
				break;
		}
	}

	/*		while ((face = (KuduFace*)kudu_selection_list_next_do()) != NULL) {
				if (kudu_face_for_each_vertex_do(face)) {
					while ((current_vertex = kudu_face_for_each_vertex_next_do()) != NULL) {
						current_vertex->av[0] -= (hscroll / 10);
					}
				}
			}
		}*/

	/*kudu_program_skin_list_regen();*/

	return TRUE;
}

int kudu_skin_edit_unanchor(KuduSelectionList *selection_list, int edit_mode)
{
	if (selection_list == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	kudu_skin_edit_selection(selection_list, -anchor_h, -anchor_v, edit_mode);

	anchor_h = 0.0;
	anchor_v = 0.0;

	return TRUE;
}

int kudu_skin_edit_object(KuduObject *object, float opt_h, float opt_v, int edit_mode)
{
	if (object == NULL) return FALSE;

	switch (edit_mode) {
		case EDIT_MODE_SHIFT_X:
			object->position[0] += (opt_h / 10);
			break;
		case EDIT_MODE_SHIFT_Y:
			object->position[1] += (opt_h / 10);
			break;
		case EDIT_MODE_SHIFT_Z:
			object->position[2] += (opt_h / 10);
			break;
	}
	return TRUE;
}

