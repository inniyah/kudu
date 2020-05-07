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
#include "objectsmodule.h"
#include "structmember.h"

static KuduObject *first_object = NULL;
static KuduFrameList *frame_list = NULL;
static KuduSelectionList *selection_list = NULL, *bones_selection_list = NULL;

static int update_bones, update_vertices, update_normals;

typedef struct {
	PyObject_HEAD
	KuduObject *d_object;
} o_object;

typedef struct {
	PyObject_HEAD
	KuduBone *d_bone;
	KuduObject *d_object;
} o_bone;

typedef struct {
	PyObject_HEAD
	KuduVertex *d_vertex;
	KuduObject *d_object;
	int tmp;
} o_vertex;

typedef struct {
	PyObject_HEAD
	KuduFace *d_face;
	KuduObject *d_object;
} o_face;

typedef struct {
	PyObject_HEAD
	KuduEdge *d_edge;
	KuduObject *d_object;
} o_edge;

typedef struct {
	PyObject_HEAD
	KuduShape *d_shape;
	KuduObject *d_object;
} o_shape;


/*******************************************************************/
/* creation/init/dealloc routines for edges, faces, verts & shapes */
/*******************************************************************/

/************************************/
/*   o_face creation/init/dealloc   */
/************************************/
static void face_dealloc(o_face *self)
{
	self->ob_type->tp_free((PyObject*)self);
}

static int face_init(o_face *self, PyObject *args, PyObject *kwds)
{
	self->d_face = NULL;
	self->d_object = NULL;

	return 0;
}

static PyObject *face_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
	o_face *self;

	self = (o_face*)type->tp_alloc(type, 0);

	return (PyObject*)self;
}

/************************************/
/*   o_edge creation/init/dealloc   */
/************************************/
static void edge_dealloc(o_edge *self)
{
	self->ob_type->tp_free((PyObject*)self);
}

static int edge_init(o_edge *self, PyObject *args, PyObject *kwds)
{
	self->d_edge = NULL;
	self->d_object = NULL;

	return 0;
}

static PyObject *edge_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
	o_edge *self;

	self = (o_edge*)type->tp_alloc(type, 0);

	return (PyObject*)self;
}

/************************************/
/*  o_vertex creation/init/dealloc  */
/************************************/
static void vertex_dealloc(o_vertex *self)
{
	self->ob_type->tp_free((PyObject*)self);
}

static int vertex_init(o_vertex *self, PyObject *args, PyObject *kwds)
{
	self->d_vertex = NULL;
	self->d_object = NULL;

	return 0;
}

static PyObject *vertex_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
	o_vertex *self;

	self = (o_vertex*)type->tp_alloc(type, 0);

	return (PyObject*)self;
}

/************************************/
/*  o_shape creation/init/dealloc   */
/************************************/
static void shape_dealloc(o_shape *self)
{
	self->ob_type->tp_free((PyObject*)self);
}

static int shape_init(o_shape *self, PyObject *args, PyObject *kwds)
{
	self->d_shape = NULL;
	self->d_object = NULL;

	return 0;
}

static PyObject *shape_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
	o_shape *self;

	self = (o_shape*)type->tp_alloc(type, 0);

	return (PyObject*)self;
}

/*******************************************************************/
/*  method declarations and types for edges, faces, verts & shapes */
/*******************************************************************/

/************************************/
/*   o_face methods declarations    */
/************************************/
static PyObject *face_id(o_face*);
static PyObject *face_edge(o_face*);

static PyMethodDef o_face_methods[] = {
	{"id", (PyCFunction)face_id, METH_NOARGS, "Return the face's id"},
	{"edge", (PyCFunction)face_edge, METH_NOARGS, "Return the incident edge for this face"},
	{NULL, NULL, 0, NULL}
};

static PyTypeObject o_faceType = {
	PyObject_HEAD_INIT(NULL)
	0, "objects.face", sizeof(o_face),
	0, (destructor)face_dealloc, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, Py_TPFLAGS_DEFAULT,
	"K_Face", 0, 0, 0, 0, 0, 0, o_face_methods, 0, 0, 0, 0, 0, 0, 0, (initproc)face_init, 0, face_new,
};

/************************************/
/*   o_edge methods declarations    */
/************************************/
static PyObject *edge_id(o_edge*);
static PyObject *edge_left_successor(o_edge*);
static PyObject *edge_left_predecessor(o_edge*);
static PyObject *edge_right_successor(o_edge*);
static PyObject *edge_right_predecessor(o_edge*);
static PyObject *edge_set_left_successor(o_edge*, PyObject*);
static PyObject *edge_set_left_predecessor(o_edge*, PyObject*);
static PyObject *edge_set_right_successor(o_edge*, PyObject*);
static PyObject *edge_set_right_predecessor(o_edge*, PyObject*);
static PyObject *edge_set_start_vertex(o_edge*, PyObject*);
static PyObject *edge_set_end_vertex(o_edge*, PyObject*);
static PyObject *edge_set_left_face(o_edge*, PyObject*);
static PyObject *edge_set_right_face(o_edge*, PyObject*);

static PyMethodDef o_edge_methods[] = {
	{"id", (PyCFunction)edge_id, METH_NOARGS, "Return the edge's id"},
	{"left_succ", (PyCFunction)edge_left_successor, METH_NOARGS, "Return the left successor"},
	{"left_pred", (PyCFunction)edge_left_predecessor, METH_NOARGS, "Return the left predecessor"},
	{"right_succ", (PyCFunction)edge_right_successor, METH_NOARGS, "Return the right successor"},
	{"right_pred", (PyCFunction)edge_right_predecessor, METH_NOARGS, "Return the right predeccessor"},
	{"set_left_succ", (PyCFunction)edge_set_left_successor, METH_VARARGS, "Set the left successor"},
	{"set_left_pred", (PyCFunction)edge_set_left_predecessor, METH_VARARGS, "Set the left predecessor"},
	{"set_right_succ", (PyCFunction)edge_set_right_successor, METH_VARARGS, "Set the right successor"},
	{"set_right_pred", (PyCFunction)edge_set_right_predecessor, METH_VARARGS, "Set the right predecessor"},
	{"set_start_vertex", (PyCFunction)edge_set_start_vertex, METH_VARARGS, "Set the start vertex"},
	{"set_end_vertex", (PyCFunction)edge_set_end_vertex, METH_VARARGS, "Set the end vertex"},
	{"set_left_face", (PyCFunction)edge_set_left_face, METH_VARARGS, "Set the left face"},
	{"set_right_face", (PyCFunction)edge_set_right_face, METH_VARARGS, "Set the right face"},
	{NULL, NULL, 0, NULL}
};

static PyTypeObject o_edgeType = {
	PyObject_HEAD_INIT(NULL)
	0, "objects.edge", sizeof(o_edge),
	0, (destructor)edge_dealloc, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, Py_TPFLAGS_DEFAULT,
	"K_Edge", 0, 0, 0, 0, 0, 0, o_edge_methods, 0, 0, 0, 0, 0, 0, 0, (initproc)edge_init, 0, edge_new,
};

