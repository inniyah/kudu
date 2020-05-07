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
#include "rwxobject.h"

int kudu_object_import_rwx_from_file(KuduObject *object, char *filename)
{
	if (object == NULL) return FALSE;

	KuduMaterial *current_material = NULL, *temp_material;
	KuduVertex *current_vertex = NULL, *vertex1, *vertex2, *vertex3, *vertex4;
	KuduShape *current_shape;
	KuduFace *current_face;
	KuduEdge *edge1, *edge2, *edge3, *current_edge;

	FILE *file;
	char data[1024], *tmp;
	int *tmpi, num, a, b, c, d, e1r, e2r, e3r, e1vs, e2vs, e3vs, prl = FALSE;
	float *vertices, tmpv[6];
	int vertices_cnt = 0,	READY = GL_FALSE, com_cnt = 0;
	int vertexIdPos = 0;

	temp_material = kudu_material_new_with_id(object, NULL, 0);
	if (temp_material == NULL) return FALSE;

	if ((file = fopen(filename, "r")) == NULL) return -1;

	/* vertices = (GLfloat*)malloc((sizeof(GLfloat))*3); */
	while (fgets(data, 1024, file) != NULL){
		if (strncmp(data, "#", 1) == 0) continue;
		tmp = strtok(data, " ");

		if (strcasecmp(tmp, "clumpbegin") == 0){
			if (READY) continue;
			tmpi = (GLint*)malloc((sizeof(GLint)*4));
			vertexIdPos = kudu_vertex_next_id(object)-1;
			READY = GL_TRUE;
			prl = FALSE;

			current_shape = kudu_shape_new(object, object->skin);
			if (object->skin == NULL) object->skin = current_shape;

			continue;
		}

		if (strcasecmp(tmp, "clumpend") == 0){
			if (!READY) continue;
			READY = GL_FALSE;
			free(tmpi);
			continue;
		}

		if (strcasecmp(tmp, "Color") == 0){
			tmpv[0] = kudu_util_strtod_strtok(" ");
			tmpv[1] = kudu_util_strtod_strtok(" ");
			tmpv[2] = kudu_util_strtod_strtok(" ");
			tmpv[3] = 0.3;

			for (c = 0; c < 3; c++) {
				temp_material->ambient[c] = tmpv[c];
				temp_material->diffuse[c] = tmpv[c];
			}
			current_material = kudu_material_find_with_same_values(object->material, temp_material);
			if (current_material != NULL) continue;

			current_material = kudu_material_new(object, object->material);
			if (object->material == NULL) object->material = current_material;

			for (c = 0; c < 3; c++) {
				current_material->ambient[c] = tmpv[c];
				current_material->diffuse[c] = tmpv[c];
			}
			continue;
		}

		if (strcasecmp(tmp, "Opacity") == 0){
			if (current_material == NULL) continue;
			current_material->diffuse[3] = kudu_util_strtod_strtok(" ");
			continue;
		}

		if (strcasecmp(tmp, "Surface") == 0){

			continue;
		}

		if (strcasecmp(tmp, "Vertex") == 0){
			tmpv[0] = kudu_util_strtod_strtok(" ");
			tmpv[1] = kudu_util_strtod_strtok(" ");
			tmpv[2] = kudu_util_strtod_strtok(" ");
			tmp = strtok(NULL, " ");

			if (strcasecmp(tmp, "#!PreLight") == 0) prl = TRUE;
			if ((strcasecmp(tmp, "#!PreLight") != 0) && (prl)) continue;

			current_vertex = kudu_vertex_new(object, current_shape->vertex);
			if (current_shape->vertex == NULL) current_shape->vertex = current_vertex;

			for (c = 0; c < 3; c++) {
				current_vertex->v[c] = tmpv[c];
			/*	current_vertex->av[c] = tmpv[c];*/
			}
			continue;
		}


		if (strcasecmp(tmp, "Triangle") == 0){
			tmpi[0] = kudu_util_atoi_strtok(" ") + vertexIdPos;
			tmpi[1] = kudu_util_atoi_strtok(" ") + vertexIdPos;
			tmpi[2] = kudu_util_atoi_strtok(" ") + vertexIdPos;

			current_face = kudu_face_new(object, current_shape->face);
			if (current_shape->face == NULL) current_shape->face = current_face;
			current_face->material = current_material;

			edge1 = kudu_edge_find_with_vertex_ids(current_shape->edge, tmpi[0], tmpi[1]);
			edge2 = kudu_edge_find_with_vertex_ids(current_shape->edge, tmpi[1], tmpi[2]);
			edge3 = kudu_edge_find_with_vertex_ids(current_shape->edge, tmpi[2], tmpi[0]);

			e1vs = FALSE;
			e2vs = FALSE;
			e3vs = FALSE;

			if (edge1 == NULL) edge1 = kudu_edge_new(object, current_shape->edge); else e1vs = TRUE;
			if (current_shape->edge == NULL) {
				current_shape->edge = edge1;
				edge1 = current_shape->edge;
			}

			if (edge2 == NULL) edge2 = kudu_edge_new(object, current_shape->edge); else e2vs = TRUE;
			if (edge3 == NULL) edge3 = kudu_edge_new(object, current_shape->edge); else e3vs = TRUE;

			if (edge1->right_face == NULL) e1r = TRUE; else e1r = FALSE;
			if (edge2->right_face == NULL) e2r = TRUE; else e2r = FALSE;
			if (edge3->right_face == NULL) e3r = TRUE; else e3r = FALSE;

			if (e1r) kudu_edge_faces_set(edge1, NULL, current_face);
			else kudu_edge_faces_set(edge1, current_face, NULL);

			if (e2r) kudu_edge_faces_set(edge2, NULL, current_face);
			else kudu_edge_faces_set(edge2, current_face, NULL);

			if (e3r) kudu_edge_faces_set(edge3, NULL, current_face);
			else	kudu_edge_faces_set(edge3, current_face, NULL);

			if (e1r) kudu_edge_wings_set(edge1, NULL, NULL, edge2, edge3);
			else	kudu_edge_wings_set(edge1, edge2, edge3, NULL, NULL);

			if (e2r) kudu_edge_wings_set(edge2, NULL, NULL, edge3, edge1);
			else	kudu_edge_wings_set(edge2, edge3, edge1, NULL, NULL);

			if (e3r) kudu_edge_wings_set(edge3, NULL, NULL, edge1, edge2);
			else	kudu_edge_wings_set(edge3, edge1, edge2, NULL, NULL);

			current_vertex = kudu_vertex_find_with_id(current_shape->vertex, tmpi[0]);
			if (!e1vs) kudu_edge_vertices_set(edge1, current_vertex, NULL);
			if (!e3vs) kudu_edge_vertices_set(edge3, NULL, current_vertex);

			current_vertex = kudu_vertex_find_with_id(current_shape->vertex, tmpi[1]);
			if (!e1vs) kudu_edge_vertices_set(edge1, NULL, current_vertex);
			if (!e2vs) kudu_edge_vertices_set(edge2, current_vertex, NULL);

			current_vertex = kudu_vertex_find_with_id(current_shape->vertex, tmpi[2]);
			if (!e2vs) kudu_edge_vertices_set(edge2, NULL, current_vertex);
			if (!e3vs) kudu_edge_vertices_set(edge3, current_vertex, NULL);

		/*	if (object->draw_item == NULL) {
				object->draw_item = kudu_draw_item_new(NULL);
				current_item = object->draw_item;
			} else	current_item = kudu_draw_item_new(object->draw_item);

			current_item->type = DRAW_POLYGON;
			current_item->material = current_material;
			for (c = 0; c < 3; c++) {
				current_vertex = kudu_vertex_find_with_id(object->vertex, tmpi[c]);
				kudu_draw_item_vertex_add(current_item, current_vertex);
			}*/
			continue;
		}

		if (strcasecmp(tmp, "Quad") == 0){
			tmpi[0] = kudu_util_atoi_strtok(" ") + vertexIdPos;
			tmpi[1] = kudu_util_atoi_strtok(" ") + vertexIdPos;
			tmpi[2] = kudu_util_atoi_strtok(" ") + vertexIdPos;
			tmpi[3] = kudu_util_atoi_strtok(" ") + vertexIdPos;

			current_face = kudu_face_new(object, current_shape->face);
			if (current_shape->face == NULL) current_shape->face = current_face;
			current_face->material = current_material;

			for (a = 0; a < 4; a++) {
				b = a-1;
				if (b < 0) b = 3;
				c = a+1;
				if (c > 3) c = 0;
				d = c+1;
				if (d > 3) d = 0;

				edge1 = kudu_edge_find_with_vertex_ids(current_shape->edge, tmpi[b], tmpi[a]);
				current_edge = kudu_edge_find_with_vertex_ids(current_shape->edge, tmpi[a], tmpi[c]);
				edge2 = kudu_edge_find_with_vertex_ids(current_shape->edge, tmpi[c], tmpi[d]);

				vertex1 = kudu_vertex_find_with_id(current_shape->vertex, tmpi[b]);
				vertex2 = kudu_vertex_find_with_id(current_shape->vertex, tmpi[a]);
				vertex3 = kudu_vertex_find_with_id(current_shape->vertex, tmpi[c]);
				vertex4 = kudu_vertex_find_with_id(current_shape->vertex, tmpi[d]);

				if (edge1 == NULL) {
					edge1 = kudu_edge_new(object, current_shape->edge);
					kudu_edge_vertices_set(edge1, vertex1, vertex2);
				}
				if (current_shape->edge == NULL) {
					current_shape->edge = edge1;
					edge1 = current_shape->edge;
				}

				if (edge2 == NULL) {
					edge2 = kudu_edge_new(object, current_shape->edge);
					kudu_edge_vertices_set(edge2, vertex3, vertex4);
				}

				if (current_edge == NULL) {
					current_edge = kudu_edge_new(object, current_shape->edge);
					kudu_edge_vertices_set(current_edge, vertex2, vertex3);
				}

				if (current_edge->right_face == NULL) {
					kudu_edge_faces_set(current_edge, NULL, current_face);
					kudu_edge_wings_set(current_edge, NULL, NULL, edge2, edge1);
				} else {
					kudu_edge_faces_set(current_edge, current_face, NULL);
					kudu_edge_wings_set(current_edge, edge2, edge1, NULL, NULL);
				}

			}



		/*	if (object->draw_item == NULL) {
				object->draw_item = kudu_draw_item_new(NULL);
				current_item = object->draw_item;
			} else	current_item = kudu_draw_item_new(object->draw_item);

			current_item->type = DRAW_POLYGON;
			current_item->material = current_material;
			for (c = 0; c < 4; c++) {
				current_vertex = kudu_vertex_find_with_id(object->vertex, tmpi[c]);
				kudu_draw_item_vertex_add(current_item, current_vertex);
			}*/
			continue;
		}


		if (strcasecmp(tmp, "Polygon") == 0){
			num = kudu_util_atoi_strtok(" ");
			if (num < 0) continue;
			tmpi = (GLint*)realloc(tmpi, (sizeof(GLint)*num));
			for (c = 0; c < num; c++) tmpi[c] = kudu_util_atoi_strtok(" ") + vertexIdPos;

			current_face = kudu_face_new(object, current_shape->face);
			if (current_shape->face == NULL) current_shape->face = current_face;
			current_face->material = current_material;

			for (a = 0; a < num; a++) {
				b = a-1;
				if (b < 0) b = num-1;
				c = a+1;
				if (c > num-1) c = 0;
				d = c+1;
				if (d > num-1) d = 0;

				edge1 = kudu_edge_find_with_vertex_ids(current_shape->edge, tmpi[b], tmpi[a]);
				current_edge = kudu_edge_find_with_vertex_ids(current_shape->edge, tmpi[a], tmpi[c]);
				edge2 = kudu_edge_find_with_vertex_ids(current_shape->edge, tmpi[c], tmpi[d]);

				vertex1 = kudu_vertex_find_with_id(current_shape->vertex, tmpi[b]);
				vertex2 = kudu_vertex_find_with_id(current_shape->vertex, tmpi[a]);
				vertex3 = kudu_vertex_find_with_id(current_shape->vertex, tmpi[c]);
				vertex4 = kudu_vertex_find_with_id(current_shape->vertex, tmpi[d]);

				if (edge1 == NULL) {
					edge1 = kudu_edge_new(object, current_shape->edge);
					kudu_edge_vertices_set(edge1, vertex1, vertex2);
				}
				if (current_shape->edge == NULL) {
					current_shape->edge = edge1;
					edge1 = current_shape->edge;
				}

				if (edge2 == NULL) {
					edge2 = kudu_edge_new(object, current_shape->edge);
					kudu_edge_vertices_set(edge2, vertex3, vertex4);
				}

				if (current_edge == NULL) {
					current_edge = kudu_edge_new(object, current_shape->edge);
					kudu_edge_vertices_set(current_edge, vertex2, vertex3);
				}

				if (current_edge->right_face == NULL) {
					kudu_edge_faces_set(current_edge, NULL, current_face);
					kudu_edge_wings_set(current_edge, NULL, NULL, edge2, edge1);
				} else {
					kudu_edge_faces_set(current_edge, current_face, NULL);
					kudu_edge_wings_set(current_edge, edge2, edge1, NULL, NULL);
				}

			}

			continue;
		}
	}

	fclose(file);

	current_shape = NULL;

	do {
		if (current_shape == NULL) current_shape = object->skin;
		else	current_shape = current_shape->next_shape;

		current_edge = NULL;
		do {
			if (current_edge == NULL) current_edge = current_shape->edge;
			else	current_edge = current_edge->next_edge;

			if (current_edge->right_face == NULL) printf("weird!\n");
			if (current_edge->right_succ == NULL) printf("weird!1\n");
			if (current_edge->right_pred == NULL) printf("weird!2\n");
			if (current_edge->left_face == NULL) current_edge->left_face = current_edge->right_face;
			if (current_edge->left_succ == NULL) current_edge->left_succ = current_edge->right_pred;
			if (current_edge->left_pred == NULL) current_edge->left_pred = current_edge->right_succ;


		} while (current_edge->next_edge != NULL);

	} while (current_shape->next_shape != NULL);

	current_shape = NULL;

	do {
		if (current_shape == NULL) current_shape = object->skin;
		else	current_shape = current_shape->next_shape;

		kudu_vertex_update_vertices(current_shape->vertex);
		kudu_shape_normals_calculate(current_shape);
	} while (current_shape->next_shape != NULL);



	kudu_material_destroy(temp_material);

	return TRUE;
}

