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
#include "materials.h"

static KuduMaterial *do_material = NULL;

int kudu_material_force_next_id(KuduObject *object, int materialID)
{
	if (object == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	object->next_material_id = materialID;
	return TRUE;
}

int kudu_material_next_id(KuduObject *object)
{
	if (object == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	return object->next_material_id;
}

int kudu_material_id_counter_reset(KuduObject *object)
{
	if (object == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	object->next_material_id = 1;

	return TRUE;
}

KuduMaterial *kudu_material_new(KuduObject *object, KuduMaterial *previous_material)
{
	if (object == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	int a;
	KuduMaterial *material = NULL, *old_next_material = NULL;

	material = (KuduMaterial*)malloc(sizeof(KuduMaterial));
	if (material == NULL) return NULL;

	if (previous_material == NULL) {
		material->next_material = NULL;
		material->previous_material = NULL;
	} else {
		material->previous_material = previous_material;
		old_next_material = previous_material->next_material;
		material->next_material = old_next_material;
		previous_material->next_material = material;

		if (old_next_material != NULL) old_next_material->previous_material = material;
	}

	for (a = 0; a < 4; a++) {
		material->diffuse[a] = 0.0;
		material->ambient[a] = 0.0;
		material->specular[a] = 0.0;
		material->emission[a] = 0.0;
	}

	material->diffuse[3] = 1.0;
	material->shininess = 50.0;
	material->texture = NULL;
	material->id = object->next_material_id++;
	sprintf(material->name, "Material %d", material->id);

	return material;
}

KuduMaterial *kudu_material_new_with_id(KuduObject *object, KuduMaterial *previous_material, int id)
{
	if (object == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	KuduMaterial *material = kudu_material_new(object, previous_material);

	material->id = id;

	if (id <= object->next_material_id) object->next_material_id = id+1;

	return material;
}

int kudu_material_destroy(KuduMaterial *material)
{
	if (material == NULL) return FALSE;

	KuduMaterial *previous_material, *next_material;

	previous_material = material->previous_material;
	next_material = material->next_material;

	if (previous_material != NULL) previous_material->next_material = next_material;
	if (next_material != NULL) next_material->previous_material = previous_material;

	free(material);
	material = NULL;

	return TRUE;
}

int kudu_material_destroy_all(KuduMaterial *material)
{
	if (material == NULL) return FALSE;

	KuduMaterial *current_material = NULL, *first_material = NULL, *destroy_material = NULL;

	do {
		if (first_material == NULL) first_material = material;
		else	first_material = first_material->previous_material;

	} while (first_material->previous_material != NULL);

	do {
		if (destroy_material != NULL) kudu_material_destroy(destroy_material);
		destroy_material = current_material;

		if (current_material == NULL) current_material = first_material;
		else	current_material = current_material->next_material;

	} while (current_material->next_material != NULL);

	kudu_material_destroy(current_material);

	material = NULL;

	return TRUE;
}

int kudu_material_for_each_do(KuduMaterial *material)
{
	if (material == NULL) return FALSE;

	KuduMaterial *first_material = NULL;

	do {
		if (first_material == NULL) first_material = material;
		else	first_material = first_material->previous_material;

	} while (first_material->previous_material != NULL);

	do_material = first_material;

	return TRUE;
}

KuduMaterial *kudu_material_next_do(void)
{
	if (do_material == NULL) return NULL;

	KuduMaterial *return_material;

	return_material = do_material;

	do_material = do_material->next_material;

	return return_material;
}

KuduMaterial *kudu_material_find_with_id(KuduMaterial *material, int find_id)
{
	KuduMaterial *current_material;
	int parse_backwards, found_or_eol;

	if (material == NULL) return NULL;

	for (parse_backwards = 0; parse_backwards < 2; parse_backwards++) {
		current_material = NULL;

		do {
			if (current_material == NULL) current_material = material;
			else {
				if (!parse_backwards) current_material = current_material->next_material;
				else	current_material = current_material->previous_material;
			}

			if (!parse_backwards) {
				if ((current_material->next_material != NULL) && (current_material->id != find_id))
					found_or_eol = FALSE;
				else	found_or_eol = TRUE;
			} else {
				if ((current_material->previous_material != NULL) && (current_material->id != find_id))
					found_or_eol = FALSE;
				else	found_or_eol = TRUE;
			}

		} while (!found_or_eol);

		if (current_material->id == find_id) return current_material;
	}

	return NULL;
}

KuduMaterial *kudu_material_find_with_same_values(KuduMaterial *material, KuduMaterial *find)
{
	if (material == NULL) return NULL;
	if (find == NULL) return NULL;

	KuduMaterial *current_material;
	int parse_backwards, found, eol, a;

	for (parse_backwards = 0; parse_backwards < 2; parse_backwards++) {
		current_material = NULL;

		do {
			if (current_material == NULL) current_material = material;
			else {
				if (!parse_backwards) current_material = current_material->next_material;
				else	current_material = current_material->previous_material;
			}

			found = TRUE;
			for (a = 0; a < 4; a++) {
				if (current_material->diffuse[a] != find->diffuse[a]) found = FALSE;
				if (current_material->ambient[a] != find->ambient[a]) found = FALSE;
				if (current_material->specular[a] != find->specular[a]) found = FALSE;
				if (current_material->emission[a] != find->emission[a]) found = FALSE;
				if (!found) a = 3;
			}
			if ((found) && (current_material->shininess != find->shininess)) found = FALSE;


			if (!parse_backwards) {
				if (current_material->next_material != NULL) eol = FALSE;
				else	eol = TRUE;
			} else {
				if (current_material->previous_material != NULL) eol = FALSE;
				else	eol = TRUE;
			}

		} while ((!found) && (!eol));

		if (found) return current_material;
	}

	return NULL;
}

int kudu_material_num_after(KuduMaterial *material)
{
	if (material == NULL) return 0;

	int count = 0;
	KuduMaterial *current_material = NULL;

	do {
		if (current_material == NULL) current_material = material;
		else	current_material = current_material->next_material;

		count++;
	} while (current_material->next_material != NULL);

	return count;
}

KuduMaterial *kudu_material_find_with_name(KuduMaterial *material, char *name)
{
	if ((material == NULL) || (name == NULL)) return NULL;


	KuduMaterial *current_material;
	int parse_backwards, found_or_eol;

	for (parse_backwards = 0; parse_backwards < 2; parse_backwards++) {
		current_material = NULL;

		do {
			if (current_material == NULL) current_material = material;
			else {
				if (!parse_backwards) current_material = current_material->next_material;
				else	current_material = current_material->previous_material;
			}

			if (!parse_backwards) {
				if ((current_material->next_material != NULL) && (strcmp(current_material->name, name) != 0))
					found_or_eol = FALSE;
				else	found_or_eol = TRUE;
			} else {
				if ((current_material->previous_material != NULL) && (strcmp(current_material->name, name) != 0))
					found_or_eol = FALSE;
				else	found_or_eol = TRUE;
			}

		} while (!found_or_eol);

		if (strcmp(current_material->name, name) == 0) return current_material;
	}

	return NULL;
}