/***********************************/
/*  o_vertex methods declarations  */
/***********************************/
static PyObject *vertex_id(o_vertex*);
static PyObject *vertex_pos3d(o_vertex*);
static PyObject *vertex_offset3d(o_vertex*);
static PyObject *vertex_set_pos3d(o_vertex*, PyObject*);
static PyObject *vertex_set_offset3d(o_vertex*, PyObject*);
static PyObject *vertex_set_frame(o_vertex*, PyObject*);
static PyObject *vertex_key_frames(o_vertex *self);
static PyObject *vertex_next(o_vertex*);
static PyObject *vertex_previous(o_vertex*);
static PyObject *vertex_edge(o_vertex*);

static PyMethodDef o_vertex_methods[] = {
	{"id", (PyCFunction)vertex_id, METH_NOARGS, "Return the vertex ID"},
	{"position", (PyCFunction)vertex_pos3d, METH_NOARGS, "Return the vertex position"},
	{"offset", (PyCFunction)vertex_offset3d, METH_NOARGS, "Return the vertex offset position"},
	{"set_position", (PyCFunction)vertex_set_pos3d, METH_VARARGS, "Set the vertex position"},
	{"set_offset", (PyCFunction)vertex_set_offset3d, METH_VARARGS, "Set the vertex offset position"},
	{"set_frame", (PyCFunction)vertex_set_frame, METH_VARARGS, "Set a frame for the vertex"},
	{"key_frames", (PyCFunction)vertex_key_frames, METH_NOARGS, "Return the vertex's key frame numbers in a tuple"},
	{"next", (PyCFunction)vertex_next, METH_NOARGS, "Return the next vertex in the list (if any)"},
	{"previous", (PyCFunction)vertex_previous, METH_NOARGS, "Return the previous vertex in the list (if any)"},
	{"edge", (PyCFunction)vertex_edge, METH_NOARGS, "Return the vertex edge"},
	{NULL, NULL, 0, NULL}
};

static PyMemberDef o_vertex_members[] = {
	{"temp", T_INT, offsetof(o_vertex, tmp), 0, "Temporary int"},
	{NULL, 0, 0, 0, NULL}
};

static PyTypeObject o_vertexType = {
	PyObject_HEAD_INIT(NULL)
	0, "objects.vertex", sizeof(o_vertex),
	0, (destructor)vertex_dealloc, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, Py_TPFLAGS_DEFAULT,
	"K_Vertex", 0, 0, 0, 0, 0, 0, o_vertex_methods, o_vertex_members, 0, 0, 0, 0, 0, 0, (initproc)vertex_init, 0, vertex_new,
};

/************************************/
/*  o_shape methods declarations    */
/************************************/
static PyObject *shape_id(o_shape*);
static PyObject *shape_new_vertex(o_shape*, PyObject*, PyObject*);
static PyObject *shape_new_edge(o_shape*, PyObject*, PyObject*);
static PyObject *shape_new_face(o_shape*, PyObject*, PyObject*);

static PyMethodDef o_shape_methods[] = {
	{"id", (PyCFunction)shape_id, METH_NOARGS, "Return the shape's ID"},
	{"new_vertex", (PyCFunction)shape_new_vertex, METH_NOARGS, "Create a new vertex on this shape"},
	{"new_edge", (PyCFunction)shape_new_edge, METH_NOARGS, "Create a new edge on this shape"},
	{"new_face", (PyCFunction)shape_new_face, METH_NOARGS, "Create a new face on this shape"},
	{NULL, NULL, 0, NULL}
};

static PyTypeObject o_shapeType = {
	PyObject_HEAD_INIT(NULL)
	0, "objects.shape", sizeof(o_shape),
	0, (destructor)shape_dealloc, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, Py_TPFLAGS_DEFAULT,
	"K_Shape", 0, 0, 0, 0, 0, 0, o_shape_methods, 0, 0, 0, 0, 0, 0, 0, (initproc)shape_init, 0, shape_new,
};


/*******************************************************************/
/*           methods for edges, faces, verts & shapes              */
/*******************************************************************/


/************************************/
/*          o_face methods          */
/************************************/
static PyObject *face_id(o_face *self)
{
	KuduFace *d_face = self->d_face;
	if (d_face == NULL) return Py_BuildValue("i", -1);

	return Py_BuildValue("i", d_face->id);
}

static PyObject *face_edge(o_face *self)
{
	KuduFace *d_face = self->d_face;
	if (d_face == NULL) RETURN_NONE
	if (d_face->edge == NULL) RETURN_NONE

	o_edge *edge = (o_edge*)edge_new(&o_edgeType, NULL, NULL);
	edge->d_edge = d_face->edge;
	edge->d_object = self->d_object;

	return (PyObject*)edge;
}


/************************************/
/*         o_edge methods           */
/************************************/
static PyObject *edge_id(o_edge *self)
{
	KuduEdge *d_edge = self->d_edge;
	if (d_edge == NULL) return Py_BuildValue("i", -1);

	return Py_BuildValue("i", d_edge->id);
}

static PyObject *edge_left_successor(o_edge *self)
{
	KuduEdge *d_edge = self->d_edge;
	if (d_edge == NULL) RETURN_NONE
	if (d_edge->left_succ == NULL) RETURN_NONE

	o_edge *edge = (o_edge*)edge_new(&o_edgeType, NULL, NULL);
	edge->d_edge = d_edge->left_succ;
	edge->d_object = self->d_object;

	return (PyObject*)edge;
}

static PyObject *edge_left_predecessor(o_edge *self)
{
	KuduEdge *d_edge = self->d_edge;
	if (d_edge == NULL) RETURN_NONE
	if (d_edge->left_pred == NULL) RETURN_NONE

	o_edge *edge = (o_edge*)edge_new(&o_edgeType, NULL, NULL);
	edge->d_edge = d_edge->left_pred;
	edge->d_object = self->d_object;

	return (PyObject*)edge;
}

static PyObject *edge_right_successor(o_edge *self)
{
	KuduEdge *d_edge = self->d_edge;
	if (d_edge == NULL) RETURN_NONE
	if (d_edge->right_succ == NULL) RETURN_NONE

	o_edge *edge = (o_edge*)edge_new(&o_edgeType, NULL, NULL);
	edge->d_edge = d_edge->right_succ;
	edge->d_object = self->d_object;

	return (PyObject*)edge;
}

static PyObject *edge_right_predecessor(o_edge *self)
{
	KuduEdge *d_edge = self->d_edge;
	if (d_edge == NULL) RETURN_NONE
	if (d_edge->right_pred == NULL) RETURN_NONE

	o_edge *edge = (o_edge*)edge_new(&o_edgeType, NULL, NULL);
	edge->d_edge = d_edge->right_pred;
	edge->d_object = self->d_object;

	return (PyObject*)edge;
}

