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
#include "about.h"

GtkWidget *kudu_about_dialog(void)
{
	GtkWidget *dialog, *logo, *label, *license_view, *scrolled_window, *frame, *tab, *vbox, *credits_view;
	GtkTextBuffer *license, *credits;
	const char *image_dir = kudu_options_get_string(KO_DIR_KUDU_IMAGES);
	char *imgf;

	imgf = (char*)malloc(strlen(image_dir)+50);

	dialog = gtk_dialog_new();
	gtk_window_set_title(GTK_WINDOW(dialog), "About Kudu");

	/* Create "notebook" (aka tab) */
	tab = gtk_notebook_new();
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), tab, TRUE, TRUE, 0);

	/* Copyright info tab */
	label = gtk_label_new("Copyright");
	vbox = gtk_vbox_new(FALSE, 0);
	gtk_notebook_append_page(GTK_NOTEBOOK(tab), vbox, label);

	/* Contents of "Copyright info" tab */

	/* Logo */
	sprintf(imgf, "%slogo.png\0", image_dir);
	logo = gtk_image_new_from_file(imgf);
	gtk_box_pack_start(GTK_BOX(vbox), logo, FALSE, FALSE, 0);

	/* Version string */
	label = gtk_label_new(KUDU_VERSION_STRING);
	gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 0);

	/* Author info (aka: me!) */
	label = gtk_label_new("Copyright(C) "KUDU_VERSION_DATE_Y" Daniel Pekelharing");
	gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 5);
	label = gtk_label_new("<redarrow@users.sourceforge.net>");
	gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 5);

	/* GPL license header */
	frame = gtk_frame_new(NULL);
	gtk_box_pack_start(GTK_BOX(vbox), frame, TRUE, TRUE, 5);

	license_view = gtk_text_view_new();
	license = gtk_text_view_get_buffer(GTK_TEXT_VIEW(license_view));

	gtk_text_buffer_set_text(license, "Kudu Animator\n"
					  "Copyright(C) "KUDU_VERSION_DATE_Y" Daniel Pekelharing\n\n"
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

	GTK_TEXT_VIEW(license_view)->editable = FALSE;

	scrolled_window = gtk_scrolled_window_new(NULL, NULL);

	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_widget_set_size_request(GTK_WIDGET(scrolled_window), 300, 50);
	gtk_container_add(GTK_CONTAINER(scrolled_window), license_view);

	gtk_container_add(GTK_CONTAINER(frame), scrolled_window);


	/* Credits tab */
	label = gtk_label_new("Credits");
	vbox = gtk_vbox_new(FALSE, 0);
	gtk_notebook_append_page(GTK_NOTEBOOK(tab), vbox, label);

	frame = gtk_frame_new(NULL);
	gtk_box_pack_start(GTK_BOX(vbox), frame, TRUE, TRUE, 5);

	credits_view = gtk_text_view_new();
	credits = gtk_text_view_get_buffer(GTK_TEXT_VIEW(credits_view));

	gtk_text_buffer_set_text(credits, "Lead Programmer / Project Coordinator:\n"
					  "Daniel Pekelharing <redarrow@users.sourceforge.net>\n\n"
					  "Splash screen logo:\n"
					  "Manuel Ponce <manueldezyns@hotmail.com>\n\n"
					  , -1);

	GTK_TEXT_VIEW(credits_view)->editable = FALSE;

	scrolled_window = gtk_scrolled_window_new(NULL, NULL);

	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_widget_set_size_request(GTK_WIDGET(scrolled_window), 300, 50);

	gtk_container_add(GTK_CONTAINER(scrolled_window), credits_view);
	gtk_container_add(GTK_CONTAINER(frame), scrolled_window);

	gtk_dialog_add_button(GTK_DIALOG(dialog), "OK", GTK_RESPONSE_OK);

	free(imgf);

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

/* Print simple version info out to command line */
int kudu_about_printout(void)
{
	printf("\nKudu Animator\n\n"KUDU_VERSION_STRING"\nCopyright(C) "KUDU_VERSION_DATE_Y" Daniel Pekelharing <redarrow@users.sourceforge.net>\n\n");
	printf("Compiled with Python support: ");
	#ifdef SUPPORT_PYTHON
		printf("Yes\n");
	#else
		printf("No\n");
	#endif
	printf("\n");

	return TRUE;
}

/* Static varibles for splash screen */
static GtkWidget *splash_win = NULL, *status = NULL;
static GTimer *splash_time = NULL;
/* Delayed gets set to true if kudu_about_splash_close() has been called when the splash screen has been displayed for less than a few seconds... */
static int delayed = FALSE;

/* Creates the splash window */
GtkWidget *kudu_about_splash_window(void)
{
	GtkWidget *window, *splash_logo, *vbox, *label;
	GdkColor color;
	const char *image_dir = kudu_options_get_string(KO_DIR_KUDU_IMAGES);
	char *imgf;

	imgf = (char*)malloc(strlen(image_dir)+50);

	color.red = 65535;
	color.blue = 65535;
	color.green = 65535;

	gtk_window_set_auto_startup_notification(FALSE);

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_decorated(GTK_WINDOW(window), FALSE);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER_ALWAYS);

	gtk_widget_modify_bg(window, GTK_STATE_NORMAL, &color);

	vbox = gtk_vbox_new(FALSE, 10);
	gtk_container_add(GTK_CONTAINER(window), vbox);

	sprintf(imgf, "%ssplash.png\0", image_dir);
	splash_logo = gtk_image_new_from_file(imgf);
	gtk_box_pack_start(GTK_BOX(vbox), splash_logo, TRUE, TRUE, 10);

	label = gtk_label_new(KUDU_VERSION_STRING);
	gtk_label_set_markup(GTK_LABEL(label), "<span size=\"large\">"KUDU_VERSION_STRING"</span>");
	gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 10);

	status = gtk_label_new("Loading...");
	gtk_box_pack_start(GTK_BOX(vbox), status, FALSE, FALSE, 10);

	/*label = gtk_label_new("Copyright(C) "KUDU_VERSION_DATE_Y" Daniel Pekelharing");
	gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 5);

	label = gtk_label_new("<redarrow@users.sourceforge.net>");
	gtk_label_set_markup(GTK_LABEL(label), "<i>redarrow@users.sourceforge.net</i>");
	gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 5);*/

	gtk_window_set_auto_startup_notification(TRUE);

	free(imgf);

	return window;
}

