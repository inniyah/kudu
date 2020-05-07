/******************************************************************************/
/*                                                                            */
/* Kudu Animator                                                              */
/* Copyright (C) 2005-2006 Daniel Pekelharing                                 */
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

void kudu_math_angles_between(float X1, float Y1, float Z1,
	float X2, float Y2, float Z2, float *HORIZ, float *VERT)
{
	float dx, dy, dz, angle, ang1, ang2, ang3, dist, xang, zang;

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

float kudu_math_distance_between(float *p1, float *p2)
{
	if ((p1 == NULL) || (p2 == NULL)) return 0.0;

	int a;
	float d[3], dist;

	for (a = 0; a < 3; a++) {
		d[a] = p1[a] - p2[a];
		d[a] *= d[a];
	}

	dist = (float)sqrt(d[0] + d[1] + d[2]);

	return dist;
}

void kudu_math_degrees_clamp(float *degrees)
{
	if (*degrees > 360) *degrees -= 360;
	else
	if (*degrees < 0) *degrees += 360;

	if ((*degrees >= 360.0F) || (*degrees <= 0.0F)) *degrees = 0.0;
}

int kudu_math_are_vertices_aligned(GLfloat v1[3], GLfloat v2[3], GLfloat v3[3])
{
	float vert1[3], vert2[3], nv[3], d;
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

	d = (float)sqrt(nv[0]*nv[0]+nv[1]*nv[1]+nv[2]*nv[2]);
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


int kudu_math_vertex_normalize(float *v)
{
	float d;

	d = (float)sqrt((double)(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]));

	/*if ((d < 0.0001) && (d > -0.0001)) {*/
	if (d == 0.0f) {
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

int kudu_math_normal_vector(float v1[3], float v2[3], float v3[3], float *rv)
{
	int a;
	float norm1[3], norm2[3];

	for (a = 0; a < 3; a++) {
		norm1[a] = (v1[a] - v2[a]);
		norm2[a] = (v2[a] - v3[a]);
	}

	rv[0] = ((norm1[1]*norm2[2]) - (norm1[2]*norm2[1]));
	rv[1] = ((norm1[2]*norm2[0]) - (norm1[0]*norm2[2]));
	rv[2] = ((norm1[0]*norm2[1]) - (norm1[1]*norm2[0]));

	return kudu_math_vertex_normalize(rv);
}

/* Return the dot product of two vectors */
float kudu_math_vector_dot_product(float vec1[3], float vec2[3])
{
	float dp;

	dp = ((vec1[0] * vec2[0]) + (vec1[1] * vec2[1]) + (vec1[2] * vec2[2]));

	return dp;
}



/* Matrices */
/* Kudu Matrices are stored the same way as OpenGL handles them - as a one dimensonal array..
   i.e. Data is as follows:
   matrix[16]   |0  4  8   12 |
		|1  5  9   13 |
		|2  6  10  14 |
		|3  7  11  15 |		*/

void kudu_math_matrix_multiply(float a[16], float b[16])
{
	float temp[16];

	temp[0] = a[0] * b[0] + a[1] * b[4] +
		     a[2] * b[8] + a[3] * b[12];
	temp[4] = a[4] * b[0] + a[5] * b[4] +
		     a[6] * b[8] + a[7] * b[12];
	temp[8] = a[8] * b[0] + a[9] * b[4] +
		     a[10] * b[8] + a[11] * b[12];
	temp[12] = a[12] * b[0] + a[13] * b[4] +
		     a[14] * b[8] + a[15] * b[12];

	temp[1] = a[0] * b[1] + a[1] * b[5] +
		     a[2] * b[9] + a[3] * b[13];
	temp[5] = a[4] * b[1] + a[5] * b[5] +
		     a[6] * b[9] + a[7] * b[13];
	temp[9] = a[8] * b[1] + a[9] * b[5] +
		     a[10] * b[9] + a[11] * b[13];
	temp[13] = a[12] * b[1] + a[13] * b[5] +
		     a[14] * b[9] + a[15] * b[13];

	temp[2] = a[0] * b[2] + a[1] * b[6] +
		     a[2] * b[10] + a[3] * b[14];
	temp[6] = a[4] * b[2] + a[5] * b[6] +
		     a[6] * b[10] + a[7] * b[14];
	temp[10] = a[8] * b[2] + a[9] * b[6] +
		     a[10] * b[10] + a[11] * b[14];
	temp[14] = a[12] * b[2] + a[13] * b[6] +
		     a[14] * b[10] + a[15] * b[14];

	temp[3] = a[0] * b[3] + a[1] * b[7] +
		     a[2] * b[11] + a[3] * b[15];
	temp[7] = a[4] * b[3] + a[5] * b[7] +
		     a[6] * b[11] + a[7] * b[15];
	temp[11] = a[8] * b[3] + a[9] * b[7] +
		     a[10] * b[11] + a[11] * b[15];
	temp[15] = a[12] * b[3] + a[13] * b[7] +
		     a[14] * b[11] + a[15] * b[15];

	kudu_math_matrix_copy(a, temp);
}

void kudu_math_matrix_translate_by_vertex3(float matrix[16], float x, float y, float z)
{
	float transmat[16];

	kudu_math_matrix_set_translation(transmat, x, y, z);

	kudu_math_matrix_multiply(matrix, transmat);
}

void kudu_math_matrix_translate_by_vertex3v(float matrix[16], float v[3])
{
	kudu_math_matrix_translate_by_vertex3(matrix, v[0], v[1], v[2]);
}


void kudu_math_matrix_copy(float a[16], float b[16])
{

	int c;

	for (c = 0; c < 16; c++) a[c] = b[c];
}

/* This function is *not* working... was just some experimenting */
/* this is not how to invert a matrix.. */
int kudu_math_matrix_inverse(float matrix[16])
{
/*	float omatrix[4][4];
	int a, b;

	kudu_math_matrix_copy(omatrix, matrix);

	for (a = 0; a < 4; a++)
		for (b = 0; b < 4; b++) {
			matrix[a][b] = -omatrix[a][a];
		}
*/
	return TRUE;
}

void kudu_math_transform_vertex3_by_matrix(float *X, float *Y, float *Z, float matrix[16])
{
	float x, y, z;

	x = *X;
	y = *Y;
	z = *Z;

	*X = 	(x * (matrix[0])) +
		(y * (matrix[4])) +
		(z * (matrix[8])) +
		(matrix[12]);

	*Y = 	(x * (matrix[1])) +
		(y * (matrix[5])) +
		(z * (matrix[9])) +
		(matrix[13]);

	*Z =	(x * (matrix[2])) +
		(y * (matrix[6])) +
		(z * (matrix[10])) +
		(matrix[14]);
}

void kudu_math_transform_vertex3v_by_matrix(float *v, float matrix[16])
{
	if (v == NULL) return;

	kudu_math_transform_vertex3_by_matrix(&v[0], &v[1], &v[2], matrix);
}

void kudu_math_partial_transform_vertex(float *X, float *Y, float *Z, float matrix[16], float p)
{
	float x, y, z;

	x = *X;
	y = *Y;
	z = *Z;

	*X = 	(p*(x * (matrix[0])) +
		(y * (matrix[4])) +
		(z * (matrix[8])) +
		(matrix[12]));

	*Y = 	(p*(x * (matrix[1])) +
		(y * (matrix[5])) +
		(z * (matrix[9])) +
		(matrix[13]));

	*Z =	(p*(x * (matrix[2])) +
		(y * (matrix[6])) +
		(z * (matrix[10])) +
		(matrix[14]));
}

void kudu_math_transform_vertex3_by_matrix_transpose(float *X, float *Y, float *Z, float matrix[16])
{
	float x, y, z;

	x = *X;
	y = *Y;
	z = *Z;

	*X = 	(x * (matrix[0])) +
		(y * (matrix[1])) +
		(z * (matrix[2])) +
		(matrix[3]);

	*Y = 	(x * (matrix[4])) +
		(y * (matrix[5])) +
		(z * (matrix[6])) +
		(matrix[7]);

	*Z =	(x * (matrix[8])) +
		(y * (matrix[9])) +
		(z * (matrix[10])) +
		(matrix[11]);
}

void kudu_math_transform_vertex3v_by_matrix_transpose(float *v, float matrix[16])
{
	if (v == NULL) return;

	kudu_math_transform_vertex3_by_matrix_transpose(&v[0], &v[1], &v[2], matrix);
}


/* Reset a 4x4 matrix to be an identity matrix */
void kudu_math_matrix_set_identity(float matrix[16])
{
	matrix[0] = 1.0;
	matrix[1] = 0.0;
	matrix[2] = 0.0;
	matrix[3] = 0.0;

	matrix[4] = 0.0;
	matrix[5] = 1.0;
	matrix[6] = 0.0;
	matrix[7] = 0.0;

	matrix[8] = 0.0;
	matrix[9] = 0.0;
	matrix[10] = 1.0;
	matrix[11] = 0.0;

	matrix[12] = 0.0;
	matrix[13] = 0.0;
	matrix[14] = 0.0;
	matrix[15] = 1.0;
}

/* Create a translation matrix */
void kudu_math_matrix_set_translation(float matrix[16], float x, float y, float z)
{
	matrix[0] = 1.0;
	matrix[1] = 0.0;
	matrix[2] = 0.0;
	matrix[3] = 0.0;

	matrix[4] = 0.0;
	matrix[5] = 1.0;
	matrix[6] = 0.0;
	matrix[7] = 0.0;

	matrix[8] = 0.0;
	matrix[9] = 0.0;
	matrix[10] = 1.0;
	matrix[11] = 0.0;

	matrix[12] = x;
	matrix[13] = y;
	matrix[14] = z;
	matrix[15] = 1.0;
}


/* Quaternions */
/* Quaternions are stored in a one dimensional array data arranged as:
   quat[4] = {x, y, z, w}	*/

/* Copy q2 into q1 */
void kudu_math_quat_copy(float q1[4], float q2[4])
{
	q1[0] = q2[0];
	q1[1] = q2[1];
	q1[2] = q2[2];
	q1[3] = q2[3];
}

/* Multiply q1 by q2 */
void kudu_math_quat_multiply(float q1[4], float q2[4])
{
	float rq[4];

	rq[0] = q1[3]*q2[0] + q1[0]*q2[3] + q1[1]*q2[2] - q1[2]*q2[1];
	rq[1] = q1[3]*q2[1] + q1[1]*q2[3] + q1[2]*q2[0] - q1[0]*q2[2];
	rq[2] = q1[3]*q2[2] + q1[2]*q2[3] + q1[0]*q2[1] - q1[1]*q2[0];
	rq[3] = q1[3]*q2[3] - q1[0]*q2[0] - q1[1]*q2[1] - q1[2]*q2[2];

	kudu_math_quat_copy(q1, rq);
}

/* Convert a quaternion (quat) into a matrix (matrix) */
void kudu_math_quat_to_matrix(float quat[4], float matrix[16])
{
	float xx, xy, xz, xw, yy, yz, yw, zz, zw;

	xx = quat[0] * quat[0];
	xy = quat[0] * quat[1];
	xz = quat[0] * quat[2];
	xw = quat[0] * quat[3];

	yy = quat[1] * quat[1];
	yz = quat[1] * quat[2];
	yw = quat[1] * quat[3];

	zz = quat[2] * quat[2];
	zw = quat[2] * quat[3];

	matrix[0] = 1 - 2 * (yy + zz);
	matrix[1] = 2 * (xy - zw);
	matrix[2] = 2 * (xz + yw);
	matrix[4] = 2 * (xy + zw);
	matrix[5] = 1 - 2 * (xx + zz);
	matrix[6] = 2 * (yz - xw);
	matrix[8] = 2 * (xz - yw);
	matrix[9] = 2 * (yz + xw);
	matrix[10] = 1 - 2 * (xx + yy);
	matrix[3] = matrix[7] = matrix[11] = matrix[12] = matrix[13] = matrix[14] = 0;
	matrix[15] = 1;
}

/* Normalize a quaternion */
void kudu_math_quat_normalize(float quat[4])
{
	float len;

	len = (float)sqrt(quat[3]*quat[3] + quat[0]*quat[0] + quat[1]*quat[1] + quat[2]*quat[2]);

	if (len != 0.0) {
		quat[0] /= len;
		quat[1] /= len;
		quat[2] /= len;
		quat[3] /= len;
	}
}

/* Set a rotation on a quaternion */
void kudu_math_quat_rotate(float quat[4], float ang, float x, float y, float z)
{
	float angle = ang * HRADDEG;
	float sinA = (float)sin(angle);

	quat[0] = x * sinA;
	quat[1] = y * sinA;
	quat[2] = z * sinA;
	quat[3] = (float)cos(angle);
}

/* Reset a quaternion to the identity */
void kudu_math_quat_set_identity(float quat[4])
{
	quat[0] = 0.0;
	quat[1] = 0.0;
	quat[2] = 0.0;
	quat[3] = 1.0;
}

/* Convert a set of euler rotation angles to a quaternion */
void kudu_math_quat_from_euler(float quat[4], float ax, float ay, float az)
{
	float qx[4], qy[4], qz[4];

	kudu_math_quat_rotate(qx, ax, 1.0, 0.0, 0.0);
	kudu_math_quat_rotate(qy, ay, 0.0, 1.0, 0.0);
	kudu_math_quat_rotate(qz, az, 0.0, 0.0, 1.0);

	kudu_math_quat_copy(quat, qx);
	kudu_math_quat_multiply(quat, qy);
	kudu_math_quat_multiply(quat, qz);
}

/* Multiply a quaternion by a rotation quaternion of angle about axis, 0 = x, 1 = y, 2 = z */
void kudu_math_quat_apply_rotation(float quat[4], int axis, float angle)
{
	float rot[4];

	switch (axis) {
		case 0:
			kudu_math_quat_rotate(rot, angle, 1.0, 0.0, 0.0);
			break;
		case 1:
			kudu_math_quat_rotate(rot, angle, 0.0, 1.0, 0.0);
			break;
		case 2:
			kudu_math_quat_rotate(rot, angle, 0.0, 0.0, 1.0);
			break;
	}

	kudu_math_quat_multiply(quat, rot);
}

/* Convert a quaternion to a set of euler angles */
void kudu_math_quat_to_euler(float quat[4], float *x, float *y, float *z)
{
	float sqx = quat[0] * quat[0];
	float sqy = quat[1] * quat[1];
	float sqz = quat[2] * quat[2];
	float sqw = quat[3] * quat[3];

	float unit = sqx + sqy + sqz + sqw;
	float test = quat[0]*quat[1] + quat[2]*quat[3];

	if (test > 0.499*unit) {
		*x = 0;
		*y = (2 * atan2(quat[0], quat[3]) / RADDEG);
		*z = (M_PI/2) / RADDEG;
	} else if (test < -0.499*unit) {
		*x = 0;
		*y = (-2 * atan2(quat[0], quat[3])) / RADDEG;
		*z = (-M_PI/2) / RADDEG;
	} else {
		*x = atan2(2*quat[0]*quat[3] - 2*quat[1]*quat[2], -sqx + sqy - sqz + sqw) / RADDEG;
		*y = atan2(2*quat[1]*quat[3] - 2*quat[0]*quat[2], sqx - sqy - sqz + sqw) / RADDEG;
		*z = asin(2*test/unit) / RADDEG;
	}

	kudu_math_degrees_clamp(x);
	kudu_math_degrees_clamp(y);
	kudu_math_degrees_clamp(z);
}

/* Sets a rotation quaternion's z axis to point from vf to vt */
void kudu_math_quat_aim(float quat[4], float vf[3], float vt[3])
{
	float vec[3];
	int a;

	for (a = 0; a < 3; a++) vec[a] = (vt[a] - vf[a]);
	kudu_math_vertex_normalize(vec);

	quat[0] = vec[1];
	quat[1] = -vec[0];
	quat[2] = 0;
	quat[3] = 1.0 - vec[2];

	kudu_math_quat_normalize(quat);
}

/* Inverts a quaternion */
void kudu_math_quat_invert(float quat[4])
{
	quat[0] = -quat[0];
	quat[1] = -quat[1];
	quat[2] = -quat[2];
}


