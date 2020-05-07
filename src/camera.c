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

	camera->last_stack_operation = 0; /* This var tracks the previous stack operation .. 0 = nothing,  < 0 mode popped,  > 0 mode pushed */
	camera->cmode = 0;
	camera->mode_stack[camera->cmode] = CAMERA_MODE_FIXED;

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

int kudu_camera_set_mode(KuduCamera *camera)
{
	if (camera == NULL) return FALSE;

	if (program.bone_mode != BONE_MODE_FIXED) return FALSE;

	int a;

	/*camera->old_mode = camera->mode;*/
	camera->mode = camera->mode_stack[camera->cmode];

	switch (camera->mode_stack[camera->cmode]) {
		case CAMERA_MODE_RESET:
			camera->ha = camera->old_ha;
			camera->va = camera->old_va;
			for (a = 0; a < 3; a++) camera->position[a] = camera->old_position[a];
			kudu_camera_swing(camera, 0, 0, 0);
			camera->mode = CAMERA_MODE_FIXED;
			camera->last_stack_operation = 0;
			camera->cmode = 0;
			camera->mode_stack[camera->cmode] = CAMERA_MODE_FIXED;
		case CAMERA_MODE_FIXED:
			kudu_gui_show_cursor();
			kudu_gui_restore_pointer();
			break;
		default:
			kudu_gui_save_pointer();
			kudu_gui_hide_cursor();
			kudu_gui_center_pointer();

			if ((camera->mode_stack[camera->cmode-1] == CAMERA_MODE_FIXED) && (camera->last_stack_operation > 0)) {
				camera->old_ha = camera->ha;
				camera->old_va = camera->va;
				for (a = 0; a < 3; a++) camera->old_position[a] = camera->position[a];
			}
			break;
	}

	return TRUE;
}

