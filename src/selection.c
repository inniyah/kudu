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
#include "selection.h"

static KuduSelection *do_selection = NULL;
static int(*object_selector)(KuduObject*) = NULL;
static KuduHitsList *hit_list = NULL;

/* The "hits list" simply stores pointers in an array, allowing us to assiociate an "index" number
   with a pointer for faster location.. (faster than directly searching through linked lists) */
int kudu_hits_list_init(void)
{
	if (hit_list != NULL) return FALSE;

	int a;

	hit_list = (KuduHitsList*)malloc(sizeof(KuduHitsList));
	if (hit_list == NULL) return FALSE;

	/* The current position, this gets increased each time we "push" a pointer into the list */
	hit_list->cpos = 0;

	/* Allocate memory for the default amount of records */
	hit_list->num_records = HL_DEFAULT_NUM;
	hit_list->record = (KuduHit*)malloc(sizeof(KuduHit)*hit_list->num_records);

	/* Init all pointers to NULL */
	for (a = 0; a < hit_list->num_records; a++) hit_list->record[a].data = NULL;

	return TRUE;
}

/* Close the hits list */
int kudu_hits_list_destroy(void)
{
	if (hit_list == NULL) return FALSE;

	hit_list->cpos = 0;
	hit_list->num_records = 0;
	free(hit_list->record);
	free(hit_list);
	hit_list = NULL;

	return TRUE;
}

/* Pushes a pointer into the hits list and returns the index value, this value can be passed into
   OpenGL's "name stack" for selection mode */
unsigned int kudu_hits_list_push_item(void *data)
{
	if (hit_list == NULL) return -1;

	unsigned int cpos = hit_list->cpos;

	/* Automatically increase the allocated memory if we need to add more records than the currect amount */
	if (cpos >= hit_list->num_records) {
		hit_list->num_records += HL_DEFAULT_INC;
		hit_list->record = (KuduHit*)realloc(hit_list->record, sizeof(KuduHit)*hit_list->num_records);
	}

	hit_list->record[cpos].data = data;

	hit_list->cpos++;

	return cpos;
}

/* Retrieve a pointer from the hits list */
void *kudu_hits_list_get_item(unsigned int num)
{
	if (hit_list == NULL) return NULL;

	if (num >= hit_list->num_records) return NULL;

	return hit_list->record[num].data;
}

/* Clear the hits list (set it to start a position 0 again) */
int kudu_hits_list_clear(void)
{
	if (hit_list == NULL) return FALSE;

	int a;

	hit_list->cpos = 0;

	return TRUE;
}


KuduSelection *kudu_selection_new(KuduSelection *previous_item)
{
	KuduSelection *selection, *old_next_item;

	selection = (KuduSelection*)malloc(sizeof(KuduSelection));
	if (selection == NULL) return NULL;

	if (previous_item == NULL) {
		selection->previous_item = NULL;
		selection->next_item = NULL;
	} else {
		selection->previous_item = previous_item;
		old_next_item = previous_item->next_item;
		selection->next_item = old_next_item;
		previous_item->next_item = selection;

		if (old_next_item != NULL) old_next_item->previous_item = selection;
	}

	selection->item = NULL;

	return selection;
}

int kudu_selection_destroy(KuduSelection *selection)
{
	if (selection == NULL) return FALSE;
	KuduSelection *previous_item, *next_item;

	previous_item = selection->previous_item;
	next_item = selection->next_item;

	if (previous_item != NULL) previous_item->next_item = next_item;
	if (next_item != NULL) next_item->previous_item = previous_item;

	free(selection);

	return TRUE;
}

KuduSelectionList *kudu_selection_list_new(void)
{
	KuduSelectionList *selection_list;

	selection_list = (KuduSelectionList*)malloc(sizeof(KuduSelection));
	if (selection_list == NULL) return NULL;

	selection_list->first_item = NULL;
	selection_list->selected_items = 0;
	selection_list->type = SELECT_NOTHING;

	return selection_list;
}

KuduSelectionList *kudu_selection_list_new_from_copy(KuduSelectionList *copy)
{
	if (copy == NULL) return NULL;
	if (copy->first_item == NULL) return NULL;

	KuduSelectionList *selection_list;
	KuduSelection *item = NULL;

	selection_list = kudu_selection_list_new();
	selection_list->type = copy->type;

	do {
		if (item == NULL) item = copy->first_item;
		else	item = item->next_item;

		kudu_selection_list_add_item(selection_list, item->item);
	} while (item->next_item != NULL);

	return selection_list;
}

int kudu_selection_list_destroy(KuduSelectionList *selection_list)
{
	if (selection_list == NULL) return FALSE;

	if (selection_list->first_item != NULL) kudu_selection_list_remove_all_items(selection_list);

	free(selection_list);

	return TRUE;
}