static PyObject *edge_set_left_successor(o_edge *self, PyObject *args)
{
	KuduEdge *d_edge = self->d_edge;
	if (d_edge == NULL) RETURN_FALSE

	o_edge *ls;
	if (!(PyArg_ParseTuple(args, "O", &ls))) return NULL;

	kudu_edge_wings_set(d_edge, ls->d_edge, NULL, NULL, NULL);

	RETURN_TRUE
}

static PyObject *edge_set_left_predecessor(o_edge *self, PyObject *args)
{
	KuduEdge *d_edge = self->d_edge;
	if (d_edge == NULL) RETURN_FALSE

	o_edge *lp;
	if (!(PyArg_ParseTuple(args, "O", &lp))) return NULL;

	kudu_edge_wings_set(d_edge, NULL, lp->d_edge, NULL, NULL);

	RETURN_TRUE
}

static PyObject *edge_set_right_successor(o_edge *self, PyObject *args)
{
	KuduEdge *d_edge = self->d_edge;
	if (d_edge == NULL) RETURN_FALSE

	o_edge *rs;
	if (!(PyArg_ParseTuple(args, "O", &rs))) return NULL;

	kudu_edge_wings_set(d_edge, NULL, NULL, rs->d_edge, NULL);

	RETURN_TRUE
}

static PyObject *edge_set_right_predecessor(o_edge *self, PyObject *args)
{
	KuduEdge *d_edge = self->d_edge;
	if (d_edge == NULL) RETURN_FALSE

	o_edge *rp;
	if (!(PyArg_ParseTuple(args, "O", &rp))) return NULL;

	kudu_edge_wings_set(d_edge, NULL, NULL, NULL, rp->d_edge);

	RETURN_TRUE
}

static PyObject *edge_set_start_vertex(o_edge *self, PyObject *args)
{
	KuduEdge *d_edge = self->d_edge;
	if (d_edge == NULL) RETURN_FALSE

	o_vertex *sv;
	if (!(PyArg_ParseTuple(args, "O", &sv))) return NULL;

	kudu_edge_vertices_set(d_edge, sv->d_vertex, NULL);

	RETURN_TRUE
}

static PyObject *edge_set_end_vertex(o_edge *self, PyObject *args)
{
	KuduEdge *d_edge = self->d_edge;
	if (d_edge == NULL) RETURN_FALSE

	o_vertex *ev;
	if (!(PyArg_ParseTuple(args, "O", &ev))) return NULL;

	kudu_edge_vertices_set(d_edge, NULL, ev->d_vertex);

	RETURN_TRUE
}

static PyObject *edge_set_left_face(o_edge *self, PyObject *args)
{
	KuduEdge *d_edge = self->d_edge;
	if (d_edge == NULL) RETURN_FALSE

	o_face *lf;
	if (!(PyArg_ParseTuple(args, "O", &lf))) return NULL;

	kudu_edge_faces_set(d_edge, lf->d_face, NULL);

	RETURN_TRUE
}

static PyObject *edge_set_right_face(o_edge *self, PyObject *args)
{
	KuduEdge *d_edge = self->d_edge;
	if (d_edge == NULL) RETURN_FALSE

	o_face *rf;
	if (!(PyArg_ParseTuple(args, "O", &rf))) return NULL;

	kudu_edge_faces_set(d_edge, NULL, rf->d_face);

	RETURN_TRUE
}



/********************************/
/*	 o_vertex methods	*/
/********************************/
static PyObject *vertex_id(o_vertex *self)
{
	KuduVertex *d_vertex = self->d_vertex;
	if (d_vertex == NULL) return Py_BuildValue("i", -1);

	return Py_BuildValue("i", d_vertex->id);
}

static PyObject *vertex_pos3d(o_vertex *self)
{
	KuduVertex *d_vertex = self->d_vertex;
	if (d_vertex == NULL) RETURN_NONE

	return Py_BuildValue("fff", d_vertex->v[0] + d_vertex->ov[0], d_vertex->v[1] + d_vertex->ov[1],
				    d_vertex->v[2] + d_vertex->ov[2]);
}

static PyObject *vertex_offset3d(o_vertex *self)
{
	KuduVertex *d_vertex = self->d_vertex;
	if (d_vertex == NULL) RETURN_NONE

	return Py_BuildValue("fff", d_vertex->ov[0], d_vertex->ov[1], d_vertex->ov[2]);
}

static PyObject *vertex_set_pos3d(o_vertex *self, PyObject *args)
{
	KuduVertex *d_vertex = self->d_vertex;
	if (d_vertex == NULL) RETURN_FALSE

	float v[3];
	int a;

	if (!(PyArg_ParseTuple(args, "fff", &v[0], &v[1], &v[2]))) return NULL;

	for (a = 0; a < 3; a++) d_vertex->av[a] = v[a];

	RETURN_TRUE
}

static PyObject *vertex_set_offset3d(o_vertex *self, PyObject *args)
{
	KuduVertex *d_vertex = self->d_vertex;
	if (d_vertex == NULL) RETURN_FALSE

	float v[3];
	int a;

	if (!(PyArg_ParseTuple(args, "fff", &v[0], &v[1], &v[2]))) return NULL;

	for (a = 0; a < 3; a++) d_vertex->ov[a] = v[a];

	RETURN_TRUE
}

static PyObject *vertex_set_frame(o_vertex *self, PyObject *args)
{
	KuduVertex *d_vertex = self->d_vertex;
	KuduObject *d_object = self->d_object;

	if ((d_vertex == NULL) || (d_object == NULL) || (frame_list == NULL)) RETURN_FALSE

	int frame;

	if (!(PyArg_ParseTuple(args, "i", &frame))) return NULL;

	kudu_frame_list_set_frame_operation(frame_list, d_object, d_vertex, frame, OPERATION_VERTEX_SET);

	RETURN_TRUE
}

static PyObject *vertex_key_frames(o_vertex *self)
{
	KuduVertex *d_vertex = self->d_vertex;
	KuduObject *d_object = self->d_object;

	if ((d_vertex == NULL) || (d_object == NULL) || (frame_list == NULL)) RETURN_NONE
	if (frame_list->first_object == NULL) RETURN_NONE

	KuduObjectFrame *o_frame = NULL;
	KuduItemFrame *i_frame = NULL;
	KuduFrame *frame = NULL;
	PyObject *tuple;
	int count = 0;

	do {
		if (o_frame == NULL) o_frame = frame_list->first_object;
		else	o_frame = o_frame->next_object;
	} while ((o_frame->next_object != NULL) && (o_frame->object != d_object));
	if (o_frame->object != d_object) RETURN_NONE
	if (o_frame->first_item == NULL) RETURN_NONE

	do {
		if (i_frame == NULL) i_frame = o_frame->first_item;
		else	i_frame = i_frame->next_item;
	} while ((i_frame->next_item != NULL) && (i_frame->item != d_vertex));
	if (i_frame->item != d_vertex) RETURN_NONE
	if (i_frame->first_frame == NULL) RETURN_NONE

	do {
		if (frame == NULL) frame = i_frame->first_frame;
		else	frame = frame->next_frame;

		if (frame != NULL) {
			count++;
			if (count == 1) tuple = PyTuple_New(1);
			else	_PyTuple_Resize(&tuple, count);

			PyTuple_SetItem(tuple, count-1, Py_BuildValue("i", frame->frame));
		}

	} while (frame->next_frame != NULL);

	return tuple;
}

