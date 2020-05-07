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
#include "guimodule.h"

typedef struct {
	PyObject_HEAD
	GtkWidget *widget;
} g_widget;

/*****************************************/
/*  generic widget creation/destruction  */
/*****************************************/

/* Callback for "destroy" signal, sets g_widget->widget to NULL, solves the problem of widgets destroying children */
static gboolean widget_destroy(GtkWidget *widget, g_widget *pyobj)
{
	if (pyobj != NULL) pyobj->widget = NULL;

	return FALSE;
}

static PyObject *widget_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
	g_widget *self;
	int none = FALSE;

	self = (g_widget*)type->tp_alloc(type, 0);

	if (args != NULL) PyArg_ParseTuple(args, "|i", &none);

	if (none) self->widget = NULL;
	else
	if (strcmp(type->tp_name, "gui.dialog") == 0) self->widget = gtk_dialog_new();
	else
	if (strcmp(type->tp_name, "gui.button") == 0) self->widget = gtk_button_new();
	else
	if (strcmp(type->tp_name, "gui.label") == 0) self->widget = gtk_label_new("");
	else
	if (strcmp(type->tp_name, "gui.vbox") == 0) self->widget = gtk_vbox_new(TRUE, 0);
	else
	if (strcmp(type->tp_name, "gui.hbox") == 0) self->widget = gtk_hbox_new(TRUE, 0);
	else
	if (strcmp(type->tp_name, "gui.entry") == 0) self->widget = gtk_entry_new();
	else
	if (strcmp(type->tp_name, "gui.checkbox") == 0) self->widget = gtk_check_button_new();
	else
	if (strcmp(type->tp_name, "gui.radiobox") == 0) self->widget = gtk_radio_button_new(NULL);
	else
	if (strcmp(type->tp_name, "gui.spinbutton") == 0) self->widget = gtk_spin_button_new_with_range(0, 100, 1);

	if (self->widget != NULL)
		g_signal_connect(G_OBJECT(self->widget), "destroy", G_CALLBACK(widget_destroy), self);

	return (PyObject*)self;
}

static void widget_dealloc(g_widget *self)
{
	if ((self->widget != NULL) && (GTK_IS_WIDGET(self->widget))) gtk_widget_destroy(self->widget);
	self->ob_type->tp_free((PyObject*)self);
}

/*****************************/
/*   generic widget methods  */
/*****************************/
static PyObject *widget_show(g_widget *self, PyObject *args)
{
	if (self->widget == NULL) RETURN_FALSE

	gtk_widget_show(self->widget);

	RETURN_TRUE
}

static PyObject *widget_show_all(g_widget *self, PyObject *args)
{
	if (self->widget == NULL) RETURN_FALSE

	gtk_widget_show_all(self->widget);

	RETURN_TRUE
}

static PyObject *widget_hide(g_widget *self, PyObject *args)
{
	if (self->widget == NULL) RETURN_FALSE

	gtk_widget_hide(self->widget);

	RETURN_TRUE
}

static PyObject *widget_hide_all(g_widget *self, PyObject *args)
{
	if (self->widget == NULL) RETURN_FALSE

	gtk_widget_hide_all(self->widget);

	RETURN_TRUE
}

static PyObject *widget_activate(g_widget *self, PyObject *args)
{
	gtk_widget_activate(self->widget);

	RETURN_TRUE
}

static int widget_callback(GtkWidget *widget, PyObject *data)
{
	if (data == NULL) return FALSE;

	PyObject *arglist = NULL;
	PyObject *result = NULL;

	if (PyTuple_Size(data) < 3) {
		arglist = Py_BuildValue("(O)", PyTuple_GetItem(data, 0));
	} else {
		arglist = Py_BuildValue("(OO)", PyTuple_GetItem(data, 0), PyTuple_GetItem(data, 2));
	}

	Py_DECREF(PyEval_CallObject(PyTuple_GetItem(data, 1), arglist));

	Py_DECREF(arglist);

	return FALSE;
}