int kudu_selection_list_convert_edge(KuduSelectionList *selection_list, K_SELECTION_TYPE type, KuduEdge *edge, int change)
{
	if (selection_list == NULL) return FALSE;
	if (edge == NULL) return FALSE;

	KuduVertex *vertex;
	KuduFace *face;
	KuduShape *shape;

	switch (type) {
		case SELECT_POINTS:
			vertex = edge->start_vertex;
			if (vertex != NULL) {
				if (!kudu_selection_list_item_added(selection_list, vertex)) {
					kudu_selection_list_add_item(selection_list, vertex);
					if (change) vertex->selected = TRUE;
				}
			}

			vertex = edge->end_vertex;
			if (vertex != NULL) {
				if (!kudu_selection_list_item_added(selection_list, vertex)) {
					kudu_selection_list_add_item(selection_list, vertex);
					if (change) vertex->selected = TRUE;
				}
			}
			break;
		case SELECT_EDGES:
			if (!kudu_selection_list_item_added(selection_list, edge)) {
				kudu_selection_list_add_item(selection_list, edge);
				if (change) edge->selected = TRUE;
			}
			break;
		case SELECT_FACES:
			face = edge->left_face;
			if (face != NULL) {
				if (!kudu_selection_list_item_added(selection_list, face)) {
					if (change) face->selected = TRUE;
					kudu_selection_list_add_item(selection_list, face);
				}
			}
			face = edge->right_face;
			if (face != NULL) {
				if (!kudu_selection_list_item_added(selection_list, face)) {
					if (change) face->selected = TRUE;
					kudu_selection_list_add_item(selection_list, face);
				}
			}
			break;
		case SELECT_SHAPES:
			shape = edge->shape;
			if (shape == NULL) break;
			if (!kudu_selection_list_item_added(selection_list, shape)) {
				if (change) shape->selected = TRUE;
				kudu_selection_list_add_item(selection_list, shape);
			}
			break;
	}

}

int kudu_selection_list_set_type(KuduSelectionList *selection_list, K_SELECTION_TYPE type, int change)
{
	if (selection_list == NULL) return FALSE;
	K_SELECTION_TYPE old_type = selection_list->type;

	selection_list->type = type;
	if (old_type == type) return TRUE;
	if (selection_list->first_item == NULL) return TRUE;

	KuduSelection *orig_item = selection_list->first_item, *current_item = NULL, *destroy_item;
	KuduEdge *edge;
	KuduVertex *vertex;
	KuduFace *face;
	KuduShape *shape;
	KuduBone *bone;
	KuduJoint *joint;

	/* "Disconnect" all items from the selection list */
	selection_list->selected_items = 0;
	selection_list->first_item = NULL;

	/* First "convert" all items to edges */
	/* Then call kudu_selection_list_convert_edge, which will convert the edge to whatever */
	do {
		destroy_item = current_item;
		if (current_item == NULL) current_item = orig_item;
		else	current_item = current_item->next_item;

		kudu_selection_destroy(destroy_item);

		switch (old_type) {
			case SELECT_NOTHING:
				/* Selecting nothing .. just deselect everything */
				break;
			case SELECT_POINTS:
				vertex = (KuduVertex*)current_item->item;
				if (vertex == NULL) break;
				if (change) vertex->selected = FALSE;
				if (!((type > S_MESH) && (type < E_MESH))) break; /* Not switching to another "mesh" type */
				if (!kudu_vertex_for_each_edge_do(vertex)) break;
				while ((edge = kudu_vertex_for_each_edge_next_do()) != NULL) {
					kudu_selection_list_convert_edge(selection_list, type, edge, change);
				}
				break;
			case SELECT_EDGES:
				edge = (KuduEdge*)current_item->item;
				if (edge == NULL) break;
				if (change) edge->selected = FALSE;
				if (!((type > S_MESH) && (type < E_MESH))) break; /* Not switching to another "mesh" type */
				kudu_selection_list_convert_edge(selection_list, type, edge, change);
				break;
			case SELECT_FACES:
				face = (KuduFace*)current_item->item;
				if (face == NULL) break;
				if (change) face->selected = FALSE;
				if (!((type > S_MESH) && (type < E_MESH))) break; /* Not switching to another "mesh" type */
				if (!kudu_face_for_each_edge_do(face)) break;
				while ((edge = kudu_face_for_each_edge_next_do()) != NULL) {
					kudu_selection_list_convert_edge(selection_list, type, edge, change);
				}
				break;
			case SELECT_SHAPES:
				shape = (KuduShape*)current_item->item;
				if (shape == NULL) break;
				if (change) shape->selected = FALSE;
				if (!((type > S_MESH) && (type < E_MESH))) break; /* Not switching to another "mesh" type */
				if (!kudu_edge_for_each_do(shape->edge)) break;
				while ((edge = kudu_edge_next_do()) != NULL) {
					kudu_selection_list_convert_edge(selection_list, type, edge, change);
				}
				break;

			case SELECT_BONES:
				bone = (KuduBone*)current_item->item;
				if (bone == NULL) break;
				if (change) bone->selected = FALSE;
				if (type == SELECT_JOINTS) {
					joint = bone->s_joint;	/* Select start joint */
					if (!joint->selected) kudu_selection_list_add_item(selection_list, joint);
					joint->selected = TRUE;

					joint = bone->e_joint;	/* Select end joint */
					if (!joint->selected) kudu_selection_list_add_item(selection_list, joint);
					joint->selected = TRUE;
				}
				break;
			case SELECT_JOINTS:
				joint = (KuduJoint*)current_item->item;
				if (joint == NULL) break;
				if (change) joint->selected = FALSE;
				if (type == SELECT_BONES) {
					bone = joint->bone;
					if (!bone->selected) kudu_selection_list_add_item(selection_list, bone);
					bone->selected = TRUE;
				}			
				break;
			case SELECT_OBJECTS:
				break;
		}

	} while (current_item->next_item != NULL);
	kudu_selection_destroy(current_item);

	return TRUE;
}

