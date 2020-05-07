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

enum {
	BONE_NAME,
	BONE_ID,
	N_COLUMNS
};

void kudu_gui_vertex_properties_select_bone(GtkTreeSelection *tree_selection, gint bone_id)
{
	GtkTreeIter iter;
	GtkTreeModel *model;

	if (gtk_tree_selection_get_selected(tree_selection, &model, &iter)) {
		gtk_tree_model_get(model, &iter, BONE_ID, bone_id, -1);
	}

}

void kudu_gui_vertex_properties_attach_percent(GtkWidget *widget, gint *percent)
{
	*percent = (gint)gtk_spin_button_get_value(GTK_SPIN_BUTTON(widget));
}

gint kudu_gui_vertex_properties_edit(KuduObject *object, KuduSelectionList *selected_items)
{
	if (object == NULL) return FALSE;
	if (selected_items == NULL) return FALSE;

	char text[64];
	GtkWidget *dialog;
	GtkWidget *bone_list_view;
	GtkListStore *bone_list;
	GtkCellRenderer *cell_renderer;
	GtkTreeIter bone_list_iter, selected_iter;
	GtkTreeSelection *bone_list_selection;
	GtkTreeViewColumn *bone_list_column;
	GtkTreePath *tree_path;
	GtkWidget *scrolled_window;
	GtkWidget *hbox;
	GtkWidget *attach_percent;
	GtkWidget *label;
	GtkObject *attach_adjustment;

	int selected_num = 0, attached_percent = 0, gone_back, bone_id, bone_selected = FALSE, rv;

	KuduVertex *current_vertex = NULL;
	KuduBone *selected_bone = NULL, *current_bone = NULL, *root_bone = NULL;
	KuduSelectionList *selected;

	selected = kudu_selection_list_new_from_copy(selected_items);

	if (selected == NULL) {
		kudu_gui_message("Please select at least one vertex!", GTK_MESSAGE_ERROR);
		return FALSE;
	}

	kudu_selection_list_set_type(selected, SELECT_POINTS, FALSE);

	if (kudu_selection_list_for_each_do(selected)) {
		while ((current_vertex = (KuduVertex*)kudu_selection_list_next_do()) != NULL) {
			attached_percent += current_vertex->attached_percent;
			if (selected_bone == NULL) selected_bone = current_vertex->bone;
			if (selected_bone != NULL) bone_id = selected_bone->id;
		}
	}

	selected_num = selected->selected_items;
	attached_percent /= selected_num;

	dialog = gtk_dialog_new();
	gtk_window_set_title(GTK_WINDOW(dialog), "Vertex Attachment Properties");

	sprintf(text, "%d vertices selected", selected_num);
	label = gtk_label_new(text);
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), label, FALSE, FALSE, 5);
	gtk_widget_show(label);

	bone_list = gtk_list_store_new(N_COLUMNS, G_TYPE_STRING, G_TYPE_INT);

	cell_renderer = gtk_cell_renderer_text_new();
	bone_list_column = gtk_tree_view_column_new_with_attributes("Attach Selected Vertices to Bone:",
		cell_renderer, "text", BONE_NAME, NULL);

	bone_list_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(bone_list));
	gtk_tree_view_append_column(GTK_TREE_VIEW(bone_list_view), bone_list_column);

	bone_list_selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(bone_list_view));
	gtk_tree_selection_set_mode(bone_list_selection, GTK_SELECTION_SINGLE);
	g_signal_connect(G_OBJECT(bone_list_selection), "changed",
		G_CALLBACK(kudu_gui_vertex_properties_select_bone), &bone_id);


	do {
		if (root_bone == NULL) root_bone = object->bone;
		else	root_bone = root_bone->next_sibling;

		current_bone = root_bone;
		gone_back = FALSE;

		do {
			if (!gone_back) {
				gtk_list_store_append(GTK_LIST_STORE(bone_list), &bone_list_iter);
				gtk_list_store_set(bone_list, &bone_list_iter, BONE_NAME, current_bone->name,
					BONE_ID, current_bone->id, -1);

				if (current_bone->id == bone_id) {
					selected_iter = bone_list_iter;
					bone_selected = TRUE;
				}
			}

			if ((current_bone->first_child != NULL) && (!gone_back)) current_bone = current_bone->first_child;
			else	if ((current_bone->next_sibling != NULL) && (current_bone != root_bone)) {
					current_bone = current_bone->next_sibling;
					gone_back = FALSE;
				} else {
					if (current_bone->parent != NULL) current_bone = current_bone->parent;
					gone_back = TRUE;
				}

		} while (current_bone != root_bone);
	} while (root_bone->next_sibling != NULL);


	scrolled_window = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
	gtk_widget_set_size_request(GTK_WIDGET(scrolled_window), 300, 200);
	gtk_container_add(GTK_CONTAINER(scrolled_window), bone_list_view);

	gtk_widget_show(bone_list_view);
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), scrolled_window, TRUE, TRUE, 5);
	gtk_widget_show(scrolled_window);

	hbox = gtk_hbox_new(FALSE, 10);
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), hbox, FALSE, FALSE, 5);
	gtk_widget_show(hbox);

	label = gtk_label_new("Attach Percent:");
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 5);
	gtk_widget_show(label);

	attach_adjustment = gtk_adjustment_new(attached_percent, 0.0, 100.0, 1.0, 1.0, 0);
	attach_percent = gtk_spin_button_new(GTK_ADJUSTMENT(attach_adjustment), 1.0, 0);
	g_signal_connect(G_OBJECT(attach_percent), "value_changed",
		G_CALLBACK(kudu_gui_vertex_properties_attach_percent), &attached_percent);

	gtk_box_pack_start(GTK_BOX(hbox), attach_percent, TRUE, TRUE, 5);
	gtk_widget_show(attach_percent);

	gtk_dialog_add_button(GTK_DIALOG(dialog), GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL);
	gtk_dialog_add_button(GTK_DIALOG(dialog), GTK_STOCK_OK, GTK_RESPONSE_OK);

	gtk_widget_show(dialog);

	if (bone_selected) {
		gtk_tree_selection_select_iter(GTK_TREE_SELECTION(bone_list_selection), &selected_iter);
		gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(bone_list_view),
			gtk_tree_model_get_path(GTK_TREE_MODEL(bone_list), &selected_iter), NULL, TRUE, 0.5, 0.5);
	}

	switch (gtk_dialog_run(GTK_DIALOG(dialog))) {
		case GTK_RESPONSE_OK:
			selected_bone = kudu_bone_find_with_id(object->bone, bone_id);
			if (selected_bone == NULL) return FALSE;

			if (kudu_selection_list_for_each_do(selected)) {
				while ((current_vertex = (KuduVertex*)kudu_selection_list_next_do()) != NULL) {
					kudu_vertex_detach(current_vertex);
					if (attached_percent == 100)
						kudu_vertex_attach_to_bone(current_vertex, selected_bone);
					else	kudu_vertex_attach_to_bone_by_percent(current_vertex, selected_bone,
								attached_percent);
				}
			}
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



