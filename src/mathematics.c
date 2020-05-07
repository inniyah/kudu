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
#include "mathematics.h"

void kudu_math_angles_between(GLfloat X1, GLfloat Y1, GLfloat Z1,
	GLfloat X2, GLfloat Y2, GLfloat Z2, GLfloat *HORIZ, GLfloat *VERT)
{
	GLfloat dx, dy, dz, angle, ang1, ang2, ang3, dist, xang, zang;

	dx = (X1 - X2);
	dy = (Y1 - Y2);
	dz = (Z1 - Z2);

	ang1 = (asin(dz / (sqrt((dx * dx) + (dy * dy) + (dz * dz)))) / RADDEG);
	ang2 = (asin(dx / (sqrt((dx * dx) + (dy * dy) + (dz * dz)))) / RADDEG);
	ang3 = (asin(dy / (sqrt((dx * dx) + (dy * dy) + (dz * dz)))) / RADDEG);

	if (dx < 0) dx -= dx;
	if (dy < 0) dy -= dy;
	if (dz < 0) dz -= dz;
	
	dist = sqrt(((dx * dx) + (dy * dy) + (dz * dz)));

	zang = (90 - ang1);
	xang = (90 - ang2);

	if ((xang < 90) && (zang >= 90)) angle = 90 - xang;
	if ((xang < 90) && (zang < 90)) angle = xang + 90;
	if ((xang >= 90) && (zang < 90)) angle = xang + 90;
	if ((xang >= 90) && (zang >= 90)) angle = zang + 180;

	angle += 180;
	if (angle > 360) angle -= 360;
	if (angle < 0) angle += 360;

/*	ang3 += 180;
	if (ang3 > 360) ang3 -= 360;*/

	*VERT = ang3;
	*HORIZ = angle;

}

int kudu_math_angles_between_v(float *p1, float *p2, float *horiz, float *vert)
{
	kudu_math_angles_between(p1[0], p1[1], p1[2], p2[0], p2[1], p2[2], horiz, vert);

	return TRUE;
}

void kudu_math_degrees_clamp(GLfloat *degrees)
{
	if (*degrees > 360) *degrees -= 360;
	else
	if (*degrees < 0) *degrees += 360;
}

void kudu_math_matrix_multiply(GLfloat a[4][4], GLfloat b[4][4])
{
	GLfloat temp[4][4];

	temp[0][0] = a[0][0] * b[0][0] + a[0][1] * b[1][0] +
		     a[0][2] * b[2][0] + a[0][3] * b[3][0];
	temp[1][0] = a[1][0] * b[0][0] + a[1][1] * b[1][0] +
		     a[1][2] * b[2][0] + a[1][3] * b[3][0];
	temp[2][0] = a[2][0] * b[0][0] + a[2][1] * b[1][0] +
		     a[2][2] * b[2][0] + a[2][3] * b[3][0];
	temp[3][0] = a[3][0] * b[0][0] + a[3][1] * b[1][0] +
		     a[3][2] * b[2][0] + a[3][3] * b[3][0];

	temp[0][1] = a[0][0] * b[0][1] + a[0][1] * b[1][1] +
		     a[0][2] * b[2][1] + a[0][3] * b[3][1];
	temp[1][1] = a[1][0] * b[0][1] + a[1][1] * b[1][1] +
		     a[1][2] * b[2][1] + a[1][3] * b[3][1];
	temp[2][1] = a[2][0] * b[0][1] + a[2][1] * b[1][1] +
		     a[2][2] * b[2][1] + a[2][3] * b[3][1];
	temp[3][1] = a[3][0] * b[0][1] + a[3][1] * b[1][1] +
		     a[3][2] * b[2][1] + a[3][3] * b[3][1];

	temp[0][2] = a[0][0] * b[0][2] + a[0][1] * b[1][2] +
		     a[0][2] * b[2][2] + a[0][3] * b[3][2];
	temp[1][2] = a[1][0] * b[0][2] + a[1][1] * b[1][2] +
		     a[1][2] * b[2][2] + a[1][3] * b[3][2];
	temp[2][2] = a[2][0] * b[0][2] + a[2][1] * b[1][2] +
		     a[2][2] * b[2][2] + a[2][3] * b[3][2];
	temp[3][2] = a[3][0] * b[0][2] + a[3][1] * b[1][2] +
		     a[3][2] * b[2][2] + a[3][3] * b[3][2];

	temp[0][3] = a[0][0] * b[0][3] + a[0][1] * b[1][3] +
		     a[0][2] * b[2][3] + a[0][3] * b[3][3];
	temp[1][3] = a[1][0] * b[0][3] + a[1][1] * b[1][3] +
		     a[1][2] * b[2][3] + a[1][3] * b[3][3];
	temp[2][3] = a[2][0] * b[0][3] + a[2][1] * b[1][3] +
		     a[2][2] * b[2][3] + a[2][3] * b[3][3];
	temp[3][3] = a[3][0] * b[0][3] + a[3][1] * b[1][3] +
		     a[3][2] * b[2][3] + a[3][3] * b[3][3];


	a[0][0] = temp[0][0];
	a[1][0] = temp[1][0];
	a[2][0] = temp[2][0];
	a[3][0] = temp[3][0];

	a[0][1] = temp[0][1];
	a[1][1] = temp[1][1];
	a[2][1] = temp[2][1];
	a[3][1] = temp[3][1];

	a[0][2] = temp[0][2];
	a[1][2] = temp[1][2];
	a[2][2] = temp[2][2];
	a[3][2] = temp[3][2];

	a[0][3] = temp[0][3];
	a[1][3] = temp[1][3];
	a[2][3] = temp[2][3];
	a[3][3] = temp[3][3];
}