int kudu_selection_list_item_added(KuduSelectionList *selection_list, void *data)
{
	if (selection_list == NULL) return FALSE;
	if (selection_list->first_item == NULL) return FALSE;
	if (data == NULL) return FALSE;

	KuduSelection *current_item = NULL;

	do {
		if (current_item == NULL) current_item = selection_list->first_item;
		else	current_item = current_item->next_item;

	} while ((current_item->next_item != NULL) && (current_item->item != data));

	if (current_item->item == data) return TRUE;

	return FALSE;
}

int kudu_selection_list_add_item(KuduSelectionList *selection_list, void *data)
{
	if (selection_list == NULL) return FALSE;
	if (data == NULL) return FALSE;

	KuduSelection *item;

	if (selection_list->first_item != NULL) {
		item = kudu_selection_new(selection_list->first_item);
	} else {
		selection_list->first_item = kudu_selection_new(NULL);
		item = selection_list->first_item;
	}

	item->item = data;
	selection_list->selected_items++;

	return TRUE;
}

int kudu_selection_list_remove_item(KuduSelectionList *selection_list, void *data)
{
	if (selection_list == NULL) return FALSE;
	if (selection_list->first_item == NULL) return FALSE;
	if (data == NULL) return FALSE;

	KuduSelection *current_item = NULL;

	do {
		if (current_item == NULL) current_item = selection_list->first_item;
		else	current_item = current_item->next_item;

	} while ((current_item->next_item != NULL) && (current_item->item != data));

	if (current_item->item == data) {
		if (current_item == selection_list->first_item) selection_list->first_item = current_item->next_item;
		kudu_selection_destroy(current_item);
		selection_list->selected_items--;

		return TRUE;
	}

	return FALSE;
}

int kudu_selection_list_remove_all_items(KuduSelectionList *selection_list)
{
	if (selection_list == NULL) return FALSE;
	if (selection_list->first_item == NULL) return FALSE;

	KuduSelection *current_item = NULL, *trash_item;

	do {
		trash_item = current_item;
		if (current_item == NULL) current_item = selection_list->first_item;
		else	current_item = current_item->next_item;

		kudu_selection_destroy(trash_item);
	} while ((current_item->next_item != NULL));

	kudu_selection_destroy(current_item);

	selection_list->first_item = NULL;
	selection_list->selected_items = 0;

	return TRUE;
}

int kudu_selection_list_for_each_do(KuduSelectionList *selection_list)
{
	if (selection_list == NULL) return FALSE;
	if (selection_list->first_item == NULL) return FALSE;

	do_selection = selection_list->first_item;

	return TRUE;
}

void *kudu_selection_list_next_do(void)
{
	if (do_selection == NULL) return NULL;

	KuduSelection *return_selection = do_selection;
	do_selection = do_selection->next_item;

	return return_selection->item;
}

void *kudu_selection_list_get_first_item(KuduSelectionList* selection_list)
{
	if (selection_list == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return NULL;
	}

	KuduSelection *selection = selection_list->first_item;

	if (selection != NULL) return selection->item;

	return NULL;
}

int kudu_selection_list_anything_selected(KuduSelectionList* selection_list)
{
	if (selection_list == NULL) return FALSE;
	if (selection_list->first_item == NULL) return FALSE;

	return TRUE;
}

