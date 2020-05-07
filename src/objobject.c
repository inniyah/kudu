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
#include "objobject.h"

int kudu_object_import_obj_material_lib(KuduObject *object, char *path, char *filename)
{
	if ((object == NULL) || (filename == NULL)) return FALSE;

	FILE *file;
	char data[1024], *tmp, *fpath, *tpath;
	int a;
	KuduMaterial *current_material = NULL;

	a = strlen(path) + strlen(filename) + 2;
	fpath = (char*)malloc(a);
	sprintf(fpath, "%s/%s\0", path, filename);

	if ((file = fopen(fpath, "r")) == NULL) {
		free(fpath);
		return FALSE;
	}

	free(fpath);

	while (fgets(data, 1024, file) != NULL) {
		if (strncmp(data, "#", 1) == 0) continue;
		tmp = strtok(data, " ");

		if (strcasecmp(tmp, "newmtl") == 0) { /* Create new material */
			current_material = kudu_material_new(object, object->material);
			if (object->material == NULL) object->material = current_material;
			if (current_material != NULL) {
				tmp = strtok(NULL, "\n");
				a = strlen(tmp);
				if (a >= 64) {
					a = 64;
					tmp[63] = '\0';
				}
				sprintf(current_material->name, "%s", tmp);
				current_material->name[a-1] = '\0';
			}
			continue;
		}

		/* Don't continue unless we have created a new material */
		if (current_material == NULL) continue;

		if (strcasecmp(tmp, "Ns") == 0) { /* Material shininess */
			current_material->shininess = kudu_util_strtod_strtok(" ");
			continue;
		}

		if (strcasecmp(tmp, "d") == 0) { /* Material opacity */
			current_material->diffuse[3] = kudu_util_strtod_strtok(" ");
			continue;
		}

		if (strcasecmp(tmp, "Kd") == 0) { /* Material diffuse */
			for (a = 0; a < 3; a++) current_material->diffuse[a] = kudu_util_strtod_strtok(" ");
			continue;
		}

		if (strcasecmp(tmp, "Ka") == 0) { /* Material ambient */
			for (a = 0; a < 3; a++) current_material->ambient[a] = kudu_util_strtod_strtok(" ");
			continue;
		}

		if (strcasecmp(tmp, "Ks") == 0) { /* Material specular */
			for (a = 0; a < 3; a++) current_material->specular[a] = kudu_util_strtod_strtok(" ");
			continue;
		}

		if (strcasecmp(tmp, "Ke") == 0) { /* Material emission */
			for (a = 0; a < 3; a++) current_material->emission[a] = kudu_util_strtod_strtok(" ");
			continue;
		}

		if (strcasecmp(tmp, "map_Kd") == 0) { /* Texture image */
			tmp = strtok(NULL, "\n");
			tmp[strlen(tmp)-1] = '\0';
			a = strlen(path) + strlen(tmp) + 2;
			tpath = (char*)malloc(a);
			sprintf(tpath, "%s/%s\0", path, tmp);
			current_material->texture = kudu_texture_new_autoload_image(object, object->texture, tpath);
			free(tpath);
			continue;
		}

	}

	fclose(file);
	return TRUE;
}

