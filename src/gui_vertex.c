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
#include "gui_vertex.h"

KuduVertex *fvertex = NULL;
KuduBone *gv_selected_bone = NULL;
KuduSelectionList *gv_selection_list = NULL;
GtkWidget *attach_percent = NULL;
GtkTreeSelection *bone_list_selection = NULL;
GtkListStore *bone_list = NULL;
int setting = FALSE;

enum {
	BONE,		/* Pointer to bone */
	BONE_NAME,	/* Bone name */
	BONE_INFLUENCE,	/* Influence percent */

	N_COLUMNS
};

void kudu_gui_vertex_selection_do(GtkTreeModel *tree_model, GtkTreePath *path, GtkTreeIter *iter, gpointer data)
{
	float influence;
	KuduBone *current_bone;

	gtk_tree_model_get(tree_model, iter, BONE, &current_bone, -1);

}

void kudu_gui_vertex_properties_select_bone(GtkTreeSelection *tree_selection, gpointer data)
{
	GtkTreeIter iter;
	GtkTreeModel *model;

	setting = TRUE;

	gtk_tree_selection_selected_foreach(tree_selection, kudu_gui_vertex_selection_do, NULL);
/*	if (gtk_tree_selection_get_selected(tree_selection, &model, &iter)) {
		gtk_tree_model_get(model, &iter, BONE, &gv_selected_bone, -1);

		if (attach_percent != NULL)
			gtk_spin_button_set_value(GTK_SPIN_BUTTON(attach_percent), (double)kudu_vertex_get_bone_influence(fvertex, gv_selected_bone));
	}*/

	setting = FALSE;
}


/* Sets the influence setting for each selected bone on each selected vertex */
void kudu_gui_vertex_selection_set_influence(GtkTreeModel *tree_model, GtkTreePath *path, GtkTreeIter *iter, gpointer data)
{

	KuduVertex *current_vertex;
	KuduBone *current_bone;
	float influence = *(float*)data;

	/* Retrieve the bone in the current item */
	gtk_tree_model_get(tree_model, iter, BONE, &current_bone, -1);

	if (!kudu_selection_list_for_each_do(gv_selection_list)) return;
		
	/* Apply the influence to each vertex in the selection list */
	while ((current_vertex = (KuduVertex*)kudu_selection_list_next_do()) != NULL) {
		kudu_vertex_set_bone_influence(current_vertex, current_bone, influence);
	}

	/* Update the progress bar to display the new influence setting */
	gtk_list_store_set(bone_list, iter, BONE_INFLUENCE, influence,  -1);
}

void kudu_gui_vertex_properties_attach_percent(GtkWidget *widget, GtkTreeSelection *tree_selection)
{
	if (setting) return;

	float influence;
	influence = (float)gtk_spin_button_get_value(GTK_SPIN_BUTTON(widget));

	gtk_tree_selection_selected_foreach(tree_selection, kudu_gui_vertex_selection_set_influence, &influence);

/*	KuduVertex *current_vertex;
	GtkTreeIter iter;
	GtkTreeModel *model;*/

	/*if (gv_selected_bone == NULL) return;*/

	/* Set the influence value for gv_selected_bone on each vertex in the selection list */
	/*if (kudu_selection_list_for_each_do(gv_selection_list)) {
		while ((current_vertex = (KuduVertex*)kudu_selection_list_next_do()) != NULL) {
			kudu_vertex_set_bone_influence(current_vertex, gv_selected_bone, influence);
		}
	}*/

	/* Update the apropriate progress bar in the tree_view list */
	/*if (gtk_tree_selection_get_selected(bone_list_selection, &model, &iter)) {
		gtk_list_store_set(bone_list, &iter, BONE_INFLUENCE, influence,  -1);
	}*/
}

void kudu_gui_vertex_properties_auto_balance(GtkWidget *widget, GtkTreeSelection *tree_selection)
{
	float influence = 100.0;
	int num = gtk_tree_selection_count_selected_rows(tree_selection);

	influence /= num;

	gtk_tree_selection_selected_foreach(tree_selection, kudu_gui_vertex_selection_set_influence, &influence);
}

void kudu_gui_vertex_properties_select_all(GtkWidget *widget, GtkTreeSelection *tree_selection)
{
	gtk_tree_selection_select_all(tree_selection);
}

void kudu_gui_vertex_properties_select_none(GtkWidget *widget, GtkTreeSelection *tree_selection)
{
	gtk_tree_selection_unselect_all(tree_selection);
}

