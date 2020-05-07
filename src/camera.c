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
#include "camera.h"

KuduCamera *kudu_camera_new(void)
{
	KuduCamera *camera;
	int a;

	camera = (KuduCamera*)malloc(sizeof(KuduCamera));
	if (camera == NULL) return NULL;

	for (a = 0; a < 3; a++) camera->position[a] = 2.0;
	for (a = 0; a < 3; a++) camera->focus[a] = 0.0;

	camera->ha = 315;
	camera->va = 145;

	camera->up[0] = 0.0;
	camera->up[1] = 1.0;
	camera->up[2] = 0.0;
	kudu_camera_get_distance(camera);

	camera->mode = CAMERA_MODE_FIXED;
	camera->fov = 60.0;
	camera->ortho = FALSE;
	camera->near_plane = 0.1;
	camera->far_plane = 1250.0;

	return camera;
}

int kudu_camera_destroy(KuduCamera* camera)
{
	if (camera == NULL) return FALSE;

	free(camera);

	return TRUE;
}

float kudu_camera_get_distance(KuduCamera *camera)
{
	if (camera == NULL) return 0;

	float d[3];
	int a;

	for (a = 0; a < 3; a++) {
		d[a] = camera->position[a] - camera->focus[a];
		d[a] *= d[a];
	}

	camera->distance = (float)sqrt(d[0] + d[1] + d[2]);

	return camera->distance;
}

int kudu_camera_slide(KuduCamera *camera, float distance, int direction)
{
	if (camera == NULL) return FALSE;

	float v_ang, h_ang, dx, dy, dz, temp;

	kudu_math_angles_between_v(camera->position, camera->focus, &h_ang, &v_ang);

	h_ang = camera->ha;
	v_ang = camera->va;

	dx = camera->position[0] - camera->focus[0];
	dy = camera->position[1] - camera->focus[1];
	dz = camera->position[2] - camera->focus[2];

	distance += sqrt((dx * dx) + (dy * dy) + (dz * dz));
	temp = cos((GLdouble)(v_ang * RADDEG));

	if (direction == SLIDE_FORWARD) distance += sqrt((dx * dx) + (dy * dy) + (dz * dz));
	if (direction == SLIDE_BACKWARD) distance -= sqrt((dx * dx) + (dy * dy) + (dz * dz));

	camera->position[0] = camera->focus[0] - (distance * (temp * (float)sin((double)(h_ang * RADDEG))));
	camera->position[1] = camera->focus[1] + (distance * (temp * (float)cos((double)(h_ang * RADDEG))));
	camera->position[2] = camera->focus[2] + (distance * (float)sin((double)(v_ang * RADDEG)));

	return TRUE;
}

/* Swing the camera horizontally by horiz_ang, vertically by vertic_ang */
/* and shift it forwards or backwards by move_dist */
int kudu_camera_swing(KuduCamera *camera, float horiz_ang, float vertic_ang, float move_dist)
{
	if (camera == NULL) return FALSE;

	float h_ang, v_ang, distance, dx, dy, dz;

	h_ang = camera->ha + horiz_ang;
	v_ang = camera->va + vertic_ang;

	CLAMP_360(h_ang);
	CLAMP_360(v_ang);

	float sin_h = (float)sin((double)(h_ang * RADDEG));
	float cos_h = (float)cos((double)(h_ang * RADDEG));
	float sin_v = (float)sin((double)(v_ang * RADDEG));
	float cos_v = (float)cos((double)(v_ang * RADDEG));

	if ((v_ang > 90) && (v_ang < 270)) camera->up[1] = 1.0;
	else	camera->up[1] = -1.0;

	dx = camera->position[0] - camera->focus[0];
	dy = camera->position[1] - camera->focus[1];
	dz = camera->position[2] - camera->focus[2];

	distance = sqrt((dx * dx) + (dy * dy) + (dz * dz)) + move_dist;
	if (distance < 0.1) distance = 0.1;
	camera->distance = distance;

	camera->position[0] = camera->focus[0] + (distance * (cos_v * sin_h));
	camera->position[2] = camera->focus[2] - (distance * (cos_v * cos_h));
	camera->position[1] = camera->focus[1] + (distance * sin_v);

	camera->ha = h_ang;
	camera->va = v_ang;
	return TRUE;
}

int kudu_camera_move(KuduCamera *camera, GLfloat x_dis, GLfloat y_dis)
{
	if (camera == NULL) return FALSE;

	GLfloat dx, dy, dz, temp;

	dy = (y_dis * (GLfloat)cos((GLdouble)(camera->va * RADDEG)));
	temp = (y_dis * (GLfloat)sin((GLdouble)(camera->va * RADDEG)));

	dx = (x_dis * (GLfloat)cos((GLdouble)(camera->ha * RADDEG)));
	dz = (x_dis * (GLfloat)sin((GLdouble)(camera->ha * RADDEG)));

	camera->position[1] -= dy;
	camera->focus[1] -= dy;

	camera->position[0] -= dx-(temp * sin((GLdouble)(camera->ha * RADDEG)));
	camera->focus[0] -= dx-(temp * sin((GLdouble)(camera->ha * RADDEG)));

	camera->position[2] -= dz+(temp * cos((GLdouble)(camera->ha * RADDEG)));
	camera->focus[2] -= dz+(temp * cos((GLdouble)(camera->ha * RADDEG)));

	return TRUE;
}

int kudu_camera_set_position(KuduCamera *camera, float x, float y, float z)
{
	if (camera == NULL) return FALSE;

	camera->position[0] = x;
	camera->position[1] = y;
	camera->position[2] = z;

	return TRUE;
}

