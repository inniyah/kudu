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
#ifndef VERTICES_H_INCLUDED
#define VERTICES_H_INCLUDED

#include "globals.h"
#include "bones.h"
#include "mathematics.h"

int kudu_vertex_force_next_id(KuduObject*, int);
int kudu_vertex_id_counter_reset(KuduObject*);
int kudu_vertex_next_id(KuduObject*);
KuduVertex *kudu_vertex_new(KuduObject*, KuduVertex*);
KuduVertex *kudu_vertex_new_with_id(KuduObject*, KuduVertex*, int);
KuduVertex *kudu_vertex_find_with_id(KuduVertex*, int);
int kudu_vertices_find_with_ids(KuduVertex*, int, int*, KuduVertex**);
int kudu_vertex_for_each_do(KuduVertex*);
KuduVertex *kudu_vertex_next_do(void);
int kudu_vertex_destroy(KuduVertex*);
int kudu_vertex_destroy_all(KuduVertex*);
int kudu_vertex_update(KuduVertex*);
int kudu_vertex_update_vertices(KuduVertex*);
int kudu_vertex_smart_update_vertices(KuduVertex*);
int kudu_vertex_attach_to_bone(KuduVertex*, KuduBone*);
int kudu_vertex_attach_to_bone_by_percent(KuduVertex*, KuduBone*, GLubyte);
int kudu_vertex_detach(KuduVertex*);



#endif /* VERTICES_H_INCLUDED */