void kudu_math_matrix_copy(GLfloat a[4][4], GLfloat b[4][4])
{
	a[0][0] = b[0][0];
	a[1][0] = b[1][0];
	a[2][0] = b[2][0];
	a[3][0] = b[3][0];

	a[0][1] = b[0][1];
	a[1][1] = b[1][1];
	a[2][1] = b[2][1];
	a[3][1] = b[3][1];

	a[0][2] = b[0][2];
	a[1][2] = b[1][2];
	a[2][2] = b[2][2];
	a[3][2] = b[3][2];

	a[0][3] = b[0][3];
	a[1][3] = b[1][3];
	a[2][3] = b[2][3];
	a[3][3] = b[3][3];
}


void kudu_math_translate_vertex(GLfloat *X, GLfloat *Y, GLfloat *Z, GLfloat matrix[4][4])
{
	GLfloat x, y, z;

	x = *X;
	y = *Y;
	z = *Z;

	*X = 	(x * (matrix[0][0])) +
		(y * (matrix[1][0])) +
		(z * (matrix[2][0])) +
		(matrix[3][0]);

	*Y = 	(x * (matrix[0][1])) +
		(y * (matrix[1][1])) +
		(z * (matrix[2][1])) +
		(matrix[3][1]);

	*Z =	(x * (matrix[0][2])) +
		(y * (matrix[1][2])) +
		(z * (matrix[2][2])) +
		(matrix[3][2]);
}

void kudu_math_translate_vertex_inverse(GLfloat *X, GLfloat *Y, GLfloat *Z, GLfloat matrix[4][4])
{
	GLfloat x, y, z;

	x = *X;
	y = *Y;
	z = *Z;

	*X = 	(x * (matrix[0][0])) +
		(y * (matrix[0][1])) +
		(z * (matrix[0][2])) +
		(matrix[0][3]);

	*Y = 	(x * (matrix[1][0])) +
		(y * (matrix[1][1])) +
		(z * (matrix[1][2])) +
		(matrix[1][3]);

	*Z =	(x * (matrix[2][0])) +
		(y * (matrix[2][1])) +
		(z * (matrix[2][2])) +
		(matrix[2][3]);
}


