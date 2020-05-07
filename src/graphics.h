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
#ifndef GRAPHICS_H_INCLUDED
#define GRAPHICS_H_INCLUDED

#include "globals.h"
#include "font.h"
#include "utils.h"
#include "texture.h"
#include "bones.h"
#include "options.h"

#define DRAW_SKELETON_SHOW_NAMES 1
#define DRAW_SKELETON_SHOW_SELECTED_NAME 2
#define DRAW_DISABLE_LIGHTING 4
#define DRAW_SELECTION_MODE 8
#define DRAW_POINTS 16
#define DRAW_WIREFRAME 32
#define DRAW_FILLED 64
#define DRAW_NORMALS 128
#define DRAW_SKELETON_NONE_SELECTED 256

void kudu_graphics_colours_reload(void);
void kudu_draw_axes(void);
int kudu_draw_axes_labels(KuduCamera*);
int kudu_draw_grid(int, int, float, int);
void kudu_draw_block(GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLenum);
int kudu_draw_bone(KuduBone*, int);
int kudu_draw_all_bones(KuduBone*, int);
/*void kudu_draw_terrain(K_TERRAIN*, K_TEXTURE_LIST*, GLint);*/
int kudu_draw_point(KuduVertex*, int);
int kudu_draw_edge(KuduEdge*, int);
int kudu_draw_face(KuduFace*, int);
int kudu_draw_shape(KuduShape*, int);
int kudu_draw_shapes(KuduShape*, int);
int kudu_draw_object(KuduObject *object, int);
int kudu_draw_objects(KuduObject *object, int);

#endif /* GRAPHICS_H_INCLUDED */