static PyObject *vertex_next(o_vertex *self)
{
	KuduVertex *d_vertex = self->d_vertex;
	if (d_vertex == NULL) RETURN_NONE
	if (d_vertex->next_vertex == NULL) RETURN_NONE

	o_vertex *next = (o_vertex*)vertex_new(&o_vertexType, NULL, NULL);
	next->d_vertex = d_vertex->next_vertex;
	next->d_object = self->d_object;

	return (PyObject*)next;
}

static PyObject *vertex_previous(o_vertex *self)
{
	KuduVertex *d_vertex = self->d_vertex;
	if (d_vertex == NULL) RETURN_NONE
	if (d_vertex->previous_vertex == NULL) RETURN_NONE

	o_vertex *prev = (o_vertex*)vertex_new(&o_vertexType, NULL, NULL);
	prev->d_vertex = d_vertex->previous_vertex;
	prev->d_object = self->d_object;

	return (PyObject*)prev;
}

static PyObject *vertex_edge(o_vertex *self)
{
	KuduVertex *d_vertex = self->d_vertex;
	if (d_vertex == NULL) RETURN_NONE
	if (d_vertex->edge == NULL) RETURN_NONE

	o_edge *edge = (o_edge*)edge_new(&o_edgeType, NULL, NULL);
	edge->d_edge = d_vertex->edge;
	edge->d_object = self->d_object;

	return (PyObject*)edge;
}


/************************************/
/*        o_shape methods           */
/************************************/
static PyObject *shape_id(o_shape *self)
{
	KuduShape *d_shape = self->d_shape;
	if (d_shape == NULL) return Py_BuildValue("i", -1);

	return Py_BuildValue("i", d_shape->id);
}

static PyObject *shape_new_vertex(o_shape *self, PyObject *args, PyObject *kwds)
{
	KuduShape *d_shape = self->d_shape;
	KuduObject *d_object = self->d_object;
	if ((d_shape == NULL) || (d_object == NULL)) RETURN_NONE

	o_vertex *vertex = (o_vertex*)vertex_new(&o_vertexType, NULL, NULL);
	KuduVertex *d_vertex = kudu_vertex_new(d_object, d_shape->vertex);
	if (d_shape->vertex == NULL) d_shape->vertex = d_vertex;

	vertex->d_vertex = d_vertex;
	vertex->d_object = d_object;

	return (PyObject*)vertex;
}

static PyObject *shape_new_edge(o_shape *self, PyObject *args, PyObject *kwds)
{
	KuduShape *d_shape = self->d_shape;
	KuduObject *d_object = self->d_object;
	if ((d_shape == NULL) || (d_object == NULL)) RETURN_NONE

	o_edge *edge = (o_edge*)edge_new(&o_edgeType, NULL, NULL);
	KuduEdge *d_edge = kudu_edge_new(d_object, d_shape->edge);
	if (d_shape->edge == NULL) d_shape->edge = d_edge;

	edge->d_edge = d_edge;
	edge->d_object = d_object;

	return (PyObject*)edge;
}

static PyObject *shape_new_face(o_shape *self, PyObject *args, PyObject *kwds)
{
	KuduShape *d_shape = self->d_shape;
	KuduObject *d_object = self->d_object;
	if ((d_shape == NULL) || (d_object == NULL)) RETURN_NONE

	o_face *face = (o_face*)face_new(&o_faceType, NULL, NULL);
	KuduFace *d_face = kudu_face_new(d_object, d_shape->face);
	if (d_shape->face == NULL) d_shape->face = d_face;

	face->d_face = d_face;
	face->d_object = d_object;

	return (PyObject*)face;
}




/**********************************/
/*  o_bone creation/init/dealloc  */
/**********************************/
static void bone_dealloc(o_bone *self)
{
	self->ob_type->tp_free((PyObject*)self);
}

static int bone_init(o_bone *self, PyObject *args, PyObject *kwds)
{
	self->d_bone = NULL;
	self->d_object = NULL;

	return 0;
}

static PyObject *bone_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
	o_bone *self;

	self = (o_bone*)type->tp_alloc(type, 0);

	return (PyObject*)self;
}

/*********************************/
/*  o_bone methods declarations  */
/*********************************/
static PyObject *bone_id(o_bone*);
static PyObject *bone_name(o_bone*);
static PyObject *bone_hAngle(o_bone*);
static PyObject *bone_vAngle(o_bone*);
static PyObject *bone_rAngle(o_bone*);
static PyObject *bone_length(o_bone*);
static PyObject *bone_parent(o_bone*);
static PyObject *bone_first_child(o_bone*);
static PyObject *bone_next_sibling(o_bone*);
static PyObject *bone_previous_sibling(o_bone*);
static PyObject *bone_set_hAngle(o_bone*, PyObject*);
static PyObject *bone_set_vAngle(o_bone*, PyObject*);
static PyObject *bone_set_rAngle(o_bone*, PyObject*);
static PyObject *bone_set_length(o_bone*, PyObject*);
static PyObject *bone_set_frame(o_bone*, PyObject*);
static PyObject *bone_clear_frame(o_bone*, PyObject*);
static PyObject *bone_set(o_bone*, PyObject*);
static PyObject *bone_children(o_bone*);
static PyObject *bone_key_frames(o_bone*);
static PyObject *bone_add_child(o_bone*);
static PyObject *bone_add_parent(o_bone*);
static PyObject *bone_destroy(o_bone*);

static PyMethodDef o_bone_methods[] = {
	{"id", (PyCFunction)bone_id, METH_NOARGS, "Return the bone ID"},
	{"name", (PyCFunction)bone_name, METH_NOARGS, "Return the bone's Name"},
	{"hAngle", (PyCFunction)bone_hAngle, METH_NOARGS, "Return the bone's hAngle"},
	{"vAngle", (PyCFunction)bone_vAngle, METH_NOARGS, "Return the bone's vAngle"},
	{"rAngle", (PyCFunction)bone_rAngle, METH_NOARGS, "Return the bone's rAngle"},
	{"length", (PyCFunction)bone_length, METH_NOARGS, "Return the bone's length"},
	{"parent", (PyCFunction)bone_parent, METH_NOARGS, "Return the bone's parent"},
	{"first_child", (PyCFunction)bone_first_child, METH_NOARGS, "Return the bone's first child"},
	{"next_sibling", (PyCFunction)bone_next_sibling, METH_NOARGS, "Return the bone's next sibling"},
	{"previous_sibling", (PyCFunction)bone_previous_sibling, METH_NOARGS, "Return the bone's previous sibling"},
	{"set_hAngle", (PyCFunction)bone_set_hAngle, METH_VARARGS, "Set the bone's hAngle"},
	{"set_vAngle", (PyCFunction)bone_set_vAngle, METH_VARARGS, "Set the bone's vAngle"},
	{"set_rAngle", (PyCFunction)bone_set_rAngle, METH_VARARGS, "Set the bone's rAngle"},
	{"set_length", (PyCFunction)bone_set_length, METH_VARARGS, "Set the bone's length"},
	{"set_frame", (PyCFunction)bone_set_frame, METH_VARARGS, "Set the frame for the bone"},
	{"clear_frame", (PyCFunction)bone_clear_frame, METH_VARARGS, "Clears the frame for the bone"},
	{"set", (PyCFunction)bone_set, METH_VARARGS, "Set's the h,v,r angles and length of the bone"},
	{"children", (PyCFunction)bone_children, METH_NOARGS, "Return all the bone's children in a tuple"},
	{"key_frames", (PyCFunction)bone_key_frames, METH_NOARGS, "Return all the bone's key frame numbers in a tuple"},
	{"add_child", (PyCFunction)bone_add_child, METH_NOARGS, "Create a new child bone"},
	{"add_parent", (PyCFunction)bone_add_parent, METH_NOARGS, "Create a new parent bone"},
	{"destroy", (PyCFunction)bone_destroy, METH_NOARGS, "Destroy a bone"},
	{NULL, NULL, 0, NULL},
};