int kudu_gui_vertex_properties_edit(KuduObject *object, KuduSelectionList *selected_items, KuduSelectionList *selected_bones)
{
	if ((object == NULL) || (selected_items == NULL) || (selected_bones == NULL)) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	char text[64];
	GtkWidget *dialog;
	GtkWidget *bone_list_view;
	GtkCellRenderer *cell_renderer;
	GtkTreeIter bone_list_iter, selected_iter;
	GtkTreeViewColumn *bone_list_column, *check_column;
	GtkTreePath *tree_path;
	GtkWidget *scrolled_window, *button;
	GtkWidget *hbox;
	GtkWidget *label;
	GtkObject *attach_adjustment;

	int selected_num = 0, attached_percent = 0, gone_back, bone_id, bone_selected = FALSE, rv, a;
	float inf;

	KuduVertex *current_vertex = NULL;
	KuduBone *selected_bone = NULL, *current_bone = NULL, *root_bone = NULL;
	KuduSelectionList *selected;

	/* Create a new selection list from the original one */
	selected = kudu_selection_list_new_from_copy(selected_items);

	if (selected == NULL) {
		kudu_gui_message("Please select at least one vertex!", GTK_MESSAGE_ERROR);
		return FALSE;
	}

	if (selected_bones->selected_items <= 0) {
		kudu_gui_message("Please select at least one bone!", GTK_MESSAGE_ERROR);
		return FALSE;
	}

	/* Set selection list type to vertices (SELECT_POINTS) as only vertices can be attached to bones */
	kudu_selection_list_set_type(selected, SELECT_POINTS, FALSE);

	/* Set the global selection list pointer to point to this list */
	gv_selection_list = selected;

	fvertex = kudu_selection_list_get_first_item(selected);

/*	if (kudu_selection_list_for_each_do(selected)) {
		while ((current_vertex = (KuduVertex*)kudu_selection_list_next_do()) != NULL) {
			if (fvertex == NULL) fvertex = current_vertex;*/
		/*	attached_percent += current_vertex->attached_percent;*/
		/*	if (selected_bone == NULL) selected_bone = current_vertex->bone;
			if (selected_bone != NULL) bone_id = selected_bone->id;*/
/*		}
	}*/

/*	printf("%d :: %d\n", selected->selected_items, attached_percent);
	selected_num = selected->selected_items;
	attached_percent /= selected_num;*/

	/* Create a new dialog */
	dialog = gtk_dialog_new();
	gtk_window_set_title(GTK_WINDOW(dialog), "Vertex Attachment Properties");

	/* Label */
	sprintf(text, "%d vertices and %d bone(s) selected", selected->selected_items, selected_bones->selected_items);
	label = gtk_label_new(text);
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), label, FALSE, FALSE, 5);

	/* Create a gtk_list_store for the treeview column */
	bone_list = gtk_list_store_new(N_COLUMNS, G_TYPE_POINTER, G_TYPE_STRING, G_TYPE_FLOAT);

	/* Create the tree_view */
	bone_list_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(bone_list));

	bone_list_selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(bone_list_view));
	gtk_tree_selection_set_mode(bone_list_selection, GTK_SELECTION_MULTIPLE);
	g_signal_connect(G_OBJECT(bone_list_selection), "changed",
		G_CALLBACK(kudu_gui_vertex_properties_select_bone), NULL);

	/* First column, the list of bone names */
	cell_renderer = gtk_cell_renderer_text_new();
	bone_list_column = gtk_tree_view_column_new_with_attributes("Bones:",
		cell_renderer, "text", BONE_NAME, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(bone_list_view), bone_list_column);

	/* Second column the list of progress bars indicating attached percent */
	cell_renderer = gtk_cell_renderer_progress_new();
	check_column = gtk_tree_view_column_new_with_attributes("Influence %", cell_renderer, "value", BONE_INFLUENCE, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(bone_list_view), check_column);


	/* "Walk" the entire bone structure adding each one to the tree_view's list_store */
/*	do {
		if (root_bone == NULL) root_bone = object->bone;
		else	root_bone = root_bone->next_sibling;

		current_bone = root_bone;
		gone_back = FALSE;

		do {
			if (!gone_back) {*/


	if (kudu_selection_list_for_each_do(selected_bones)) {
		while ((current_bone = (KuduBone*)kudu_selection_list_next_do()) != NULL) {

				inf = 0.0;
				for (a = 0; a < fvertex->num_bones; a++) {
					if (current_bone == fvertex->bone[a]) {
						inf = fvertex->influence[a];
					}
				}

				/* Add current_bone to the list_store */
				gtk_list_store_append(GTK_LIST_STORE(bone_list), &bone_list_iter);
				gtk_list_store_set(bone_list, &bone_list_iter, BONE, current_bone, BONE_NAME, current_bone->name,
					BONE_INFLUENCE, inf, -1);
		}

	}