static PyObject *widget_connect_signal(g_widget *self, PyObject *args, PyObject *kwds)
{

	static char *kwlist[] = {"signal", "callback", "data", NULL};
	const char *signal;
	PyObject *data_tuple = NULL;
	PyObject *callback = NULL;
	PyObject *data = NULL;
	int id;

	if (PyArg_ParseTupleAndKeywords(args, kwds, "sO|O", kwlist, &signal, &callback, &data)) {
		if (!PyCallable_Check(callback)) return NULL;
	} else return NULL;

	if (data == NULL) {
		data_tuple = PyTuple_New(2);
		Py_INCREF(callback);
		PyTuple_SetItem(data_tuple, 0, (PyObject*)self);
		PyTuple_SetItem(data_tuple, 1, callback);
	} else {
		data_tuple = PyTuple_New(3);
		Py_INCREF(callback);
		Py_INCREF(data);
		PyTuple_SetItem(data_tuple, 0, (PyObject*)self);
		PyTuple_SetItem(data_tuple, 1, callback);
		PyTuple_SetItem(data_tuple, 2, data);
	}

	id = g_signal_connect(G_OBJECT(self->widget), signal, G_CALLBACK(widget_callback), data_tuple);

	return Py_BuildValue("i", id);
}

/*****************************************/
/* Header definition for generic widgets */
/*****************************************/

#define g_widget_method_HEAD \
	{"connect", (PyCFunction)widget_connect_signal, METH_VARARGS | METH_KEYWORDS, "Connect a signal to this widget"},\
	{"activate", (PyCFunction)widget_activate, METH_NOARGS, "Activate this widget"},\
	{"show", (PyCFunction)widget_show, METH_NOARGS, "Show this widget"},\
	{"hide", (PyCFunction)widget_hide, METH_NOARGS, "Hide this widget"},\
	{"show_all", (PyCFunction)widget_show_all, METH_NOARGS, "Show this widget and all its children"},\
	{"hide_all", (PyCFunction)widget_hide_all, METH_NOARGS, "Hide this widget and all its children"},


/*******************************/
/*  g_spinbutton methods/type  */
/*******************************/
static PyObject *gui_spinbutton_get_value(g_widget*, PyObject*);
static PyObject *gui_spinbutton_set_value(g_widget*, PyObject*);
static PyObject *gui_spinbutton_set(g_widget*, PyObject*, PyObject*);
static PyObject *gui_spinbutton_set_digits(g_widget*, PyObject*);

static PyMethodDef g_spinbutton_methods[] = {
	g_widget_method_HEAD
	{"value", (PyCFunction)gui_spinbutton_get_value, METH_NOARGS, "Return the current value"},
	{"set_value", (PyCFunction)gui_spinbutton_set_value, METH_VARARGS, "Set the current value"},
	{"set", (PyCFunction)gui_spinbutton_set, METH_VARARGS | METH_KEYWORDS, "Set the min, max, step & page"},
	{"set_digits", (PyCFunction)gui_spinbutton_set_digits, METH_VARARGS , "Set the displayed decimal precision"},
	{NULL, NULL, 0, NULL},
};

static PyTypeObject g_spinbuttonType = {
	PyObject_HEAD_INIT(NULL)
	0, "gui.spinbutton", sizeof(g_widget),
	0, (destructor)widget_dealloc, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, Py_TPFLAGS_DEFAULT,
	"G_SpinButton", 0, 0, 0, 0, 0, 0, g_spinbutton_methods, 0, 0, 0, 0, 0, 0, 0, 0, 0, widget_new,
};

/**********************************/
/*  g_spinbutton special methods  */
/**********************************/
static PyObject *gui_spinbutton_get_value(g_widget *self, PyObject *args)
{
	return Py_BuildValue("d", gtk_spin_button_get_value(GTK_SPIN_BUTTON(self->widget)));
}

static PyObject *gui_spinbutton_set_value(g_widget *self, PyObject *args)
{
	double value;

	if (!PyArg_ParseTuple(args, "d", &value)) return NULL;

	gtk_spin_button_set_value(GTK_SPIN_BUTTON(self->widget), value);

	RETURN_TRUE
}