static PyTypeObject o_boneType = {
	PyObject_HEAD_INIT(NULL)
	0, "objects.bone", sizeof(o_bone),
	0, (destructor)bone_dealloc, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, Py_TPFLAGS_DEFAULT,
	"K_Bone", 0, 0, 0, 0, 0, 0, o_bone_methods, 0, 0, 0, 0, 0, 0, 0, (initproc)bone_init, 0, bone_new,
};

int o_bone_update(o_bone *bone)
{
	if ((bone->d_bone == NULL) || (bone->d_object == NULL))return FALSE;

	if (update_bones) kudu_bone_update(bone->d_bone);
	else	return TRUE;

	KuduShape *shape;

	if (update_vertices) {
		if (kudu_shape_for_each_do(bone->d_object->skin)) {
			while ((shape = kudu_shape_next_do()) != NULL) {
					kudu_vertex_update_vertices(shape->vertex);

					if (update_normals) kudu_shape_normals_calculate(shape);
			}
		}
	}

	return TRUE;
}

/********************************/
/*	  o_bone methods	*/
/********************************/
static PyObject *bone_id(o_bone *self)
{
	KuduBone *d_bone = self->d_bone;
	if (d_bone == NULL) return Py_BuildValue("i", -1);

	return Py_BuildValue("i", d_bone->id);
}

static PyObject *bone_name(o_bone *self)
{
	KuduBone *d_bone = self->d_bone;
	if (d_bone == NULL) return Py_BuildValue("i", -1);

	return Py_BuildValue("s", d_bone->name);
}

static PyObject *bone_hAngle(o_bone *self)
{
	KuduBone *d_bone = self->d_bone;
	if (d_bone == NULL) return Py_BuildValue("i", -1);

	/*return Py_BuildValue("f", d_bone->hAngle);*/
}

static PyObject *bone_vAngle(o_bone *self)
{
	KuduBone *d_bone = self->d_bone;
	if (d_bone == NULL) return Py_BuildValue("i", -1);

	/*return Py_BuildValue("f", d_bone->vAngle);*/
}

static PyObject *bone_rAngle(o_bone *self)
{
	KuduBone *d_bone = self->d_bone;
	if (d_bone == NULL) return Py_BuildValue("i", -1);

	kudu_bone_update(d_bone);

	/*return Py_BuildValue("f", d_bone->rAngle);*/
}

static PyObject *bone_length(o_bone *self)
{
	KuduBone *d_bone = self->d_bone;
	if (d_bone == NULL) return Py_BuildValue("i", -1);

	return Py_BuildValue("f", d_bone->length);
}

static PyObject *bone_parent(o_bone *self)
{
	KuduBone *d_bone = self->d_bone;
	if (d_bone == NULL) RETURN_NONE
	if (d_bone->parent == NULL) RETURN_NONE

	o_bone *parent = (o_bone*)bone_new(&o_boneType, NULL, NULL);
	parent->d_bone = d_bone->parent;
	parent->d_object = self->d_object;

	return (PyObject*)parent;
}

static PyObject *bone_first_child(o_bone *self)
{
	KuduBone *d_bone = self->d_bone;
	if (d_bone == NULL) RETURN_NONE
	if (d_bone->first_child == NULL) RETURN_NONE

	o_bone *child = (o_bone*)bone_new(&o_boneType, NULL, NULL);
	child->d_bone = d_bone->first_child;
	child->d_object = self->d_object;

	return (PyObject*)child;
}

static PyObject *bone_next_sibling(o_bone *self)
{
	KuduBone *d_bone = self->d_bone;
	if (d_bone == NULL) RETURN_NONE
	if (d_bone->next_sibling == NULL) RETURN_NONE

	o_bone *next = (o_bone*)bone_new(&o_boneType, NULL, NULL);
	next->d_bone = d_bone->next_sibling;
	next->d_object = self->d_object;

	return (PyObject*)next;
}

static PyObject *bone_previous_sibling(o_bone *self)
{
	KuduBone *d_bone = self->d_bone;
	if (d_bone == NULL) RETURN_NONE
	if (d_bone->previous_sibling == NULL) RETURN_NONE

	o_bone *previous = (o_bone*)bone_new(&o_boneType, NULL, NULL);
	previous->d_bone = d_bone->previous_sibling;
	previous->d_object = self->d_object;

	return (PyObject*)previous;
}

static PyObject *bone_set_hAngle(o_bone *self, PyObject *args)
{
	KuduBone *d_bone = self->d_bone;
	float val;

	if ((d_bone == NULL) || (!(PyArg_ParseTuple(args, "f", &val))))
		RETURN_FALSE

	/*d_bone->hAngle = val;
	kudu_math_degrees_clamp(&d_bone->hAngle);*/
	o_bone_update(self);

	RETURN_TRUE
}

static PyObject *bone_set_vAngle(o_bone *self, PyObject *args)
{
	KuduBone *d_bone = self->d_bone;
	float val;

	if ((d_bone == NULL) || (!(PyArg_ParseTuple(args, "f", &val))))
		RETURN_FALSE

	/*d_bone->vAngle = val;
	kudu_math_degrees_clamp(&d_bone->vAngle);*/
	o_bone_update(self);

	RETURN_TRUE
}

static PyObject *bone_set_rAngle(o_bone *self, PyObject *args)
{
	KuduBone *d_bone = self->d_bone;
	float val;

	if ((d_bone == NULL) || (!(PyArg_ParseTuple(args, "f", &val))))
		RETURN_FALSE

	/*d_bone->rAngle = val;
	kudu_math_degrees_clamp(&d_bone->rAngle);*/
	o_bone_update(self);

	RETURN_TRUE
}

