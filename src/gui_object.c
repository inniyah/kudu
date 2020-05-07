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
#include "gui_object.h"


void kudu_gui_object_edit_name(GtkWidget *widget, KuduObject *object)
{
	const char *name = gtk_entry_get_text(GTK_ENTRY(widget));

	sprintf(object->name, name);

	char kname[32];
	sprintf(kname, "_object_%d", object->id);

	GtkWidget *menu_item = STACK_GET_WIDGET(kname);
	GtkWidget *label;
	if (menu_item != NULL) {
		label = gtk_bin_get_child(GTK_BIN(menu_item));
		gtk_label_set_text(GTK_LABEL(label), object->name);
	}
}

void kudu_gui_object_edit_author(GtkWidget *widget, KuduObject *object)
{
	const char *artist = gtk_entry_get_text(GTK_ENTRY(widget));
	kudu_object_set_author(object, (char*)artist);
}

void kudu_gui_object_edit_email(GtkWidget *widget, KuduObject *object)
{
	const char *email = gtk_entry_get_text(GTK_ENTRY(widget));
	kudu_object_set_email(object, (char*)email);
}

void kudu_gui_object_edit_url(GtkWidget *widget, KuduObject *object)
{
	const char *url = gtk_entry_get_text(GTK_ENTRY(widget));
	kudu_object_set_url(object, (char*)url);
}

int kudu_gui_object_edit(KuduObject *object)
{
	if (object == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}


	GtkWidget *dialog, *entry, *label, *hbox, *frame, *vbox;

	dialog = gtk_dialog_new();
	gtk_window_set_title(GTK_WINDOW(dialog), "Object Properties");

	/* Object Properties */
	frame = gtk_frame_new("Object");
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), frame, TRUE, TRUE, 5);

	vbox = gtk_vbox_new(FALSE, 5);
	gtk_container_add(GTK_CONTAINER(frame), vbox);

	hbox = gtk_hbox_new(FALSE, 5);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, TRUE, 5);

	label = gtk_label_new("Name:");

	entry = gtk_entry_new();
	gtk_entry_set_max_length(GTK_ENTRY(entry), 255);
	gtk_entry_set_text(GTK_ENTRY(entry), object->name);
	g_signal_connect(G_OBJECT(entry), "changed", G_CALLBACK(kudu_gui_object_edit_name), object);

	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 5);
	gtk_box_pack_start(GTK_BOX(hbox), entry, TRUE, TRUE, 5);

	/* Author info, name, mail, url */
	frame = gtk_frame_new("Author");
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), frame, TRUE, TRUE, 5);

	vbox = gtk_vbox_new(FALSE, 5);
	gtk_container_add(GTK_CONTAINER(frame), vbox);

	/* Author name */
	hbox = gtk_hbox_new(FALSE, 5);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, FALSE, 0);
	label = gtk_label_new("Name:");
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 5);
	entry = gtk_entry_new();
	gtk_entry_set_max_length(GTK_ENTRY(entry), 255);
	if (object->author_length > 0) gtk_entry_set_text(GTK_ENTRY(entry), object->author);
	gtk_box_pack_start(GTK_BOX(hbox), entry, TRUE, TRUE, 5);
	g_signal_connect(G_OBJECT(entry), "changed", G_CALLBACK(kudu_gui_object_edit_author), object);

	/* Email */
	hbox = gtk_hbox_new(FALSE, 5);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, FALSE, 0);
	label = gtk_label_new("Email:");
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 5);
	entry = gtk_entry_new();
	gtk_entry_set_max_length(GTK_ENTRY(entry), 255);
	if (object->email_length > 0) gtk_entry_set_text(GTK_ENTRY(entry), object->email);
	gtk_box_pack_start(GTK_BOX(hbox), entry, TRUE, TRUE, 5);
	g_signal_connect(G_OBJECT(entry), "changed", G_CALLBACK(kudu_gui_object_edit_email), object);

	/* Url */
	hbox = gtk_hbox_new(FALSE, 5);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, FALSE, 5);
	label = gtk_label_new("Url:");
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 5);
	entry = gtk_entry_new();
	gtk_entry_set_max_length(GTK_ENTRY(entry), 255);
	if (object->url_length > 0) gtk_entry_set_text(GTK_ENTRY(entry), object->url);
	gtk_box_pack_start(GTK_BOX(hbox), entry, TRUE, TRUE, 5);
	g_signal_connect(G_OBJECT(entry), "changed", G_CALLBACK(kudu_gui_object_edit_url), object);


	gtk_dialog_add_button(GTK_DIALOG(dialog), "OK", GTK_RESPONSE_OK);
	gtk_widget_show_all(dialog);
	gtk_dialog_run(GTK_DIALOG(dialog));

	gtk_widget_destroy(dialog);

	return TRUE;
}

