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
#ifndef GLOBALS_H_INCLUDED
#define GLOBALS_H_INCLUDED

/* Includes */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef SUPPORT_PYTHON
#include <Python.h>
#endif

#ifdef HAVE_GL_GL_H
#include <GL/gl.h>
#endif

#ifdef HAVE_GL_GLU_H
#include <GL/glu.h>
#endif

#include <gdk/gdk.h>
#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
#include <gtk/gtkgl.h>
#include <X11/Xlib.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/timeb.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <getopt.h>

#include "kerror.h"

#ifndef KUDU_DATADIR
#define KUDU_DATADIR ../
#endif

#define KUDU_IMAGES_DIR KUDU_DATADIR"/images/"
#define KUDU_SCRIPTS_DIR KUDU_DATADIR"/scripts/"
#define KUDU_EXAMPLES_DIR KUDU_DATADIR"/examples/"

/* Definitions */

#ifndef MAXINT
#define MAXINT 2147483647
#endif

#ifndef M_PI
#define M_PI (3.1415926536)
#endif

/* Covert degrees to radians */
#define RADDEG (M_PI/180.0)

/* Convert half angle in degrees to radians (needed for quaternion rotations) */
#define HRADDEG (M_PI/360.0)

/* Setup version string */
#ifndef CVS_VERSION
#define KUDU_VERSION_STRING PACKAGE_STRING
#else
#define KUDU_VERSION_STRING PACKAGE_STRING" (CVS version)"
#endif

/* Year date to be display in version / copyright messages etc.. (string) */
#define KUDU_VERSION_DATE_Y "2006"

#define MOUSE_LEFT_BUTTON 1
#define MOUSE_MIDDLE_BUTTON 2
#define MOUSE_RIGHT_BUTTON 3
#define MOUSE_WHEEL_UP 4
#define MOUSE_WHEEL_DOWN 5
#define BUTTON_DOWN 1
#define BUTTON_UP 2

#define SLIDE_FORWARD 1
#define SLIDE_BACKWARD 2
#define SLIDE_LEFT 4
#define SLIDE_RIGHT 8
#define SWING_UP 1
#define SWING_DOWN 2
#define SWING_LEFT 4
#define SWING_RIGHT 8
#define HORIZONTAL 16
#define VERTICAL 32

#define K_RENDER_FACES 1
#define K_RENDER_EDGES 2
#define K_RENDER_POINTS 4
#define K_RENDER_LIGHTING 8
#define K_RENDER_NO_COLOURS 16
#define K_RENDER_SELECT 32
#define K_RENDER_SHOW_SELECTED 64
#define K_RENDER_SHOW_SELECTED_INFO 128
#define K_RENDER_SHOW_INFO 256
#define K_RENDER_FLAT_SHADING 512
#define K_RENDER_SHOW_SELECTED_FACES 1024
#define K_RENDER_SHOW_SELECTED_EDGES 2048
#define K_RENDER_SHOW_SELECTED_POINTS 4096
#define K_RENDER_POINTS_NO_COLOURS 8192
#define K_RENDER_EDGES_NO_COLOURS 16384
#define K_RENDER_SHAPE_SELECTED 32768
#define K_RENDER_SHOW_SELECTED_SHAPES 65536
#define K_RENDER_SELECT_SHAPES 131072
#define K_RENDER_SELECT_OBJECTS 262144
#define K_RENDER_OMIT_SELECTED_OBJECT 524288
#define K_RENDER_SHOW_AXES 1048576
#define K_RENDER_SHOW_SELECTED_AXES 2097152
#define K_RENDER_TRANSLATE_POS 4194304
#define K_RENDER_TEXTURES 8388608
#define K_RENDER_JOINTS 16777216

#define K_SELECT_PICK 1
#define K_SELECT_RECTANGLE 2

/*#define SELECT_POINTS 1
#define SELECT_EDGES 2
#define SELECT_FACES 3
#define SELECT_SHAPES 4
#define SELECT_BONES 5
#define SELECT_OBJECTS 6*/

/* Program mode names */
#define PROGRAM_MODE_EDIT 1
#define PROGRAM_MODE_ANIMATION 2

/* Program sub mode names */
#define PROGRAM_MODE_SKELETON 3
#define PROGRAM_MODE_SKIN 4
#define PROGRAM_MODE_ATTACH 5
#define PROGRAM_MODE_VIEW 6
/* Sub mode names for animation mode, just for convenience */
#define PROGRAM_MODE_SEQUENCE 3
#define PROGRAM_MODE_SCRIPT 4
#define PROGRAM_MODE_SET 5
#define PROGRAM_MODE_PLAYBACK 6