static PyObject *bone_set_length(o_bone *self, PyObject *args)
{
	KuduBone *d_bone = self->d_bone;
	float val;

	if ((d_bone == NULL) || (!(PyArg_ParseTuple(args, "f", &val))))
		RETURN_FALSE

	d_bone->length = val;
	if (d_bone->length < 0.0) d_bone->length = 0.0;
	o_bone_update(self);

	RETURN_TRUE
}

static PyObject *bone_set_frame(o_bone *self, PyObject *args)
{
	KuduBone *d_bone = self->d_bone;
	KuduObject *d_object = self->d_object;
	int frame;

	if ((d_bone == NULL) || (d_object == NULL) || (frame_list == NULL) ||
	   (!(PyArg_ParseTuple(args, "i", &frame)))) RETURN_FALSE

	kudu_frame_list_set_frame_operation(frame_list, d_object, d_bone, frame, OPERATION_BONE_SET);

	RETURN_TRUE
}

static PyObject *bone_clear_frame(o_bone *self, PyObject *args)
{
	KuduBone *d_bone = self->d_bone;
	KuduObject *d_object = self->d_object;
	int frame;

	if ((d_bone == NULL) || (d_object == NULL)) RETURN_FALSE
	if (!PyArg_ParseTuple(args, "i", &frame)) return NULL;

	kudu_frame_list_clear_frame(frame_list, d_object, d_bone, frame);

	RETURN_TRUE
}

static PyObject *bone_set(o_bone *self, PyObject *args)
{
	KuduBone *d_bone = self->d_bone;
	float ha, va, ra, l;

	if ((d_bone == NULL) || (!(PyArg_ParseTuple(args, "ffff", &ha, &va, &ra, &l)))) RETURN_FALSE

	/*d_bone->hAngle = ha;
	d_bone->vAngle = va;
	d_bone->rAngle = ra;*/
	d_bone->length = l;

	/*kudu_math_degrees_clamp(&d_bone->hAngle);
	kudu_math_degrees_clamp(&d_bone->vAngle);
	kudu_math_degrees_clamp(&d_bone->rAngle);*/
	if (d_bone->length < 0.0) d_bone->length = 0.0;
	o_bone_update(self);

	RETURN_TRUE
}

static PyObject *bone_children(o_bone *self)
{
	KuduBone *d_bone = self->d_bone;

	if (d_bone == NULL) RETURN_NONE
	if (d_bone->first_child == NULL) RETURN_NONE

	PyObject *tuple;
	o_bone *child;
	KuduBone *c_bone = NULL;
	int count = 0;

	do {
		if (c_bone == NULL) c_bone = d_bone->first_child;
		else	c_bone = c_bone->next_sibling;

		if (c_bone != NULL) {
			child = (o_bone*)bone_new(&o_boneType, NULL, NULL);
			child->d_bone = c_bone;
			child->d_object = self->d_object;

			count++;
			if (count == 1) tuple = PyTuple_New(1);
			else _PyTuple_Resize(&tuple, count);

			PyTuple_SetItem(tuple, count-1, (PyObject*)child);
		}

	} while (c_bone->next_sibling != NULL);

	return tuple;
}

static PyObject *bone_key_frames(o_bone *self)
{
	KuduBone *d_bone = self->d_bone;
	KuduObject *d_object = self->d_object;

	if ((d_bone == NULL) || (d_object == NULL) || (frame_list == NULL)) RETURN_NONE
	if (frame_list->first_object == NULL) RETURN_NONE

	KuduObjectFrame *o_frame = NULL;
	KuduItemFrame *i_frame = NULL;
	KuduFrame *frame = NULL;
	PyObject *tuple;
	int count = 0;

	do {
		if (o_frame == NULL) o_frame = frame_list->first_object;
		else	o_frame = o_frame->next_object;
	} while ((o_frame->next_object != NULL) && (o_frame->object != d_object));
	if (o_frame->object != d_object) RETURN_NONE
	if (o_frame->first_item == NULL) RETURN_NONE

	do {
		if (i_frame == NULL) i_frame = o_frame->first_item;
		else	i_frame = i_frame->next_item;
	} while ((i_frame->next_item != NULL) && (i_frame->item != d_bone));
	if (i_frame->item != d_bone) RETURN_NONE
	if (i_frame->first_frame == NULL) RETURN_NONE

	do {
		if (frame == NULL) frame = i_frame->first_frame;
		else	frame = frame->next_frame;

		if (frame != NULL) {
			count++;
			if (count == 1) tuple = PyTuple_New(1);
			else	_PyTuple_Resize(&tuple, count);

			PyTuple_SetItem(tuple, count-1, Py_BuildValue("i", frame->frame));
		}

	} while (frame->next_frame != NULL);

	return tuple;
}

static PyObject *bone_add_child(o_bone *self)
{
	KuduBone *d_bone = self->d_bone;
	KuduObject *d_object = self->d_object;
	if ((d_bone == NULL) || (d_object == NULL)) return Py_BuildValue("i", -1);

	KuduBone *new_bone = kudu_bone_add_child(d_bone);

	o_bone *r_bone = (o_bone*)bone_new(&o_boneType, NULL, NULL);
	r_bone->d_bone = new_bone;
	r_bone->d_object = d_object;

	return (PyObject*)r_bone;
}

static PyObject *bone_add_parent(o_bone *self)
{
	KuduBone *d_bone = self->d_bone;
	KuduObject *d_object = self->d_object;
	if ((d_bone == NULL) || (d_object == NULL)) return Py_BuildValue("i", -1);

	KuduBone *new_bone = kudu_bone_add_parent(d_bone);
 
	if (d_bone == d_object->bone) d_object->bone = new_bone;

	o_bone *r_bone = (o_bone*)bone_new(&o_boneType, NULL, NULL);
	r_bone->d_bone = new_bone;
	r_bone->d_object = d_object;

	return (PyObject*)r_bone;
}

static PyObject *bone_destroy(o_bone *self)
{
	KuduBone *d_bone = self->d_bone;
	KuduObject *d_object = self->d_object;
	if ((d_bone == NULL) || (d_object == NULL)) return Py_BuildValue("i", -1);

	KuduBone *new_bone = d_bone->parent;
	if (d_bone == d_object->bone) {
		if (d_bone->first_child != NULL) d_object->bone = d_bone->first_child;
		else	d_object->bone = d_bone->next_sibling;
	}

	if (d_bone->parent == NULL) {
		if (d_bone->first_child != NULL) new_bone = d_bone->first_child;
	}

	kudu_bone_destroy(d_bone);

	PyObject_Del(self);

	if (new_bone != NULL) {
		o_bone *r_bone = (o_bone*)bone_new(&o_boneType, NULL, NULL);
		r_bone->d_bone = new_bone;
		r_bone->d_object = d_object;

		return (PyObject*)r_bone;
	}

	RETURN_NONE
}


/**********************************/
/* o_object creation/init/dealloc */
/**********************************/
static void object_dealloc(o_object *self)
{
	self->ob_type->tp_free((PyObject*)self);
}