int kudu_math_vertex_normalize(GLfloat *v)
{
	GLfloat d;

	d = (GLfloat)sqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]);

	if ((d < 0.0001) && (d > -0.0001)) {
		v[0] = 0.0;
		v[1] = 0.0;
		v[2] = 0.0;
		return FALSE;
	} else {
		v[0] /= d;
		v[1] /= d;
		v[2] /= d;
		return TRUE;
	}
}

int kudu_math_normal_vector(GLfloat v1[3], GLfloat v2[3], GLfloat v3[3], GLfloat *rv)
{
	GLint a;
	GLfloat norm1[3], norm2[3];

	for (a = 0; a < 3; a++) {
		norm1[a] = (v1[a] - v2[a]);
		norm2[a] = (v2[a] - v3[a]);
	}

	rv[0] = (norm1[1]*norm2[2] - norm1[2]*norm2[1]);
	rv[1] = (norm1[2]*norm2[0] - norm1[0]*norm2[2]);
	rv[2] = (norm1[0]*norm2[1] - norm1[1]*norm2[0]);

	return kudu_math_vertex_normalize(rv);
}

int kudu_math_are_vertices_aligned(GLfloat v1[3], GLfloat v2[3], GLfloat v3[3])
{
	GLfloat vert1[3], vert2[3], nv[3], d;
	int a;

	for (a = 0; a < 3; a++) {
	/*	vert1[a] = (v2[a] - v3[a]);
		vert2[a] = (v1[a] - vert1[a]);*/
		vert1[a] = (v1[a] - v2[a]);
		vert2[a] = (v2[a] - v3[a]);
	}

	nv[0] = (vert1[1]*vert2[2] - vert1[2]*vert2[1]);
	nv[1] = (vert1[2]*vert2[0] - vert1[0]*vert2[2]);
	nv[2] = (vert1[0]*vert2[1] - vert1[1]*vert2[0]);

	d = (GLfloat)sqrt(nv[0]*nv[0]+nv[1]*nv[1]+nv[2]*nv[2]);
/*
	nv[0] /= d;
	nv[1] /= d;
	nv[2] /= d;*/

/*	kudu_math_normal_vector(v1, v2, v3, nv);*/
	printf("nv[0]: %4.2f,  nv[1]:%4.2f,  nv[2]:%4.2f  d:%4.12f\n", nv[0], nv[1], nv[2], d);
	kudu_math_vertex_normalize(nv);

	printf("nv[0]: %4.2f,  nv[1]:%4.2f,  nv[2]:%4.2f  d:%4.6f\n", nv[0], nv[1], nv[2], d);

	if ((nv[0] == 0) && (nv[1] == 0) && (nv[2] == 0)) return TRUE;

	return FALSE;
}

void ANGLES_BETWEEN(GLfloat X1, GLfloat Y1, GLfloat Z1, GLfloat X2, GLfloat Y2, GLfloat Z2, GLfloat *HORIZ, GLfloat *VERT)
{
	kudu_math_angles_between(X1, Y1, Z1, X2, Y2, Z2, HORIZ, VERT);
}

void CLAMP_DEGREES(GLfloat *degrees)
{
	kudu_math_degrees_clamp(degrees);
}

void MULTIPLY_MATRIX(GLfloat a[4][4], GLfloat b[4][4])
{
	kudu_math_matrix_multiply(a, b);
}

void COPY_MATRIX(GLfloat a[4][4], GLfloat b[4][4])
{
	kudu_math_matrix_copy(a, b);
}

void CALCULATE_VERTEX(GLfloat *X, GLfloat *Y, GLfloat *Z, GLfloat matrix[4][4])
{
	kudu_math_translate_vertex(X, Y, Z, matrix);
}

void CALCULATE_INVERSE_VERTEX(GLfloat *X, GLfloat *Y, GLfloat *Z, GLfloat matrix[4][4])
{
	kudu_math_translate_vertex_inverse(X, Y, Z, matrix);
}

void NORMALIZE(GLfloat *v)
{
	kudu_math_vertex_normalize(v);
}

void NORMAL_VECTOR(GLfloat v1[3], GLfloat v2[3], GLfloat v3[3], GLfloat *rv)
{
	kudu_math_normal_vector(v1, v2, v3, rv);
}






