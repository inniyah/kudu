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
#include "gui_materials.h"

static KuduObject *working_object;
static KuduMaterial **selected_material;
static KuduMaterial **root_material;
static KuduMaterial *picked_material = NULL;
static gulong name_change;

enum {
	MATERIAL_NAME,
	MATERIAL_ID,
	MATERIAL_COLUMNS
};

struct mat_list {
	GtkListStore *list_store;
	GtkTreeSelection *list_selection;
	GtkWidget *list_view;
};

KuduMaterial *kudu_gui_materials_edit_get_picked_material(void)
{
	return picked_material;
}

static gboolean kudu_gui_materials_edit_reshape(GtkWidget *widget, GdkEventConfigure *event, gpointer data)
{
	GdkGLContext *glContext = gtk_widget_get_gl_context(widget);
	GdkGLDrawable *glDrawable = gtk_widget_get_gl_drawable(widget);

	int w = widget->allocation.width;
	int h = widget->allocation.height;

	if (!gdk_gl_drawable_gl_begin(glDrawable, glContext)) return FALSE;

	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (GLfloat)w / (GLfloat)h, 0.1, 10.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gdk_gl_drawable_gl_end(glDrawable);
	return TRUE;
}

int kudu_gui_materials_edit_init(GtkWidget *widget, gpointer data)
{
	GdkGLContext *glContext = gtk_widget_get_gl_context(widget);
	GdkGLDrawable *glDrawable = gtk_widget_get_gl_drawable(widget);

	if (!gdk_gl_drawable_gl_begin(glDrawable, glContext)) return;

	GLfloat light_pos[] = {5.0, 0.0, 0.0, 1.0};
	GLfloat white_light[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat lmodel_ambient[] = {0.1, 0.1, 0.1, 1.0};

	glShadeModel(GL_SMOOTH);
	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, white_light);
	glLightfv(GL_LIGHT0, GL_SPECULAR, white_light);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(0.6, 0.6, 0.6, 1.0);

	gdk_gl_drawable_gl_end(glDrawable);
}

int kudu_gui_materials_edit_display(GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
	KuduMaterial *material;
	material = *selected_material;
	if (material == NULL) return FALSE;

	GdkGLContext *glContext = gtk_widget_get_gl_context(widget);
	GdkGLDrawable *glDrawable = gtk_widget_get_gl_drawable(widget);

	if (!gdk_gl_drawable_gl_begin(glDrawable, glContext)) return FALSE;

	glLoadIdentity();
	gluLookAt(0.0, 0.0, -10.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	glClear(GL_COLOR_BUFFER_BIT);

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, material->ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, material->diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, material->specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, material->emission);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, material->shininess);


	gdk_gl_draw_sphere(TRUE, 4.0, 32, 32);

	glFlush();

	gdk_gl_drawable_gl_end(glDrawable);
	return TRUE;
}

int kudu_gui_materials_edit_select_material(GtkTreeSelection *tree_selection, gpointer data)
{
	KuduMaterial *material;
	material = *selected_material;
	if (material == NULL) return FALSE;

	GtkTreeIter iter;
	GtkTreeModel *model;
	int material_id;

	if (gtk_tree_selection_get_selected(tree_selection, &model, &iter)) {
		gtk_tree_model_get(model, &iter, MATERIAL_ID, &material_id, -1);
		material = kudu_material_find_with_id(material, material_id);

		if (material != NULL) {
			*selected_material = material;
			picked_material = material;
		}
	}

	return TRUE;
}

int kudu_gui_materials_edit_redisplay(GtkWidget *widget, GtkWidget *refresh)
{
	int w, h;

	w = refresh->allocation.width;
	h = refresh->allocation.height;

	gtk_widget_queue_draw_area(refresh, 0, 0, w, h);
}