static int object_init(o_object *self, PyObject *args, PyObject *kwds)
{
	self->d_object = NULL;

	return 0;
}

static PyObject *object_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
	o_object *self;

	self = (o_object*)type->tp_alloc(type, 0);

	return (PyObject*)self;
}


/*********************************/
/* o_object methods declarations */
/*********************************/
static PyObject *object_next(o_object*);
static PyObject *object_previous(o_object*);
static PyObject *object_get_pos3(o_object*);
static PyObject *object_set_pos3(o_object*, PyObject*);
static PyObject *object_id(o_object*);
static PyObject *object_root_bone(o_object*);
static PyObject *object_set_frame(o_object*, PyObject*);
static PyObject *object_key_frames(o_object*);
static PyObject *object_new_shape(o_object*, PyObject*, PyObject*);

static PyMethodDef o_object_methods[] = {
	{"id", (PyCFunction)object_id, METH_NOARGS, "Return the object ID"},
	{"next", (PyCFunction)object_next, METH_NOARGS, "Return the next object if any"},
	{"previous", (PyCFunction)object_previous, METH_NOARGS, "Return the previous object if any"},
	{"position", (PyCFunction)object_get_pos3, METH_NOARGS, "Return the position of this object"},
	{"set_position", (PyCFunction)object_set_pos3, METH_VARARGS, "Set the position of this object"},
	{"root_bone", (PyCFunction)object_root_bone, METH_NOARGS, "Return the root bone"},
	{"set_frame", (PyCFunction)object_set_frame, METH_VARARGS, "Set a frame on this object"},
	{"key_frames", (PyCFunction)object_key_frames, METH_NOARGS, "Return the key frame numbers for this object"},
	{"new_shape", (PyCFunction)object_new_shape, METH_NOARGS, "Create a new shape for this object"},
	{NULL, NULL, 0, NULL},
};

static PyTypeObject o_objectType = {
	PyObject_HEAD_INIT(NULL)
	0, "objects.object", sizeof(o_object),
	0, (destructor)object_dealloc, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, Py_TPFLAGS_DEFAULT,
	"K_Object", 0, 0, 0, 0, 0, 0, o_object_methods, 0, 0, 0, 0, 0, 0, 0, (initproc)object_init, 0, object_new,
};



/********************************/
/*	o_object methods	*/
/********************************/
static PyObject *object_id(o_object *self)
{
	KuduObject *d_object = self->d_object;

	if (d_object == NULL) return Py_BuildValue("i", -1);
	else	return Py_BuildValue("i", d_object->id);
}

static PyObject *object_next(o_object *self)
{
	KuduObject *d_object = self->d_object, *nd_object;
	o_object *return_obj;

	if (d_object == NULL) RETURN_NONE
	if (d_object->next_object == NULL) RETURN_NONE

	nd_object = d_object->next_object;

	return_obj = (o_object*)object_new(&o_objectType, NULL, NULL);
	return_obj->d_object = nd_object;

	return (PyObject*)return_obj;
}

static PyObject *object_previous(o_object *self)
{
	KuduObject *d_object = self->d_object, *nd_object;
	o_object *return_obj;

	if (d_object == NULL) RETURN_NONE
	if (d_object->previous_object == NULL) RETURN_NONE

	nd_object = d_object->previous_object;

	return_obj = (o_object*)object_new(&o_objectType, NULL, NULL);
	return_obj->d_object = nd_object;

	return (PyObject*)return_obj;
}

static PyObject *object_get_pos3(o_object *self)
{
	KuduObject *d_object = self->d_object;
	if (d_object == NULL) RETURN_NONE

	return Py_BuildValue("fff", d_object->position[0], d_object->position[1], d_object->position[2]);
}

static PyObject *object_set_pos3(o_object *self, PyObject *args)
{
	KuduObject *d_object = self->d_object;

	float x,y,z;

	if ((d_object == NULL) || (!(PyArg_ParseTuple(args, "fff", &x, &y, &z)))) RETURN_FALSE

	d_object->position[0] = x;
	d_object->position[1] = y;
	d_object->position[2] = z;

	RETURN_TRUE
}

static PyObject *object_root_bone(o_object *self)
{
	KuduObject *d_object = self->d_object;
	if (d_object == NULL) RETURN_NONE

	KuduBone *d_bone = d_object->bone;
	if (d_bone == NULL) RETURN_NONE

	o_bone *return_bone;

	return_bone = (o_bone*)bone_new(&o_boneType, NULL, NULL);
	return_bone->d_bone = d_bone;
	return_bone->d_object = d_object;

	return (PyObject*)return_bone;
}

static PyObject *object_set_frame(o_object *self, PyObject *args)
{
	KuduObject *d_object = self->d_object;

	int frame;

	if ((d_object == NULL) | (!(PyArg_ParseTuple(args, "i", &frame)))) RETURN_FALSE

	kudu_frame_list_set_frame_operation(frame_list, d_object, d_object, frame, OPERATION_OBJECT_SET);

	RETURN_TRUE
}

static PyObject *object_key_frames(o_object *self)
{
	KuduObject *d_object = self->d_object;

	if ((d_object == NULL) || (frame_list == NULL)) RETURN_NONE
	if (frame_list->first_object == NULL) RETURN_NONE

	KuduObjectFrame *o_frame = NULL;
	KuduItemFrame *i_frame = NULL;
	KuduFrame *frame = NULL;
	PyObject *tuple;
	int count = 0;

	do {
		if (o_frame == NULL) o_frame = frame_list->first_object;
		else	o_frame = o_frame->next_object;
	} while ((o_frame->next_object != NULL) && (o_frame->object != d_object));
	if (o_frame->object != d_object) RETURN_NONE
	if (o_frame->first_item == NULL) RETURN_NONE

	do {
		if (i_frame == NULL) i_frame = o_frame->first_item;
		else	i_frame = i_frame->next_item;
	} while ((i_frame->next_item != NULL) && (i_frame->item != d_object));
	if (i_frame->item != d_object) RETURN_NONE
	if (i_frame->first_frame == NULL) RETURN_NONE

	do {
		if (frame == NULL) frame = i_frame->first_frame;
		else	frame = frame->next_frame;

		if (frame != NULL) {
			count++;
			if (count == 1) tuple = PyTuple_New(1);
			else	_PyTuple_Resize(&tuple, count);

			PyTuple_SetItem(tuple, count-1, Py_BuildValue("i", frame->frame));
		}

	} while (frame->next_frame != NULL);

	return tuple;
}

static PyObject *object_new_shape(o_object *self, PyObject *args, PyObject *kwds)
{
	KuduObject *d_object = self->d_object;
	if (d_object == NULL) RETURN_NONE

	o_shape *shape = (o_shape*)shape_new(&o_shapeType, NULL, NULL);
	KuduShape *d_shape = kudu_shape_new(d_object, d_object->skin);
	if (d_object->skin == NULL) d_object->skin = d_shape;

	shape->d_shape = d_shape;
	shape->d_object = d_object;

	return (PyObject*)shape;
}


