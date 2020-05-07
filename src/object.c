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
#include "object.h"

static int OBJECT_ID = 1, c_file_ver[2];
static KuduObject *do_object = NULL;

int kudu_object_force_next_id(int objectID)
{
	OBJECT_ID = objectID;
	return OBJECT_ID;
}

int kudu_object_id_counter_reset(void)
{
	OBJECT_ID = 0;
	return OBJECT_ID;
}

int kudu_object_set_filename(KuduObject *object, char *filename)
{
	if ((object == NULL) || (filename == NULL)) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	int len = strlen(filename)+1;

	if (len > object->filename_length) {
		if (object->filename_length > 0) object->filename = (char*)realloc(object->filename, len);
		else	object->filename = (char*)malloc(len);

		if (object->filename == NULL) {
			kudu_error(KE_NO_MEM);
			return FALSE;
		}

		object->filename_length = len;
	}

	sprintf(object->filename, "%s\0", filename);

	return TRUE;
}

int kudu_object_clear_filename(KuduObject *object)
{
	if (object == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	if (object->filename_length > 0) free(object->filename);
	object->filename = NULL;
	object->filename_length = 0;

	return TRUE;
}

int kudu_object_set_author(KuduObject *object, char *name)
{
	if ((object == NULL) || (name == NULL)) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	int len = strlen(name)+1;

	if (len > object->author_length) {
		if (object->author_length > 0) object->author = (char*)realloc(object->author, len);
		else	object->author = (char*)malloc(len);

		if (object->author == NULL) {
			kudu_error(KE_NO_MEM);
			return FALSE;
		}

		object->author_length = len;
	}

	sprintf(object->author, "%s\0", name);

	return TRUE;
}

int kudu_object_clear_author(KuduObject *object)
{
	if (object == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	if (object->author_length > 0) free(object->author);
	object->author = NULL;
	object->author_length = 0;

	return TRUE;
}

int kudu_object_set_email(KuduObject *object, char *email)
{
	if ((object == NULL) || (email == NULL)) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	int len = strlen(email)+1;

	if (len > object->email_length) {
		if (object->email_length > 0) object->email = (char*)realloc(object->email, len);
		else	object->email = (char*)malloc(len);

		if (object->email == NULL) {
			kudu_error(KE_NO_MEM);
			return FALSE;
		}

		object->email_length = len;
	}

	sprintf(object->email, "%s\0", email);

	return TRUE;
}

int kudu_object_clear_email(KuduObject *object)
{
	if (object == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	if (object->email_length > 0) free(object->email);
	object->email = NULL;
	object->email_length = 0;

	return TRUE;
}


int kudu_object_set_url(KuduObject *object, char *url)
{
	if ((object == NULL) || (url == NULL)) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	int len = strlen(url)+1;

	if (len > object->url_length) {
		if (object->url_length > 0) object->url = (char*)realloc(object->url, len);
		else	object->url = (char*)malloc(len);

		if (object->url == NULL) {
			kudu_error(KE_NO_MEM);
			return FALSE;
		}

		object->url_length = len;
	}

	sprintf(object->url, "%s\0", url);

	return TRUE;
}

int kudu_object_clear_url(KuduObject *object)
{
	if (object == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	if (object->url_length > 0) free(object->url);
	object->url = NULL;
	object->url_length = 0;

	return TRUE;
}

KuduObject *kudu_object_new(KuduObject *previous_object)
{
	KuduObject *object = NULL, *old_next_object;

	int a;
	object = (KuduObject*)malloc(sizeof(KuduObject));
	if (object == NULL) {
		kudu_error(KE_NO_MEM);
		return NULL;
	}

	if (previous_object == NULL) {
		object->next_object = NULL;
		object->previous_object = NULL;
	} else {
		object->previous_object = previous_object;
		old_next_object = previous_object->next_object;
		object->next_object = old_next_object;
		previous_object->next_object = object;	

		if (old_next_object != NULL) old_next_object->previous_object = object;
	}

	for (a = 0; a < 3; a++) object->position[a] = 0.0;
	object->id = OBJECT_ID++;
	/*object->joint_list = kudu_joint_list_new();*/
	object->joint = NULL;
	object->bone = NULL;
	object->material = NULL;
	object->skin = NULL;
	object->filename = NULL;
	object->filename_length = 0;
	object->author = NULL;
	object->email = NULL;
	object->url = NULL;
	object->texture = NULL;
	object->author_length = 0;
	object->email_length = 0;
	object->url_length = 0;
	object->next_joint_id = 1;
	object->next_bone_id = 1;
	object->next_edge_id = 1;
	object->next_vertex_id = 1;
	object->next_face_id = 1;
	object->next_shape_id = 1;
	object->next_material_id = 1;
	object->next_texture_id = 1;
	sprintf(object->name, "Object %d", object->id);


	return object;
}

KuduObject *kudu_object_new_with_id(KuduObject *previous_object, int id)
{
	KuduObject *object = kudu_object_new(previous_object);

	object->id = id;

	if (id >= OBJECT_ID) OBJECT_ID = id+1;

	return object;
}

KuduObject *kudu_object_new_with_defaults(KuduObject *previous_object)
{
	KuduObject *object;

	object = kudu_object_new(previous_object);
	if (object == NULL) {
		kudu_error(KE_NO_MEM);
		return NULL;
	}

/*	object->bone = kudu_bone_new_with_id(0);
	if (object->bone != NULL) {
		object->bone->length = 1.0;
		kudu_bone_update(object->bone);
	}*/
	
	return object;
}

KuduObject *kudu_object_find_with_id(KuduObject *object, int find_id)
{
	if (object == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return NULL;
	}

	KuduObject *current_object;
	int parse_backwards, found_or_eol;

	for (parse_backwards = 0; parse_backwards < 2; parse_backwards++) {
		current_object = NULL;

		do {
			if (current_object == NULL) current_object = object;
			else {
				if (!parse_backwards) current_object = current_object->next_object;
				else	current_object = current_object->previous_object;
			}

			if (!parse_backwards) {
				if ((current_object->next_object != NULL) && (current_object->id != find_id))
					found_or_eol = FALSE;
				else	found_or_eol = TRUE;
			} else {
				if ((current_object->previous_object != NULL) && (current_object->id != find_id))
					found_or_eol = FALSE;
				else	found_or_eol = TRUE;
			}

		} while (!found_or_eol);

		if (current_object->id == find_id) return current_object;
	}

	kudu_error(KE_OBJECT_NOT_FOUND);
	return NULL;
}

int kudu_object_destroy(KuduObject *object)
{
	if (object == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	KuduObject *previous_object, *next_object;

	previous_object = object->previous_object;
	next_object = object->next_object;

	if (previous_object != NULL) previous_object->next_object = next_object;
	if (next_object != NULL) next_object->previous_object = previous_object;

	kudu_object_clear_filename(object);
	kudu_object_clear_author(object);
	kudu_object_clear_email(object);
	kudu_object_clear_url(object);

	kudu_bone_destroy_with_children(object->bone);
	kudu_material_destroy_all(object->material);
	kudu_shape_destroy_all(object->skin);
	/*kudu_joint_list_destroy(object->joint_list);*/
	/*kudu_draw_item_destroy_all(object->draw_item);*/
	/*kudu_vertex_destroy_all(object->vertex);*/

	object->bone = NULL;
	object->material = NULL;

	free(object);

	return TRUE;
}

int kudu_object_destroy_all(KuduObject *object)
{
	if (object == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	KuduObject *current_object = NULL, *destroy_object, *first_object = NULL;

	do {
		if (first_object == NULL) first_object = object;
		else	first_object = first_object->previous_object;
	} while (first_object->previous_object != NULL);

	do {
		destroy_object = current_object;
		if (current_object == NULL) current_object = first_object;
		else	current_object = current_object->next_object;

		if (destroy_object != NULL) kudu_object_destroy(destroy_object);

	} while (current_object->next_object != NULL);

	kudu_object_destroy(current_object);

	return TRUE;
}

int kudu_object_for_each_do(KuduObject *object)
{
	if (object == NULL) return FALSE;

	do_object = object;

	return TRUE;
}

KuduObject *kudu_object_next_do(void)
{
	if (do_object == NULL) return NULL;

	KuduObject *r_object = do_object;

	do_object = do_object->next_object;

	return r_object;
}

/*int kudu_object_internal_dump_bones(KuduBone *bone, FILE *file)
{
	if (bone == NULL) return FALSE;
	if (file == NULL) return FALSE;

	KuduBone *current_bone, *p_bone;
	int gone_back = FALSE, misc[4];

	misc[0] = kudu_bone_num_children(bone);
	fwrite(misc, sizeof(int), 1, file);

	current_bone = bone;

	do {
		if (!gone_back) {
			fwrite(current_bone, (sizeof(int)+64+(sizeof(float)*4)), 1, file);
			p_bone = current_bone->parent;
			if (p_bone == NULL) misc[0] = current_bone->id;
			else	misc[0] = p_bone->id;
			fwrite(misc, sizeof(int), 1, file);
		}

		if ((current_bone->first_child != NULL) && (!gone_back)) current_bone = current_bone->first_child;
		else	if (current_bone->next_sibling != NULL) {
				current_bone = current_bone->next_sibling;
				gone_back = FALSE;
			} else {
				if (current_bone->parent != NULL) current_bone = current_bone->parent;
				gone_back = TRUE;
			}

	} while (current_bone != bone);

	return TRUE;
}*/

/*int kudu_object_internal_dump_vertices(KuduVertex *vertex, FILE *file)
{
	if (vertex == NULL) return FALSE;

	int misc[4];
	KuduVertex *current_vertex = NULL;
	KuduBone *bone;
	KuduEdge *edge;

	do {
		if (current_vertex == NULL) current_vertex = vertex;
		else	current_vertex = current_vertex->next_vertex;

		fwrite(current_vertex, sizeof(int) + sizeof(unsigned char)  + (sizeof(float) * 6), 1, file);
		edge = current_vertex->edge;
		bone = current_vertex->bone;
		if (edge != NULL) misc[0] = edge->id;
		else	misc[0] = -1;
		if (bone != NULL) misc[1] = bone->id;
		else	misc[1] = -1;

		fwrite(misc, sizeof(int), 2, file);

	} while (current_vertex->next_vertex != NULL);

}*/

/*int kudu_object_internal_dump_faces(KuduFace *face, FILE *file)
{
	if (face == NULL) return FALSE;
	if (file == NULL) return FALSE;

	int misc[4];
	KuduFace *current_face = NULL;
	KuduMaterial *material;
	KuduEdge *edge;

	do {
		if (current_face == NULL) current_face = face;
		else	current_face = current_face->next_face;

		fwrite(current_face, sizeof(int), 1, file);

		edge = current_face->edge;
		material = current_face->material;
		if (edge != NULL) misc[0] = edge->id;
		else	misc[0] = -1;
		if (material != NULL) misc[1] = material->id;
		else	misc[0] = -1;

		fwrite(misc, sizeof(int), 2, file);

	} while (current_face->next_face != NULL);

	return TRUE;
}*/

/*int kudu_object_internal_dump_edges(KuduEdge *edge, FILE *file)
{
	if (edge == NULL) return FALSE;
	if (file == NULL) return FALSE;

	int misc[8], a;
	KuduEdge *current_edge = NULL, *lp, *ls, *rp, *rs;
	KuduFace *lf, *rf;
	KuduVertex *sv, *ev;

	do {
		if (current_edge == NULL) current_edge = edge;
		else	current_edge = current_edge->next_edge;
		fwrite(current_edge, sizeof(int), 1, file);

		lf = current_edge->left_face;
		rf = current_edge->right_face;
		lp = current_edge->left_pred;
		ls = current_edge->left_succ;
		rp = current_edge->right_pred;
		rs = current_edge->right_succ;
		sv = current_edge->start_vertex;
		ev = current_edge->end_vertex;

		for (a = 0; a < 8; a++) misc[a] = -1;
		if (lf != NULL) misc[0] = lf->id;
		if (rf != NULL) misc[1] = rf->id;
		if (lp != NULL) misc[2] = lp->id;
		if (ls != NULL) misc[3] = ls->id;
		if (rp != NULL) misc[4] = rp->id;
		if (rs != NULL) misc[5] = rs->id;
		if (sv != NULL) misc[6] = sv->id;
		if (ev != NULL) misc[7] = ev->id;

		fwrite(misc, sizeof(int), 8, file);

	} while (current_edge->next_edge != NULL);
}*/

/*int kudu_object_internal_dump_skin(KuduShape *skin, FILE *file)
{
	if (skin == NULL) return FALSE;
	if (file == NULL) return FALSE;

	int misc[4];

	KuduShape *current_shape = NULL;

	do {
		if (current_shape == NULL) current_shape = skin;
		else	current_shape = current_shape->next_shape;

		misc[0] = current_shape->id;
		misc[1] = kudu_shape_num_vertices(current_shape);
		misc[2] = kudu_shape_num_faces(current_shape);
		misc[3] = kudu_shape_num_edges(current_shape);
		fwrite(misc, sizeof(int), 4, file);
		if (misc[1] > 0) kudu_object_internal_dump_vertices(current_shape->vertex, file);
		if (misc[2] > 0) kudu_object_internal_dump_faces(current_shape->face, file);
		if (misc[3] > 0) kudu_object_internal_dump_edges(current_shape->edge, file);

	} while (current_shape->next_shape != NULL);


	return TRUE;
}*/

/*int kudu_object_internal_dump_materials(KuduMaterial *material, FILE *file)
{
	if (material == NULL) return FALSE;
	if (file == NULL) return FALSE;

	int misc[4];
	KuduMaterial *current_material = NULL;

	misc[0] = kudu_material_num_after(material);
	fwrite(misc, sizeof(int), 1, file);

	do {
		if (current_material == NULL) current_material = material;
		else	current_material = current_material->next_material;

		fwrite(current_material, sizeof(int) + 64+ (sizeof(float) * 17), 1, file);

	} while (current_material->next_material != NULL);

	return TRUE;
}*/


/*int kudu_object_write_to_file(KuduObject *object, char *filename)
{
	if (object == NULL) return FALSE;
	if (filename == NULL) return FALSE;

	FILE *file;
	char header_id[6] = {"danobj"};
	char header_comment[256] = {"Created with Kudubuilder"};
	unsigned char header_version[4] = {PROGRAM_VERSION_MAJOR, PROGRAM_VERSION_MINOR, 0, 0};
	KuduVertex *current_vertex;
	KuduEdge *current_edge;
	KuduFace *current_face;
	KuduShape *current_shape;

	if ((file = fopen(filename, "w")) == NULL) return FALSE;
*/
	/* Create file header */
/*	fwrite(header_id, 1, 6, file);
	fwrite(header_version, 1, 4, file);
	fwrite(header_comment, 1, 256, file);
	fwrite(object, 1, 256, file);

	kudu_object_internal_dump_bones(object->bone, file);

	kudu_object_internal_dump_materials(object->material, file);

	kudu_object_internal_dump_skin(object->skin, file);

	fclose(file);

	return TRUE;
}*/

/* Save the author's name, email and url (if they have been specified) */
int kudu_object_save_author_details(KuduObject *object, FILE *file)
{
	if ((file == NULL) || (object == NULL)) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

/*	const char *author_name = kudu_options_get_string("USER_NAME");
	const char *author_mail = kudu_options_get_string("USER_MAIL");
	const char *author_url = kudu_options_get_string("USER_URL");*/
	const char *author_name = object->author;
	const char *author_mail = object->email;
	const char *author_url = object->url;

	/* Save Author's name */
	if (author_name != NULL) 
		if (strlen(author_name) > 0) {
			fputc(KOF_AUTHOR_NAME, file);
			fputc(strlen(author_name), file);
			fwrite(author_name, strlen(author_name), 1, file);
		}

	/* Save Author's email */
	if (author_mail != NULL)
		if (strlen(author_mail) > 0) {
			fputc(KOF_AUTHOR_MAIL, file);
			fputc(strlen(author_mail), file);
			fwrite(author_mail, strlen(author_mail), 1, file);
		}

	/* Save Author's url */
	if (author_url != NULL)
		if (strlen(author_url) > 0) {
			fputc(KOF_AUTHOR_URL, file);
			fputc(strlen(author_url), file);
			fwrite(author_url, strlen(author_url), 1, file);
		}

	return TRUE;
}

int kudu_object_save_bones(KuduBone *bone, FILE *file)
{
	if ((bone == NULL) || (file == NULL)) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	KuduBone *current_bone, *p_bone;
	KuduJoint *joint;
	int gone_back = FALSE, p;
	unsigned char nl;

	current_bone = bone;

	do {
		if (!gone_back) {
			/* Basic Bone info header */
			fputc(KOF_BONE, file);
			/* The basic bone info includes id, name, h, v, r angles, length, and parent id */
			/* Bone id */
			fwrite(&current_bone->id, sizeof(int), 1, file);
			/* Length of bone name & actual name if longer than 0 */
			nl = (unsigned char)strlen(current_bone->name);
			fputc(nl, file);
			if (nl > 0) fwrite(current_bone->name, nl, 1, file);

			/* h, v, r angles and length of bone */
			/*fwrite(&current_bone->hAngle, sizeof(float), 1, file);
			fwrite(&current_bone->vAngle, sizeof(float), 1, file);
			fwrite(&current_bone->rAngle, sizeof(float), 1, file);
			fwrite(&current_bone->length, sizeof(float), 1, file);*/

			/* Important quaternions */
			fwrite(current_bone->lquat, sizeof(float), 4, file);
			fwrite(current_bone->gquat, sizeof(float), 4, file);

			/* Bone length */
			fwrite(&current_bone->length, sizeof(float), 1, file);

			/* Oops... there's no reason to save pose quats... */
			/*fwrite(current_bone->lpquat, sizeof(float), 4, file);*/

			/* Parent id, if parent is null then write own id indicating a root bone */
			p_bone = current_bone->parent;
			if (p_bone == NULL) p = current_bone->id;
			else	p = p_bone->id;
			fwrite(&p, sizeof(int), 1, file);

			if (p == current_bone->id) {	/* Root bone save the bone position */
				fputc(KOF_BONE_POS, file);
				joint = current_bone->s_joint;

				fwrite(&joint->pos[0], sizeof(float), 3, file);

				/*fwrite(&current_bone->posX, sizeof(float), 1, file);
				fwrite(&current_bone->posY, sizeof(float), 1, file);
				fwrite(&current_bone->posZ, sizeof(float), 1, file);*/
			}
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

/* Save all vertices in this linked-list */
int kudu_object_save_vertices(KuduVertex *vertex, FILE *file)
{
	if ((vertex == NULL) || (file == NULL)) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	KuduVertex *current_vertex = NULL;
	KuduBone *bone;
	int a;

	do {
		if (current_vertex == NULL) current_vertex = vertex;
		else	current_vertex = current_vertex->next_vertex;

		/* Basic vertex info header */
		fputc(KOF_VERTEX, file);
		/* The basic vertex info includes id and x,y,z position */
		/* Vertex id */
		fwrite(&current_vertex->id, sizeof(int), 1, file);
		/* Vertex x,y,z */
		fwrite(current_vertex->v, sizeof(float), 3, file);

		if (current_vertex->num_bones > 0) {	/* Vertex is attached, save attachment info */
			/* Vertex attachment properties header */
			fputc(KOF_VERTEX_ATTACHMENT, file);
			/* No. of attached bones */
			fwrite(&current_vertex->num_bones, sizeof(int), 1, file);

			for (a = 0; a < current_vertex->num_bones; a++) {
				bone = current_vertex->bone[a];
				/* Bone id */
				fwrite(&bone->id, sizeof(int), 1, file);
				/* Attached percent */
				fwrite(&current_vertex->influence[a], sizeof(float), 1, file);
			}
		}

	} while (current_vertex->next_vertex != NULL);

	return TRUE;
}

/* Save face linked list */
int kudu_object_save_faces(KuduFace *face, FILE *file)
{
	if ((face == NULL) || (file == NULL)) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	KuduFace *current_face = NULL;
	KuduMaterial *material;
	int nl;

	do {
		if (current_face == NULL) current_face = face;
		else	current_face = current_face->next_face;

		/* Basic face info header */
		fputc(KOF_FACE, file);
		/* The basic face info includes id and material id */
		/* Face id */
		fwrite(&current_face->id, sizeof(int), 1, file);

		/* Material id */
		material = current_face->material;
		if (material != NULL) nl = material->id;
		else	nl = -1;
		fwrite(&nl, sizeof(int), 1, file);

	} while (current_face->next_face != NULL);


	return TRUE;
}

/* Save edge linked list */
int kudu_object_save_edges(KuduEdge *edge, FILE *file)
{
	if ((edge == NULL) || (file == NULL)) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	int misc[8], a;
	KuduEdge *current_edge = NULL, *lp, *ls, *rp, *rs;
	KuduFace *lf, *rf;
	KuduVertex *sv, *ev;

	do {
		if (current_edge == NULL) current_edge = edge;
		else	current_edge = current_edge->next_edge;

		/* Basic edge info header */
		fputc(KOF_EDGE, file);
		/* The basic edge info includes id, all incident edges left & right faces, start & end vertices */
		/* Edge id */
		fwrite(&current_edge->id, sizeof(int), 1, file);

		/* All incident ids */
		lf = current_edge->left_face;
		rf = current_edge->right_face;
		lp = current_edge->left_pred;
		ls = current_edge->left_succ;
		rp = current_edge->right_pred;
		rs = current_edge->right_succ;
		sv = current_edge->start_vertex;
		ev = current_edge->end_vertex;

		for (a = 0; a < 8; a++) misc[a] = -1;
		if (lf != NULL) misc[0] = lf->id;	/* Left face id */
		if (rf != NULL) misc[1] = rf->id;	/* Right face id */
		if (lp != NULL) misc[2] = lp->id;	/* Left Pred edge id */
		if (ls != NULL) misc[3] = ls->id;	/* Left Succ edge id */
		if (rp != NULL) misc[4] = rp->id;	/* Right Pred edge id */
		if (rs != NULL) misc[5] = rs->id;	/* Right Succ edge id */
		if (sv != NULL) misc[6] = sv->id;	/* Start Vertex id */
		if (ev != NULL) misc[7] = ev->id;	/* End Vertex id */

		fwrite(misc, sizeof(int), 8, file);

		/* If all tex coords are 0 then don't save them, as most likely there is no texture... */
		/* .. even if they are, default values are 0 any way... */
		if ((current_edge->s_uv[0] != 0) || (current_edge->s_uv[1] != 0) ||
		    (current_edge->e_uv[0] != 0) || (current_edge->e_uv[1] != 0)) {
			fputc(KOF_EDGE_TEX_COORDS, file);
			fwrite(current_edge->s_uv, sizeof(float), 2, file);
			fwrite(current_edge->e_uv, sizeof(float), 2, file);
		}

	} while (current_edge->next_edge != NULL);

	return TRUE;
}

int kudu_object_save_skin(KuduShape *skin, FILE *file)
{
	if ((skin == NULL) || (file == NULL)) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	KuduShape *current_shape = NULL;

	do {
		if (current_shape == NULL) current_shape = skin;
		else	current_shape = current_shape->next_shape;

		/* Basic shape info header */
		fputc(KOF_SHAPE, file);
		/* The basic shape info includes the id */
		/* Shape id */
		fwrite(&current_shape->id, sizeof(int), 1, file);

		/* Write all verts, faces & edges */
		if (current_shape->vertex != NULL) kudu_object_save_vertices(current_shape->vertex, file);
		if (current_shape->face != NULL) kudu_object_save_faces(current_shape->face, file);
		if (current_shape->edge != NULL) kudu_object_save_edges(current_shape->edge, file);


	} while (current_shape->next_shape != NULL);

	return TRUE;
}

/* Save material linked list */
int kudu_object_save_materials(KuduMaterial *material, FILE *file)
{
	if ((material == NULL) || (file == NULL)) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	KuduMaterial *current_material = NULL;
	KuduTexture *texture = NULL;
	KuduImage *image = NULL;
	unsigned char nl;

	do {
		if (current_material == NULL) current_material = material;
		else	current_material = current_material->next_material;

		/* Basic material info header */
		fputc(KOF_MATERIAL, file);
		/* The basic material info includes id, name, diffuse, ambient, specular, emission and shininess */
		/* Material id */
		fwrite(&current_material->id, sizeof(int), 1, file);
		/* Length of name */
		nl = strlen(current_material->name);
		fputc(nl, file);
		/* Name if longer than 0 */
		if (nl > 0) fwrite(current_material->name, nl, 1, file);
		/* Diffuse */
		fwrite(current_material->diffuse, sizeof(float), 4, file);
		/* Ambient */
		fwrite(current_material->ambient, sizeof(float), 4, file);
		/* Specular */
		fwrite(current_material->specular, sizeof(float), 4, file);
		/* Emission */
		fwrite(current_material->emission, sizeof(float), 4, file);
		/* Shininess */
		fwrite(&current_material->shininess, sizeof(float), 1, file);

		texture = current_material->texture;
		if (texture != NULL) image = texture->image;
		if (image != NULL) {
			fputc(KOF_TEXTURE, file);	/* Texture header */
			fputc(KOFS_TEXTURE_TYPE_INTERNAL_RAW, file);	/* Texture type sub-header ... for now just do internal */

			fwrite(&image->width, sizeof(unsigned int), 1, file);
			fwrite(&image->height, sizeof(unsigned int), 1, file);
			fwrite(&image->channels, sizeof(unsigned int), 1, file);
			fwrite(&image->bpp, sizeof(unsigned int), 1, file);
			fwrite(&image->size, sizeof(unsigned int), 1, file);
			fwrite(image->data, sizeof(unsigned char), image->size, file);
		}

	} while (current_material->next_material != NULL);

	return TRUE;
}

int kudu_object_save_to_file(KuduObject *object, char *filename)
{
	if ((object == NULL) || (filename == NULL)) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	FILE *file;
	KuduBone *current_bone = NULL;

	if ((file = fopen(filename, "w")) == NULL) return FALSE;

	/* Create the main file header */
	/* Twelve bytes id header */
	fwrite(KuduObject_HEADER, sizeof(KuduObject_HEADER), 1, file);
	/* Two bytes file version - Meaning file format version *not* program version */
	fwrite(KuduObject_FILE_VERSION, sizeof(KuduObject_FILE_VERSION), 1, file);
	/* Optional string comment, one byte determines length of comment - max 255, 0 = no comment */
	fputc(strlen(KuduObject_COMMENT), file);
	if (strlen(KuduObject_COMMENT) > 0) fwrite(KuduObject_COMMENT, sizeof(KuduObject_COMMENT), 1, file);

	/* Dump author details if specified */
	kudu_object_save_author_details(object, file);

	/* Save object name (if set) */
	if (strlen(object->name) > 0) {
		fputc(KOF_OBJECT_NAME, file);
		fputc(strlen(object->name), file);
		fwrite(object->name, strlen(object->name), 1, file);
	}

	/* Save object bones */
	if (object->bone != NULL) {
		do {
			if (current_bone == NULL) current_bone = object->bone;
			else	current_bone = current_bone->next_sibling;
			kudu_object_save_bones(current_bone, file);
		} while (current_bone->next_sibling != NULL);
	}

	/* Save materials */
	if (object->material != NULL) kudu_object_save_materials(object->material, file);
	/* Save skin (shapes, vertices, faces & edges) */
	if (object->skin != NULL) kudu_object_save_skin(object->skin, file);

	fclose(file);

	return TRUE;
}

int kudu_object_load_bone(KuduObject *object, FILE *file, int what)
{
	static KuduBone *buffer = NULL;
	static KuduBone *bone = NULL;

	/* This function creates a temporary skeleton structure */
	/* for storing newly loaded bones that cant yet be inserted */
	/* into the true skeleton, once the whole file has been read */
	/* it must be called with NULL for both pointers to destroy it */
	if ((object == NULL) || (file == NULL)) {
		if (buffer != NULL) {
			kudu_bone_destroy(buffer);
			buffer = NULL;
			bone = NULL;
			return TRUE;
		}
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	KuduBone *pbone = NULL;
	KuduJoint *joint;
	unsigned char nl;
	int p, id;
	float x, y, h, v, r;

	/* Initialize the bone buffer if it is null */
	if (buffer == NULL) {
		buffer = kudu_bone_new_with_id(object, 0);
	}

	if (what == KOF_BONE) {	/* Load a bone */
		bone = NULL;

		/* First we read the bone id */
		fread(&id, sizeof(int), 1, file);

		/* Now we check if it already exists */
		if (object->bone != NULL) {
			if (kudu_bone_next_id(object) >= id) {	/* Bone *might* already exist */
				bone = kudu_bone_find_with_id(object->bone, id);
				/* If bone was not found in the skeleton, then check the buffer */
				if (bone == NULL) {
					bone = kudu_bone_find_with_id(buffer, id);
					/* If we found the bone in the buffer then we must "unlink" it */
					if (bone != NULL) kudu_bone_unlink(bone);
				}
			}
		}

		if (bone == NULL) {	/* Bone does not exist - create it */
			bone = kudu_bone_new_with_id(object, id);
			if (object->bone == NULL) object->bone = bone;
		}

		if (bone == NULL) return FALSE;	/* Weird error somewhere */

		/* Length of bone name */
		fread(&nl, sizeof(unsigned char), 1, file);
		/* Read name if longer than 0 */
		if (nl > 0) {
			fread(bone->name, nl, 1, file);
			bone->name[nl] = '\0';
		}


		if ((c_file_ver[0] >= 0) && (c_file_ver[1] > 1)) {
			fread(bone->lquat, sizeof(float), 4, file);
			fread(bone->gquat, sizeof(float), 4, file);

			/* Stupid mistake in ver 0.2 ... was saving local pose quat ...
			   just skip the bytes and ignore them .. */
			if (c_file_ver[1] == 2)	fseek(file, (sizeof(float)*4), SEEK_CUR);
			/* Another stupid mistake... was not saving bone length... */
			else fread(&bone->length, sizeof(float), 1, file);

		} else if ((c_file_ver[0] == 0) && (c_file_ver[1] < 2)) {
			/* h, v, r angles and length of bone */
			fread(&h, sizeof(float), 1, file);
			fread(&v, sizeof(float), 1, file);
			fread(&r, sizeof(float), 1, file);
			fread(&bone->length, sizeof(float), 1, file);

			kudu_bone_apply_rotation(bone, v, 0, FALSE);
			kudu_bone_apply_rotation(bone, h, 1, FALSE);
			bone->quat[0] = h;
			bone->quat[1] = v;
			bone->quat[2] = r;
		}

		/* Parent id */
		fread(&p, sizeof(int), 1, file);

		if (p == bone->id) {
			if (object->bone != bone) {	/* Is this the *first* root bone ?? if not it must be adopted */
				kudu_bone_adopt_sibling(object->bone, bone);
			}
			return TRUE;	/* Ok this is a root bone - no further work required */
		}

		/* Does parent bone exist */
		if (kudu_bone_next_id(object) >= p) {	/* Parent bone *might* already exist */
			pbone = kudu_bone_find_with_id(object->bone, p);
			/* If bone was not found in the skeleton then check the buffer */
			if (pbone == NULL)  {
				pbone = kudu_bone_find_with_id(buffer, p);
				if (pbone != NULL) kudu_bone_unlink(pbone);
			}
		}

		if (pbone == NULL) {	/* Parent bone does not exist - create it */
			pbone = kudu_bone_new_with_id(object, p);
			if (object->bone == bone) object->bone = pbone;
			else {	/* Right parent bone has nowhere to go (yet), so temporary store it in the buffer */
				kudu_bone_adopt_child(buffer, pbone);
			}
		}

		kudu_bone_adopt_child(pbone, bone);

	} else if (what == KOF_BONE_POS) {	/* Load the position of the last loaded bone */
						/* Should only be saved for root bones */
		if (bone == NULL) {
			kudu_error(KE_OBJECT_INVALID);
			return FALSE;
		}
		joint = bone->s_joint;
		fread(&joint->pos[0], sizeof(float), 3, file);

		/*fread(&bone->posX, sizeof(float), 1, file);
		fread(&bone->posY, sizeof(float), 1, file);
		fread(&bone->posZ, sizeof(float), 1, file);*/
	}

	return TRUE;
}

int kudu_object_load_material(KuduObject *object, FILE *file, int what)
{
	static KuduMaterial *current_material = NULL;

	if ((object == NULL) || (file == NULL)) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	unsigned char nl;
	KuduTexture *texture = NULL;
	KuduImage *image = NULL;

	switch (what) {
		case KOF_MATERIAL:
			current_material = kudu_material_new(object, object->material);
			if (current_material == NULL) return FALSE;

			if (object->material == NULL) object->material = current_material;

			/* Material id */
			fread(&current_material->id, sizeof(int), 1, file);
			/* Length of name */

			fread(&nl, sizeof(unsigned char), 1, file);
			/* Name if longer than 0 */
			if (nl > 0) {
				fread(current_material->name, nl, 1, file);
				current_material->name[nl] = '\0';
			}

			/* Diffuse */
			fread(current_material->diffuse, sizeof(float), 4, file);
			/* Ambient */
			fread(current_material->ambient, sizeof(float), 4, file);
			/* Specular */
			fread(current_material->specular, sizeof(float), 4, file);
			/* Emission */
			fread(current_material->emission, sizeof(float), 4, file);
			/* Shininess */
			fread(&current_material->shininess, sizeof(float), 1, file);
			break;

		case KOF_TEXTURE:
			fread(&nl, sizeof(unsigned char), 1, file);
			switch (nl) {
				case KOFS_TEXTURE_TYPE_EXTERNAL:
					break;
				case KOFS_TEXTURE_TYPE_INTERNAL_RAW:
					texture = kudu_texture_new(object, object->texture);
					image = kudu_image_new();
					fread(&image->width, sizeof(unsigned int), 1, file);
					fread(&image->height, sizeof(unsigned int), 1, file);
					fread(&image->channels, sizeof(unsigned int), 1, file);
					fread(&image->bpp, sizeof(unsigned int), 1, file);
					fread(&image->size, sizeof(unsigned int), 1, file);

					image->data = (unsigned char*)malloc(image->size);
					fread(image->data, sizeof(unsigned char), image->size, file);

					kudu_texture_assign_image(texture, image);
					break;
			}
			if (object->texture == NULL) object->texture = texture;
			current_material->texture = texture;
			break;
	}

	return TRUE;
}

int kudu_object_load_shape(KuduObject *object, FILE *file)
{
	if ((object == NULL) || (file == NULL)) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	KuduShape *last = NULL, *current_shape;

	/* Resolve the last shape in the skin list (if any) */
	if (object->skin != NULL) {
		do {
			if (last == NULL) last = object->skin;
			else	last = last->next_shape;
		} while (last->next_shape != NULL);
	}

	/* Create new shape, set as first in the list if it is empty */
	current_shape = kudu_shape_new(object, last);
	if (object->skin == NULL) object->skin = current_shape;

	/* Shape id */
	fread(&current_shape->id, sizeof(int), 1, file);

	return TRUE;
}

int kudu_object_load_vertex(KuduObject *object, FILE *file, int what)
{
	static KuduShape *current_shape = NULL;

	if ((object == NULL) || (file == NULL)) {
		if (current_shape != NULL) {
			current_shape = NULL;
			return TRUE;
		}
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}
	if (object->skin == NULL) return FALSE;

	KuduVertex *current_vertex;
	KuduBone *bone;
	int id, num, a;
	float influence;

	/* Resolve last shape in skin list / make sure we still have the last shape */
	if ((current_shape == NULL) || (current_shape->next_shape != NULL)) {
		do {
			if (current_shape == NULL) current_shape = object->skin;
			else	current_shape = current_shape->next_shape;
		} while (current_shape->next_shape != NULL);
	}


	switch (what) {
		case KOF_VERTEX:
			/* Vertex id */
			fread(&id, sizeof(int), 1, file);

			/* Does vertex exist ?? otherwise create it */
			current_vertex = kudu_vertex_find_with_id(current_shape->vertex, id);
			if (current_vertex == NULL) {
				current_vertex = kudu_vertex_new_with_id(object, current_shape->vertex, id);
				if (current_shape->vertex == NULL) current_shape->vertex = current_vertex;
			}

			fread(current_vertex->v, sizeof(float), 3, file);
			break;

		case KOF_VERTEX_ATTACHMENT:
			/* Find the most recently created vertex, normally the second one in the list */
			current_vertex = current_shape->vertex;
			if (current_vertex->next_vertex != NULL) current_vertex = current_vertex->next_vertex;
			if (current_vertex == NULL) return FALSE;

			if ((c_file_ver[0] >= 0) && (c_file_ver[1] > 1)) {
				/* No. of attached bones */
				fread(&num, sizeof(int), 1, file);

				for (a = 0; a < num; a++) {
					/* Bone id */
					fread(&id, sizeof(int), 1, file);
					/* Influence */
					fread(&influence, sizeof(float), 1, file);

					/* Assign the bone to the vertex */
					if (object->bone != NULL) {
						bone = kudu_bone_find_with_id(object->bone, id);
						kudu_vertex_set_bone_influence(current_vertex, bone, influence);
					}
				}

			} else if ((c_file_ver[0] == 0) && (c_file_ver[1] < 2)) { /* Older file version... */
				/* Attached percent */
				fread(&current_vertex->attached_percent, sizeof(unsigned char), 1, file);
				influence = (float)current_vertex->attached_percent;
				/* Bone id */
				fread(&id, sizeof(int), 1, file);
				if (object->bone != NULL) {
					bone = kudu_bone_find_with_id(object->bone, id);
					if (bone != NULL) {
						/* The older internal system worked a bit differently...
						   with the new system an older file will never be 100% exactly perfect on opening.. too bad..
						   increasing influence by 10% makes it pretty accurate though.. */
						influence += 10;
						if (influence > 100) influence = 100;
						kudu_vertex_set_bone_influence(current_vertex, bone, influence);

						/* File version 0.1 only had support for a max of two bones per vertice, the second of which
						   always was the parent of the first... if influence is less than 100% it assumes the inverse
						   percent attachment on the parent bone. */
						if ((influence < 100.0) && (bone->parent != NULL)) {
							kudu_vertex_set_bone_influence(current_vertex, bone->parent, (100.0 - influence));
						}
					}
				}
			}
			break;
	}
	
	return TRUE;
}

int kudu_object_load_face(KuduObject *object, FILE *file, int what)
{
	static KuduShape *current_shape = NULL;

	if ((object == NULL) || (file == NULL)) {
		if (current_shape != NULL) {
			current_shape = NULL;
			return TRUE;
		}
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}
	if (object->skin == NULL) return FALSE;

	int id;
	KuduFace *current_face = NULL;

	/* Resolve last shape in skin list / make sure we still have the last shape */
	if ((current_shape == NULL) || (current_shape->next_shape != NULL)) {
		do {
			if (current_shape == NULL) current_shape = object->skin;
			else	current_shape = current_shape->next_shape;
		} while (current_shape->next_shape != NULL);
	}

	switch (what) {
		case KOF_FACE:
			/* Face id */
			fread(&id, sizeof(int), 1, file);

			if (kudu_face_next_id(object) >= id) {	/* Face *might* already exist */
				current_face = kudu_face_find_with_id(current_shape->face, id);
			}

			/* If face does not exist - create it */
			if (current_face == NULL) current_face = kudu_face_new_with_id(object, current_shape->face, id);
			if (current_face == NULL) return FALSE;
			if (current_shape->face == NULL) current_shape->face = current_face;

			/* Material id */
			fread(&id, sizeof(int), 1, file);
			if (id >= 0) current_face->material = kudu_material_find_with_id(object->material, id);
			break;
	}

	return TRUE;
}

int kudu_object_load_edge(KuduObject *object, FILE *file, int what)
{
	static KuduShape *current_shape = NULL;
	static KuduEdge *current_edge = NULL;

	if ((object == NULL) || (file == NULL)) {
		current_shape = NULL;
		current_edge = NULL;
		return TRUE;
	}
	if (object->skin == NULL) return FALSE;

	int id, misc[4], edge_ids[5], a, b;
	KuduFace *face_list[2];
	KuduEdge *edge_list[5];
	KuduVertex *vertex_list[2];

	/* Resolve last shape in skin list / make sure we still have the last shape */
	if ((current_shape == NULL) || (current_shape->next_shape != NULL)) {
		do {
			if (current_shape == NULL) current_shape = object->skin;
			else	current_shape = current_shape->next_shape;
		} while (current_shape->next_shape != NULL);
	}

	switch (what) {
		case KOF_EDGE:
			/* Edge id */
			fread(edge_ids, sizeof(int), 1, file);
			id = edge_ids[0];

			/* All incident ids */
			fread(misc, sizeof(int), 2, file);	/* Left & Right face */
			fread(&edge_ids[1], sizeof(int), 4, file);	/* LP, LS, RP, RS edges */
			fread(&misc[2], sizeof(int), 2, file);	/* Start & End vertices */

			/* Search for edges that may exist already */
			if (current_shape->edge != NULL) kudu_edges_find_with_ids(current_shape->edge, 5, edge_ids, edge_list);
			else for (a = 0; a < 5; a++) edge_list[a] = NULL;

			/* Create edges that do not exist */
			for (a = 0; a < 5; a++) {
				if (edge_list[a] == NULL) {
					edge_list[a] = kudu_edge_new_with_id(object, current_shape->edge, edge_ids[a]);
					if (current_shape->edge == NULL) current_shape->edge = edge_list[a];
					edge_list[a]->shape = current_shape;

					/* Just in case an edge "further" in the list than this one is actually */
					/* the same as this one we just created, prevent double creating */
					for (b = a+1; b < 5; b++)
						if (edge_ids[b] == edge_ids[a])
							edge_list[b] = edge_list[a];
				}
			}

			current_edge = edge_list[0];

			/* Search for left and right faces */
			kudu_faces_find_with_ids(current_shape->face, 2, misc, face_list);

			/* Search for start and end vertices */
			kudu_vertices_find_with_ids(current_shape->vertex, 2, &misc[2], vertex_list);

			kudu_edge_vertices_set(current_edge, vertex_list[0], vertex_list[1]);
			kudu_edge_faces_set(current_edge, face_list[0], face_list[1]);

			kudu_edge_wings_set(current_edge, edge_list[2], edge_list[1], edge_list[4], edge_list[3]);

			break;
		case KOF_EDGE_TEX_COORDS:
			fread(current_edge->s_uv, sizeof(float), 2, file);
			fread(current_edge->e_uv, sizeof(float), 2, file);
			break;
	}

	return TRUE;
}

int kudu_object_load_from_file(KuduObject *object, char *filename)
{
	if (filename == NULL) {
		kudu_error(KE_FILE_NAME_INVALID);
		return FALSE;
	}
	if (object == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	FILE *file;
	unsigned char header[12], file_ver[2], cl, sh;
	int a, rv = TRUE;
	char inf[256];

	if ((file = fopen(filename, "r")) == NULL) {
		kudu_error(KE_FILE_CANNOT_OPEN);
		return FALSE;
	}

	if (fread(header, 12, 1, file) == 0) {
		kudu_error(KE_FILE_INVALID);
		fclose(file);
		return FALSE;
	}

	if (memcmp(KuduObject_HEADER, header, 12) != 0) {	/* Check validity of file */
		kudu_error(KE_FILE_INVALID);
		fclose(file);
		return FALSE;
	}

	if (fread(file_ver, 2, 1, file) == 0) {
		kudu_error(KE_FILE_INVALID);
		fclose(file);
		kudu_object_destroy(object);
		return FALSE;
	}

	if ((file_ver[0] > KuduObject_FILE_VER_MAJOR) || (file_ver[1] > KuduObject_FILE_VER_MINOR)) {
		kudu_error(KE_FILE_NEWER_VERSION);
		fclose(file);
		kudu_object_destroy(object);
		return FALSE;
	}

	/* Set the global file version varibles so that the various loading routines know which version file we are loading */
	c_file_ver[0] = file_ver[0];
	c_file_ver[1] = file_ver[1];

	kudu_bone_force_next_id(object, 0);
	kudu_vertex_force_next_id(object, 0);
	kudu_edge_force_next_id(object, 0);
	kudu_face_force_next_id(object, 0);
	kudu_material_force_next_id(object, 0);
	kudu_shape_force_next_id(object, 0);

	/* Length of comment */
	fread(&cl, sizeof(unsigned char), 1, file);
	fseek(file, (long)cl, SEEK_CUR); /* Not interested in comment - just pass it by */

	/* Thats all the fixed info read, now we read one byte "sub-headers" to determine whats coming */

	while ((fread(&sh, sizeof(unsigned char), 1, file) != 0) && (rv)) {	/* Read one "sub-header" */

		switch (sh) {
			case KOF_AUTHOR_NAME:	/* Authors name */
				fread(&cl, sizeof(unsigned char), 1, file);
				if (cl > 0) {
					/*fseek(file, (long)cl, SEEK_CUR);*/
					fread(inf, sizeof(char), cl, file);
					inf[cl] = '\0';
					kudu_object_set_author(object, inf);
				}
				break;
			case KOF_AUTHOR_MAIL:	/* Authors email */
				fread(&cl, sizeof(unsigned char), 1, file);
				if (cl > 0) {
					/*fseek(file, (long)cl, SEEK_CUR);*/
					fread(inf, sizeof(char), cl, file);
					inf[cl] = '\0';
					kudu_object_set_email(object, inf);
				}
				break;
			case KOF_AUTHOR_URL:	/* Authors url */
				fread(&cl, sizeof(unsigned char), 1, file);
				if (cl > 0) {
					/*fseek(file, (long)cl, SEEK_CUR);*/
					fread(inf, sizeof(char), cl, file);
					inf[cl] = '\0';
					kudu_object_set_url(object, inf);
				}
				break;
			case KOF_OBJECT_NAME:
				fread(&cl, sizeof(unsigned char), 1, file);	/* Length of name */
				if (cl > 0) {
					fread(object->name, cl, 1, file);	/* Name */
					object->name[cl] = '\0';
				}
				break;
			case KOF_BONE:
				rv = kudu_object_load_bone(object, file, KOF_BONE);
				break;
			case KOF_BONE_POS:
				rv = kudu_object_load_bone(object, file, KOF_BONE_POS);
				break;
			case KOF_SHAPE:
				rv = kudu_object_load_shape(object, file);
				break;
			case KOF_EDGE:
				rv = kudu_object_load_edge(object, file, KOF_EDGE);
				break;
			case KOF_EDGE_TEX_COORDS:
				rv = kudu_object_load_edge(object, file, KOF_EDGE_TEX_COORDS);
				break;
			case KOF_VERTEX:
				rv = kudu_object_load_vertex(object, file, KOF_VERTEX);
				break;
			case KOF_VERTEX_ATTACHMENT:
				rv = kudu_object_load_vertex(object, file, KOF_VERTEX_ATTACHMENT);
				break;
			case KOF_FACE:
				rv = kudu_object_load_face(object, file, KOF_FACE);
				break;
			case KOF_MATERIAL:
				rv = kudu_object_load_material(object, file, KOF_MATERIAL);
				break;
			case KOF_TEXTURE:
				rv = kudu_object_load_material(object, file, KOF_TEXTURE);
				break;
			default:
				break;
		}

	}

	/* These functions create buffers or retain pointers to some or other */
	/* things, passing them NULL values will destroy/clear these pointers */
	kudu_object_load_bone(NULL, NULL, 0);
	kudu_object_load_vertex(NULL, NULL, 0);
	kudu_object_load_face(NULL, NULL, 0);
	kudu_object_load_edge(NULL, NULL, 0);

	kudu_object_repair_old_structure(object);

	kudu_object_update(object);

	return rv;
}

/* "Repair" anything that might have been changed in the case of loading an older file version... well attempt to at least :-/ */
int kudu_object_repair_old_structure(KuduObject *object)
{
	if (object == NULL) return FALSE;

	int a, b;
	float v[3], ov[3], inf;
	KuduBone *bone;
	KuduJoint *joint;
	KuduShape *shape;
	KuduVertex *vertex;

	/* Repair vertices loaded from version 0.1 */
	if ((c_file_ver[0] == 0) && (c_file_ver[1] == 1)) {
		kudu_bone_update_all(object->bone);

		if (object->skin != NULL) {
			shape = NULL;
			for (shape = object->skin; shape != NULL; shape = shape->next_shape) {
				if (shape->vertex != NULL)
					for (vertex = shape->vertex; vertex != NULL; vertex = vertex->next_vertex) {
						if (vertex->num_bones == 1) {
							bone = vertex->bone[0];
							joint = bone->e_joint;

							kudu_math_transform_vertex3v_by_matrix(vertex->v, bone->matrix);
							for (a = 0; a < 3; a++) vertex->v[a] += joint->pos[a];

						} else if (vertex->num_bones == 2) {
							for (a = 0; a < 3; a++) {
								ov[a] = vertex->v[a];
								vertex->v[a] = 0.0;
							}

							for (b = 0; b < 2; b++) {
								bone = vertex->bone[b];
								joint = bone->e_joint;
								inf = (vertex->influence[b] / 100.0f);
								for (a = 0; a < 3; a++) v[a] = ov[a];
								kudu_math_transform_vertex3v_by_matrix(v, bone->matrix);
								for (a = 0; a < 3; a++) {
									vertex->v[a] += (v[a] * inf);// + (joint->pos[a] * inf);
									if (b == 0) vertex->v[a] += joint->pos[a];
								}
							}
						}
					}
			}
		}	
	}

	return TRUE;
}

KuduObject *kudu_object_new_from_file(KuduObject *previous_object, char *filename)
{
	if (filename == NULL) {
		kudu_error(KE_FILE_NAME_INVALID);
		return NULL;
	}

	KuduObject *object = kudu_object_new(previous_object);
	if (kudu_object_load_from_file(object, filename)) return object;

	kudu_object_destroy(object);

	return NULL;
}

/* Completely update a KuduObject, bones vertices etc... */
/* Obviously this is not performance optimised... */
int kudu_object_update(KuduObject *object)
{
	if (object == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	KuduShape *current_shape = NULL;

	if (object->bone != NULL) kudu_bone_update_all(object->bone);


	if (object->skin != NULL) {
		do {
			if (current_shape == NULL) current_shape = object->skin;
			else	current_shape = current_shape->next_shape;

			if (current_shape->vertex != NULL) kudu_vertex_update_vertices(current_shape->vertex);

			kudu_shape_normals_calculate(current_shape);

		} while (current_shape->next_shape != NULL);
	}

	return TRUE;
}

/* Update the object skin (vertices) and optionally the normals as well */
int kudu_object_update_vertices(KuduObject *object, int do_normals)
{
	if (object == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	KuduShape *current_shape = NULL;


	if (object->skin != NULL) {
		do {
			if (current_shape == NULL) current_shape = object->skin;
			else	current_shape = current_shape->next_shape;

			if (current_shape->vertex != NULL) kudu_vertex_update_vertices(current_shape->vertex);

			if (do_normals) kudu_shape_normals_calculate(current_shape);

		} while (current_shape->next_shape != NULL);
	}

	return TRUE;
}