#define SKELETON_DETAIL_SHOW_ALL 1
#define SKELETON_DETAIL_SHOW_SELECTED 2
#define SKELETON_DETAIL_SHOW_NONE 3

#define EDIT_MODE_NONE 0
#define EDIT_MODE_SHIFT_X 1
#define EDIT_MODE_SHIFT_Y 2
#define EDIT_MODE_SHIFT_Z 3

#define PLAYBACK_LOOP 16
#define PLAYBACK_CAMERA 32
#define PO_SHOW_NORMALS 64

#define CAMERA_MODE_FIXED 0
#define CAMERA_MODE_SWING 1
#define CAMERA_MODE_ZOOM 2
#define CAMERA_MODE_SLIDE 4
#define CAMERA_MODE_RESET 5


#define JOINT_MODE_FIXED 0
#define JOINT_MODE_MOVEX 1
#define JOINT_MODE_MOVEY 2
#define JOINT_MODE_MOVEZ 3

#define OBJECT_MODE_FIXED 0
#define OBJECT_MODE_MOVEX 1
#define OBJECT_MODE_MOVEY 2
#define OBJECT_MODE_MOVEZ 3

#define VERTEX_NULL -1
#define MATERIAL_NULL -1
#define OBJECT_NULL -1
#define OBJECT_POS -2
#define FRAME_NULL -1
#define TEXTURE_NULL -1

#define DO_SHOW_NAMES 1
#define DO_SHOW_SELECTED_NAME 2
#define DO_DISABLE_LIGHTING 4
#define DO_SELECTION_MODE 8
#define DO_ONLY_POINTS 16
#define DO_DRAW_WIRE 32
#define DO_DRAW_REAL_COLOURS 64
#define DO_SHOW_NORMALS 128
#define DO_NONE_SELECTED 256

#define VERTEX_SELECTED 1
#define VERTEX_NAMED 2
#define VERTEX_ON_JOINT 4


#define DRAW_NULL -1
#define DRAW_QUAD 1
#define DRAW_POLYGON 2
#define DRAW_TRIANGLE 3
#define DRAW_POINT 4
#define DRAW_SET_COLOUR 10
#define DRAW_SET_ALPHA 11

#define NOKE_PARENT_SELECTED 1
#define NOKE_ANGLE_SHIFTED 2
#define NOKE_POSITION_SHIFTED 4

#define TERRAIN_TEXTURED 1
#define TERRAIN_COLOUR_COPY_MAPPED 2
#define TERRAIN_COLOUR_MAPPED 3
#define TERRAIN_TEXTURE_MAPPED 4

#define TERRAIN_LOADED 1

#define BF_GUI_EDIT_OPEN 1

/* Macros */

#ifndef RETURN_NONE
#define RETURN_NONE return (Py_INCREF(Py_None), Py_None);
#endif

#ifndef RETURN_TRUE
#define RETURN_TRUE return (Py_INCREF(Py_True), Py_True);
#endif

#ifndef RETURN_FALSE
#define RETURN_FALSE return (Py_INCREF(Py_False), Py_False);
#endif

/* Const declarations */


/* Enumarated Lists */

typedef enum {
	OPERATION_NONE,
	OPERATION_BONE_SET,
	OPERATION_VERTEX_SET,
	OPERATION_OBJECT_SET,
} K_FRAME_OPERATION;


typedef enum {
	SET_ALL_SCENE,
	SET_ALL_OBJECT,
	SET_BONES_SCENE,
	SET_BONES_OBJECT,
	SET_BONES_SELECTED,
	SET_VERTICES_SCENE,
	SET_VERTICES_OBJECT,
	SET_VERTICES_SELECTED,
	SET_POSITION_SCENE,
	SET_POSITION_OBJECT,
} K_FRAME_SETS;


typedef enum {
	SELECT_NOTHING,
	SELECT_ALL,
	SELECT_INVERSE,
	S_MESH,
	SELECT_POINTS,
	SELECT_EDGES,
	SELECT_FACES,
	SELECT_SHAPES,
	E_MESH,
	SELECT_BONES,
	SELECT_JOINTS,
	SELECT_BONES_PARENTS,
	SELECT_BONES_FIRST_CHILD,
	SELECT_BONES_NEXT_SIBLING,
	SELECT_BONES_PREVIOUS_SIBLING,
	SELECT_BONES_ALL_CHILDREN,
	SELECT_OBJECTS,
} K_SELECTION_TYPE;

