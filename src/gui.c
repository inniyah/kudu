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
#include "gui.h"

static int old_x, old_y;
static GdkModifierType mask;

int kudu_gui_warp_pointer(GtkWidget *window, gint x, gint y)
{
	Display *display = (Display*)gdk_x11_drawable_get_xdisplay(window->window);
	XWarpPointer(display, None, gdk_x11_drawable_get_xid(window->window), 0, 0, 0, 0, x, y);
	return TRUE;
}

int kudu_gui_center_pointer(void)
{
	static GtkWidget *window = NULL;

	if (window == NULL) window = STACK_GET_WIDGET("main_window");
	if (window == NULL) return FALSE;

	int nx, ny;

	ny = (program.windowHeight / 2);
	nx = (program.windowWidth / 2);

	nx += program.drawing_area_xpos;
	ny += program.drawing_area_ypos;

	kudu_gui_warp_pointer(window, nx, ny);

	return TRUE;
}

int kudu_gui_save_pointer(void)
{
	static GtkWidget *window = NULL;

	if (window == NULL) window = STACK_GET_WIDGET("main_window");
	if (window == NULL) return FALSE;

	gdk_window_get_pointer(window->window, &old_x, &old_y, &mask);

	return TRUE;
}

int kudu_gui_restore_pointer(void)
{
	static GtkWidget *window = NULL;

	if (window == NULL) window = STACK_GET_WIDGET("main_window");
	if (window == NULL) return FALSE;

	kudu_gui_warp_pointer(window, old_x, old_y);

	return TRUE;
}


GdkCursor *kudu_gui_blank_cursor(void)
{
	GdkCursor *cursor = STACK_GET("blank_cursor");
	if (cursor != NULL) return cursor;

	GdkPixmap *pix;
	GdkColor col = {0, 0, 0, 0};
	unsigned char dat[] = {0};

	pix = gdk_bitmap_create_from_data(NULL, dat, 1, 1);
	cursor = gdk_cursor_new_from_pixmap(pix, pix, &col, &col, 0, 0);
	gdk_pixmap_unref(pix);

	kudu_stack_list_add_item("blank_cursor", cursor);
	return cursor;
}

int kudu_gui_hide_cursor(void)
{
	GtkWidget *widget = STACK_GET_WIDGET("main_window");
	if (widget == NULL) return FALSE;
	GdkWindow *window = widget->window;
	if (window == NULL) return FALSE;

	GdkCursor *cursor = kudu_gui_blank_cursor();
	if (cursor == NULL) return FALSE;

	gdk_window_set_cursor(window, cursor);

	return TRUE;
}

int kudu_gui_show_cursor(void)
{
	GtkWidget *widget = STACK_GET_WIDGET("main_window");
	if (widget == NULL) return FALSE;
	GdkWindow *window = widget->window;
	if (window == NULL) return FALSE;

	gdk_window_set_cursor(window, NULL);

	return TRUE;
}

GtkWidget *kudu_gui_open_dialog_callback(char *title, char *filter_name, char *filter, GCallback call_back)
{
	GtkWidget *file_selection;
	GtkFileFilter *file_filter;
	int s = kudu_options_strlen(KO_DIR_LAST_OPEN);

	file_selection = gtk_file_chooser_dialog_new(title, NULL,
		GTK_FILE_CHOOSER_ACTION_OPEN, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_OPEN,
		GTK_RESPONSE_ACCEPT, NULL);

	file_filter = gtk_file_filter_new();
	gtk_file_filter_add_pattern(GTK_FILE_FILTER(file_filter), filter);
	gtk_file_filter_set_name(GTK_FILE_FILTER(file_filter), filter_name);
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(file_selection), GTK_FILE_FILTER(file_filter));
	if (s > 0) gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(file_selection),
		kudu_options_get_string(KO_DIR_LAST_OPEN));

	g_signal_connect(G_OBJECT(file_selection), "response", G_CALLBACK(call_back), NULL);
	g_signal_connect_after(G_OBJECT(file_selection), "response", G_CALLBACK(gtk_widget_destroy), file_selection);

	return file_selection;
}

