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
#include "about.h"

GtkWidget *kudu_about_dialog(void)
{
	GtkWidget *dialog, *logo, *label, *license_view, *scrolled_window, *frame;
	GtkTextBuffer *license;

	dialog = gtk_dialog_new();
	gtk_window_set_title(GTK_WINDOW(dialog), "About Kudu");

	logo = gtk_image_new_from_file(KUDU_IMAGES_DIR"logo.png");
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), logo, FALSE, FALSE, 0);

	label = gtk_label_new(PACKAGE_STRING);
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), label, FALSE, FALSE, 0);

	label = gtk_label_new("Copyright(C) 2005 Daniel Pekelharing");
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), label, FALSE, FALSE, 5);
	label = gtk_label_new("<redarrow@users.sourceforge.net>");
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), label, FALSE, FALSE, 5);


	frame = gtk_frame_new(NULL);
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), frame, TRUE, TRUE, 5);

	license_view = gtk_text_view_new();
	license = gtk_text_view_get_buffer(GTK_TEXT_VIEW(license_view));

	gtk_text_buffer_set_text(license, "Kudu Animatior\n"
					  "Copyright(C) 2005 Daniel Pekelharing\n\n"
					  "This program is free software; you can redistribute it and/or modify\n"
					  "it under the terms of the GNU General Public License as published by\n"
					  "the Free Software Foundation; either version 2 of the License, or\n"
					  "(at your option) any later version.\n\n"
					  "This program is distributed in the hope that it will be useful,\n"
					  "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
					  "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
					  "GNU General Public License for more details.\n\n"
					  "You should have received a copy of the GNU General Public License\n"
					  "along with this program; if not, write to the Free Software\n"
					  "Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA\n"
					  , -1);

	scrolled_window = gtk_scrolled_window_new(NULL, NULL);

	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_widget_set_size_request(GTK_WIDGET(scrolled_window), 300, 50);
	gtk_container_add(GTK_CONTAINER(scrolled_window), license_view);

	gtk_container_add(GTK_CONTAINER(frame), scrolled_window);

	gtk_dialog_add_button(GTK_DIALOG(dialog), "OK", GTK_RESPONSE_OK);

	return dialog;
}

int kudu_about_dialog_run(void)
{
	GtkWidget *dialog;

	dialog = kudu_about_dialog();
	gtk_widget_show_all(dialog);

	gtk_dialog_run(GTK_DIALOG(dialog));

	gtk_widget_destroy(dialog);

	return TRUE;
}