typedef enum {
	KS_GENERAL,
	KS_EMBED,
} K_ScriptType;

typedef enum {
	BONE_MODE_FIXED,
	BONE_MODE_ROT_LX,
	BONE_MODE_ROT_LY,
	BONE_MODE_ROT_LZ,
	BONE_MODE_ROT_GX,
	BONE_MODE_ROT_GY,
	BONE_MODE_ROT_GZ,
	BONE_MODE_ROTATE,
	BONE_MODE_STRETCH,
	BONE_MODE_MOVEX,
	BONE_MODE_MOVEY,
	BONE_MODE_MOVEZ,
} K_BONE_MODE;

/* Structure Declarations */

typedef struct _KuduScript {
	char *filename;
	char *name;
	char *author;
	char *menu;
	K_ScriptType type;
	struct _KuduScript *next_script;
	struct _KuduScript *previous_script;
} KuduScript;

typedef struct {
	float ha;
	float va;
	float dist;
	float distance;
	float old_ha;
	float old_va;
	float near_plane;
	float far_plane;
	float fov;
	int ortho;
	int mode;
	int old_mode;
	float old_position[3];
	float position[3];
	float focus[3];
	float up[3];
} KuduCamera;

typedef struct {
	GLint sx;
	GLint sy;
	GLint ex;
	GLint ey;
	GLbyte dragging;
} K_MOUSE;


typedef struct _KuduMaterial {
	int id;
	char name[64];
	float diffuse[4];
	float ambient[4];
	float specular[4];
	float emission[4];
	float shininess;
	struct _KuduTexture *texture;
	struct _KuduMaterial *next_material;
	struct _KuduMaterial *previous_material;
} KuduMaterial;

typedef struct {
	unsigned int width;
	unsigned int height;
	unsigned int channels;
	unsigned int bpp;
	unsigned int size;
	unsigned char *data;
} KuduImage;

typedef struct _KuduTexture {
	int id;
	unsigned int tex;
	unsigned int width;
	unsigned int height;
	struct _KuduTexture *next_texture;
	struct _KuduTexture *previous_texture;
	KuduImage *image;
} KuduTexture;

/*typedef struct {
	unsigned char type;
	GLint flags;
	GLint width;
	GLint length;
	float lengthM;
	float widthM;
	float heightM;
	char filename[1024];
	float *terrainv;
	unsigned char *rgbcols;
} K_TERRAIN;*/

typedef struct _KuduJoint {
	int id;
	int num_bones;
	float pos[3];	/* Position */
	float ppos[3];	/* Position offset for pose mode */
	float temp;
	float zlen;
	unsigned char selected;
	struct _KuduObject *object;
	struct _KuduBone *bone;
	struct _KuduJoint *next_joint;
	struct _KuduJoint *previous_joint;
} KuduJoint;

/*typedef struct _KuduJointList {
	int num_joints;
	int num_avail;
	KuduJoint *joint;
} KuduJointList;*/

typedef struct _KuduBone {
	int id;
	char name[64];
	float axis[3];
	float rotation;		/* Bone rotation */
	float protation;	/* Bone rotation offset in pose mode*/
	float length;		/* Bone length */
	float plength;		/* Bone length offset in pose mode */
	float temp;
	KuduJoint *s_joint;	/* Base Joint */
	KuduJoint *e_joint;	/* End Joint */
	float lquat[4];		/* Local Rotation quaternion - stores rotations about the local axes */
	float gquat[4];		/* Global Rotation quaternion - stores rotations about the global axes */
	float quat[4];		/* Final quaternion - the absoloute position of the bone */
	float matrix[16];	/* Final matrix */
	float lmatrix[16];	/* Local matrix - stores rotations about the local axis */
	float gmatrix[16];	/* Global matrix - stores rotations about the global axis */
	/* Posing */
	float lpquat[4];	/* Local rotation pose quaternion */
	float gpquat[4];	/* Global rotation pose quaternion */
	float pquat[4];		/* Pose quaternion - effectively the rotation of the bone in pose mode */
	float pmatrix[16];	/* Pose matrix - position in pose mode */
	int magic_touch;
	unsigned char selected;
	int num_verts;		/* Number of vertices attached to this bone */
	struct _KuduVertex **vertex;	/* List of pointers to attached vertices */
	struct _KuduBone *parent;
	struct _KuduBone *first_child;
	struct _KuduBone *next_sibling;
	struct _KuduBone *previous_sibling;
	struct _KuduObject *object;
} KuduBone;