int kudu_gui_materials_edit_material(GtkWidget *widget, gpointer in_value)
{
	KuduMaterial *material;
	material = *selected_material;
	if (material == NULL) return FALSE;

	GLfloat alpha;
	GdkColor colour;

	int value = (int)in_value;

	if (value < 3) alpha = (GLfloat)gtk_range_get_value(GTK_RANGE(widget));
	else	if (value > 5) gtk_color_button_get_color(GTK_COLOR_BUTTON(widget), &colour);

	switch (value) {
		case 1:
			material->diffuse[3] = alpha;
			break;
		case 2:
			material->shininess = alpha;
			break;
		case 3:
			sprintf(material->name, "%s", gtk_entry_get_text(GTK_ENTRY(widget)));
			break;
		case 6:
			material->diffuse[0] = (GLfloat)colour.red / 65535;
			material->diffuse[1] = (GLfloat)colour.green / 65535;
			material->diffuse[2] = (GLfloat)colour.blue / 65535;
			break;
		case 7:
			material->ambient[0] = (GLfloat)colour.red / 65535;
			material->ambient[1] = (GLfloat)colour.green / 65535;
			material->ambient[2] = (GLfloat)colour.blue / 65535;
			break;
		case 8:
			material->specular[0] = (GLfloat)colour.red / 65535;
			material->specular[1] = (GLfloat)colour.green / 65535;
			material->specular[2] = (GLfloat)colour.blue / 65535;
			break;
		case 9:
			material->emission[0] = (GLfloat)colour.red / 65535;
			material->emission[1] = (GLfloat)colour.green / 65535;
			material->emission[2] = (GLfloat)colour.blue / 65535;
			break;
	}

	return TRUE;
}

int kudu_gui_materials_edit_diffuse_button(GtkWidget *widget, GtkWidget *button)
{
	KuduMaterial *material;
	material = *selected_material;
	if (material == NULL) return FALSE;

	GdkColor colour;

	colour.red = (guint16)(material->diffuse[0] * 65535);
	colour.green = (guint16)(material->diffuse[1] * 65535);
	colour.blue = (guint16)(material->diffuse[2] * 65535);

	gtk_color_button_set_color(GTK_COLOR_BUTTON(button), &colour);

	return TRUE;
}

int kudu_gui_materials_edit_ambient_button(GtkWidget *widget, GtkWidget *button)
{
	KuduMaterial *material;
	material = *selected_material;
	if (material == NULL) return FALSE;

	GdkColor colour;

	colour.red = (guint16)(material->ambient[0] * 65535);
	colour.green = (guint16)(material->ambient[1] * 65535);
	colour.blue = (guint16)(material->ambient[2] * 65535);

	gtk_color_button_set_color(GTK_COLOR_BUTTON(button), &colour);

	return TRUE;
}

int kudu_gui_materials_edit_specular_button(GtkWidget *widget, GtkWidget *button)
{
	KuduMaterial *material;
	material = *selected_material;
	if (material == NULL) return FALSE;

	GdkColor colour;

	colour.red = (guint16)(material->specular[0] * 65535);
	colour.green = (guint16)(material->specular[1] * 65535);
	colour.blue = (guint16)(material->specular[2] * 65535);

	gtk_color_button_set_color(GTK_COLOR_BUTTON(button), &colour);

	return TRUE;
}

int kudu_gui_materials_edit_emission_button(GtkWidget *widget, GtkWidget *button)
{
	KuduMaterial *material;
	material = *selected_material;
	if (material == NULL) return FALSE;

	GdkColor colour;

	colour.red = (guint16)(material->emission[0] * 65535);
	colour.green = (guint16)(material->emission[1] * 65535);
	colour.blue = (guint16)(material->emission[2] * 65535);

	gtk_color_button_set_color(GTK_COLOR_BUTTON(button), &colour);

	return TRUE;
}

int kudu_gui_materials_edit_diffuse_slider(GtkWidget *widget, GtkWidget *slider)
{
	KuduMaterial *material;
	material = *selected_material;
	if (material == NULL) return FALSE;

	gtk_range_set_value(GTK_RANGE(slider), (gdouble)material->diffuse[3]);

	return TRUE;
}

int kudu_gui_materials_edit_opacity_slider(GtkWidget *widget, GtkWidget *slider)
{
	KuduMaterial *material;
	material = *selected_material;
	if (material == NULL) return FALSE;

	gtk_range_set_value(GTK_RANGE(slider), (gdouble)material->diffuse[3]);

	return TRUE;
}

int kudu_gui_materials_edit_shininess_slider(GtkWidget *widget, GtkWidget *slider)
{
	KuduMaterial *material;
	material = *selected_material;
	if (material == NULL) return FALSE;

	gtk_range_set_value(GTK_RANGE(slider), (gdouble)material->shininess);

	return TRUE;
}