static PyObject *gui_spinbutton_set(g_widget *self, PyObject *args, PyObject *kwds)
{
	double min, max, step, page;
	static char *kwlist[] = {"min", "max", "step", "page", NULL};

	gtk_spin_button_get_range(GTK_SPIN_BUTTON(self->widget), &min, &max);
	gtk_spin_button_get_increments(GTK_SPIN_BUTTON(self->widget), &step, &page);

	if (PyArg_ParseTupleAndKeywords(args, kwds, "|dddd", kwlist, &min, &max, &step, &page)) {
		gtk_spin_button_set_range(GTK_SPIN_BUTTON(self->widget), min, max);
		gtk_spin_button_set_increments(GTK_SPIN_BUTTON(self->widget), step, page);
	} else return NULL;

	RETURN_TRUE
}

static PyObject *gui_spinbutton_set_digits(g_widget *self, PyObject *args)
{
	int digits;

	if (!PyArg_ParseTuple(args, "i", &digits)) return NULL;

	gtk_spin_button_set_digits(GTK_SPIN_BUTTON(self->widget), digits);

	RETURN_TRUE
}

/**************************/
/*  g_entry methods/type  */
/**************************/
static PyObject *gui_entry_get_text(g_widget*, PyObject*);
static PyObject *gui_entry_set_text(g_widget*, PyObject*);

static PyMethodDef g_entry_methods[] = {
	g_widget_method_HEAD
	{"text", (PyCFunction)gui_entry_get_text, METH_NOARGS, "Return current entry text"},
	{"set_text", (PyCFunction)gui_entry_set_text, METH_VARARGS, "Set current entry text"},
	{NULL, NULL, 0, NULL},
};

static PyTypeObject g_entryType = {
	PyObject_HEAD_INIT(NULL)
	0, "gui.entry", sizeof(g_widget),
	0, (destructor)widget_dealloc, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, Py_TPFLAGS_DEFAULT,
	"G_Entry", 0, 0, 0, 0, 0, 0, g_entry_methods, 0, 0, 0, 0, 0, 0, 0, 0, 0, widget_new,
};

/*****************************/
/*  g_entry special methods  */
/*****************************/
static PyObject *gui_entry_get_text(g_widget *self, PyObject *args)
{
	return Py_BuildValue("s", gtk_entry_get_text(GTK_ENTRY(self->widget)));
}

static PyObject *gui_entry_set_text(g_widget *self, PyObject *args)
{
	const char *text;

	if (!PyArg_ParseTuple(args, "s", &text)) return NULL;

	gtk_entry_set_text(GTK_ENTRY(self->widget), text);

	RETURN_TRUE
}



/*****************************/
/*  g_radiobox methods/type  */
/*****************************/
static PyObject *gui_check_box_active(g_widget*, PyObject*);
static PyObject *gui_check_box_set_active(g_widget*, PyObject*);

static PyMethodDef g_radiobox_methods[] = {
	g_widget_method_HEAD
	{"active", (PyCFunction)gui_check_box_active, METH_NOARGS, "Return the activated status of the check"},
	{"set_active", (PyCFunction)gui_check_box_set_active, METH_VARARGS, "Sets the activated status of the check"},
	{NULL, NULL, 0, NULL},
};

static PyTypeObject g_radioboxType = {
	PyObject_HEAD_INIT(NULL)
	0, "gui.radiobox", sizeof(g_widget),
	0, (destructor)widget_dealloc, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, Py_TPFLAGS_DEFAULT,
	"G_RadioBox", 0, 0, 0, 0, 0, 0, g_radiobox_methods, 0, 0, 0, 0, 0, 0, 0, 0, 0, widget_new,
};

/*******************************************/
/*  g_radiobox/g_checkbox special methods  */
/*******************************************/
static PyObject *gui_check_box_active(g_widget *self, PyObject *args)
{
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(self->widget))) RETURN_TRUE

	RETURN_FALSE
}

static PyObject *gui_check_box_set_active(g_widget *self, PyObject *args)
{
	int state;

	if (!PyArg_ParseTuple(args, "i", &state)) return NULL;

	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(self->widget), state);

	RETURN_TRUE
}


/*****************************/
/*  g_checkbox methods/type  */
/*****************************/

static PyMethodDef g_checkbox_methods[] = {
	g_widget_method_HEAD
	{"active", (PyCFunction)gui_check_box_active, METH_NOARGS, "Return the activated status of the check"},
	{"set_active", (PyCFunction)gui_check_box_set_active, METH_VARARGS, "Sets the activated status of the check"},
	{NULL, NULL, 0, NULL},
};

