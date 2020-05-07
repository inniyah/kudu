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
#ifndef MATHEMATICS_H_INCLUDED
#define MATHEMATICS_H_INCLUDED

#include "globals.h"

#define CLAMP_360(deg) (kudu_math_degrees_clamp(&deg))

void kudu_math_angles_between(float, float, float, float, float, float, float*, float*);
int kudu_math_angles_between_v(float*, float*, float*, float*);
float kudu_math_distance_between(float*, float*);
void kudu_math_degrees_clamp(float*);

float kudu_math_vector_dot_product(float[3], float[3]);

void kudu_math_matrix_multiply(float[16], float[16]);
void kudu_math_matrix_translate_by_vertex3(float[16], float, float, float);
void kudu_math_matrix_translate_by_vertex3v(float[16], float[3]);
void kudu_math_matrix_copy(float[16], float[16]);
int kudu_math_matrix_inverse(float[16]);
void kudu_math_transform_vertex3_by_matrix(float*, float*, float*, float[16]);
void kudu_math_transform_vertex3v_by_matrix(float*, float[16]);
void kudu_math_partial_transform_vertex(float*, float*, float*, float[16], float);
void kudu_math_transform_vertex3_by_matrix_transpose(float*, float*, float*, float[16]);
void kudu_math_transform_vertex3v_by_matrix_transpose(float*, float[16]);
int kudu_math_vertex_normalize(float*);
int kudu_math_normal_vector(float[3], float[3], float[3], float*);
int kudu_math_are_vertices_aligned(GLfloat[3], GLfloat[3], GLfloat[3]);
void kudu_math_matrix_set_identity(float[16]);
void kudu_math_matrix_set_translation(float[16], float, float, float);

void kudu_math_quat_copy(float[4], float[4]);
void kudu_math_quat_multiply(float[4], float[4]);
void kudu_math_quat_to_matrix(float[4], float[16]);
void kudu_math_quat_normalize(float[4]);
void kudu_math_quat_rotate(float[4], float, float, float, float);
void kudu_math_quat_set_identity(float[4]);
void kudu_math_quat_from_euler(float[4], float, float, float);
void kudu_math_quat_apply_rotation(float[4], int, float);
void kudu_math_quat_to_euler(float[4], float*, float*, float*);
void kudu_math_quat_aim(float[4], float[3], float[3]);
void kudu_math_quat_invert(float[4]);


#endif /* MATHEMATICS_H_INCLUDED */