int kudu_gui_materials_edit_name_entry(GtkWidget *widget, GtkWidget *name)
{
	KuduMaterial *material;
	material = *selected_material;
	if (material == NULL) return FALSE;

	g_signal_handler_block(G_OBJECT(name), name_change);
	gtk_entry_set_text(GTK_ENTRY(name), material->name);
	g_signal_handler_unblock(G_OBJECT(name), name_change);

	return TRUE;
}

int kudu_gui_materials_edit_add(GtkWidget *widget, struct mat_list *mat)
{
	KuduMaterial *material, *sel_material = NULL;
	GtkTreeIter iter;
	material = *root_material;

	GtkListStore *material_list = mat->list_store;
	GtkTreeSelection *material_list_selection = mat->list_selection;
	GtkWidget *material_list_view = mat->list_view;

	if (material == NULL) {
		sel_material = kudu_material_new(working_object, NULL);
		*selected_material = sel_material;
		*root_material = sel_material;
	} else {
		while (material->next_material != NULL)
			material = material->next_material;
		sel_material = kudu_material_new(working_object, material);
	}

	gtk_list_store_append(GTK_LIST_STORE(material_list), &iter);
	gtk_list_store_set(GTK_LIST_STORE(material_list), &iter, MATERIAL_NAME, sel_material->name, 
			MATERIAL_ID, sel_material->id, -1);


	gtk_tree_selection_select_iter(GTK_TREE_SELECTION(material_list_selection), &iter);
	gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(material_list_view),
			gtk_tree_model_get_path(GTK_TREE_MODEL(material_list), &iter), NULL, TRUE, 0.5, 0.5);
	return TRUE;
}

int kudu_gui_materials_edit_delete(GtkWidget *widget, GtkListStore *material_list)
{
	KuduMaterial *material;
	material = *selected_material;
	if (material == NULL) return FALSE;

	GtkTreeIter iter;
	int valid, material_id;
	gchar *material_name;

	valid = gtk_tree_model_get_iter_first(GTK_TREE_MODEL(material_list), &iter);

	while (valid) {

		gtk_tree_model_get(GTK_TREE_MODEL(material_list), &iter, MATERIAL_NAME, &material_name,
				MATERIAL_ID, &material_id, -1);
		if (material_id == material->id) {
			gtk_list_store_remove(GTK_LIST_STORE(material_list), &iter);
			if (material != *root_material) *selected_material = *root_material;
			else {
				*selected_material = (*root_material)->next_material;
				*root_material = *selected_material;
			}
			kudu_material_destroy(material);
			return TRUE;
		}

		valid = gtk_tree_model_iter_next(GTK_TREE_MODEL(material_list), &iter);
	}

	return TRUE;
}

int kudu_gui_materials_edit_list_name(GtkWidget *widget, GtkListStore *material_list)
{
	KuduMaterial *material;
	material = *selected_material;
	if (material == NULL) return FALSE;

	GtkTreeIter iter;
	int valid, material_id;
	gchar *material_name;

	valid = gtk_tree_model_get_iter_first(GTK_TREE_MODEL(material_list), &iter);

	while (valid) {

		gtk_tree_model_get(GTK_TREE_MODEL(material_list), &iter, MATERIAL_NAME, &material_name,
				MATERIAL_ID, &material_id, -1);
		if (material_id == material->id) {
			gtk_list_store_set(GTK_LIST_STORE(material_list), &iter, MATERIAL_NAME, material->name,
				MATERIAL_ID, material_id, -1);
		}

		valid = gtk_tree_model_iter_next(GTK_TREE_MODEL(material_list), &iter);
	}

	return TRUE;
}