int kudu_selection_list_get_center_point(KuduSelectionList* selection_list, float *cp)
{
	if (selection_list == NULL) return FALSE;
	if (selection_list->first_item == NULL) return FALSE;
	if (cp == NULL) return FALSE;

	KuduVertex *vertex;
	KuduEdge *edge;
	KuduFace *face;
	KuduShape *shape;
	KuduBone *bone;
	KuduJoint *s_joint, *e_joint;
	KuduSelection *selection = NULL;
	float minx = 1000000, maxx = -1000000, miny = 1000000, maxy = -1000000, minz = 1000000, maxz = -1000000;

	do {
		if (selection == NULL) selection = selection_list->first_item;
		else	selection = selection->next_item;

		switch (selection_list->type) {
			case SELECT_POINTS:
				vertex = (KuduVertex*)selection->item;
				if (vertex->av[0] < minx) minx = vertex->av[0];
				if (vertex->av[0] > maxx) maxx = vertex->av[0];
				if (vertex->av[1] < miny) miny = vertex->av[1];
				if (vertex->av[1] > maxy) maxy = vertex->av[1];
				if (vertex->av[2] < minz) minz = vertex->av[2];
				if (vertex->av[2] > maxz) maxz = vertex->av[2];
				break;
			case SELECT_EDGES:
				edge = (KuduEdge*)selection->item;
				if (edge->start_vertex != NULL) {
					vertex = edge->start_vertex;
					if (vertex->av[0] < minx) minx = vertex->av[0];
					if (vertex->av[0] > maxx) maxx = vertex->av[0];
					if (vertex->av[1] < miny) miny = vertex->av[1];
					if (vertex->av[1] > maxy) maxy = vertex->av[1];
					if (vertex->av[2] < minz) minz = vertex->av[2];
					if (vertex->av[2] > maxz) maxz = vertex->av[2];
				}
				if (edge->end_vertex != NULL) {
					vertex = edge->end_vertex;
					if (vertex->av[0] < minx) minx = vertex->av[0];
					if (vertex->av[0] > maxx) maxx = vertex->av[0];
					if (vertex->av[1] < miny) miny = vertex->av[1];
					if (vertex->av[1] > maxy) maxy = vertex->av[1];
					if (vertex->av[2] < minz) minz = vertex->av[2];
					if (vertex->av[2] > maxz) maxz = vertex->av[2];
				}
				break;
			case SELECT_FACES:
				face = (KuduFace*)selection->item;
				if (!kudu_face_for_each_vertex_do(face)) break;
				while ((vertex = kudu_face_for_each_vertex_next_do()) != NULL) {
					if (vertex->av[0] < minx) minx = vertex->av[0];
					if (vertex->av[0] > maxx) maxx = vertex->av[0];
					if (vertex->av[1] < miny) miny = vertex->av[1];
					if (vertex->av[1] > maxy) maxy = vertex->av[1];
					if (vertex->av[2] < minz) minz = vertex->av[2];
					if (vertex->av[2] > maxz) maxz = vertex->av[2];
				}
				break;
			case SELECT_SHAPES:
				shape = (KuduShape*)selection->item;
				if (!kudu_vertex_for_each_do(shape->vertex)) break;
				while ((vertex = kudu_vertex_next_do()) != NULL) {
					if (vertex->av[0] < minx) minx = vertex->av[0];
					if (vertex->av[0] > maxx) maxx = vertex->av[0];
					if (vertex->av[1] < miny) miny = vertex->av[1];
					if (vertex->av[1] > maxy) maxy = vertex->av[1];
					if (vertex->av[2] < minz) minz = vertex->av[2];
					if (vertex->av[2] > maxz) maxz = vertex->av[2];
				}
				break;
			case SELECT_BONES:
				bone = (KuduBone*)selection->item;
				if (bone == NULL) break;
				s_joint = bone->s_joint;
				e_joint = bone->e_joint;

				if (s_joint->pos[0] < minx) minx = s_joint->pos[0];
				if (s_joint->pos[0] > maxx) maxx = s_joint->pos[0];
				if (s_joint->pos[1] < miny) miny = s_joint->pos[1];
				if (s_joint->pos[1] > maxy) maxy = s_joint->pos[1];
				if (s_joint->pos[2] < minz) minz = s_joint->pos[2];
				if (s_joint->pos[2] > maxz) maxz = s_joint->pos[2];

				if (e_joint->pos[0] < minx) minx = e_joint->pos[0];
				if (e_joint->pos[0] > maxx) maxx = e_joint->pos[0];
				if (e_joint->pos[1] < miny) miny = e_joint->pos[1];
				if (e_joint->pos[1] > maxy) maxy = e_joint->pos[1];
				if (e_joint->pos[2] < minz) minz = e_joint->pos[2];
				if (e_joint->pos[2] > maxz) maxz = e_joint->pos[2];
				break;
			case SELECT_JOINTS:
				s_joint = (KuduJoint*)selection->item;
				if (s_joint == NULL) break;
				if (s_joint->pos[0] < minx) minx = s_joint->pos[0];
				if (s_joint->pos[0] > maxx) maxx = s_joint->pos[0];
				if (s_joint->pos[1] < miny) miny = s_joint->pos[1];
				if (s_joint->pos[1] > maxy) maxy = s_joint->pos[1];
				if (s_joint->pos[2] < minz) minz = s_joint->pos[2];
				if (s_joint->pos[2] > maxz) maxz = s_joint->pos[2];
				break;
		}

	} while (selection->next_item != NULL);

	cp[0] = ((minx + maxx) / 2);
	cp[1] = ((miny + maxy) / 2);
	cp[2] = ((minz + maxz) / 2);

	return TRUE;
}