GtkWidget *kudu_gui_save_dialog_callback(char *title, char *filter_name, char *filter, char *default_name,
		 GCallback call_back)
{
	GtkWidget *file_selection;
	GtkFileFilter *file_filter;
	int s = kudu_options_strlen(KO_DIR_LAST_SAVE);

	file_selection = gtk_file_chooser_dialog_new(title, NULL,
		GTK_FILE_CHOOSER_ACTION_SAVE, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
		GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT, NULL);

	gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(file_selection), default_name);

	file_filter = gtk_file_filter_new();
	gtk_file_filter_add_pattern(GTK_FILE_FILTER(file_filter), filter);
	gtk_file_filter_set_name(GTK_FILE_FILTER(file_filter), filter_name);
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(file_selection), GTK_FILE_FILTER(file_filter));
	if (s > 0) gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(file_selection), 
		kudu_options_get_string(KO_DIR_LAST_SAVE));

	g_signal_connect(G_OBJECT(file_selection), "response", G_CALLBACK(call_back), NULL);
	g_signal_connect_after(G_OBJECT(file_selection), "response", G_CALLBACK(gtk_widget_destroy), file_selection);

	return file_selection;
}

int kudu_gui_message(char *message, int type)
{
	GtkWidget *mdialog;
	int return_val;

	mdialog = gtk_message_dialog_new(NULL, GTK_DIALOG_DESTROY_WITH_PARENT, type,
		GTK_BUTTONS_OK, message);
	return_val = gtk_dialog_run(GTK_DIALOG(mdialog));

	gtk_widget_destroy(mdialog);

	return return_val;
}

int kudu_gui_confirm(char *message, int type)
{
	GtkWidget *mdialog;
	int return_val;

	mdialog = gtk_message_dialog_new(NULL, GTK_DIALOG_DESTROY_WITH_PARENT, type,
		GTK_BUTTONS_OK_CANCEL, message);
	return_val = gtk_dialog_run(GTK_DIALOG(mdialog));

	gtk_widget_destroy(mdialog);

	return return_val;
}

GtkWidget *kudu_gui_open_dialog(char *title, char *filter_name, char *filter)
{
	GtkWidget *file_selection;
	GtkFileFilter *file_filter;
	int s = kudu_options_strlen(KO_DIR_LAST_OPEN);

	file_selection = gtk_file_chooser_dialog_new(title, NULL,
		GTK_FILE_CHOOSER_ACTION_OPEN, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_OPEN,
		GTK_RESPONSE_ACCEPT, NULL);

	file_filter = gtk_file_filter_new();
	gtk_file_filter_add_pattern(GTK_FILE_FILTER(file_filter), filter);
	gtk_file_filter_set_name(GTK_FILE_FILTER(file_filter), filter_name);
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(file_selection), GTK_FILE_FILTER(file_filter));
	if (s > 0) gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(file_selection),
		kudu_options_get_string(KO_DIR_LAST_OPEN));

	return file_selection;
}

GtkWidget *kudu_gui_save_dialog(char *title, char *filter_name, char *filter, char *default_name)
{
	GtkWidget *file_selection;
	GtkFileFilter *file_filter;
	int s = kudu_options_strlen(KO_DIR_LAST_SAVE);

	file_selection = gtk_file_chooser_dialog_new(title, NULL,
		GTK_FILE_CHOOSER_ACTION_SAVE, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
		GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT, NULL);

	gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(file_selection), default_name);

	file_filter = gtk_file_filter_new();
	gtk_file_filter_add_pattern(GTK_FILE_FILTER(file_filter), filter);
	gtk_file_filter_set_name(GTK_FILE_FILTER(file_filter), filter_name);
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(file_selection), GTK_FILE_FILTER(file_filter));
	if (s > 0) gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(file_selection), 
		kudu_options_get_string(KO_DIR_LAST_SAVE));

	return file_selection;
}

int kudu_gui_toolbar_add_separator(GtkWidget *toolbar, int position, int draw, int expand)
{
	if (toolbar == NULL) return FALSE;

	GtkToolItem *tool_item;

	tool_item = gtk_separator_tool_item_new();
	gtk_separator_tool_item_set_draw(GTK_SEPARATOR_TOOL_ITEM(tool_item), draw);
	gtk_tool_item_set_expand(tool_item, expand);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), tool_item, position);

	return TRUE;
}

static GSList *radio_group = NULL;

GtkToolItem *kudu_gui_toolbar_add_radio_button(GtkWidget *toolbar, int position, int new_group, char *label, char *image_f)
{
	if (toolbar == NULL) return NULL;
	if (new_group) radio_group = NULL;

	GtkToolItem *tool_item;
	GtkWidget *image;

	tool_item = gtk_radio_tool_button_new(radio_group);

	if (label != NULL) gtk_tool_button_set_label(GTK_TOOL_BUTTON(tool_item), label);
	if (image_f != NULL) {
		image = gtk_image_new_from_file(image_f);
		gtk_tool_button_set_icon_widget(GTK_TOOL_BUTTON(tool_item), image);
	}

	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), tool_item, position);

	radio_group = gtk_radio_tool_button_get_group(GTK_RADIO_TOOL_BUTTON(tool_item));

	return tool_item;
}





