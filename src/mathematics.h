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

void kudu_math_angles_between(GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat*, GLfloat*);
int kudu_math_angles_between_v(float*, float*, float*, float*);
void kudu_math_degrees_clamp(GLfloat*);
void kudu_math_matrix_multiply(GLfloat[4][4], GLfloat[4][4]);
void kudu_math_matrix_copy(GLfloat[4][4], GLfloat[4][4]);
void kudu_math_translate_vertex(GLfloat*, GLfloat*, GLfloat*, GLfloat[4][4]);
void kudu_math_translate_vertex_inverse(GLfloat*, GLfloat*, GLfloat*, GLfloat[4][4]);
int kudu_math_vertex_normalize(GLfloat*);
int kudu_math_normal_vector(GLfloat[3], GLfloat[3], GLfloat[3], GLfloat*);
int kudu_math_are_vertices_aligned(GLfloat[3], GLfloat[3], GLfloat[3]);


/* Older Function Names */
void ANGLES_BETWEEN(GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat*, GLfloat*);
void CLAMP_DEGREES(GLfloat*);
void MULTIPLY_MATRIX(GLfloat[4][4], GLfloat[4][4]);
void COPY_MATRIX(GLfloat[4][4], GLfloat[4][4]);
void CALCULATE_VERTEX(GLfloat*, GLfloat*, GLfloat*, GLfloat[4][4]);
void CALCULATE_INVERSE_VERTEX(GLfloat*, GLfloat*, GLfloat*, GLfloat[4][4]);
void NORMALIZE(GLfloat*);
void NORMAL_VECTOR(GLfloat[3], GLfloat[3], GLfloat[3], GLfloat*);

#endif /* MATHEMATICS_H_INCLUDED */