static PyTypeObject g_checkboxType = {
	PyObject_HEAD_INIT(NULL)
	0, "gui.checkbox", sizeof(g_widget),
	0, (destructor)widget_dealloc, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, Py_TPFLAGS_DEFAULT,
	"G_CheckBox", 0, 0, 0, 0, 0, 0, g_checkbox_methods, 0, 0, 0, 0, 0, 0, 0, 0, 0, widget_new,
};


/**********************************/
/*   g_vbox/g_hbox methods/type   */
/**********************************/
static PyObject *gui_box_pack_start(g_widget*, PyObject*, PyObject*);
static PyObject *gui_box_pack_end(g_widget*, PyObject*, PyObject*);
static PyObject *gui_box_get_homogeneous(g_widget*, PyObject*);
static PyObject *gui_box_set_homogeneous(g_widget*, PyObject*);
static PyObject *gui_box_get_spacing(g_widget*, PyObject*);
static PyObject *gui_box_set_spacing(g_widget*, PyObject*);

static PyMethodDef g_box_methods[] = {
	g_widget_method_HEAD
	{"pack_start", (PyCFunction)gui_box_pack_start, METH_VARARGS | METH_KEYWORDS, "Pack a widget in the box"},
	{"pack_end", (PyCFunction)gui_box_pack_end, METH_VARARGS | METH_KEYWORDS, "Pack a widget in the box"},
	{"homogeneous", (PyCFunction)gui_box_get_homogeneous, METH_NOARGS, "Return the state of the homogeneous flag"},
	{"set_homogeneous", (PyCFunction)gui_box_set_homogeneous, METH_VARARGS, "Set the state of the homogeneous flag"},
	{"spacing", (PyCFunction)gui_box_get_spacing, METH_NOARGS, "Return the amount of spacing"},
	{"set_spacing", (PyCFunction)gui_box_set_spacing, METH_VARARGS, "Set the amount of spacing"},
	{NULL, NULL, 0, NULL},
};

static PyTypeObject g_vboxType = {
	PyObject_HEAD_INIT(NULL)
	0, "gui.vbox", sizeof(g_widget),
	0, (destructor)widget_dealloc, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, Py_TPFLAGS_DEFAULT,
	"G_VBox", 0, 0, 0, 0, 0, 0, g_box_methods, 0, 0, 0, 0, 0, 0, 0, 0, 0, widget_new,
};

static PyTypeObject g_hboxType = {
	PyObject_HEAD_INIT(NULL)
	0, "gui.hbox", sizeof(g_widget),
	0, (destructor)widget_dealloc, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, Py_TPFLAGS_DEFAULT,
	"G_HBox", 0, 0, 0, 0, 0, 0, g_box_methods, 0, 0, 0, 0, 0, 0, 0, 0, 0, widget_new,
};

/*************************************/
/*   g_vbox/g_hbox special methods   */
/*************************************/
static PyObject *gui_box_pack_start(g_widget *self, PyObject *args, PyObject *kwds)
{
	g_widget *pack;
	int expand = FALSE, fill = FALSE, padding = 0;
	static char *kwlist[] = {"item", "expand", "fill", "padding", NULL};

	if (!(PyArg_ParseTupleAndKeywords(args, kwds, "O|iii", kwlist, &pack, &expand, &fill, &padding))) return NULL;

	gtk_box_pack_start(GTK_BOX(self->widget), pack->widget, (gboolean)expand, (gboolean)fill, (guint)padding);

	Py_INCREF(pack);

	RETURN_TRUE
}

static PyObject *gui_box_pack_end(g_widget *self, PyObject *args, PyObject *kwds)
{
	g_widget *pack;
	int expand = FALSE, fill = FALSE, padding = 0;
	static char *kwlist[] = {"item", "expand", "fill", "padding", NULL};

	if (!(PyArg_ParseTupleAndKeywords(args, kwds, "O|iii", kwlist, &pack, &expand, &fill, &padding))) return NULL;

	gtk_box_pack_end(GTK_BOX(self->widget), pack->widget, (gboolean)expand, (gboolean)fill, (guint)padding);

	Py_INCREF(pack);

	RETURN_TRUE
}