int kudu_selection_list_select(KuduSelectionList *selection_list, KuduObject *object, int selection_mode, K_SELECTION_TYPE what)
{
	if (selection_list == NULL) return FALSE;
	if (object == NULL) return FALSE;

	KuduFace *current_face;
	KuduEdge *current_edge;
	KuduVertex *current_vertex;
	KuduShape *current_shape;

	if (!kudu_shape_for_each_do(object->skin)) return FALSE;

	while ((current_shape = kudu_shape_next_do()) != NULL) {
		switch (selection_mode) {
			case SELECT_POINTS:
				if (!kudu_vertex_for_each_do(current_shape->vertex)) break;
				while ((current_vertex = kudu_vertex_next_do()) != NULL) {
					if (what == SELECT_NOTHING) {
						if (current_vertex->selected)
							kudu_selection_list_remove_item(selection_list, current_vertex);
						current_vertex->selected = FALSE;
					} else if (what == SELECT_ALL) {
						if (!current_vertex->selected)
							kudu_selection_list_add_item(selection_list, current_vertex);
						current_vertex->selected = TRUE;
					} else if (what == SELECT_INVERSE) {
						if (!current_vertex->selected)
							kudu_selection_list_remove_item(selection_list, current_vertex);
						else	kudu_selection_list_add_item(selection_list, current_vertex);
						current_vertex->selected = !current_vertex->selected;
					}
				}
				break;
			case SELECT_EDGES:
				if (!kudu_edge_for_each_do(current_shape->edge)) break;
				while ((current_edge = kudu_edge_next_do()) != NULL) {
					if (what == SELECT_NOTHING) {
						if (current_edge->selected)
							kudu_selection_list_remove_item(selection_list, current_edge);
						current_edge->selected = FALSE;
					} else if (what == SELECT_ALL) {
						if (!current_edge->selected)
							kudu_selection_list_add_item(selection_list, current_edge);
						current_edge->selected = TRUE;
					} else if (what == SELECT_INVERSE) {
						if (!current_edge->selected)
							kudu_selection_list_remove_item(selection_list, current_edge);
						else	kudu_selection_list_add_item(selection_list, current_edge);
						current_edge->selected = !current_edge->selected;
					}
				}
				break;
			case SELECT_FACES:
				if (!kudu_face_for_each_do(current_shape->face)) break;
				while ((current_face = kudu_face_next_do()) != NULL) {
					if (what == SELECT_NOTHING) {
						if (current_face->selected)
							kudu_selection_list_remove_item(selection_list, current_face);
						current_face->selected = FALSE;
					} else if (what == SELECT_ALL) {
						if (!current_face->selected)
							kudu_selection_list_add_item(selection_list, current_face);
						current_face->selected = TRUE;
					} else if (what == SELECT_INVERSE) {
						if (!current_face->selected)
							kudu_selection_list_remove_item(selection_list, current_face);
						else	kudu_selection_list_add_item(selection_list, current_face);
						current_face->selected = !current_face->selected;
					}
				}
				break;
			case SELECT_SHAPES:
				if (what == SELECT_NOTHING) {
					if (current_shape->selected)
						kudu_selection_list_remove_item(selection_list, current_shape);
					current_shape->selected = FALSE;
				} else if (what == SELECT_ALL) {
					if (!current_shape->selected)
						kudu_selection_list_add_item(selection_list, current_shape);
					current_shape->selected = TRUE;
				} else if (what == SELECT_INVERSE) {
					if (!current_shape->selected)
						kudu_selection_list_remove_item(selection_list, current_shape);
					else	kudu_selection_list_add_item(selection_list, current_shape);
					current_shape->selected = !current_shape->selected;
				}
				break;
		}
	}

	return TRUE;
}

int kudu_selection_list_select_bones(KuduSelectionList *selection_list, KuduBone *root_bone, K_SELECTION_TYPE what)
{
	if (selection_list == NULL) return FALSE;
	if (root_bone == NULL) return FALSE;

	KuduBone *current_bone, *next_bone = NULL;
	int gone_back;

	do {
		if (next_bone == NULL) next_bone = root_bone;
		else	next_bone = next_bone->next_sibling;

		current_bone = next_bone;
		gone_back = FALSE;

		do {
			if (!gone_back) {
				switch (what) {
					case SELECT_NOTHING:
						if (!current_bone->selected) break;
						kudu_selection_list_remove_item(selection_list, current_bone);
						current_bone->selected = FALSE;
						break;
					case SELECT_ALL:
						if (current_bone->selected) break;
						kudu_selection_list_add_item(selection_list, current_bone);
						current_bone->selected = TRUE;
						break;
					case SELECT_INVERSE:
						if (current_bone->selected) kudu_selection_list_remove_item(selection_list, current_bone);
						else	kudu_selection_list_add_item(selection_list, current_bone);
						current_bone->selected = !current_bone->selected;
						break;
				}
			}


			if ((current_bone->first_child != NULL) && (!gone_back)) {
				current_bone = current_bone->first_child;
			} else	if ((current_bone->next_sibling != NULL) && (current_bone != next_bone)) {
				current_bone = current_bone->next_sibling;
				gone_back = FALSE;
			} else {
				if (current_bone->parent != NULL) current_bone = current_bone->parent;
				gone_back = TRUE;
			}
		} while (current_bone != next_bone);
	} while (next_bone->next_sibling != NULL);

}

int kudu_selection_list_select_joints(KuduSelectionList *selection_list, KuduJoint *joint, K_SELECTION_TYPE what)
{
	if ((selection_list == NULL) || (joint == NULL)) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	KuduJoint *current_joint = NULL;

	do {
		if (current_joint == NULL) current_joint = joint;
		else current_joint = current_joint->next_joint;

		switch (what) {
			case SELECT_NOTHING:
				if (!current_joint->selected) break;
				kudu_selection_list_remove_item(selection_list, current_joint);
				current_joint->selected = FALSE;
				break;
			case SELECT_ALL:
				if (current_joint->selected) break;
				kudu_selection_list_add_item(selection_list, current_joint);
				current_joint->selected = TRUE;
				break;
			case SELECT_INVERSE:
				if (current_joint->selected) kudu_selection_list_remove_item(selection_list, current_joint);
				else	kudu_selection_list_add_item(selection_list, current_joint);
				current_joint->selected = !current_joint->selected;
				break;
		}

	} while (current_joint->next_joint != NULL);

	return TRUE;
}