/*			}

			if ((current_bone->first_child != NULL) && (!gone_back)) current_bone = current_bone->first_child;
			else	if ((current_bone->next_sibling != NULL) && (current_bone != root_bone)) {
					current_bone = current_bone->next_sibling;
					gone_back = FALSE;
				} else {
					if (current_bone->parent != NULL) current_bone = current_bone->parent;
					gone_back = TRUE;
				}

		} while (current_bone != root_bone);
	} while (root_bone->next_sibling != NULL);*/

	/* Create a new gtk_scrolled_window */
	scrolled_window = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
	gtk_widget_set_size_request(GTK_WIDGET(scrolled_window), 300, 200);
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), scrolled_window, TRUE, TRUE, 5);

	/* Insert the tree_view into the scrolled_window */
	gtk_container_add(GTK_CONTAINER(scrolled_window), bone_list_view);

	/* Select all / none and auto balance buttons */
	hbox = gtk_hbox_new(FALSE, 10);
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), hbox, FALSE, FALSE, 5);

	button = gtk_button_new_with_label("Select All");
	gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(kudu_gui_vertex_properties_select_all), bone_list_selection);

	button = gtk_button_new_with_label("Select None");
	gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(kudu_gui_vertex_properties_select_none), bone_list_selection);

	button = gtk_button_new_with_label("Auto Balance");
	gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(kudu_gui_vertex_properties_auto_balance), bone_list_selection);

	/* Create the spin_button and label for setting influence % */
	hbox = gtk_hbox_new(FALSE, 10);
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), hbox, FALSE, FALSE, 5);

	/* Label */
	label = gtk_label_new("Influence %:");
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 5);

	/* Influence spin_button */
	attach_adjustment = gtk_adjustment_new(attached_percent, 0.0, 100.0, 1.0, 1.0, 0);
	attach_percent = gtk_spin_button_new(GTK_ADJUSTMENT(attach_adjustment), 1.0, 0);
	g_signal_connect(G_OBJECT(attach_percent), "value_changed", G_CALLBACK(kudu_gui_vertex_properties_attach_percent), bone_list_selection);

	gtk_box_pack_start(GTK_BOX(hbox), attach_percent, TRUE, TRUE, 5);


	/* Add OK button */
	/*gtk_dialog_add_button(GTK_DIALOG(dialog), GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL);*/
	gtk_dialog_add_button(GTK_DIALOG(dialog), GTK_STOCK_OK, GTK_RESPONSE_OK);

	/* Show the dialog */
	gtk_widget_show_all(dialog);

	/*if (bone_selected) {
		gtk_tree_selection_select_iter(GTK_TREE_SELECTION(bone_list_selection), &selected_iter);
		gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(bone_list_view),
			gtk_tree_model_get_path(GTK_TREE_MODEL(bone_list), &selected_iter), NULL, TRUE, 0.5, 0.5);
	}*/

	switch (gtk_dialog_run(GTK_DIALOG(dialog))) {
		case GTK_RESPONSE_OK:
	/*		selected_bone = kudu_bone_find_with_id(object->bone, bone_id);
			if (selected_bone == NULL) return FALSE;

			if (kudu_selection_list_for_each_do(selected)) {
				while ((current_vertex = (KuduVertex*)kudu_selection_list_next_do()) != NULL) {
					kudu_vertex_detach(current_vertex);
					if (attached_percent == 100)
						kudu_vertex_attach_to_bone(current_vertex, selected_bone);
					else	kudu_vertex_attach_to_bone_by_percent(current_vertex, selected_bone,
								attached_percent);
				}
			}*/
			rv = TRUE;
			break;
		case GTK_RESPONSE_CANCEL:
			rv = FALSE;
			break;
		default:
			rv = FALSE;
			break;
	}

	kudu_selection_list_destroy(selected);
	gtk_widget_destroy(dialog);

	return rv;
}

int kudu_gui_vertex_properties_reset(KuduSelectionList *selected_items)
{
	if (selected_items == NULL) return FALSE;

	char warn_msg[1024];
	KuduVertex *current_vertex;
	KuduSelectionList *selected = kudu_selection_list_new_from_copy(selected_items);

	if (selected == NULL) {
		kudu_gui_message("Please select at least one vertex!", GTK_MESSAGE_ERROR);
		return FALSE;
	}

	kudu_selection_list_set_type(selected, SELECT_POINTS, FALSE);

	switch (selected_items->type) {
		case SELECT_POINTS:
			sprintf(warn_msg, "Reset attachments on %d selected vertice(s)?\n\n"
					  "Click \"OK\" to continue or \"Cancel\" to quit",
					   selected->selected_items);
			break;
		case SELECT_EDGES:
			sprintf(warn_msg, "Reset attachments on %d vertices(s) in %d selected edges(s)?\n\n"
					  "Click \"OK\" to continue or \"Cancel\" to quit",
					   selected->selected_items, selected_items->selected_items);
			break;
		case SELECT_FACES:
			sprintf(warn_msg, "Reset attachments on %d vertices(s) in %d selected face(s)?\n\n"
					  "Click \"OK\" to continue or \"Cancel\" to quit",
					   selected->selected_items, selected_items->selected_items);
			break;
		case SELECT_SHAPES:
			sprintf(warn_msg, "Reset attachments on %d vertices(s) in %d selected shape(s)?\n\n"
					  "Click \"OK\" to continue or \"Cancel\" to quit",
					   selected->selected_items, selected_items->selected_items);
			break;
	}

	if (kudu_gui_confirm(warn_msg, GTK_MESSAGE_WARNING) == GTK_RESPONSE_CANCEL) {
		return FALSE;
	}

	if (kudu_selection_list_for_each_do(selected)) {
		while ((current_vertex = (KuduVertex*)kudu_selection_list_next_do()) != NULL) {
			kudu_vertex_detach(current_vertex);
		}
	}

}