int kudu_camera_lookat(KuduCamera *camera, float x, float y, float z)
{
	if (camera == NULL) return FALSE;

	camera->focus[0] = x;
	camera->focus[1] = y;
	camera->focus[2] = z;

	return TRUE;
}

int kudu_camera_set_gl_viewpoint(KuduCamera *camera)
{
	if (camera == NULL) return FALSE;

	if (camera->ortho) kudu_camera_set_gl_matrix(camera, -1 , -1);

	gluLookAt(camera->position[0], camera->position[1], camera->position[2],
		  camera->focus[0], camera->focus[1], camera->focus[2], camera->up[0], camera->up[1], camera->up[2]);

	return TRUE;
}

int kudu_camera_set_gl_matrix(KuduCamera *camera, int w, int h)
{
	if (camera == NULL) return FALSE;

	float tmp, aspect;
	if ((w < 0) || (h < 0)) {
		int viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);
		aspect = (float)viewport[2] / (float)viewport[3];
	} else aspect = (float)w / (float)h;

	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	if (!camera->ortho) gluPerspective(camera->fov, aspect, camera->near_plane, camera->far_plane);
	else	{
		tmp = kudu_camera_get_distance(camera);
		tmp *= tan(camera->fov * M_PI / 180 / 2);
		glOrtho(-tmp*aspect, tmp*aspect, -tmp, tmp, camera->near_plane, camera->far_plane);
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	return TRUE;
}

/* Set up the viewport for gl picking */
int kudu_camera_set_gl_pick_matrix(KuduCamera *camera, int sx, int sy, int ex, int ey)
{
	if (camera == NULL) return FALSE;

	int viewport[4], x, y, x2, y2, selection_type;
	float tmp, aspect;

	if (sx < ex) {
		x = sx;
		x2 = (ex - sx);
	} else {
		x = ex;
		x2 = (sx - ex);
	}

	sy = (program.windowHeight - sy);
	ey = (program.windowHeight - ey);
	if (sy > ey) {
		y = sy;
		y2 = (ey - sy);
	} else {
		y = ey;
		y2 = (sy - ey);
	}

	if (x2 < 0) x2 = -x2;
	if (y2 < 0) y2 = -y2;
	if (x2 < 5) {
		x2 = 5;
		x -= 3;
	}
	if (y2 < 5) {
		y2 = 5;
		y += 3;
	}

	if ((x2 == 5) && (y2 == 5)) selection_type = K_SELECT_PICK;
	else	selection_type = K_SELECT_RECTANGLE;

	x += (GLint)(x2/2);
	y -= (GLint)(y2/2);
	if (x < 0) x = -x;
	if (y < 0) y = -y;

	glGetIntegerv(GL_VIEWPORT, viewport);
	aspect = (float)viewport[2] / (float)viewport[3];

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	gluPickMatrix((double)x, (double)y, (double)x2, (double)y2, viewport);

	if (!camera->ortho) gluPerspective(camera->fov, aspect, camera->near_plane, camera->far_plane);
	else	{
		tmp = kudu_camera_get_distance(camera);
		tmp *= tan(camera->fov * M_PI / 180 / 2);
		glOrtho(-tmp*aspect, tmp*aspect, -tmp, tmp, camera->near_plane, camera->far_plane);
	}
	
	glMatrixMode(GL_MODELVIEW);

	return selection_type;
}

int kudu_camera_ortho(KuduCamera *camera, int on)
{
	if (camera == NULL) return FALSE;

	int wo = camera->ortho;
	camera->ortho = on;
	if (wo != on) {
		kudu_camera_set_gl_matrix(camera, -1, -1);
		kudu_camera_set_gl_viewpoint(camera);
	}

	return TRUE;
}

int kudu_camera_set_mode(KuduCamera *camera, int mode)
{
	if (camera == NULL) return FALSE;

	if (program.bone_mode != BONE_MODE_FIXED) return FALSE;

	int a;

	camera->old_mode = camera->mode;
	camera->mode = mode;

	switch (mode) {
		case CAMERA_MODE_RESET:
			camera->ha = camera->old_ha;
			camera->va = camera->old_va;
			for (a = 0; a < 3; a++) camera->position[a] = camera->old_position[a];
			kudu_camera_swing(camera, 0, 0, 0);
			camera->mode = CAMERA_MODE_FIXED;
		case CAMERA_MODE_FIXED:
			kudu_gui_show_cursor();
			kudu_gui_restore_pointer();
			break;
		default:
			kudu_gui_save_pointer();
			kudu_gui_hide_cursor();
			kudu_gui_center_pointer();

			if (camera->old_mode == CAMERA_MODE_FIXED) {
				camera->old_ha = camera->ha;
				camera->old_va = camera->va;
				for (a = 0; a < 3; a++) camera->old_position[a] = camera->position[a];
			}
			break;
	}

	return TRUE;
}

int kudu_camera_mouse_action(KuduCamera *camera, float hscroll, float vscroll)
{
	if (camera == NULL) return FALSE;
	float amount;

	/* Swing, slide or zoom camera as appropriate */
	switch (camera->mode) {
		case CAMERA_MODE_SWING:
			kudu_camera_swing(camera, hscroll, -vscroll, 0);
			break;
		case CAMERA_MODE_ZOOM: /* Not technically "zooming" we're just moving the camera forwards */
			amount = (kudu_camera_get_distance(camera) / 100);
			kudu_camera_swing(camera, 0, 0, -(vscroll *amount));
			break;
		case CAMERA_MODE_SLIDE:
			kudu_camera_move(camera, (hscroll / 10), (vscroll / 10));
			break;
		default:	/* Camera is in fixed position mode - do nothing */
			return FALSE;
			break;
	}

	return TRUE;
}