int kudu_selection_list_pick_bone(KuduSelectionList *selection_list, KuduBone *pick_bone, int selection_type, unsigned int modifiers)
{
	if (selection_list == NULL) return FALSE;
	if (pick_bone == NULL) return FALSE;

	int bs;

	bs = pick_bone->selected;

	if (selection_type == K_SELECT_RECTANGLE) {
		if (modifiers & GDK_CONTROL_MASK) pick_bone->selected = FALSE;
		else if (modifiers & GDK_SHIFT_MASK) pick_bone->selected = !pick_bone->selected;
		else	pick_bone->selected = TRUE;
	} else	if (selection_type == K_SELECT_PICK) {
		pick_bone->selected = !pick_bone->selected;
	}

	if (bs != pick_bone->selected) {
		if (pick_bone->selected) kudu_selection_list_add_item(selection_list, pick_bone);
		else	kudu_selection_list_remove_item(selection_list, pick_bone);
	}

	return TRUE;
}

int kudu_selection_list_pick_joint(KuduSelectionList *selection_list, KuduJoint *pick_joint, int selection_type, unsigned int modifiers)
{
	if (selection_list == NULL) return FALSE;
	if (pick_joint == NULL) return FALSE;

	int bs = pick_joint->selected;

	if (selection_type == K_SELECT_RECTANGLE) {
		if (modifiers & GDK_CONTROL_MASK) pick_joint->selected = FALSE;
		else if (modifiers & GDK_SHIFT_MASK) pick_joint->selected = !pick_joint->selected;
		else	pick_joint->selected = TRUE;
	} else if (selection_type == K_SELECT_PICK) {
		pick_joint->selected = !pick_joint->selected;
	}

	if (bs != pick_joint->selected) {
		if (pick_joint->selected) kudu_selection_list_add_item(selection_list, pick_joint);
		else	kudu_selection_list_remove_item(selection_list, pick_joint);
	}

	return TRUE;
}

/* Set the callback for object selection */
void kudu_selection_set_object_selector(int(*func)(KuduObject*))
{
	object_selector = func;
}

int kudu_selection_pick_object(KuduObject *obj)
{
	if (obj == NULL) return FALSE;

	if (object_selector != NULL) object_selector(obj);

	return TRUE;
}

int kudu_selection_list_draw_info(KuduSelectionList *selection_list, int sx, int sy)
{
	if (selection_list == NULL) return FALSE;

/*	if (selection_list->first_item == NULL) {
		switch (selection_list->type) {
			case SELECT_POINTS:
				break;
			case SELECT_BONES:
				kudu_font_builtin_write_2d(sx, sy, "No Bones selected");
				break;
		}

		return TRUE;
	}*/

	int count = selection_list->selected_items;
	char text[1024];

	switch (selection_list->type) {
		case SELECT_POINTS:
			if (count == 0)
				sprintf(text, "No Vertices selected");
			else if (count == 1)
				sprintf(text, "1 selected Vertex");
			else
				sprintf(text, "%d selected Vertices", count);
			break;
		case SELECT_EDGES:
			if (count == 0)
				sprintf(text, "No Edges selected");
			else if (count == 1)
				sprintf(text, "1 selected Edge");
			else
				sprintf(text, "%d selected Edges", count);
			break;
		case SELECT_FACES:
			if (count == 0)
				sprintf(text, "No Faces selected");
			else if (count == 1)
				sprintf(text, "1 selected Face");
			else
				sprintf(text, "%d selected Faces", count);
			break;
		case SELECT_SHAPES:
			if (count == 0)
				sprintf(text, "No Shapes selected");
			else if (count == 1)
				sprintf(text, "1 selected Shape");
			else
				sprintf(text, "%d selected Shapes", count);
			break;
		case SELECT_BONES:
			if (count == 0)
				sprintf(text, "No Bones selected");
			else if (count == 1)
				sprintf(text, "1 selected Bone");
			else
				sprintf(text, "%d selected Bones", count);
			break;
		case SELECT_JOINTS:
			if (count == 0)
				sprintf(text, "No Joints selected");
			else if (count == 1)
				sprintf(text, "1 selected Joint");
			else
				sprintf(text, "%d selected Joints", count);
			break;
		case SELECT_OBJECTS:
			if (count == 0)
				sprintf(text, "No Objects selected");
			else if (count == 1)
				sprintf(text, "1 selected Object");
			else
				sprintf(text, "%d selected Objects", count);
			break;

	}

	kudu_font_builtin_write_2d(sx, sy, text);

	return TRUE;
}