int kudu_camera_push_mode(KuduCamera *camera, K_CameraMode mode)
{
	if (camera == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	if (camera->cmode > 3) return FALSE;

	if (camera->mode_stack[camera->cmode] == mode) return TRUE;

	camera->last_stack_operation = 1;
	camera->cmode++;

	camera->mode_stack[camera->cmode] = mode;

	kudu_camera_set_mode(camera);

	return TRUE;
}

int kudu_camera_load_mode(KuduCamera *camera, K_CameraMode mode)
{
	if (camera == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	if (camera->mode_stack[camera->cmode] == mode) return TRUE;

	if (camera->cmode == 0) camera->cmode = 1;

	camera->mode_stack[camera->cmode] = mode;

	kudu_camera_set_mode(camera);

	return TRUE;
}

int kudu_camera_pop_mode(KuduCamera *camera)
{
	if (camera == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	if (camera->cmode == 0) return FALSE;

	camera->last_stack_operation = -1;
	camera->cmode--;

	kudu_camera_set_mode(camera);

	return TRUE;
}

int kudu_camera_pop_all_modes(KuduCamera *camera)
{
	if (camera == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	camera->last_stack_operation = -2;
	camera->cmode = 0;
	camera->mode_stack[camera->cmode] = CAMERA_MODE_FIXED;

	kudu_camera_set_mode(camera);

	return TRUE;
}

int kudu_camera_get_previous_mode(KuduCamera *camera)
{
	int pm = camera->cmode;

	if (pm > 0) pm--;

	return camera->mode_stack[pm];
}

int kudu_camera_mouse_action(KuduCamera *camera, float hscroll, float vscroll)
{
	if (camera == NULL) return FALSE;
	float amount, speed, h, v, t;
	int opts;

	speed = kudu_options_get_float_no(KO_CAMERA_SPEED, 0);

	hscroll *= speed;
	vscroll *= speed;

	/* Swing, slide or zoom camera as appropriate */
	switch (camera->mode) {
		case CAMERA_MODE_TUMBLE:
			h = hscroll;
			v = -vscroll;
			opts = kudu_options_get_int_no(KO_CAMERA_TUMBLE, 4);

			if (opts & MOUSE_FLAG_REVERSE_X) h = -h;
			if (opts & MOUSE_FLAG_REVERSE_Y) v = -v;

			if (opts & MOUSE_FLAG_SWOP_XY) {
				t = h;
				h = v;
				v = t;
			}

			kudu_camera_swing(camera, h, v, 0);
			break;
		case CAMERA_MODE_DOLLY: /* Not technically "zooming" we're just moving the camera forwards */
			opts = kudu_options_get_int_no(KO_CAMERA_DOLLY, 4);
			amount = (kudu_camera_get_distance(camera) / 100);

			if (opts & MOUSE_FLAG_SWOP_XY) {
				v = -(hscroll * amount);
				if (opts & MOUSE_FLAG_REVERSE_X) v = -v;
			} else {
				v = -(vscroll * amount);
				if (opts & MOUSE_FLAG_REVERSE_Y) v = -v;
			}

			kudu_camera_swing(camera, 0, 0, v);
			break;
		case CAMERA_MODE_TRACK:
			h = (hscroll / 10);
			v = (vscroll / 10);
			opts = kudu_options_get_int_no(KO_CAMERA_TRACK, 4);

			if (opts & MOUSE_FLAG_REVERSE_X) h = -h;
			if (opts & MOUSE_FLAG_REVERSE_Y) v = -v;

			if (opts & MOUSE_FLAG_SWOP_XY) {
				t = h;
				h = v;
				v = t;
			}

			kudu_camera_move(camera, h, v);
			break;
		default:	/* Camera is in fixed position mode - do nothing */
			return FALSE;
			break;
	}

	return TRUE;
}

/* Determine which mode the camera should be in based on mouse / key states passed in "mouse" */
/* Return values:
	0 = no actions performed
	1 = actions performed - do not continue processing mouse/keyboard inputs
	2 = actions performed - do not continue processing mouse/keyboard inputs, and refresh dislpay */
int kudu_camera_determine_mode(KuduCamera *camera, KuduMouse *mouse)
{
	int cam_tumble[6], cam_track[6], cam_dolly[6], cam_globals[2], nopop = FALSE, rv = 0, pm, cm, load = FALSE;

	kudu_options_get_int(KO_CAMERA_GLOBALS, 2, cam_globals);
	kudu_options_get_int(KO_CAMERA_TUMBLE, 6, cam_tumble);
	kudu_options_get_int(KO_CAMERA_TRACK, 6, cam_track);
	kudu_options_get_int(KO_CAMERA_DOLLY, 6, cam_dolly);

	pm = kudu_camera_get_previous_mode(camera);
	cm = camera->mode;

	/* If the current mode's activation is "momentary" then set the load boolean to TRUE,
	   this will tell us to "load" any new modes rather than "pushing" them */
	if (((cm == CAMERA_MODE_TUMBLE) && (cam_tumble[3])) ||
	    ((cm == CAMERA_MODE_TRACK) && (cam_track[3])) ||
	    ((cm == CAMERA_MODE_DOLLY) && (cam_dolly[3]))) load = TRUE;

	if ((((mouse->button == cam_tumble[0]) && (mouse->state == BUTTON_DOWN)) ||
	    ((cam_tumble[0] == MOUSE_USE_KEY) && (mouse->key == cam_tumble[5]))) &&
	     (mouse->modifiers == cam_tumble[1])) {	/* Key/mouse combo matches requirments for entering "tumble" mode.. */
		if (camera->mode != CAMERA_MODE_TUMBLE) {	/* If we are already in tumble mode then skip */
			if ((cam_tumble[2] == CAMERA_MODE_ANY) || (cam_tumble[2] == camera->mode)) {	/* Are dependencies satisfied? */
				/* If the previous mode was tumble then just pop the stack to return */
				if (pm == CAMERA_MODE_TUMBLE) kudu_camera_pop_mode(camera);
				else if (load) kudu_camera_load_mode(camera, CAMERA_MODE_TUMBLE); /* Must mode be "pushed" or "loaded" ? */
				else	kudu_camera_push_mode(camera, CAMERA_MODE_TUMBLE);
				nopop = TRUE;
				mouse->drop_release = 0;
			}
		} else if ((!cam_tumble[3]) && (cam_tumble[0] == MOUSE_USE_KEY)) {
			/* If mode is "toggle" and we are using a key instead of a mouse button then pop the camera mode */
			kudu_camera_pop_mode(camera);
			nopop = TRUE;
		}
	}

	/* This code segment basically the same as for tumble mode aboce ^^ */
	if ((((mouse->button == cam_track[0]) && (mouse->state == BUTTON_DOWN)) ||
	    ((cam_track[0] == MOUSE_USE_KEY) && (mouse->key == cam_track[5]))) &&
	     (mouse->modifiers == cam_track[1])) {
		if (camera->mode != CAMERA_MODE_TRACK) {
			if ((cam_track[2] == CAMERA_MODE_ANY) || (cam_track[2] == camera->mode)) {
				if (pm == CAMERA_MODE_TRACK) kudu_camera_pop_mode(camera);
				else if (load) kudu_camera_load_mode(camera, CAMERA_MODE_TRACK);
				else	kudu_camera_push_mode(camera, CAMERA_MODE_TRACK);
				nopop = TRUE;
				mouse->drop_release = 0;
			}
		} else if ((!cam_track[3]) && (cam_track[0] == MOUSE_USE_KEY)) {
			kudu_camera_pop_mode(camera);
			nopop = TRUE;
		}
	}

	/* This code segment basically the same as for tumble mode aboce ^^ */
	if ((((mouse->button == cam_dolly[0]) && (mouse->state == BUTTON_DOWN)) ||
	    ((cam_dolly[0] == MOUSE_USE_KEY) && (mouse->key == cam_dolly[5]))) &&
	     (mouse->modifiers == cam_dolly[1])) {
		if (camera->mode != CAMERA_MODE_DOLLY) {
			if ((cam_dolly[2] == CAMERA_MODE_ANY) || (cam_dolly[2] == camera->mode)) {
				if (pm == CAMERA_MODE_DOLLY) kudu_camera_pop_mode(camera);
				else if (load) kudu_camera_load_mode(camera, CAMERA_MODE_DOLLY);
				else	kudu_camera_push_mode(camera, CAMERA_MODE_DOLLY);
				nopop = TRUE;
				mouse->drop_release = 0;
			}
		} else if ((!cam_dolly[3]) && (cam_dolly[0] == MOUSE_USE_KEY)) {
			kudu_camera_pop_mode(camera);
			nopop = TRUE;
		}
	}

	if (nopop) rv = 1;

	if (((camera->mode == CAMERA_MODE_TUMBLE) && (!cam_tumble[3])) ||
	    ((camera->mode == CAMERA_MODE_DOLLY) && (!cam_dolly[3])) ||
	    ((camera->mode == CAMERA_MODE_TRACK) && (!cam_track[3]))) {
		if (mouse->button == cam_globals[0]) {
			kudu_camera_pop_all_modes(camera);
			mouse->drop_release = 1;
			rv = 1;
			nopop = TRUE;
		} else if (mouse->button == cam_globals[1]) {
			kudu_camera_push_mode(camera, CAMERA_MODE_RESET);
			rv = 2;
			nopop = TRUE;
		}

	}


	if (!nopop) {
		if (((camera->mode == CAMERA_MODE_TUMBLE) && (cam_tumble[3])) ||
		    ((camera->mode == CAMERA_MODE_DOLLY) && (cam_dolly[3])) ||
		    ((camera->mode == CAMERA_MODE_TRACK) && (cam_track[3]))) {
			kudu_camera_pop_mode(camera);
			rv = 1;
			if ((camera->mode == CAMERA_MODE_FIXED) && (mouse->state == BUTTON_DOWN)) {
				mouse->drop_release = 1;
			}
		}
	}

	return rv;
}