static PyObject *gui_box_get_homogeneous(g_widget *self, PyObject *args)
{
	int state;

	state = gtk_box_get_homogeneous(GTK_BOX(self->widget));

	if (state) RETURN_TRUE

	RETURN_FALSE
}

static PyObject *gui_box_set_homogeneous(g_widget *self, PyObject *args)
{
	int state;

	if (!PyArg_ParseTuple(args, "i", &state)) return NULL;

	gtk_box_set_homogeneous(GTK_BOX(self->widget), (gboolean)state);

	RETURN_TRUE
}

static PyObject *gui_box_get_spacing(g_widget *self, PyObject *args)
{
	int spacing;

	spacing = gtk_box_get_spacing(GTK_BOX(self->widget));

	return Py_BuildValue("i", spacing);
}

static PyObject *gui_box_set_spacing(g_widget *self, PyObject *args)
{
	int spacing;

	if (!PyArg_ParseTuple(args, "i", &spacing)) return NULL;

	gtk_box_set_spacing(GTK_BOX(self->widget), spacing);

	RETURN_TRUE
}


/***************************/
/*  g_label  methods/type  */
/***************************/
static PyObject *gui_label_set_text(g_widget*, PyObject*);
static PyObject *gui_label_get_text(g_widget*, PyObject*);

static PyMethodDef g_label_methods[] = {
	g_widget_method_HEAD
	{"set_text", (PyCFunction)gui_label_set_text, METH_VARARGS, "Set the label text"},
	{"get_text", (PyCFunction)gui_label_get_text, METH_NOARGS, "Return the current label text"},
	{NULL, NULL, 0, NULL},
};

static PyTypeObject g_labelType = {
	PyObject_HEAD_INIT(NULL)
	0, "gui.label", sizeof(g_widget),
	0, (destructor)widget_dealloc, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, Py_TPFLAGS_DEFAULT,
	"G_Label", 0, 0, 0, 0, 0, 0, g_label_methods, 0, 0, 0, 0, 0, 0, 0, 0, 0, widget_new,
};
/*****************************/
/*  g_label special methods  */
/*****************************/
static PyObject *gui_label_set_text(g_widget *self, PyObject *args)
{
	const char *text;

	if (!(PyArg_ParseTuple(args, "s", &text))) return NULL;

	gtk_label_set_text(GTK_LABEL(self->widget), text);

	RETURN_TRUE
}

static PyObject *gui_label_get_text(g_widget *self, PyObject *args)
{
	const char *text;

	text = gtk_label_get_text(GTK_LABEL(self->widget));

	return Py_BuildValue("s", text);
}


/***************************/
/*  g_button methods/type  */
/***************************/
static PyObject *gui_button_set_label(g_widget*, PyObject*);

static PyMethodDef g_button_methods[] = {
	g_widget_method_HEAD
	{"set_label", (PyCFunction)gui_button_set_label, METH_NOARGS, "Set the button label"},
	{NULL, NULL, 0, NULL},
};

static PyTypeObject g_buttonType = {
	PyObject_HEAD_INIT(NULL)
	0, "gui.button", sizeof(g_widget),
	0, (destructor)widget_dealloc, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, Py_TPFLAGS_DEFAULT,
	"G_Button", 0, 0, 0, 0, 0, 0, g_button_methods, 0, 0, 0, 0, 0, 0, 0, 0, 0, widget_new,
};

/******************************/
/*  g_button special methods  */
/******************************/
static PyObject *gui_button_set_label(g_widget *self, PyObject *args)
{
	const char *text;

	if (!(PyArg_ParseTuple(args, "s", &text))) return NULL;

	gtk_button_set_label(GTK_BUTTON(self->widget), text);

	RETURN_TRUE
}


/***************************/
/*  g_dialog methods/type  */
/***************************/
static PyObject *gui_dialog_get_vbox(g_widget*, PyObject*);
static PyObject *gui_dialog_run(g_widget*, PyObject*);
static PyObject *gui_dialog_add_button(g_widget*, PyObject*);
static PyObject *gui_dialog_has_separator(g_widget*, PyObject*);
static PyObject *gui_dialog_set_has_separator(g_widget*, PyObject*);
static PyObject *gui_dialog_set_title(g_widget*, PyObject*);