int kudu_gui_materials_edit(KuduObject* object, KuduMaterial *init_material)
{
	if (object == NULL) return FALSE;

	working_object = object;

	static KuduMaterial *current_material;

	GtkWidget *dialog, *material_list_view, *vbox, *hbox, *scrolled_window, *label, *frame, *button;
	GtkWidget *gl_drawing_area, *diffuse_button, *ambient_button, *specular_button, *emission_button;
	GtkWidget *shininess_slider, *opacity_slider, *material_name;
	GtkWidget *main_vbox;
	GtkListStore *material_list;
	GtkCellRenderer *cell_renderer;
	GtkTreeIter material_list_iter, selected_iter;
	GtkTreeSelection *material_list_selection;
	GtkTreeViewColumn *material_list_column;
	GdkGLConfig *gl_config;
	struct mat_list mat;
	int sel = FALSE;

	dialog = gtk_dialog_new();
	gtk_window_set_title(GTK_WINDOW(dialog), "Materials Editor");

	material_list = gtk_list_store_new(MATERIAL_COLUMNS, G_TYPE_STRING, G_TYPE_INT);

	cell_renderer = gtk_cell_renderer_text_new();
	material_list_column = gtk_tree_view_column_new_with_attributes("Materials:",
		cell_renderer, "text", MATERIAL_NAME, NULL);

	material_list_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(material_list));
	gtk_tree_view_append_column(GTK_TREE_VIEW(material_list_view), material_list_column);

	kudu_material_for_each_do(object->material);

	while ((current_material = kudu_material_next_do()) != NULL) {
		gtk_list_store_append(GTK_LIST_STORE(material_list), &material_list_iter);

		gtk_list_store_set(material_list, &material_list_iter, MATERIAL_NAME, current_material->name,
			MATERIAL_ID, current_material->id, -1);
		if (current_material == init_material) {
			selected_iter = material_list_iter;
			sel = TRUE;
		}
	}

	current_material = object->material;
	selected_material = &current_material;
	root_material = &object->material;

	material_list_selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(material_list_view));
	g_signal_connect(G_OBJECT(material_list_selection), "changed", 
			G_CALLBACK(kudu_gui_materials_edit_select_material), NULL);

	scrolled_window = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
	gtk_widget_set_size_request(GTK_WIDGET(scrolled_window), 400, 150);
	gtk_container_add(GTK_CONTAINER(scrolled_window), material_list_view);

	gtk_widget_show(material_list_view);
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), scrolled_window, TRUE, TRUE, 0);
	gtk_widget_show(scrolled_window);

	hbox = gtk_hbox_new(FALSE, 10);
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), hbox, TRUE, TRUE, 10);
	gtk_widget_show(hbox);

	button = gtk_button_new_with_label("Add");
	gtk_box_pack_start(GTK_BOX(hbox), button, TRUE, TRUE, 10);
	gtk_widget_show(button);
	mat.list_store = material_list;
	mat.list_selection = material_list_selection;
	mat.list_view = material_list_view;
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(kudu_gui_materials_edit_add), &mat);

	button = gtk_button_new_with_label("Delete");
	gtk_box_pack_start(GTK_BOX(hbox), button, TRUE, TRUE, 10);
	gtk_widget_show(button);
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(kudu_gui_materials_edit_delete), material_list);


	hbox = gtk_hbox_new(FALSE, 10);
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), hbox, TRUE, TRUE, 0);
	gtk_widget_show(hbox);

	frame = gtk_frame_new("Selected Material");
	gtk_box_pack_start(GTK_BOX(hbox), frame, TRUE, TRUE, 0);
	gtk_widget_show(frame);

	main_vbox = gtk_vbox_new(FALSE, 10);
	gtk_container_add(GTK_CONTAINER(frame), main_vbox);
	gtk_widget_show(main_vbox);

	/* Material Name */
	hbox = gtk_hbox_new(FALSE, 10);
	gtk_box_pack_start(GTK_BOX(main_vbox), hbox, FALSE, FALSE, 0);
	gtk_widget_show(hbox);

	label = gtk_label_new("Material Name");
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 10);
	gtk_widget_show(label);

	material_name = gtk_entry_new();
	gtk_box_pack_start(GTK_BOX(hbox), material_name, TRUE, TRUE, 10);
	gtk_widget_show(material_name);
	name_change = g_signal_connect(G_OBJECT(material_name), "changed",
				G_CALLBACK(kudu_gui_materials_edit_material), (gpointer)3);
	g_signal_connect(G_OBJECT(material_name), "changed", G_CALLBACK(kudu_gui_materials_edit_list_name), material_list);

	/* OpenGL viewport */
	hbox = gtk_hbox_new(FALSE, 10);
	gtk_box_pack_start(GTK_BOX(main_vbox), hbox, TRUE, TRUE, 0);
	gtk_widget_show(hbox);

	gl_config = gdk_gl_config_new_by_mode(GDK_GL_MODE_RGBA | GDK_GL_MODE_SINGLE);
	gl_drawing_area = gtk_drawing_area_new();
	gtk_widget_set_size_request(gl_drawing_area, 64, 64);
	gtk_widget_set_gl_capability(gl_drawing_area, gl_config, NULL, TRUE, GDK_GL_RGBA_TYPE);
	gtk_box_pack_start(GTK_BOX(hbox), gl_drawing_area, TRUE, TRUE, 5);
	gtk_widget_show(gl_drawing_area);

	g_signal_connect(G_OBJECT(gl_drawing_area), "realize", G_CALLBACK(kudu_gui_materials_edit_init), NULL);
	g_signal_connect(G_OBJECT(gl_drawing_area), "expose_event", G_CALLBACK(kudu_gui_materials_edit_display), NULL);
	g_signal_connect(G_OBJECT(gl_drawing_area), "configure_event", G_CALLBACK(kudu_gui_materials_edit_reshape), NULL);
	g_signal_connect(G_OBJECT(material_list_selection), "changed",
		G_CALLBACK(kudu_gui_materials_edit_redisplay), gl_drawing_area);

	vbox = gtk_vbox_new(FALSE, 10);
	gtk_box_pack_start(GTK_BOX(hbox), vbox, TRUE, TRUE, 0);
	gtk_widget_show(vbox);

	/* Diffuse colour selection */
	hbox = gtk_hbox_new(FALSE, 10);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);
	gtk_widget_show(hbox);

	label = gtk_label_new("Diffuse:");
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);
	gtk_widget_show(label);

	diffuse_button = gtk_color_button_new();
	gtk_box_pack_end(GTK_BOX(hbox), diffuse_button, FALSE, FALSE, 5);
	gtk_widget_show(diffuse_button);
	g_signal_connect(G_OBJECT(diffuse_button), "color-set", G_CALLBACK(kudu_gui_materials_edit_material), (gpointer)6);


	/* Ambient colour selection */
	hbox = gtk_hbox_new(FALSE, 10);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);
	gtk_widget_show(hbox);

	label = gtk_label_new("Ambient:");
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);
	gtk_widget_show(label);

	ambient_button = gtk_color_button_new();
	gtk_box_pack_end(GTK_BOX(hbox), ambient_button, FALSE, FALSE, 5);
	gtk_widget_show(ambient_button);
	g_signal_connect(G_OBJECT(ambient_button), "color-set", G_CALLBACK(kudu_gui_materials_edit_material), (gpointer)7);

	/* Specular colour selection */
	hbox = gtk_hbox_new(FALSE, 10);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);
	gtk_widget_show(hbox);

	label = gtk_label_new("Specular:");
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);
	gtk_widget_show(label);

	specular_button = gtk_color_button_new();
	gtk_box_pack_end(GTK_BOX(hbox), specular_button, FALSE, FALSE, 5);
	gtk_widget_show(specular_button);
	g_signal_connect(G_OBJECT(specular_button), "color-set", G_CALLBACK(kudu_gui_materials_edit_material), (gpointer)8);

	/* Emission colour selection */
	hbox = gtk_hbox_new(FALSE, 10);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);
	gtk_widget_show(hbox);

	label = gtk_label_new("Emission:");
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);
	gtk_widget_show(label);

	emission_button = gtk_color_button_new();
	gtk_box_pack_end(GTK_BOX(hbox), emission_button, FALSE, FALSE, 5);
	gtk_widget_show(emission_button);
	g_signal_connect(G_OBJECT(emission_button), "color-set", G_CALLBACK(kudu_gui_materials_edit_material), (gpointer)9);

	/* Shininess */
	hbox = gtk_hbox_new(FALSE, 10);
	gtk_box_pack_start(GTK_BOX(main_vbox), hbox, FALSE, FALSE, 0);
	gtk_widget_show(hbox);

	label = gtk_label_new("Shininess");
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);
	gtk_widget_show(label);

	shininess_slider = gtk_hscale_new_with_range(0.0, 128.0, 0.1);
	gtk_box_pack_start(GTK_BOX(hbox), shininess_slider, TRUE, TRUE, 0);
	gtk_widget_show(shininess_slider);
	g_signal_connect(G_OBJECT(shininess_slider), "value_changed",
				G_CALLBACK(kudu_gui_materials_edit_material), (gpointer)2);
	g_signal_connect(G_OBJECT(shininess_slider), "value_changed",
		G_CALLBACK(kudu_gui_materials_edit_redisplay), gl_drawing_area);

	/* Opacity */
	hbox = gtk_hbox_new(FALSE, 10);
	gtk_box_pack_start(GTK_BOX(main_vbox), hbox, FALSE, FALSE, 0);
	gtk_widget_show(hbox);

	label = gtk_label_new("Opacity");
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);
	gtk_widget_show(label);

	opacity_slider = gtk_hscale_new_with_range(0.0, 1.0, 0.01);
	gtk_box_pack_start(GTK_BOX(hbox), opacity_slider, TRUE, TRUE, 0);
	gtk_widget_show(opacity_slider);
	g_signal_connect(G_OBJECT(opacity_slider), "value_changed",
				G_CALLBACK(kudu_gui_materials_edit_material), (gpointer)1);
	g_signal_connect(G_OBJECT(opacity_slider), "value_changed",
		G_CALLBACK(kudu_gui_materials_edit_redisplay), gl_drawing_area);

	/* Connect signals to set button colours when a new material is selected */
	g_signal_connect(G_OBJECT(material_list_selection), "changed", 
			G_CALLBACK(kudu_gui_materials_edit_diffuse_button), diffuse_button);
	g_signal_connect(G_OBJECT(material_list_selection), "changed", 
			G_CALLBACK(kudu_gui_materials_edit_ambient_button), ambient_button);
	g_signal_connect(G_OBJECT(material_list_selection), "changed", 
			G_CALLBACK(kudu_gui_materials_edit_specular_button), specular_button);
	g_signal_connect(G_OBJECT(material_list_selection), "changed", 
			G_CALLBACK(kudu_gui_materials_edit_emission_button), emission_button);

	/* Connect signals to set sliders when a new material is selected */
	g_signal_connect(G_OBJECT(material_list_selection), "changed", 
			G_CALLBACK(kudu_gui_materials_edit_opacity_slider), opacity_slider);
	g_signal_connect(G_OBJECT(material_list_selection), "changed", 
			G_CALLBACK(kudu_gui_materials_edit_shininess_slider), shininess_slider);

	/* Connect signals to set name when a new material is selected */
	g_signal_connect(G_OBJECT(material_list_selection), "changed", 
			G_CALLBACK(kudu_gui_materials_edit_name_entry), material_name);
	

	gtk_dialog_add_button(GTK_DIALOG(dialog), GTK_STOCK_OK, GTK_RESPONSE_OK);

	gtk_widget_show(dialog);

	if (sel) {
		gtk_tree_selection_select_iter(GTK_TREE_SELECTION(material_list_selection), &selected_iter);
		gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(material_list_view),
			gtk_tree_model_get_path(GTK_TREE_MODEL(material_list), &selected_iter), NULL, TRUE, 0.5, 0.5);
	}

	switch (gtk_dialog_run(GTK_DIALOG(dialog))) {
		case GTK_RESPONSE_OK:
			break;
	}

	gtk_widget_destroy(dialog);

	return TRUE;
}

int kudu_gui_materials_pick_for_faces(KuduObject *object, KuduSelectionList *selection_list)
{
	if (object == NULL) return FALSE;
	if (selection_list == NULL) return FALSE;
	if (selection_list->type != SELECT_FACES) return FALSE;
	if (selection_list->first_item == NULL) return FALSE;

	if (!kudu_selection_list_for_each_do(selection_list)) return FALSE;

	KuduFace *face = (KuduFace*)kudu_selection_list_next_do();

	kudu_gui_materials_edit(object, face->material);
	face->material = kudu_gui_materials_edit_get_picked_material();

	while ((face = (KuduFace*)kudu_selection_list_next_do()) != NULL)
		face->material = kudu_gui_materials_edit_get_picked_material();

	return TRUE;
}


