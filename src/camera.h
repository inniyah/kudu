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
#ifndef CAMERA_H_INCLUDED
#define CAMERA_H_INCLUDED

#define DAN_CAMERA_SLIDE_FORWARD 1
#define DAN_CAMERA_SLIDE_BACKWARD 2
#define DAN_CAMERA_SLIDE_LEFT 4
#define DAN_CAMERA_SLIDE_RIGHT 8
#define DAN_CAMERA_SWING_UP 1
#define DAN_CAMERA_SWING_DOWN 2
#define DAN_CAMERA_SWING_LEFT 4
#define DAN_CAMERA_SWING_RIGHT 8
#define HORIZONTAL 16
#define VERTICAL 32

#include "globals.h"
#include "mathematics.h"
#include "gui.h"

KuduCamera *kudu_camera_new(void);
int kudu_camera_destroy(KuduCamera*);
float kudu_camera_get_distance(KuduCamera*);
int kudu_camera_slide(KuduCamera*, float, int);
int kudu_camera_swing(KuduCamera*, float, float, float);
int kudu_camera_move(KuduCamera*, GLfloat, GLfloat);
int kudu_camera_set_position(KuduCamera*, float, float, float);
int kudu_camera_lookat(KuduCamera*, float, float, float);
int kudu_camera_set_gl_viewpoint(KuduCamera*);
int kudu_camera_set_gl_matrix(KuduCamera*, int, int);
int kudu_camera_set_gl_pick_matrix(KuduCamera*, int, int, int, int);
int kudu_camera_set_mode(KuduCamera*);
int kudu_camera_push_mode(KuduCamera*, K_CameraMode);
int kudu_camera_pop_mode(KuduCamera*);
int kudu_camera_pop_all_modes(KuduCamera*);
int kudu_camera_mouse_action(KuduCamera*, float, float);

#endif /* CAMERA_H_INCLUDED */