static PyMethodDef g_dialog_methods[] = {
	g_widget_method_HEAD
	{"vbox", (PyCFunction)gui_dialog_get_vbox, METH_NOARGS, "Return the dialog's vbox object"},
	{"run", (PyCFunction)gui_dialog_run, METH_NOARGS, "Run the dialog"},
	{"add_button", (PyCFunction)gui_dialog_add_button, METH_VARARGS, "Add a button with a response id to the dialog"},
	{"has_separator", (PyCFunction)gui_dialog_has_separator, METH_NOARGS, "Returns true if the dialog has a separator"},
	{"set_separator", (PyCFunction)gui_dialog_set_has_separator, METH_VARARGS, "Set whether the dialog has a separator"},
	{"set_title", (PyCFunction)gui_dialog_set_title, METH_VARARGS, "Set the dialogs title"},
	{NULL, NULL, 0, NULL},
};

static PyTypeObject g_dialogType = {
	PyObject_HEAD_INIT(NULL)
	0, "gui.dialog", sizeof(g_widget),
	0, (destructor)widget_dealloc, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, Py_TPFLAGS_DEFAULT,
	"G_Dialog", 0, 0, 0, 0, 0, 0, g_dialog_methods, 0, 0, 0, 0, 0, 0, 0, 0, 0, widget_new,
};

/******************************/
/*  g_dialog special methods  */
/******************************/
static PyObject *gui_dialog_get_vbox(g_widget *self, PyObject *args)
{
	g_widget *rvbox;
	GtkWidget *dialog = self->widget;
	GtkWidget *vbox = GTK_DIALOG(dialog)->vbox;

	rvbox = (g_widget*)widget_new(&g_vboxType, Py_BuildValue("(i)", TRUE), NULL);
	rvbox->widget = vbox;

	Py_INCREF(rvbox);

	return (PyObject*)rvbox;
}

static PyObject *gui_dialog_run(g_widget *self, PyObject *args)
{
	return Py_BuildValue("i", gtk_dialog_run(GTK_DIALOG(self->widget)));
}

static PyObject *gui_dialog_add_button(g_widget *self, PyObject *args)
{
	const char *text;
	int response_id;
	g_widget *button = NULL;

	if (!PyArg_ParseTuple(args, "si", &text, &response_id)) return NULL;

	button = (g_widget*)widget_new(&g_buttonType, Py_BuildValue("(i)", TRUE), NULL);
	button->widget = gtk_dialog_add_button(GTK_DIALOG(self->widget), (char*)text, response_id);
	Py_INCREF(button);

	return (PyObject*)button;
}

static PyObject *gui_dialog_has_separator(g_widget *self, PyObject *args)
{
	if (gtk_dialog_get_has_separator(GTK_DIALOG(self->widget))) RETURN_TRUE

	RETURN_FALSE
}

static PyObject *gui_dialog_set_has_separator(g_widget *self, PyObject *args)
{
	int state;

	if (!PyArg_ParseTuple(args, "i", &state)) return NULL;

	gtk_dialog_set_has_separator(GTK_DIALOG(self->widget), (gboolean)state);

	RETURN_TRUE
}

static PyObject *gui_dialog_set_title(g_widget *self, PyObject *args)
{
	const char *title;

	if (!PyArg_ParseTuple(args, "s", &title)) return NULL;

	gtk_window_set_title(GTK_WINDOW(self->widget), title);

	RETURN_TRUE
}

/******************************/
/*  gui methods declarations  */
/******************************/
static PyObject *gui_message(PyObject*, PyObject*);
static PyObject *gui_confirm(PyObject*, PyObject*);
static PyObject *gui_dialog_new(PyObject*);
static PyObject *gui_button_new(PyObject*, PyObject*);
static PyObject *gui_label_new(PyObject*, PyObject*);
static PyObject *gui_vbox_new(PyObject*, PyObject*);
static PyObject *gui_hbox_new(PyObject*, PyObject*);
static PyObject *gui_checkbox_new(PyObject*, PyObject*);
static PyObject *gui_radiobox_new(PyObject*, PyObject*, PyObject*);
static PyObject *gui_entry_new(PyObject*, PyObject*);
static PyObject *gui_spinbutton_new(PyObject*, PyObject*, PyObject *kwds);