int kudu_object_import_obj_from_file(KuduObject *object, char *filename)
{
	if (object == NULL) return FALSE;

	FILE *file;
	char data[1024], *tmp, *tmp2, *buf, *orig_buf, *root_path, *mtllib;
	float *tex_uv = NULL;
	int a, b, c, d, num, vertex_root_id, *vi, *vt, cs, vc = 0, num_uv = 0, lf;
	KuduShape *current_shape = NULL;
	KuduVertex *current_vertex = NULL, *vertex1, *vertex2, *vertex3, *vertex4;
	KuduEdge *current_edge = NULL, *edge1, *edge2;
	KuduFace *current_face = NULL;
	KuduMaterial *current_material = NULL;


	struct _vert_list {
		KuduVertex *vertex;
	} *vert_list = NULL;

	orig_buf = (char*)malloc(256);
	if (orig_buf == NULL) return FALSE;

	cs = 16;
	vi = (int*)malloc((cs*sizeof(int)));
	vt = (int*)malloc((cs*sizeof(int)));
	if ((vt == NULL) || (vi == NULL)) return FALSE;

	if ((file = fopen(filename, "r")) == NULL) return FALSE;

	vertex_root_id = kudu_vertex_next_id(object)-1;

	b = strlen(filename);
	c = b+1;
	for (a = b-1; a > 0; a--) {
		if (filename[a] == '/') {
			c = a;
			a = 0;
		}
	}

	if (c < b) {
		root_path = (char*)malloc(c+1);
		strncpy(root_path, filename, c);
		root_path[c] = '\0';
	} else {
		root_path = (char*)malloc(2);
		sprintf(root_path, ".\0");
	}

	while (fgets(data, 1024, file) != NULL) {
		if (strncmp(data, "#", 1) == 0) continue;
		tmp = strtok(data, " ");

		if (strcasecmp(tmp, "mtllib") == 0) { /* Load materials */
			tmp = strtok(NULL, "\n");
			tmp[strlen(tmp)-1] = '\0';
			kudu_object_import_obj_material_lib(object, root_path, tmp);
			/*free(mtllib);*/
			continue;
		}

		if (strcasecmp(tmp, "o") == 0) { /* Create a new shape */
			current_shape = kudu_shape_new(object, object->skin);
			if (object->skin == NULL) object->skin = current_shape;
			if (current_shape == NULL) continue;
			continue;
		}


		/* We haven't created a shape yet ... can't go on */
		if (current_shape == NULL) continue;

		if (strcasecmp(tmp, "usemtl") == 0) {	/* Set current material */
			tmp = strtok(NULL, "\n");
			tmp[strlen(tmp)-1] = '\0';
			current_material = kudu_material_find_with_name(object->material, tmp);
			continue;
		}

		if (strcasecmp(tmp, "v") == 0) {
			current_vertex = kudu_vertex_new(object, current_shape->vertex);
			if (current_shape->vertex == NULL) current_shape->vertex = current_vertex;
			if (current_vertex == NULL) continue;
			for (a = 0; a < 3; a++) current_vertex->v[a] = kudu_util_strtod_strtok(" ");

			if (vert_list == NULL) vert_list = (struct _vert_list*)malloc(sizeof(vert_list)*vc);
			else	vert_list = (struct _vert_list*)realloc(vert_list, sizeof(struct _vert_list)*(vc+1));

			vert_list[vc].vertex = current_vertex;

			vc++;
			continue;
		}

		/* Vertex normal ...ignore for now... we can figure them out for ourselves anyway */
		if (strcasecmp(tmp, "vn") == 0) {
			continue;
		}

		/* Texture UV coord */
		if (strcasecmp(tmp, "vt") == 0) {
			if (tex_uv == NULL) tex_uv = (float*)malloc((sizeof(float)*2)*(num_uv+1));
			else	tex_uv = (float*)realloc(tex_uv, (sizeof(float)*2)*(num_uv+1));

			tex_uv[(num_uv*2)] = kudu_util_strtod_strtok(" ");
			tex_uv[(num_uv*2)+1] = kudu_util_strtod_strtok(" ");
			num_uv++;
			continue;
		}

		/* groups ... ignore for now */
		if (strcasecmp(tmp, "g") == 0) {
			continue;
		}

		/* Load Faces - this means edges too... */
		if (strcasecmp(tmp, "f") == 0) {
			/* Faces are arranged like: vertex id/texture vertex id/normal id */

			num = 0;
			while ((tmp = strtok(NULL, " ")) != NULL) {
				/* strtok_r messes up the pointer so we need to make sure its pointing to the original block */
				buf = orig_buf;

				if (num >= cs) {
					cs = num+1;
					vi = (int*)malloc((cs*sizeof(int)));
					vt = (int*)malloc((cs*sizeof(int)));
				}

				/* Determine if a vertex id is specified ...*/
				tmp2 = index(tmp, '/');
				if (tmp2 == tmp) a = FALSE;
				else a = TRUE;

				/* Determine if a texture coord id is specified */
				if (tmp2[1] == '/') b = FALSE;
				else b = TRUE;

				/* Determine id vertex normal is specified */
				/* This code is commented out as we dont really need it */
				/* vertex normals can be calculated, I'll leave it here */
				/* just in case we do ever need it for some reason */
				/*tmp2 = index(tmp2, '/');
				if (tmp2[1] == '/') c = FALSE;
				else c = TRUE; */

				if (a) {	/* Have a vertex id ? */
					tmp2 = strtok_r(tmp, "/", &buf);
					vi[num] = atoi(tmp2) + vertex_root_id;
				} else vi[num] = 0;

				if (b) { 	/* Have a tex coord id ? */
					tmp2 = strtok_r(NULL, "/", &buf);
					vt[num] = atoi(tmp2) - 1;
				} else vt[num] = 0;

				/* This code not needed */
				/*if (c) {*/	/* Have a vertex normal ? */
				/*	tmp2 = strtok_r(NULL, "/", &buf);
					c = atoi(tmp2);
					printf("yep we have a vertex normal: %d\n", c);
				} */

				num++;
			}

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

				edge1 = kudu_edge_find_with_vertex_ids(current_shape->edge, vi[b], vi[a]);
				current_edge = kudu_edge_find_with_vertex_ids(current_shape->edge, vi[a], vi[c]);
				edge2 = kudu_edge_find_with_vertex_ids(current_shape->edge, vi[c], vi[d]);

				if (vi[b]-1 < vc) vertex1 = vert_list[vi[b]-1].vertex;
				if (vi[a]-1 < vc) vertex2 = vert_list[vi[a]-1].vertex;
				if (vi[c]-1 < vc) vertex3 = vert_list[vi[c]-1].vertex;
				if (vi[d]-1 < vc) vertex4 = vert_list[vi[d]-1].vertex;

				if (current_edge == NULL) {
					current_edge = kudu_edge_new(object, current_shape->edge);
					kudu_edge_vertices_set(current_edge, vertex2, vertex3);
				}
				if (current_shape->edge == NULL) current_shape->edge = current_edge;
				if (current_edge->start_vertex == vertex2) lf = TRUE;
				else lf = FALSE;

				if (edge1 == NULL) {
					edge1 = kudu_edge_new(object, current_shape->edge);
					if (lf) kudu_edge_vertices_set(edge1, vertex1, vertex2);
					else kudu_edge_vertices_set(edge1, vertex2, vertex1);
				}

				if (edge2 == NULL) {
					edge2 = kudu_edge_new(object, current_shape->edge);
					if (lf) kudu_edge_vertices_set(edge2, vertex3, vertex4);
					else kudu_edge_vertices_set(edge2, vertex4, vertex3);
				}

				if (lf) {
				/*if (current_edge->left_face == NULL) {*/
					kudu_edge_faces_set(current_edge, current_face, NULL);
					kudu_edge_wings_set(current_edge, edge2, edge1, NULL, NULL);
					if (current_material != NULL) {
						if (current_material->texture != NULL) {
							if (tex_uv != NULL) {
								if (vt[a] < num_uv) {
									current_edge->s_uv[0] = tex_uv[(vt[a]*2)];
									current_edge->s_uv[1] = tex_uv[(vt[a]*2)+1];
								}
							}
						}
					}
				/*} else if (current_edge->right_face == NULL) {*/
				} else {
					kudu_edge_faces_set(current_edge, NULL, current_face);
					kudu_edge_wings_set(current_edge, NULL, NULL, edge2, edge1);
					if (current_material != NULL) {
						if (current_material->texture != NULL) {
							if (tex_uv != NULL) {
								if (vt[a] < num_uv) {
									current_edge->e_uv[0] = tex_uv[(vt[a]*2)];
									current_edge->e_uv[1] = tex_uv[(vt[a]*2)+1];
								}
							}
						}
					}
				}
			}
			continue;
		}

	}

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

	kudu_object_update(object);

	free(vi);
	free(vt);
	free(orig_buf);
	free(root_path);

	if (tex_uv != NULL) free(tex_uv);

	fclose(file);
	return TRUE;

}