/****************************************/
/* object module functions declarations	*/
/****************************************/
static PyObject *objects_first_object(PyObject*);
static PyObject *objects_selected_object(PyObject*);
static PyObject *objects_update_bones(PyObject*, PyObject*);
static PyObject *objects_update_vertices(PyObject*, PyObject*);
static PyObject *objects_update_normals(PyObject*, PyObject*);
static PyObject *objects_current_frame(PyObject*);
static PyObject *objects_goto_frame(PyObject*, PyObject*);
static PyObject *objects_selected_verts(PyObject*);
static PyObject *objects_selected_bones(PyObject*, PyObject*);

static PyMethodDef objects_methods[] = {
	{"first_object", (PyCFunction)objects_first_object, METH_NOARGS, "Return the first object (If it exists)."},
	{"selected_object", (PyCFunction)objects_selected_object, METH_NOARGS, "Return the currently selected object (If it exists)."},
	{"update_bones", objects_update_bones, METH_VARARGS, "Set the update_bone flag"},
	{"update_vertices", objects_update_vertices, METH_VARARGS, "Set the update_vertices flag"},
	{"update_normals", objects_update_normals, METH_VARARGS, "Set the update_normals flag"},
	{"current_frame", (PyCFunction)objects_current_frame, METH_NOARGS, "Return the current frame number"},
	{"goto_frame", objects_goto_frame, METH_VARARGS, "Hop to a frame"},
	{"selected_vertices", (PyCFunction)objects_selected_verts, METH_NOARGS, "Return the selection as vertices in a tuple"},
	{"selected_bones", objects_selected_bones, METH_NOARGS, "Return the selected bones in a tuple"},
	{NULL, NULL, 0, NULL}
};

/********************************/
/*   object module functions	*/
/********************************/
static PyObject *objects_first_object(PyObject *self)
{
	if (first_object == NULL) RETURN_NONE

	o_object *return_obj;

	return_obj = (o_object*)object_new(&o_objectType, NULL, NULL);
	return_obj->d_object = first_object;

	return (PyObject*)return_obj;
}

static PyObject *objects_selected_object(PyObject *self)
{
	if (program.selected_object == NULL) RETURN_NONE

	o_object *return_obj;

	return_obj = (o_object*)object_new(&o_objectType, NULL, NULL);
	return_obj->d_object = program.selected_object;

	return (PyObject*)return_obj;
}

static PyObject *objects_update_bones(PyObject *self, PyObject *args)
{
	int val;
	if (!(PyArg_ParseTuple(args, "i", &val))) return NULL;

	if (val) update_bones = TRUE;
	else	update_bones = FALSE;

	RETURN_TRUE
}

static PyObject *objects_update_vertices(PyObject *self, PyObject *args)
{
	int val;
	if (!(PyArg_ParseTuple(args, "i", &val))) return NULL;

	if (val) update_vertices = TRUE;
	else	update_vertices = FALSE;

	RETURN_TRUE
}

static PyObject *objects_update_normals(PyObject *self, PyObject *args)
{
	int val;
	if (!(PyArg_ParseTuple(args, "i", &val))) return NULL;

	if (val) update_normals = TRUE;
	else	update_normals = FALSE;

	RETURN_TRUE
}

static PyObject *objects_current_frame(PyObject *self)
{
	if (frame_list == NULL) return Py_BuildValue("i", -1);

	return Py_BuildValue("i", frame_list->current_frame);
}

static PyObject *objects_goto_frame(PyObject *self, PyObject *args)
{
	if (frame_list == NULL) RETURN_FALSE

	int frame;

	if (!(PyArg_ParseTuple(args, "i", &frame))) return NULL;

	kudu_frame_list_goto_frame(frame_list, frame);

	RETURN_TRUE
}

static PyObject *objects_selected_verts(PyObject *self)
{
	if (selection_list == NULL) RETURN_NONE

	KuduSelectionList *selection;
	KuduVertex *d_vertex;
	o_vertex *item;
	PyObject *tuple;
	int count = 0;

	if (selection_list->type == SELECT_POINTS) selection = selection_list;
	else	selection = kudu_selection_list_new_from_copy(selection_list);

	kudu_selection_list_set_type(selection, SELECT_POINTS, FALSE);

	if (!(kudu_selection_list_for_each_do(selection))) RETURN_NONE
	while ((d_vertex = (KuduVertex*)kudu_selection_list_next_do()) != NULL) {
		item = (o_vertex*)vertex_new(&o_vertexType, NULL, NULL);
		item->d_vertex = d_vertex;
		item->d_object = program.selected_object;

		count++;
		if (count == 1) tuple = PyTuple_New(1);
		else	_PyTuple_Resize(&tuple, count);

		PyTuple_SetItem(tuple, count-1, (PyObject*)item);
	}

	return tuple;
}

static PyObject *objects_selected_bones(PyObject *self, PyObject *args)
{
	if (bones_selection_list == NULL) RETURN_NONE

	KuduBone *d_bone;
	o_bone *bone;
	PyObject *tuple;
	int count = 0;

	if (bones_selection_list->type != SELECT_BONES) RETURN_NONE

	if (!kudu_selection_list_for_each_do(bones_selection_list)) RETURN_NONE
	while ((d_bone = (KuduBone*)kudu_selection_list_next_do()) != NULL) {
		bone = (o_bone*)bone_new(&o_boneType, NULL, NULL);
		bone->d_bone = d_bone;
		bone->d_object = program.selected_object;

		count++;
		if (count == 1) tuple = PyTuple_New(1);
		else	_PyTuple_Resize(&tuple, count);

		PyTuple_SetItem(tuple, count-1, (PyObject*)bone);
	}

	return tuple;
}

/****************************************/
/*	object module init function	*/
/****************************************/
int kudu_script_init_module_objects(KuduObject *first_obj, KuduFrameList *frames, KuduSelectionList *selection, KuduSelectionList *selected_bones)
{
	first_object = first_obj;
	frame_list = frames;
	selection_list = selection;
	bones_selection_list = selected_bones;

	update_bones = FALSE;
	update_vertices = FALSE;
	update_normals = FALSE;

/*	PyObject *obj;
	PyObject *bone;*/
	if (PyType_Ready(&o_objectType) < 0) return FALSE;
	if (PyType_Ready(&o_boneType) < 0) return FALSE;
	if (PyType_Ready(&o_vertexType) < 0) return FALSE;
	if (PyType_Ready(&o_faceType) < 0) return FALSE;
	if (PyType_Ready(&o_edgeType) < 0) return FALSE;
	if (PyType_Ready(&o_shapeType) < 0) return FALSE;

/*	obj = Py_InitModule3("objects", o_object_methods, "Objects");
	bone = Py_InitModule3("objects", o_bone_methods, "Bones");*/

	PyImport_AddModule("objects");
	Py_InitModule("objects", objects_methods);

/*	PyModule_AddObject(obj, "object", (PyObject *)&o_objectType);
	PyModule_AddObject(bone, "bone", (PyObject *)&o_boneType);*/

	return TRUE;
}