static PyMethodDef gui_methods[] = {
	{"message", gui_message, METH_VARARGS, "Popup a gui message dialog"},
	{"confirm", gui_confirm, METH_VARARGS, "Popup a gui confirmation dialog"},

	{"dialog", (PyCFunction)gui_dialog_new, METH_NOARGS, "Create a new dialog"},
	{"button", (PyCFunction)gui_button_new, METH_VARARGS, "Create a new button"},
	{"label", (PyCFunction)gui_label_new, METH_VARARGS, "Create a new label"},
	{"vbox", (PyCFunction)gui_vbox_new, METH_NOARGS, "Create a new vbox"},
	{"hbox", (PyCFunction)gui_hbox_new, METH_NOARGS, "Create a new hbox"},
	{"checkbox", (PyCFunction)gui_checkbox_new, METH_VARARGS, "Create a new checkbox"},
	{"radiobox", (PyCFunction)gui_radiobox_new, METH_VARARGS | METH_KEYWORDS, "Create a new radiobox"},
	{"entry", (PyCFunction)gui_entry_new, METH_NOARGS, "Create a new entry"},
	{"spinbutton", (PyCFunction)gui_spinbutton_new, METH_VARARGS | METH_KEYWORDS, "Create a new spinbutton"},
	{NULL, NULL, 0, NULL}
};

/******************************/
/*         gui methods        */
/******************************/
static PyObject *gui_message(PyObject *self, PyObject *args)
{
	const char *message;
	int type;

	if (!PyArg_ParseTuple(args, "is", &type, &message)) return NULL;

	kudu_gui_message((char*)message, type);

	RETURN_TRUE
}

static PyObject *gui_confirm(PyObject *self, PyObject *args)
{
	const char *message;
	int type, rv;

	if (!PyArg_ParseTuple(args, "is", &type, &message)) return NULL;

	rv = kudu_gui_confirm((char*)message, type);

	if (rv == GTK_RESPONSE_OK) rv = TRUE;
	else	rv = FALSE;

	return Py_BuildValue("i", rv);
}

static PyObject *gui_dialog_new(PyObject *self)
{
	g_widget *dialog;

	dialog = (g_widget*)widget_new(&g_dialogType, NULL, NULL);

	return (PyObject*)dialog;
}

static PyObject *gui_button_new(PyObject *self, PyObject *args)
{
	g_widget *button;
	const char *text;

	button = (g_widget*)widget_new(&g_buttonType, NULL, NULL);

	if (PyArg_ParseTuple(args, "|s", &text)) gtk_button_set_label(GTK_BUTTON(button->widget), text);

	return (PyObject*)button;
}

static PyObject *gui_label_new(PyObject *self, PyObject *args)
{
	g_widget *label;
	const char *text;

	label = (g_widget*)widget_new(&g_labelType, NULL, NULL);

	if (PyArg_ParseTuple(args, "|s", &text)) gtk_label_set_text(GTK_LABEL(label->widget), text);

	return (PyObject*)label;
}

static PyObject *gui_vbox_new(PyObject *self, PyObject *args)
{
	g_widget *vbox;

	vbox = (g_widget*)widget_new(&g_vboxType, NULL, NULL);

	return (PyObject*)vbox;
}

static PyObject *gui_hbox_new(PyObject *self, PyObject *args)
{
	g_widget *hbox;

	hbox = (g_widget*)widget_new(&g_hboxType, NULL, NULL);

	return (PyObject*)hbox;
}

static PyObject *gui_checkbox_new(PyObject *self, PyObject *args)
{
	g_widget *checkbox;
	const char *text;

	checkbox = (g_widget*)widget_new(&g_checkboxType, NULL, NULL);

	if (PyArg_ParseTuple(args, "|s", &text)) gtk_button_set_label(GTK_BUTTON(checkbox->widget), text);

	return (PyObject*)checkbox;
}

