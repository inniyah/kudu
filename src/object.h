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
#ifndef OBJECT_H_INCLUDED
#define OBJECT_H_INCLUDED

#include "globals.h"
#include "utils.h"
#include "joints.h"
#include "bones.h"
#include "vertices.h"
#include "materials.h"
#include "wingedge.h"
#include "options.h"
#include "texture.h"
#include "image.h"

#define KuduObject_FILE_VER_MAJOR 0
#define KuduObject_FILE_VER_MINOR 3

/* Kudu object file format sub header values */
#define KOF_OBJECT_NAME 1

#define KOF_AUTHOR_NAME 10
#define KOF_AUTHOR_MAIL 11
#define KOF_AUTHOR_URL 12

#define KOF_BONE 20
#define KOF_BONE_POS 21

#define KOF_SHAPE 25

#define KOF_EDGE 30
#define KOF_EDGE_TEX_COORDS 31

#define KOF_VERTEX 35
#define KOF_VERTEX_ATTACHMENT 36

#define KOF_FACE 40

#define KOF_MATERIAL 45
#define KOF_TEXTURE 50
#define KOFS_TEXTURE_TYPE_EXTERNAL 1
#define KOFS_TEXTURE_TYPE_INTERNAL_RAW 2

static const unsigned char KuduObject_HEADER[12] = {0, 0, 1, 9, 0, 0, 0, 7, 1, 9, 8, 4};
static const unsigned char KuduObject_FILE_VERSION[2] = {KuduObject_FILE_VER_MAJOR, KuduObject_FILE_VER_MINOR};
static const char KuduObject_COMMENT[] = {"Created with Kudu"};

int kudu_object_force_next_id(int);
int kudu_object_id_counter_reset(void);
int kudu_object_set_filename(KuduObject*, char*);
int kudu_object_clear_filename(KuduObject*);
int kudu_object_set_author(KuduObject*, char*);
int kudu_object_clear_author(KuduObject*);
int kudu_object_set_email(KuduObject*, char*);
int kudu_object_clear_email(KuduObject*);
int kudu_object_set_url(KuduObject*, char*);
int kudu_object_clear_url(KuduObject*);
KuduObject *kudu_object_new(KuduObject*);
KuduObject *kudu_object_new_with_id(KuduObject*, int);
KuduObject *kudu_object_new_with_defaults(KuduObject*);
KuduObject *kudu_object_find_with_id(KuduObject*, int);
int kudu_object_destroy(KuduObject*);
int kudu_object_destroy_all(KuduObject*);
int kudu_object_for_each_do(KuduObject*);
KuduObject *kudu_object_next_do(void);

int kudu_object_save_author_details(KuduObject*, FILE*);
int kudu_object_save_bones(KuduBone*, FILE*);
int kudu_object_save_vertices(KuduVertex*, FILE*);
int kudu_object_save_faces(KuduFace*, FILE*);
int kudu_object_save_edges(KuduEdge*, FILE*);
int kudu_object_save_skin(KuduShape*, FILE*);
int kudu_object_save_materials(KuduMaterial*, FILE*);
int kudu_object_save_to_file(KuduObject*, char*);

int kudu_object_load_bone(KuduObject*, FILE*, int);
int kudu_object_load_material(KuduObject*, FILE*, int);
int kudu_object_load_shape(KuduObject*, FILE*);
int kudu_object_load_vertex(KuduObject*, FILE*, int);
int kudu_object_load_face(KuduObject*, FILE*, int);
int kudu_object_load_edge(KuduObject*, FILE*, int);
int kudu_object_load_from_file(KuduObject*, char*);
KuduObject *kudu_object_new_from_file(KuduObject*, char*);

int kudu_object_repair_old_structure(KuduObject*);
int kudu_object_update(KuduObject*);
int kudu_object_update_vertices(KuduObject*, int);

#endif /* OBJECT_H_INCLUDED */

