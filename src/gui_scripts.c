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
#include "gui_scripts.h"

static KuduScript *selected_script = NULL;

enum {
	SCRIPT_NAME,
	SCRIPT,
	NUM_COLS
};

void kudu_gui_script_selector_pick(GtkTreeSelection *scripts, GtkTextBuffer *info)
{
	GtkTreeIter iter;
	GtkTreeModel *model;
	GtkTextIter titer;
	KuduScript *script = NULL;

	if (gtk_tree_selection_get_selected(scripts, &model, &iter)) {
		gtk_tree_model_get(model, &iter, SCRIPT, &script, -1);

		if (script != NULL) {
			selected_script = script;

			gtk_text_buffer_set_text(GTK_TEXT_BUFFER(info), script->name, -1);
		}
	}
}

int kudu_gui_script_selector(void)
{
	GtkWidget *dialog, *hbox, *script_view, *info_view, *scrolled_window, *scrolled_window2;
	GtkTextBuffer *info;
	GtkTreeIter s_iter;
	GtkListStore *script_list = NULL;
	GtkCellRenderer *script_cells;
	GtkTreeViewColumn *script_column;
	GtkTreeSelection *script_selection;
	KuduScript *script;

	if (!kudu_script_list_for_each_do()) return FALSE;

	script_list = gtk_list_store_new(NUM_COLS, G_TYPE_STRING, G_TYPE_POINTER);

	while ((script = kudu_script_list_next_do()) != NULL) {
				gtk_list_store_append(script_list, &s_iter);
				gtk_list_store_set(script_list, &s_iter, SCRIPT_NAME, script->name, SCRIPT, script, -1);
	}

	/*if (b <= 0) {
		if (e == 0) kudu_gui_message("No installed scripts found!", GTK_MESSAGE_INFO);
		else	kudu_gui_message("Error searching for scripts!", GTK_MESSAGE_ERROR);
		return FALSE;
	}*/

	/* Construct the gui */
	dialog = gtk_dialog_new_with_buttons("Script Selector", GTK_WINDOW(STACK_GET_WIDGET("main_window")),
				GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT, "Run..", GTK_RESPONSE_OK, "Close", GTK_RESPONSE_REJECT, NULL);

	hbox = gtk_hbox_new(FALSE, 10);

	info_view = gtk_text_view_new();
	info = gtk_text_view_get_buffer(GTK_TEXT_VIEW(info_view));

	scrolled_window = gtk_scrolled_window_new(NULL, NULL);
	scrolled_window2 = gtk_scrolled_window_new(NULL, NULL);
	script_cells = gtk_cell_renderer_text_new();
	script_column = gtk_tree_view_column_new_with_attributes("Script List", script_cells, "text", SCRIPT_NAME, NULL);


	script_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(script_list));
	gtk_tree_view_append_column(GTK_TREE_VIEW(script_view), script_column);

	script_selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(script_view));
	g_signal_connect(G_OBJECT(script_selection), "changed", G_CALLBACK(kudu_gui_script_selector_pick), info);


	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
	gtk_widget_set_size_request(GTK_WIDGET(scrolled_window), 300, 200);
	gtk_container_add(GTK_CONTAINER(scrolled_window), script_view);

	gtk_widget_set_size_request(GTK_WIDGET(scrolled_window2), 300, 200);
	gtk_container_add(GTK_CONTAINER(scrolled_window2), info_view);

	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), hbox, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(hbox), scrolled_window, TRUE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(hbox), scrolled_window2, TRUE, TRUE, 5);

	gtk_widget_show_all(dialog);
	switch (gtk_dialog_run(GTK_DIALOG(dialog))) {
		case GTK_RESPONSE_OK:
			gtk_widget_destroy(dialog);
			if (selected_script != NULL) kudu_script_run(selected_script);
			break;
		default:
			gtk_widget_destroy(dialog);
			break;
	}

	selected_script = NULL;

	return TRUE;
}