static PyObject *gui_radiobox_new(PyObject *self, PyObject *args, PyObject *kwds)
{
	g_widget *radiobox, *radio_pair = NULL;
	static char *kwlist[] = {"label", "radio_pair", NULL};
	const char *text = NULL;

	PyArg_ParseTupleAndKeywords(args, kwds, "|sO!", kwlist, &text, &g_radioboxType, &radio_pair);

	radiobox = (g_widget*)widget_new(&g_radioboxType, NULL, NULL);

	if (text != NULL) gtk_button_set_label(GTK_BUTTON(radiobox->widget), text);
	if (radio_pair != NULL) gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobox->widget),
			 gtk_radio_button_get_group(GTK_RADIO_BUTTON(radio_pair->widget)));

	return (PyObject*)radiobox;
}

static PyObject *gui_entry_new(PyObject *self, PyObject *args)
{
	g_widget *entry;

	entry = (g_widget*)widget_new(&g_entryType, NULL, NULL);

	return (PyObject*)entry;
}

static PyObject *gui_spinbutton_new(PyObject *self, PyObject *args, PyObject *kwds)
{
	g_widget *spin;
	double min, max, step, page;
	static char *kwlist[] = {"min", "max", "step", "page", NULL};

	spin = (g_widget*)widget_new(&g_spinbuttonType, NULL, NULL);

	gtk_spin_button_get_range(GTK_SPIN_BUTTON(spin->widget), &min, &max);
	gtk_spin_button_get_increments(GTK_SPIN_BUTTON(spin->widget), &step, &page);

	if (PyArg_ParseTupleAndKeywords(args, kwds, "|dddd", kwlist, &min, &max, &step, &page)) {
	
		gtk_spin_button_set_range(GTK_SPIN_BUTTON(spin->widget), min, max);
		gtk_spin_button_set_increments(GTK_SPIN_BUTTON(spin->widget), step, page);
	}
	

	return (PyObject*)spin;
}


/******************************/
/*     gui module init        */
/******************************/
int kudu_script_init_module_gui(void)
{
	PyObject *gui_module = PyImport_AddModule("gui");

	if (PyType_Ready(&g_dialogType) < 0) return FALSE;
	if (PyType_Ready(&g_buttonType) < 0) return FALSE;
	if (PyType_Ready(&g_labelType) < 0) return FALSE;
	if (PyType_Ready(&g_vboxType) < 0) return FALSE;
	if (PyType_Ready(&g_hboxType) < 0) return FALSE;
	if (PyType_Ready(&g_checkboxType) < 0) return FALSE;
	if (PyType_Ready(&g_radioboxType) < 0) return FALSE;
	if (PyType_Ready(&g_entryType) < 0) return FALSE;
	if (PyType_Ready(&g_spinbuttonType) < 0) return FALSE;


	PyModule_AddIntConstant(gui_module, "MESSAGE_INFO", GTK_MESSAGE_INFO);
	PyModule_AddIntConstant(gui_module, "MESSAGE_WARNING", GTK_MESSAGE_WARNING);
	PyModule_AddIntConstant(gui_module, "MESSAGE_QUESTION", GTK_MESSAGE_QUESTION);
	PyModule_AddIntConstant(gui_module, "MESSAGE_ERROR", GTK_MESSAGE_ERROR);

	PyModule_AddIntConstant(gui_module, "RESPONSE_NONE", GTK_RESPONSE_NONE);
	PyModule_AddIntConstant(gui_module, "RESPONSE_REJECT", GTK_RESPONSE_REJECT);
	PyModule_AddIntConstant(gui_module, "RESPONSE_ACCEPT", GTK_RESPONSE_ACCEPT);
	PyModule_AddIntConstant(gui_module, "RESPONSE_OK", GTK_RESPONSE_OK);
	PyModule_AddIntConstant(gui_module, "RESPONSE_CANCEL", GTK_RESPONSE_CANCEL);
	PyModule_AddIntConstant(gui_module, "RESPONSE_CLOSE", GTK_RESPONSE_CLOSE);
	PyModule_AddIntConstant(gui_module, "RESPONSE_YES", GTK_RESPONSE_YES);
	PyModule_AddIntConstant(gui_module, "RESPONSE_NO", GTK_RESPONSE_NO);
	PyModule_AddIntConstant(gui_module, "RESPONSE_APPLY", GTK_RESPONSE_APPLY);
	PyModule_AddIntConstant(gui_module, "RESPONSE_HELP", GTK_RESPONSE_HELP);

	Py_InitModule("gui", gui_methods);

	return TRUE;
}