typedef struct _KuduVertex {
	int id;
	unsigned char attached_percent;
	float v[3];
	float tv[3];
	float av[3];
	float ov[3];
	float n[3];
	char flags;
	int magic_touch;
	unsigned char magic_normal;
	unsigned char selected;
	int num_bones;
	float *influence;
	KuduBone **bone;
	struct _KuduVertex *previous_vertex;
	struct _KuduVertex *next_vertex;
	struct _KuduEdge *edge;
} KuduVertex;

typedef struct _KuduFace {
	int id;
	unsigned char selected;
	float fn[3];
	KuduMaterial *material;
	struct _KuduFace *previous_face;
	struct _KuduFace *next_face;
	struct _KuduEdge *edge;
} KuduFace;

typedef struct _KuduEdge {
	int id;
	unsigned char hard;
	float s_uv[2];
	float e_uv[2];
	unsigned char selected;

	KuduVertex *start_vertex;
	KuduVertex *end_vertex;
	KuduFace *left_face;
	KuduFace *right_face;
	struct _KuduEdge *left_pred;
	struct _KuduEdge *left_succ;
	struct _KuduEdge *right_pred;
	struct _KuduEdge *right_succ;
	struct _KuduEdge *previous_edge;
	struct _KuduEdge *next_edge;
	struct _KuduShape *shape;
} KuduEdge;

typedef struct _KuduShape {
	int id;
	unsigned char selected;

	KuduVertex *vertex;
	KuduFace *face;
	KuduEdge *edge;
	struct _KuduShape *next_shape;
	struct _KuduShape *previous_shape;
} KuduShape;

typedef struct _KuduObject {
	char name[256];
	int id;
	char *filename;
	int filename_length;
	float position[3];
	int next_joint_id;
	int next_bone_id;
	int next_material_id;
	int next_texture_id;
	int next_vertex_id;
	int next_edge_id;
	int next_face_id;
	int next_shape_id;
	char *author;
	char *email;
	char *url;
	int author_length;
	int email_length;
	int url_length;
	KuduJoint *joint;
	KuduBone *bone;
	KuduMaterial *material;
	KuduShape *skin;
	KuduTexture *texture;
	struct _KuduObject *next_object;
	struct _KuduObject *previous_object;
} KuduObject;

typedef struct _KuduSelection {
	void *item;
	struct _KuduSelection *next_item;
	struct _KuduSelection *previous_item;
} KuduSelection;

typedef struct _KuduSelectionList {
	int selected_items;
	K_SELECTION_TYPE type;
	KuduSelection *first_item;
} KuduSelectionList;

typedef struct _KuduFrame {
	int frame;
	K_FRAME_OPERATION operation;
	float fdata[4];
	struct _KuduFrame *next_frame;
	struct _KuduFrame *previous_frame;
	struct _KuduItemFrame *item_frame;
} KuduFrame;

typedef struct _KuduItemFrame {
	void *item;
	KuduFrame *first_frame;
	KuduFrame *last_frame;
	struct _KuduItemFrame *next_item;
	struct _KuduItemFrame *previous_item;
	struct _KuduObjectFrame *object_frame;
} KuduItemFrame;

typedef struct _KuduObjectFrame {
	KuduObject *object;
	KuduItemFrame *first_item;
	struct _KuduObjectFrame *next_object;
	struct _KuduObjectFrame *previous_object;
	struct _KuduFrameList *frame_list;
} KuduObjectFrame;

typedef struct _KuduFrameList {
	int max_frame;
	int current_frame;
	int object_cnt;
	KuduObjectFrame *first_object;
} KuduFrameList;

typedef struct {
	unsigned int *buffer;
	int buf_size;
	int hits;
} K_SELECTION_HITS;

typedef struct {
	float windowWidth;
	float windowHeight;
	float offset[3];
	int drawing_area_xpos;
	int drawing_area_ypos;
	int mode;
	int sub_mode;
	int mode_opt_no;
	int selection_mode;
	int uip_mode;
	int bone_mode;
	int edit_mode;
	int joint_mode;
	int objectMode;
	int currentFrame;
	int grid_list;
	int displayList;
	int front_display_list;
	int bone_graphic_lists;
	int object_list;
	int skeleton_list;
	int skin_list;
	int back_skin_list;
	char request_refresh;
	K_SELECTION_HITS hit;
	K_MOUSE mouse;
	KuduObject *selected_object;
} K_PROGRAM;


/* Global Varibles */

extern K_PROGRAM program;

#endif /* GLOBALS_H_INCLUDED */