int kudu_selection_list_bones_are_parents(KuduSelectionList *selection_list)
{
	if (selection_list == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	if (selection_list->first_item == NULL) return FALSE;

	KuduBone *bone;

	kudu_selection_list_for_each_do(selection_list);

	while ((bone = (KuduBone*)kudu_selection_list_next_do()) != NULL) {
		if (!kudu_bone_is_parent(bone)) return FALSE;
	}

	return TRUE;
}

int kudu_selection_list_bones_select_more(KuduSelectionList *selection_list, K_SELECTION_TYPE what, int oper)
{
	if (selection_list == NULL) return FALSE;
	if (selection_list->first_item == NULL) return FALSE;
	if ((what <= SELECT_BONES) || (what >= SELECT_OBJECTS)) return FALSE;

	KuduSelectionList *temp_list;
	KuduSelection *item;
	KuduBone *bone, *new_s, *temp_b;
	int ignore, keep = (oper & SL_KEEP_CURRENT);

	temp_list = kudu_selection_list_new();
	if (temp_list == NULL) return FALSE;

	kudu_selection_list_for_each_do(selection_list);

	while ((bone = (KuduBone*)kudu_selection_list_next_do()) != NULL) {
		ignore = FALSE;
		switch (what) {
			case SELECT_BONES_PARENTS:
				new_s = bone->parent;
				break;
			case SELECT_BONES_FIRST_CHILD:
				new_s = bone->first_child;
				break;
			case SELECT_BONES_NEXT_SIBLING:
				new_s = bone->next_sibling;
				break;
			case SELECT_BONES_PREVIOUS_SIBLING:
				new_s = bone->previous_sibling;
				break;
			case SELECT_BONES_ALL_CHILDREN:
				ignore = TRUE;
				new_s = bone->first_child;
				temp_b = NULL;
				if ((keep) || (new_s == NULL)) kudu_selection_list_add_item(temp_list, bone);
				else bone->selected = FALSE;
				if (new_s == NULL) break;
				do {
					if (temp_b == NULL) temp_b = new_s;
					else	temp_b = temp_b->next_sibling;

					if (!temp_b->selected) {
						kudu_selection_list_add_item(temp_list, temp_b);
						temp_b->selected = TRUE;
					}
				} while (temp_b->next_sibling != NULL);
				break;
		}

		if (!ignore) {
			if ((keep) || (new_s == NULL)) kudu_selection_list_add_item(temp_list, bone);
			else	bone->selected = FALSE;
			if (new_s != NULL) {
				if (!new_s->selected) {
					kudu_selection_list_add_item(temp_list, new_s);
					new_s->selected = TRUE;
				}
			}
		}
	}

	item = temp_list->first_item;
	temp_list->first_item = selection_list->first_item;
	selection_list->first_item = item;
	selection_list->selected_items = temp_list->selected_items;

	kudu_selection_list_destroy(temp_list);

	return TRUE;
}



int kudu_selection_hits_process(int selection_mode, int selection_type, unsigned int modifiers, KuduObject *object,
			       KuduSelectionList *selection_list, KuduSelectionList *selected_bones_list)
{
	int hits = program.hit.hits;
	unsigned int *buffer = program.hit.buffer;

	if (hits <= 0) return FALSE;

/*	if ((selection_mode != SELECT_OBJECTS) && (program.selected_object == NULL)) return FALSE;*/

/*	printf("Number of hits: %d\n", hits);*/

	int i, j, a, id;
	unsigned int names, *ptr, dist;
	int ss, closer;
	KuduObject *selected_object = NULL;
	KuduBone *selected_bone = NULL, *pbone = NULL;
	KuduJoint *selected_joint = NULL, *pjoint = NULL;
	KuduVertex *select_vertex = NULL, *pvertex = NULL;
	KuduEdge *select_edge = NULL, *pedge = NULL;
	KuduFace *select_face = NULL, *pface = NULL;
	KuduShape *select_shape = NULL, *pshape = NULL;

	ptr = (unsigned int*)buffer;

	for (i = 0; i < hits; i++) {
		names = *ptr;
		ptr += 2;

		/* If we are performing a "pick" selection then we need to determine the nearest hit */
		if (selection_type == K_SELECT_PICK) {
			if (i == 0) {
				dist = *ptr;
				closer = TRUE;
			} else if (*ptr < dist)	{
				dist = *ptr;
				closer = TRUE;
			} else	closer = FALSE;
		}
		ptr++;

	/*	printf("Number of names for this hit: %d\n", names);*/

		for (j = 0; j < (int)names; j++) {
			a = *ptr++;
			if ((selection_mode != SELECT_OBJECTS) && (selection_mode != SELECT_BONES) && (selection_mode != SELECT_JOINTS)) {
				if (j == 0) {
					select_shape = (KuduShape*)kudu_hits_list_get_item(a);
					if (selection_mode != SELECT_SHAPES) continue;
				}
				if (select_shape == NULL) continue;
			}

			switch (selection_mode) {
				case SELECT_POINTS:
					select_vertex = (KuduVertex*)kudu_hits_list_get_item(a);
					ss = select_vertex->selected;
					if (selection_type == K_SELECT_RECTANGLE) {
						if (modifiers & GDK_CONTROL_MASK) select_vertex->selected = FALSE;
						else if (modifiers & GDK_SHIFT_MASK) select_vertex->selected = !select_vertex->selected;
						else	select_vertex->selected = TRUE;
					} else	if (selection_type == K_SELECT_PICK) {
						if (closer) pvertex = select_vertex;
					}
						if (ss == select_vertex->selected) break;
					if (select_vertex->selected) 
						kudu_selection_list_add_item(selection_list, select_vertex);
					else	kudu_selection_list_remove_item(selection_list, select_vertex);
					break;
				case SELECT_EDGES:
					select_edge = (KuduEdge*)kudu_hits_list_get_item(a);
					ss = select_edge->selected;
					if (selection_type == K_SELECT_RECTANGLE) {
						if (modifiers & GDK_CONTROL_MASK) select_edge->selected = FALSE;
						else if (modifiers & GDK_SHIFT_MASK) select_edge->selected = !select_edge->selected;
						else	select_edge->selected = TRUE;
					} else	if (selection_type == K_SELECT_PICK) {
						if (closer) pedge = select_edge;
					}

					if (ss == select_edge->selected) break;
					if (select_edge->selected)
						kudu_selection_list_add_item(selection_list, select_edge);
					else	kudu_selection_list_remove_item(selection_list, select_edge);
					break;
				case SELECT_FACES:
					select_face = (KuduFace*)kudu_hits_list_get_item(a);
					ss = select_face->selected;
					if (selection_type == K_SELECT_RECTANGLE) {
						if (modifiers & GDK_CONTROL_MASK) select_face->selected = FALSE;
						else if (modifiers & GDK_SHIFT_MASK) select_face->selected = !select_face->selected;
						else	select_face->selected = TRUE;
					} else	if (selection_type == K_SELECT_PICK) {
						if (closer) pface = select_face;
					}

					if (ss == select_face->selected) break;
					if (select_face->selected)
						kudu_selection_list_add_item(selection_list, select_face);
					else	kudu_selection_list_remove_item(selection_list, select_face);
					break;
				case SELECT_SHAPES:
					ss = select_shape->selected;
					if (selection_type == K_SELECT_RECTANGLE) {
						if (modifiers & GDK_CONTROL_MASK) select_shape->selected = FALSE;
						else if (modifiers & GDK_SHIFT_MASK) select_shape->selected = !select_shape->selected;
						else	select_shape->selected = TRUE;
					} else	if (selection_type == K_SELECT_PICK) {
						if (closer) pshape = select_shape;
					}

					if (ss == select_shape->selected) break;
					if (select_shape->selected)
						kudu_selection_list_add_item(selection_list, select_shape);
					else	kudu_selection_list_remove_item(selection_list, select_shape);
					break;
				case SELECT_BONES:
					selected_bone = (KuduBone*)kudu_hits_list_get_item(a);

					if (selection_type == K_SELECT_RECTANGLE)
						kudu_selection_list_pick_bone(selected_bones_list, selected_bone, selection_type, modifiers);
					else if (selection_type == K_SELECT_PICK) {
						if (closer) pbone = selected_bone;
					}
					break;
				case SELECT_JOINTS:
					selected_joint = (KuduJoint*)kudu_hits_list_get_item(a);

					if (selection_type == K_SELECT_RECTANGLE)
						kudu_selection_list_pick_joint(selected_bones_list, selected_joint, selection_type, modifiers);
					else if (selection_type == K_SELECT_PICK) {
						if (closer) pjoint = selected_joint;
					}
					break;
				case SELECT_OBJECTS:
					if (closer) selected_object = kudu_object_find_with_id(object, a);
					break;
			}
		}
	}

	if (selection_type != K_SELECT_PICK) return TRUE;

	switch (selection_mode) {
		case SELECT_POINTS:
			if (pvertex == NULL) break;
			ss = pvertex->selected;
			pvertex->selected = !pvertex->selected;
			if (ss == pvertex->selected) break;
			if (pvertex->selected)
				kudu_selection_list_add_item(selection_list, pvertex);
			else	kudu_selection_list_remove_item(selection_list, pvertex);	
			break;
		case SELECT_EDGES:
			if (pedge == NULL) break;
			ss = pedge->selected;
			pedge->selected = !pedge->selected;
			if (ss == pedge->selected) break;
			if (pedge->selected)
				kudu_selection_list_add_item(selection_list, pedge);
			else	kudu_selection_list_remove_item(selection_list, pedge);
			break;
		case SELECT_FACES:
			if (pface == NULL) break;
			ss = pface->selected;
			pface->selected = !pface->selected;
			if (ss == pface->selected) break;
			if (pface->selected)
				kudu_selection_list_add_item(selection_list, pface);
			else	kudu_selection_list_remove_item(selection_list, pface);
			break;
		case SELECT_SHAPES:
			if (pshape == NULL) break;
			ss = pshape->selected;
			pshape->selected = !pshape->selected;
			if (ss == pshape->selected) break;
			if (pshape->selected)
				kudu_selection_list_add_item(selection_list, pshape);
			else	kudu_selection_list_remove_item(selection_list, pshape);
			break;
		case SELECT_BONES:
			if (pbone == NULL) break;
			kudu_selection_list_pick_bone(selected_bones_list, pbone, selection_type, modifiers);
			break;
		case SELECT_JOINTS:
			if (pjoint == NULL) break;
			kudu_selection_list_pick_joint(selected_bones_list, pjoint, selection_type, modifiers);
		case SELECT_OBJECTS:
			if (selected_object == NULL) break;
			kudu_selection_pick_object(selected_object);
			break;
	}

	return TRUE;
}

