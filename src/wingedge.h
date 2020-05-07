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
#ifndef WINGEDGE_H_INCLUDED
#define WINGEDGE_H_INCLUDED

#include "globals.h"

int kudu_edge_force_next_id(KuduObject*, int);
int kudu_edge_next_id(KuduObject*);
int kudu_face_force_next_id(KuduObject*, int);
int kudu_face_next_id(KuduObject*);
int kudu_shape_force_next_id(KuduObject*, int);
int kudu_shape_next_id(KuduObject*);
KuduFace *kudu_face_new(KuduObject*, KuduFace*);
KuduFace *kudu_face_new_with_id(KuduObject*, KuduFace*, int);
KuduEdge *kudu_edge_new(KuduObject*, KuduEdge*);
KuduEdge *kudu_edge_new_with_id(KuduObject*, KuduEdge*, int);
KuduShape *kudu_shape_new(KuduObject*, KuduShape*);
KuduShape *kudu_shape_new_with_id(KuduObject*, KuduShape*, int);
int kudu_edge_vertices_set(KuduEdge*, KuduVertex*, KuduVertex*);
int kudu_edge_faces_set(KuduEdge*, KuduFace*, KuduFace*);
int kudu_edge_wings_set(KuduEdge*, KuduEdge*, KuduEdge*, KuduEdge*, KuduEdge*);
KuduEdge *kudu_edge_find_with_vertex_ids(KuduEdge*, int, int);
KuduFace *kudu_face_find_with_id(KuduFace*, int);
int kudu_faces_find_with_ids(KuduFace*, int, int*, KuduFace**);
int kudu_face_destroy(KuduFace*);
int kudu_edge_destroy(KuduEdge*);
int kudu_face_destroy_all(KuduFace*);
int kudu_edge_destroy_all(KuduEdge*);
int kudu_shape_destroy(KuduShape*);
int kudu_shape_destroy_all(KuduShape*);
KuduShape *kudu_shape_find_with_id(KuduShape*, int);
KuduEdge *kudu_edge_find_with_id(KuduEdge*, int);
int kudu_edges_find_with_ids(KuduEdge*, int, int*, KuduEdge**);
int kudu_shape_for_each_do(KuduShape*);
KuduShape *kudu_shape_next_do(void);
int kudu_face_for_each_do(KuduFace*);
KuduFace *kudu_face_next_do(void);
int kudu_edge_for_each_do(KuduEdge*);
KuduEdge *kudu_edge_next_do(void);
int kudu_face_for_each_vertex_do(KuduFace*);
KuduVertex *kudu_face_for_each_vertex_next_do(void);
int kudu_face_for_each_edge_do(KuduFace*);
KuduEdge *kudu_face_for_each_edge_next_do(void);
int kudu_vertex_for_each_edge_do(KuduVertex*);
KuduEdge *kudu_vertex_for_each_edge_next_do(void);
int kudu_diag_print_edge(KuduEdge*);
int kudu_shape_num_vertices(KuduShape*);
int kudu_shape_num_edges(KuduShape*);
int kudu_shape_num_faces(KuduShape*);

#endif /* WINGEDGE_H_INCLUDED */