/* Initializes and shows the splash window */
void kudu_about_splash_display(void)
{
	splash_win = kudu_about_splash_window();

	gtk_widget_show_all(splash_win);

	/* Start the timer... */
	splash_time = g_timer_new();
}

/* Sets parent to be the parent window of the splash window - so that the splash window remains on top */
void kudu_about_splash_set_parent(GtkWindow *parent)
{
	if (splash_win == NULL) return;
	gtk_window_set_transient_for(GTK_WINDOW(splash_win), parent);
}

/* Closes the splash screen */
gboolean kudu_about_splash_close(void)
{
	if (splash_win == NULL) return;

	gulong ms;

	/* Determine how long the splash screen has been displayed for...
	   Splash screen should be visible for at least a few seconds */
	if (g_timer_elapsed(splash_time, &ms) > 3) {
		g_timer_stop(splash_time);
		g_timer_destroy(splash_time);
		gtk_widget_destroy(splash_win);
		splash_win = NULL;
		return FALSE;
	} else {
		g_timeout_add(1000, (GSourceFunc)kudu_about_splash_close, NULL);
		kudu_about_splash_message("Initialization complete.");
		delayed = TRUE;
		return FALSE;
	}

	return FALSE;
}

/* Sets the currently displayed "loading" message in the splash screen */
void kudu_about_splash_message(char *msg)
{
	if (splash_win == NULL) return;

	gtk_label_set_text(GTK_LABEL(status), msg);

	/* If delayed is True, then kudu_about_splash_close() has already been called in which case
	   we're already in the main loop... */
	if (!delayed) while (gtk_events_pending()) gtk_main_iteration();
}



