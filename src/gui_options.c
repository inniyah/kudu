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
#include "gui_options.h"

/* Callback function for a checkbox (boolean) option */
void kudu_gui_options_checkbox_callback(GtkWidget *checkbox, gpointer data)
{
	KuduOption option = GPOINTER_TO_INT(data);
	int offset = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(checkbox), "offset"));
	int enabled = (int)gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbox));

	if (offset < 0) {
		if (enabled) kudu_options_enable(option);
		else	kudu_options_disable(option);
	} else {
		if (enabled) kudu_options_set_int_no(option, offset, TRUE);
		else	kudu_options_set_int_no(option, offset, FALSE);
	}
}

/* Create a simple gtk toggle button (checkbox) for a boolean type option (i.e. enabled / disabled) */
GtkWidget *kudu_gui_options_new_checkbox(char *label, KuduOption option, int offset)
{
	GtkWidget *checkbox;
	int i;

	checkbox = gtk_check_button_new_with_label(label);

	if (offset < 0) {
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbox), (gboolean)kudu_options_enabled(option));
	} else {
		i = kudu_options_get_int_no(option, offset);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbox), (gboolean)i);
	}

	g_object_set_data(G_OBJECT(checkbox), "offset", GINT_TO_POINTER(offset));

	g_signal_connect(G_OBJECT(checkbox), "toggled", G_CALLBACK(kudu_gui_options_checkbox_callback), GINT_TO_POINTER(option));

	return checkbox;
}

/* Callback for a toggle button using a bitmask */
void kudu_gui_options_checkbox_bitmask_callback(GtkWidget *checkbox, gpointer data)
{
	KuduOption option = GPOINTER_TO_INT(data);
	int offset = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(checkbox), "offset"));
	int mask = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(checkbox), "mask"));
	int enabled = (int)gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbox));
	int opt = kudu_options_get_int_no(option, offset);

	if (enabled) kudu_options_set_int_no(option, offset, (opt | mask));
	else kudu_options_set_int_no(option, offset, (opt ^ (opt & mask)));
}

/* Create a gtk toggle button, for a boolean type option using a bitmask */
GtkWidget *kudu_gui_options_new_checkbox_bitmask(char *label, KuduOption option, int offset, int mask)
{
	GtkWidget *checkbox;
	int opt = kudu_options_get_int_no(option, offset);

	checkbox = gtk_check_button_new_with_label(label);

	if ((opt & mask) == mask) gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbox), TRUE);
	else gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbox), FALSE);

	g_object_set_data(G_OBJECT(checkbox), "offset", GINT_TO_POINTER(offset));
	g_object_set_data(G_OBJECT(checkbox), "mask", GINT_TO_POINTER(mask));

	g_signal_connect(G_OBJECT(checkbox), "toggled", G_CALLBACK(kudu_gui_options_checkbox_bitmask_callback), GINT_TO_POINTER(option));

	return checkbox;
}


/* Callback function for a colour button */
void kudu_gui_options_colour_button_callback(GtkWidget *button, gpointer data)
{
	KuduOption option = GPOINTER_TO_INT(data);
	GdkColor colour;
	int offset = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button), "offset"));

	gtk_color_button_get_color(GTK_COLOR_BUTTON(button), &colour);

	kudu_options_set_float_no(option, offset, (float)colour.red / 65535);
	kudu_options_set_float_no(option, offset+1, (float)colour.green / 65535);
	kudu_options_set_float_no(option, offset+2, (float)colour.blue / 65535);
}

/* Create a colour button which allows a user to pick a colour
   creates a GtkLabel, GtkColorButton and packs them into a GtkHBox */
GtkWidget *kudu_gui_options_new_colour_button(char *label, KuduOption option, int offset)
{
	GtkWidget *hbox, *tag, *button;
	GdkColor colour;

	colour.red = (guint16)(kudu_options_get_float_no(option, offset) * 65535);
	colour.green = (guint16)(kudu_options_get_float_no(option, offset+1) * 65535);
	colour.blue = (guint16)(kudu_options_get_float_no(option, offset+2) * 65535);

	hbox = gtk_hbox_new(FALSE, 0);
	tag = gtk_label_new(label);
	gtk_box_pack_start(GTK_BOX(hbox), tag, FALSE, FALSE, 5);

	button = gtk_color_button_new_with_color(&colour);
	gtk_box_pack_end(GTK_BOX(hbox), button, FALSE, FALSE, 5);

	g_object_set_data(G_OBJECT(button), "offset", GINT_TO_POINTER(offset));
	g_signal_connect(G_OBJECT(button), "color-set", G_CALLBACK(kudu_gui_options_colour_button_callback), GINT_TO_POINTER(option));

	return hbox;
}

/* Callback function for a spin button */
void kudu_gui_options_spin_button_callback(GtkWidget *button, gpointer data)
{
	KuduOption option = GPOINTER_TO_INT(data);
	int offset = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button), "offset"));
	int otype, iv;
	float fv;

	otype = kudu_options_get_guess_type(option);

	if (otype == KO_TYPE_INT) {
		iv = (int)gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(button));
		kudu_options_set_int_no(option, offset, iv);
	} else if (otype == KO_TYPE_FLOAT) {
		fv = (float)gtk_spin_button_get_value(GTK_SPIN_BUTTON(button));
		kudu_options_set_float_no(option, offset, fv);
	}
}

/* Create a spin button
   creates a GtkLabel, GtkSpinButton and packs them into a GtkHBox */
GtkWidget *kudu_gui_options_new_spin_button(char *label, float min, float max, KuduOption option, int offset)
{
	GtkWidget *hbox, *button, *tag;
	GtkAdjustment *adjust;
	float fv;
	int iv, otype;

	otype = kudu_options_get_guess_type(option);

	if (otype == KO_TYPE_INT) {
		iv = kudu_options_get_int_no(option, offset);
		adjust = (GtkAdjustment*)gtk_adjustment_new((gdouble)iv, (gdouble)min, (gdouble)max, 1, 1, 10);
		button = gtk_spin_button_new(adjust, 1.0, 0);
	} else if (otype == KO_TYPE_FLOAT) {
		fv = kudu_options_get_float_no(option, offset);
		adjust = (GtkAdjustment*)gtk_adjustment_new((gdouble)fv, (gdouble)min, (gdouble)max, 0.1, 1, 1);
		button = gtk_spin_button_new(adjust, 1.0, 2);
	} else return NULL;

	g_object_set_data(G_OBJECT(button), "offset", GINT_TO_POINTER(offset));

	g_signal_connect(G_OBJECT(button), "value-changed", G_CALLBACK(kudu_gui_options_spin_button_callback), GINT_TO_POINTER(option));

	hbox = gtk_hbox_new(FALSE, 0);
	tag = gtk_label_new(label);
	gtk_box_pack_start(GTK_BOX(hbox), tag, FALSE, FALSE, 5);
	gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, FALSE, 5);

	return hbox;
}

/* Callback for a combo box */
void kudu_gui_options_combo_box_callback(GtkWidget *combobox, gpointer data)
{
	KuduOption option = GPOINTER_TO_INT(data);
	int num, offset = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(combobox), "offset"));
	GtkTreeModel *model;
	GtkTreeIter iter;

	model = gtk_combo_box_get_model(GTK_COMBO_BOX(combobox));

	gtk_combo_box_get_active_iter(GTK_COMBO_BOX(combobox), &iter);

	gtk_tree_model_get(model, &iter, 1, &num, -1);

	kudu_options_set_int_no(option, offset, num);
}


/* Create a combo box
   creates a GtkLabel, GtkComboBox and packs them into a GtkHBox */
GtkWidget *kudu_gui_options_new_combo_box(char *label, KuduOption option, int offset, ...)
{
	GtkWidget *hbox, *combobox, *tag;
	GtkListStore *list;
	GtkTreeIter iter;
	GtkCellRenderer *renderer;
	
	int num, opt, act = FALSE;
	char *text = NULL;
	va_list params;

	va_start(params, offset);

	opt = kudu_options_get_int_no(option, offset);

	list = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_INT);

	combobox = gtk_combo_box_new_with_model(GTK_TREE_MODEL(list));

	renderer = gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(combobox), renderer, TRUE);
	gtk_cell_layout_add_attribute(GTK_CELL_LAYOUT(combobox), renderer, "text", 0);

	do {
		text = (char*)va_arg(params, char*);
		if (text != NULL) {
			num = va_arg(params, int);
			gtk_list_store_append(list, &iter);
			gtk_list_store_set(list, &iter, 0, text, 1, num, -1);

			if (num == opt) {
				gtk_combo_box_set_active_iter(GTK_COMBO_BOX(combobox), &iter);
				act = TRUE;
			}
		}
	} while (text != NULL);

	if (!act) gtk_combo_box_set_active_iter(GTK_COMBO_BOX(combobox), &iter);


	hbox = gtk_hbox_new(FALSE, 0);
	tag = gtk_label_new(label);
	gtk_box_pack_start(GTK_BOX(hbox), tag, FALSE, FALSE, 5);
	gtk_box_pack_start(GTK_BOX(hbox), combobox, FALSE, FALSE, 5);

	g_object_set_data(G_OBJECT(combobox), "offset", GINT_TO_POINTER(offset));

	g_signal_connect(G_OBJECT(combobox), "changed", G_CALLBACK(kudu_gui_options_combo_box_callback), GINT_TO_POINTER(option));

	return hbox;
}

/* Callback for an entry */
void kudu_gui_options_entry_callback(GtkWidget *entry, gpointer data)
{
	KuduOption option = GPOINTER_TO_INT(data);
	int offset = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(entry), "offset"));
	int otype, maxlen;
	char c, *str;

	maxlen = gtk_entry_get_max_length(GTK_ENTRY(entry));
	str = (char*)gtk_entry_get_text(GTK_ENTRY(entry));

	if (maxlen > 1) {
		kudu_options_set_string(option, str);
	} else {
		otype = kudu_options_get_guess_type(option);
		c = str[0];

		if (otype == KO_TYPE_INT) {
			kudu_options_set_int_no(option, offset, (int)c);
		} else if (otype == KO_TYPE_CHAR) {
			kudu_options_set_char_no(option, offset, c);
		}
	}
}


/* Create an entry box
   creates a GtkEntry, GtkLabel and packs them into a GtkHBox */
GtkWidget *kudu_gui_options_new_entry(char *label, KuduOption option, int offset, int maxlen)
{
	GtkWidget *hbox, *entry, *tag;
	char c, *str;
	int otype;

	entry = gtk_entry_new();
	gtk_entry_set_max_length(GTK_ENTRY(entry), maxlen);

	if (maxlen > 1) {
		str = kudu_options_get_string(option);
		gtk_entry_set_text(GTK_ENTRY(entry), (gchar*)str);
	} else {
		otype = kudu_options_get_guess_type(option);

		if (otype == KO_TYPE_INT) {
			c = (char)kudu_options_get_int_no(option, offset);
		} else if (otype == KO_TYPE_CHAR) {
			c = kudu_options_get_char_no(option, offset);
		}

		gtk_entry_set_text(GTK_ENTRY(entry), (gchar*)&c);
	}

	hbox = gtk_hbox_new(FALSE, 0);
	tag = gtk_label_new(label);
	gtk_box_pack_start(GTK_BOX(hbox), tag, FALSE, FALSE, 5);
	gtk_box_pack_start(GTK_BOX(hbox), entry, FALSE, FALSE, 5);

	g_object_set_data(G_OBJECT(entry), "offset", GINT_TO_POINTER(offset));

	g_signal_connect(G_OBJECT(entry), "changed", G_CALLBACK(kudu_gui_options_entry_callback), GINT_TO_POINTER(option));

	return hbox;
}

/* Navigation mode profiles */
int nav_profile[NAVI_NUM_PROFILES][20] = {  {MOUSE_LEFT_BUTTON, MOUSE_RIGHT_BUTTON,	/* Mirai */
			   MOUSE_MIDDLE_BUTTON, 0, CAMERA_MODE_FIXED, 0, 0, 0,
			   MOUSE_USE_KEY, 0, CAMERA_MODE_TUMBLE, 0, 0, 113,
			   MOUSE_MIDDLE_BUTTON, 0, CAMERA_MODE_ANY, 1, 0, 0},

			  {MOUSE_LEFT_BUTTON, MOUSE_RIGHT_BUTTON,			/* Nendo */
			   MOUSE_MIDDLE_BUTTON, 0, CAMERA_MODE_FIXED, 0, 0, 0,
			   MOUSE_USE_KEY, 0, CAMERA_MODE_TUMBLE, 0, 0, 113,
			   MOUSE_MIDDLE_BUTTON, 0, CAMERA_MODE_TUMBLE, 1, 0, 0},

			  {MOUSE_LEFT_BUTTON, MOUSE_RIGHT_BUTTON,			/* Maya */
			   MOUSE_LEFT_BUTTON, 8, CAMERA_MODE_FIXED, 1, 3, 0,
			   MOUSE_MIDDLE_BUTTON, 8, CAMERA_MODE_FIXED, 1, 3, 0,
			   MOUSE_RIGHT_BUTTON, 8, CAMERA_MODE_FIXED, 1, 5, 0},

			  {MOUSE_LEFT_BUTTON, MOUSE_RIGHT_BUTTON,			/* 3ds Max */
			   MOUSE_MIDDLE_BUTTON, 8, CAMERA_MODE_FIXED, 1, 3, 0,
			   MOUSE_MIDDLE_BUTTON, 0, CAMERA_MODE_FIXED, 1, 3, 0,
			   MOUSE_MIDDLE_BUTTON, 12, CAMERA_MODE_FIXED, 1, 0, 0},

			  {MOUSE_LEFT_BUTTON, MOUSE_RIGHT_BUTTON,			/* Blender */
			   MOUSE_MIDDLE_BUTTON, 0, CAMERA_MODE_FIXED, 1, 3, 0,
			   MOUSE_MIDDLE_BUTTON, 1, CAMERA_MODE_FIXED, 1, 3, 0,
			   MOUSE_MIDDLE_BUTTON, 4, CAMERA_MODE_FIXED, 1, 0, 0},

			  {MOUSE_LEFT_BUTTON, MOUSE_RIGHT_BUTTON,			/* MotionBuilder */
			   MOUSE_LEFT_BUTTON, 5, CAMERA_MODE_FIXED, 1, 3, 0,
			   MOUSE_LEFT_BUTTON, 1, CAMERA_MODE_FIXED, 1, 3, 0,
			   MOUSE_LEFT_BUTTON, 4, CAMERA_MODE_FIXED, 1, 0, 0}
};

void kudu_gui_options_navigation_profile(GtkWidget *combobox, gpointer data)
{
	int num;
	GtkTreeModel *model;
	GtkTreeIter iter;

	model = gtk_combo_box_get_model(GTK_COMBO_BOX(combobox));

	gtk_combo_box_get_active_iter(GTK_COMBO_BOX(combobox), &iter);

	gtk_tree_model_get(model, &iter, 1, &num, -1);

	if (num < NAVI_PROFILE_CUSTOM) {
		kudu_options_set_int(KO_CAMERA_GLOBALS, 2, &nav_profile[num][0]);
		kudu_options_set_int(KO_CAMERA_TUMBLE, 6, &nav_profile[num][2]);
		kudu_options_set_int(KO_CAMERA_TRACK, 6, &nav_profile[num][8]);
		kudu_options_set_int(KO_CAMERA_DOLLY, 6, &nav_profile[num][14]);
	}

}

int kudu_gui_options_navigation_profile_get(void)
{
	int a, cam_config[20];

	kudu_options_get_int(KO_CAMERA_GLOBALS, 2, &cam_config[0]);
	kudu_options_get_int(KO_CAMERA_TUMBLE, 6, &cam_config[2]);
	kudu_options_get_int(KO_CAMERA_TRACK, 6, &cam_config[8]);
	kudu_options_get_int(KO_CAMERA_DOLLY, 6, &cam_config[14]);

	for (a = 0; a < NAVI_NUM_PROFILES; a++) {
		if ((memcmp(cam_config, &nav_profile[a][0], sizeof(int)*20)) == 0) return a;
	}

	return NAVI_PROFILE_CUSTOM;
}

void kudu_gui_options_navigation_dialog(GtkWidget *widget, gpointer data)
{
	GtkWidget *dialog, *frame, *vbox, *vbox2, *hbox, *cbox, *entry, *label, *checkbox, *combobox;
	int num, valid, a;
	GtkTreeModel *model;
	GtkTreeIter iter;

	combobox = data;

	dialog = gtk_dialog_new();
	gtk_window_set_title(GTK_WINDOW(dialog), "Navigation mode Customization");

	/*vbox = gtk_vbox_new(TRUE, 5);
	gtk_container_add(GTK_CONTAINER(dialog), vbox);*/
	vbox = GTK_DIALOG(dialog)->vbox;

	frame = gtk_frame_new("Globals");
	gtk_box_pack_start(GTK_BOX(vbox), frame, TRUE, TRUE, 5);

	vbox2 = gtk_vbox_new(TRUE, 0);
	gtk_container_add(GTK_CONTAINER(frame), vbox2);

	cbox = kudu_gui_options_new_combo_box("Acceptence mouse button:", KO_CAMERA_GLOBALS, 0, "Left button", MOUSE_LEFT_BUTTON,
						"Middle button", MOUSE_MIDDLE_BUTTON, "Right button", MOUSE_RIGHT_BUTTON, NULL);
	gtk_box_pack_start(GTK_BOX(vbox2), cbox, FALSE, FALSE, 0);

	cbox = kudu_gui_options_new_combo_box("Cancel mouse button:", KO_CAMERA_GLOBALS, 1, "Left button", MOUSE_LEFT_BUTTON,
						"Middle button", MOUSE_MIDDLE_BUTTON, "Right button", MOUSE_RIGHT_BUTTON, NULL);
	gtk_box_pack_start(GTK_BOX(vbox2), cbox, FALSE, FALSE, 5);

	/* Tumble mode */
	frame = gtk_frame_new("Tumble mode");
	gtk_box_pack_start(GTK_BOX(vbox), frame, TRUE, TRUE, 5);

	vbox2 = gtk_vbox_new(TRUE, 0);
	gtk_container_add(GTK_CONTAINER(frame), vbox2);

	hbox = gtk_hbox_new(FALSE, 5);
	gtk_box_pack_start(GTK_BOX(vbox2), hbox, FALSE, FALSE, 5);

	cbox = kudu_gui_options_new_combo_box("Activation mouse button or key:", KO_CAMERA_TUMBLE, 0, "Left button", MOUSE_LEFT_BUTTON,
						"Middle button", MOUSE_MIDDLE_BUTTON, "Right button", MOUSE_RIGHT_BUTTON,
						"Key..", MOUSE_USE_KEY,  NULL);
	gtk_box_pack_start(GTK_BOX(hbox), cbox, FALSE, FALSE, 0);

	entry = kudu_gui_options_new_entry("Key (if applicable):", KO_CAMERA_TUMBLE, 5, 1);
	gtk_box_pack_end(GTK_BOX(hbox), entry, FALSE, FALSE, 0);

	/* tumble mode - modifier keys */
	hbox = gtk_hbox_new(FALSE, 5);
	gtk_box_pack_start(GTK_BOX(vbox2), hbox, FALSE, FALSE, 5);

	label = gtk_label_new("Modifier keys: ");
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 5);

	checkbox = kudu_gui_options_new_checkbox_bitmask("Control", KO_CAMERA_TUMBLE, 1, GDK_CONTROL_MASK);
	gtk_box_pack_start(GTK_BOX(hbox), checkbox, FALSE, FALSE, 0);

	checkbox = kudu_gui_options_new_checkbox_bitmask("Alt", KO_CAMERA_TUMBLE, 1, GDK_MOD1_MASK);
	gtk_box_pack_start(GTK_BOX(hbox), checkbox, FALSE, FALSE, 0);

	checkbox = kudu_gui_options_new_checkbox_bitmask("Shift", KO_CAMERA_TUMBLE, 1, GDK_SHIFT_MASK);
	gtk_box_pack_start(GTK_BOX(hbox), checkbox, FALSE, FALSE, 0);

	/* dependency */
	cbox = kudu_gui_options_new_combo_box("Mode Dependency:", KO_CAMERA_TUMBLE, 2, "Any Mode", CAMERA_MODE_ANY, "Fixed Mode", CAMERA_MODE_FIXED,
					      "Track Mode", CAMERA_MODE_TRACK, "Dolly Mode", CAMERA_MODE_DOLLY,  NULL);
	gtk_box_pack_end(GTK_BOX(hbox), cbox, FALSE, FALSE, 0);

	/* tumble mode - inverse x/y */
	hbox = gtk_hbox_new(FALSE, 5);
	gtk_box_pack_start(GTK_BOX(vbox2), hbox, FALSE, FALSE, 5);

	checkbox = kudu_gui_options_new_checkbox_bitmask("Invert Mouse X", KO_CAMERA_TUMBLE, 4, MOUSE_FLAG_REVERSE_X);
	gtk_box_pack_start(GTK_BOX(hbox), checkbox, FALSE, FALSE, 0);

	checkbox = kudu_gui_options_new_checkbox_bitmask("Invert Mouse Y", KO_CAMERA_TUMBLE, 4, MOUSE_FLAG_REVERSE_Y);
	gtk_box_pack_start(GTK_BOX(hbox), checkbox, FALSE, FALSE, 0);

	checkbox = kudu_gui_options_new_checkbox_bitmask("Swop Mouse X/Y", KO_CAMERA_TUMBLE, 4, MOUSE_FLAG_SWOP_XY);
	gtk_box_pack_start(GTK_BOX(hbox), checkbox, FALSE, FALSE, 0);

	cbox = kudu_gui_options_new_combo_box("Activation Style:", KO_CAMERA_TUMBLE, 3, "Toggle", 0, "Momentary", 1, NULL);
	gtk_box_pack_end(GTK_BOX(hbox), cbox, FALSE, FALSE, 0);


	/* Track mode */
	frame = gtk_frame_new("Track mode");
	gtk_box_pack_start(GTK_BOX(vbox), frame, TRUE, TRUE, 5);

	vbox2 = gtk_vbox_new(TRUE, 0);
	gtk_container_add(GTK_CONTAINER(frame), vbox2);

	/*gtk_box_pack_start(GTK_BOX(vbox2), frame, TRUE, TRUE, 5);
	vbox3 = gtk_vbox_new(FALSE, 5);
	gtk_container_add(GTK_CONTAINER(frame), vbox3);*/

	hbox = gtk_hbox_new(FALSE, 5);
	gtk_box_pack_start(GTK_BOX(vbox2), hbox, FALSE, FALSE, 5);

	cbox = kudu_gui_options_new_combo_box("Activation mouse button or key:", KO_CAMERA_TRACK, 0, "Left button", MOUSE_LEFT_BUTTON,
						"Middle button", MOUSE_MIDDLE_BUTTON, "Right button", MOUSE_RIGHT_BUTTON,
						"Key..", MOUSE_USE_KEY,  NULL);
	gtk_box_pack_start(GTK_BOX(hbox), cbox, FALSE, FALSE, 0);

	entry = kudu_gui_options_new_entry("Key (if applicable):", KO_CAMERA_TRACK, 5, 1);
	gtk_box_pack_end(GTK_BOX(hbox), entry, FALSE, FALSE, 0);


	/* track mode - modifier keys */
	hbox = gtk_hbox_new(FALSE, 5);
	gtk_box_pack_start(GTK_BOX(vbox2), hbox, FALSE, FALSE, 5);

	label = gtk_label_new("Modifier keys: ");
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 5);

	checkbox = kudu_gui_options_new_checkbox_bitmask("Control", KO_CAMERA_TRACK, 1, GDK_CONTROL_MASK);
	gtk_box_pack_start(GTK_BOX(hbox), checkbox, FALSE, FALSE, 0);

	checkbox = kudu_gui_options_new_checkbox_bitmask("Alt", KO_CAMERA_TRACK, 1, GDK_MOD1_MASK);
	gtk_box_pack_start(GTK_BOX(hbox), checkbox, FALSE, FALSE, 0);

	checkbox = kudu_gui_options_new_checkbox_bitmask("Shift", KO_CAMERA_TRACK, 1, GDK_SHIFT_MASK);
	gtk_box_pack_start(GTK_BOX(hbox), checkbox, FALSE, FALSE, 0);

	/* dependency */
	cbox = kudu_gui_options_new_combo_box("Mode Dependency:", KO_CAMERA_TRACK, 2, "Any Mode", CAMERA_MODE_ANY, "Fixed Mode", CAMERA_MODE_FIXED,
					      "Tumble Mode", CAMERA_MODE_TUMBLE, "Dolly Mode", CAMERA_MODE_DOLLY,  NULL);
	gtk_box_pack_end(GTK_BOX(hbox), cbox, FALSE, FALSE, 0);

	/* track mode - inverse x/y */
	hbox = gtk_hbox_new(FALSE, 5);
	gtk_box_pack_start(GTK_BOX(vbox2), hbox, FALSE, FALSE, 5);

	checkbox = kudu_gui_options_new_checkbox_bitmask("Invert Mouse X", KO_CAMERA_TRACK, 4, MOUSE_FLAG_REVERSE_X);
	gtk_box_pack_start(GTK_BOX(hbox), checkbox, FALSE, FALSE, 0);

	checkbox = kudu_gui_options_new_checkbox_bitmask("Invert Mouse Y", KO_CAMERA_TRACK, 4, MOUSE_FLAG_REVERSE_Y);
	gtk_box_pack_start(GTK_BOX(hbox), checkbox, FALSE, FALSE, 0);

	checkbox = kudu_gui_options_new_checkbox_bitmask("Swop Mouse X/Y", KO_CAMERA_TRACK, 4, MOUSE_FLAG_SWOP_XY);
	gtk_box_pack_start(GTK_BOX(hbox), checkbox, FALSE, FALSE, 0);

	cbox = kudu_gui_options_new_combo_box("Activation Style:", KO_CAMERA_TRACK, 3, "Toggle", 0, "Momentary", 1, NULL);
	gtk_box_pack_end(GTK_BOX(hbox), cbox, FALSE, FALSE, 0);


	/* Dolly mode */
	frame = gtk_frame_new("Dolly mode");
	gtk_box_pack_start(GTK_BOX(vbox), frame, TRUE, TRUE, 5);

	vbox2 = gtk_vbox_new(TRUE, 0);
	gtk_container_add(GTK_CONTAINER(frame), vbox2);

	/*gtk_box_pack_start(GTK_BOX(vbox2), frame, TRUE, TRUE, 5);
	vbox3 = gtk_vbox_new(FALSE, 5);
	gtk_container_add(GTK_CONTAINER(frame), vbox3);*/

	hbox = gtk_hbox_new(FALSE, 5);
	gtk_box_pack_start(GTK_BOX(vbox2), hbox, FALSE, FALSE, 5);

	cbox = kudu_gui_options_new_combo_box("Activation mouse button or key:", KO_CAMERA_DOLLY, 0, "Left button", MOUSE_LEFT_BUTTON,
						"Middle button", MOUSE_MIDDLE_BUTTON, "Right button", MOUSE_RIGHT_BUTTON,
						"Key..", MOUSE_USE_KEY,  NULL);
	gtk_box_pack_start(GTK_BOX(hbox), cbox, FALSE, FALSE, 0);

	entry = kudu_gui_options_new_entry("Key (if applicable):", KO_CAMERA_DOLLY, 5, 1);
	gtk_box_pack_end(GTK_BOX(hbox), entry, FALSE, FALSE, 0);


	/* dolly mode - modifier keys */
	hbox = gtk_hbox_new(FALSE, 5);
	gtk_box_pack_start(GTK_BOX(vbox2), hbox, FALSE, FALSE, 5);

	label = gtk_label_new("Modifier keys: ");
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 5);

	checkbox = kudu_gui_options_new_checkbox_bitmask("Control", KO_CAMERA_DOLLY, 1, GDK_CONTROL_MASK);
	gtk_box_pack_start(GTK_BOX(hbox), checkbox, FALSE, FALSE, 0);

	checkbox = kudu_gui_options_new_checkbox_bitmask("Alt", KO_CAMERA_DOLLY, 1, GDK_MOD1_MASK);
	gtk_box_pack_start(GTK_BOX(hbox), checkbox, FALSE, FALSE, 0);

	checkbox = kudu_gui_options_new_checkbox_bitmask("Shift", KO_CAMERA_DOLLY, 1, GDK_SHIFT_MASK);
	gtk_box_pack_start(GTK_BOX(hbox), checkbox, FALSE, FALSE, 0);

	cbox = kudu_gui_options_new_combo_box("Mode Dependency:", KO_CAMERA_DOLLY, 2, "Any Mode", CAMERA_MODE_ANY, "Fixed Mode", CAMERA_MODE_FIXED,
					      "Tumble Mode", CAMERA_MODE_TUMBLE, "Track Mode", CAMERA_MODE_TRACK,  NULL);
	gtk_box_pack_end(GTK_BOX(hbox), cbox, FALSE, FALSE, 0);

	/* tumble mode - inverse x/y */
	hbox = gtk_hbox_new(FALSE, 5);
	gtk_box_pack_start(GTK_BOX(vbox2), hbox, FALSE, FALSE, 5);

	checkbox = kudu_gui_options_new_checkbox_bitmask("Invert Mouse X", KO_CAMERA_DOLLY, 4, MOUSE_FLAG_REVERSE_X);
	gtk_box_pack_start(GTK_BOX(hbox), checkbox, FALSE, FALSE, 0);

	checkbox = kudu_gui_options_new_checkbox_bitmask("Invert Mouse Y", KO_CAMERA_DOLLY, 4, MOUSE_FLAG_REVERSE_Y);
	gtk_box_pack_start(GTK_BOX(hbox), checkbox, FALSE, FALSE, 0);

	checkbox = kudu_gui_options_new_checkbox_bitmask("Swop Mouse X/Y", KO_CAMERA_DOLLY, 4, MOUSE_FLAG_SWOP_XY);
	gtk_box_pack_start(GTK_BOX(hbox), checkbox, FALSE, FALSE, 0);

	/*hbox2 = gtk_hbox_new(FALSE, 5);
	gtk_box_pack_start(GTK_BOX(vbox3), hbox2, FALSE, FALSE, 5);*/


	cbox = kudu_gui_options_new_combo_box("Activation Style:", KO_CAMERA_DOLLY, 3, "Toggle", 0, "Momentary", 1, NULL);
	gtk_box_pack_end(GTK_BOX(hbox), cbox, FALSE, FALSE, 0);
	/*************************************************************************************/


	gtk_widget_show_all(dialog);

	gtk_dialog_add_button(GTK_DIALOG(dialog), "Close", GTK_RESPONSE_CLOSE);

	gtk_dialog_run(GTK_DIALOG(dialog));


	/* Make sure the combo box still reflects the correct profile */
	model = gtk_combo_box_get_model(GTK_COMBO_BOX(combobox));
	num = kudu_gui_options_navigation_profile_get();

	valid = gtk_tree_model_get_iter_first(model, &iter);

	while (valid) {
		gtk_tree_model_get(model, &iter, 1, &a, -1);

		if (a == num) {
			gtk_combo_box_set_active_iter(GTK_COMBO_BOX(combobox), &iter);
			valid = FALSE;
		} else valid = gtk_tree_model_iter_next(model, &iter);
	}

	/* destroy */
	gtk_widget_destroy(dialog);
}

void kudu_gui_options_dialog_show(void)
{
	GtkWidget *dialog, *tab, *vbox, *vbox2, *vbox3, *hbox, *hbox2, *label, *frame, *checkbox, *button, *cbox, *entry, *expander;
	GtkListStore *list;
	GtkTreeIter iter;
	GtkCellRenderer *renderer;
	int a;

	dialog = gtk_dialog_new();
	gtk_window_set_title(GTK_WINDOW(dialog), "Preferences");


	/* Create "notebook" (aka tab) */
	tab = gtk_notebook_new();
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), tab, TRUE, TRUE, 0);


	/*************************************************************************************/
	/* "General" tab */
	label = gtk_label_new("General");
	vbox = gtk_vbox_new(FALSE, 0);
	gtk_notebook_append_page(GTK_NOTEBOOK(tab), vbox, label);


	/* "Window" options */
	frame = gtk_frame_new("Window");
	gtk_box_pack_start(GTK_BOX(vbox), frame, TRUE, TRUE, 5);
	vbox2 = gtk_vbox_new(FALSE, 5);
	gtk_container_add(GTK_CONTAINER(frame), vbox2);

	checkbox = kudu_gui_options_new_checkbox("Remember window position", KO_WINDOW_POSITION, -1);
	gtk_box_pack_start(GTK_BOX(vbox2), checkbox, FALSE, FALSE, 5);

	checkbox = kudu_gui_options_new_checkbox("Remember window size", KO_WINDOW_SIZE, -1);
	gtk_box_pack_start(GTK_BOX(vbox2), checkbox, FALSE, FALSE, 5);

	checkbox = kudu_gui_options_new_checkbox("Remember window maximized state", KO_WINDOW_MAXED, -1);
	gtk_box_pack_start(GTK_BOX(vbox2), checkbox, FALSE, FALSE, 5);

	checkbox = kudu_gui_options_new_checkbox("Show splash screen on startup", KO_SHOW_SPLASH, -1);
	gtk_box_pack_start(GTK_BOX(vbox2), checkbox, FALSE, FALSE, 5);

	frame = gtk_frame_new("Misc");
	gtk_box_pack_start(GTK_BOX(vbox), frame, TRUE, TRUE, 5);
	vbox2 = gtk_vbox_new(FALSE, 5);
	gtk_container_add(GTK_CONTAINER(frame), vbox2);

	checkbox = kudu_gui_options_new_checkbox("Share render settings across all modes", KO_SHARE_VIEW_SETTINGS, -1);
	gtk_box_pack_start(GTK_BOX(vbox2), checkbox, FALSE, FALSE, 5);


	/*************************************************************************************/
	/* "Colours" tab */
	label = gtk_label_new("Colours");
	hbox = gtk_hbox_new(TRUE, 5);
	gtk_notebook_append_page(GTK_NOTEBOOK(tab), hbox, label);

	vbox = gtk_vbox_new(FALSE, 5);
	gtk_box_pack_start(GTK_BOX(hbox), vbox, TRUE, TRUE, 0);


	/* Grid / Axes colours */
	/*********************************************************/
	frame = gtk_frame_new("Grid / Axii");
	gtk_box_pack_start(GTK_BOX(vbox), frame, TRUE, TRUE, 5);
	vbox2 = gtk_vbox_new(FALSE, 5);
	gtk_container_add(GTK_CONTAINER(frame), vbox2);

	button = kudu_gui_options_new_colour_button("Grid", KO_GRID_COLOUR, 0);
	gtk_box_pack_start(GTK_BOX(vbox2), button, FALSE, FALSE, 0);

	button = kudu_gui_options_new_colour_button("X Axis", KO_AXES_COLOUR, 0);
	gtk_box_pack_start(GTK_BOX(vbox2), button, FALSE, FALSE, 0);

	button = kudu_gui_options_new_colour_button("Y Axis", KO_AXES_COLOUR, 3);
	gtk_box_pack_start(GTK_BOX(vbox2), button, FALSE, FALSE, 0);

	button = kudu_gui_options_new_colour_button("Z Axis", KO_AXES_COLOUR, 6);
	gtk_box_pack_start(GTK_BOX(vbox2), button, FALSE, FALSE, 0);

	/* Elements (skin) colours */
	/*********************************************************/
	frame = gtk_frame_new("Skin");
	gtk_box_pack_start(GTK_BOX(vbox), frame, TRUE, TRUE, 5);
	vbox2 = gtk_vbox_new(FALSE, 5);
	gtk_container_add(GTK_CONTAINER(frame), vbox2);

	button = kudu_gui_options_new_colour_button("Default", KO_SKIN_COLOUR_DEFAULT, 0);
	gtk_box_pack_start(GTK_BOX(vbox2), button, FALSE, FALSE, 0);

	button = kudu_gui_options_new_colour_button("Selected", KO_SKIN_COLOUR_SELECTED, 0);
	gtk_box_pack_start(GTK_BOX(vbox2), button, FALSE, FALSE, 5);

	/* Bones / joints colours */
	/*********************************************************/
	vbox = gtk_vbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(hbox), vbox, TRUE, TRUE, 0);

	frame = gtk_frame_new("Bones / Joints");
	gtk_box_pack_start(GTK_BOX(vbox), frame, TRUE, TRUE, 5);
	vbox2 = gtk_vbox_new(FALSE, 5);
	gtk_container_add(GTK_CONTAINER(frame), vbox2);

	button = kudu_gui_options_new_colour_button("Bones", KO_BONES_COLOUR_DEFAULT, 0);
	gtk_box_pack_start(GTK_BOX(vbox2), button, FALSE, FALSE, 0);

	button = kudu_gui_options_new_colour_button("Selected bones", KO_BONES_COLOUR_SELECTED, 0);
	gtk_box_pack_start(GTK_BOX(vbox2), button, FALSE, FALSE, 0);

	button = kudu_gui_options_new_colour_button("Children of selected bones", KO_BONES_COLOUR_CHILDREN, 0);
	gtk_box_pack_start(GTK_BOX(vbox2), button, FALSE, FALSE, 0);

	button = kudu_gui_options_new_colour_button("Joints", KO_JOINTS_COLOUR_DEFAULT, 0);
	gtk_box_pack_start(GTK_BOX(vbox2), button, FALSE, FALSE, 0);

	button = kudu_gui_options_new_colour_button("Selected joints", KO_JOINTS_COLOUR_SELECTED, 0);
	gtk_box_pack_start(GTK_BOX(vbox2), button, FALSE, FALSE, 5);



	/*************************************************************************************/
	
	/* "Navigation" tab */
	label = gtk_label_new("Navigation");
	vbox = gtk_vbox_new(FALSE, 5);
	gtk_notebook_append_page(GTK_NOTEBOOK(tab), vbox, label);

	/* Mouse wheel */
	/*********************************************************/
	frame = gtk_frame_new("Mouse scroll wheel");
	gtk_box_pack_start(GTK_BOX(vbox), frame, TRUE, TRUE, 0);
	vbox2 = gtk_vbox_new(FALSE, 5);
	gtk_container_add(GTK_CONTAINER(frame), vbox2);

	hbox = gtk_hbox_new(FALSE, 5);
	gtk_box_pack_start(GTK_BOX(vbox2), hbox, FALSE, FALSE, 0);

	button = kudu_gui_options_new_checkbox("Wheel Zooms", KO_MOUSE_SCROLL_ZOOM, -1);
	gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, FALSE, 0);

	button = kudu_gui_options_new_checkbox("Reverse direction", KO_MOUSE_SCROLL_REVERSE, -1);
	gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, FALSE, 10);

	button = kudu_gui_options_new_spin_button("Zoom factor %", 0, 100, KO_MOUSE_SCROLL_ZOOM, 0);
	gtk_box_pack_start(GTK_BOX(vbox2), button, FALSE, FALSE, 5);

	/* Mouse  */
	/*********************************************************/
/*	frame = gtk_frame_new("General Mouse settings");
	gtk_box_pack_start(GTK_BOX(hbox), frame, TRUE, TRUE, 0);
	vbox2 = gtk_vbox_new(FALSE, 5);
	gtk_container_add(GTK_CONTAINER(frame), vbox2);

	button = kudu_gui_options_new_spin_button("Mouse speed multiplier", 0, 20, KO_MOUSE_SPEED, 0);
	gtk_box_pack_start(GTK_BOX(vbox2), button, FALSE, FALSE, 0);*/


	/* Camera modes  */
	/*********************************************************/
	frame = gtk_frame_new("Viewpoint camera settings");
	gtk_box_pack_start(GTK_BOX(vbox), frame, TRUE, TRUE, 5);

	vbox2 = gtk_vbox_new(FALSE, 5);
	gtk_container_add(GTK_CONTAINER(frame), vbox2);

	button = kudu_gui_options_new_spin_button("Camera Speed multiplier", 0, 20, KO_CAMERA_SPEED, 0);
	gtk_box_pack_start(GTK_BOX(vbox2), button, FALSE, FALSE, 5);

	hbox = gtk_hbox_new(FALSE, 5);
	gtk_box_pack_start(GTK_BOX(vbox2), hbox, FALSE, FALSE, 0);

	label = gtk_label_new("Camera Mode: ");
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 5);
	/* Navigation style combo box */
	/*******************************************/
	list = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_INT);

	cbox = gtk_combo_box_new_with_model(GTK_TREE_MODEL(list));

	renderer = gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(cbox), renderer, TRUE);
	gtk_cell_layout_add_attribute(GTK_CELL_LAYOUT(cbox), renderer, "text", 0);

	a = kudu_gui_options_navigation_profile_get();

	gtk_list_store_append(list, &iter);
	gtk_list_store_set(list, &iter, 0, "Mirai", 1, NAVI_PROFILE_MIRAI, -1);
	if (a == NAVI_PROFILE_MIRAI) gtk_combo_box_set_active_iter(GTK_COMBO_BOX(cbox), &iter);

	gtk_list_store_append(list, &iter);
	gtk_list_store_set(list, &iter, 0, "Nendo", 1, NAVI_PROFILE_NENDO, -1);
	if (a == NAVI_PROFILE_NENDO) gtk_combo_box_set_active_iter(GTK_COMBO_BOX(cbox), &iter);

	gtk_list_store_append(list, &iter);
	gtk_list_store_set(list, &iter, 0, "Maya", 1, NAVI_PROFILE_MAYA, -1);
	if (a == NAVI_PROFILE_MAYA) gtk_combo_box_set_active_iter(GTK_COMBO_BOX(cbox), &iter);

	gtk_list_store_append(list, &iter);
	gtk_list_store_set(list, &iter, 0, "3ds Max", 1, NAVI_PROFILE_3DS_MAX, -1);
	if (a == NAVI_PROFILE_3DS_MAX) gtk_combo_box_set_active_iter(GTK_COMBO_BOX(cbox), &iter);

	gtk_list_store_append(list, &iter);
	gtk_list_store_set(list, &iter, 0, "Blender", 1, NAVI_PROFILE_BLENDER, -1);
	if (a == NAVI_PROFILE_BLENDER) gtk_combo_box_set_active_iter(GTK_COMBO_BOX(cbox), &iter);

	gtk_list_store_append(list, &iter);
	gtk_list_store_set(list, &iter, 0, "MotionBuilder", 1, NAVI_PROFILE_MOTIONBUILDER, -1);
	if (a == NAVI_PROFILE_MOTIONBUILDER) gtk_combo_box_set_active_iter(GTK_COMBO_BOX(cbox), &iter);

	gtk_list_store_append(list, &iter);
	gtk_list_store_set(list, &iter, 0, "Custom...", 1, NAVI_PROFILE_CUSTOM, -1);
	if (a == NAVI_PROFILE_CUSTOM) gtk_combo_box_set_active_iter(GTK_COMBO_BOX(cbox), &iter);

	g_signal_connect(G_OBJECT(cbox), "changed", G_CALLBACK(kudu_gui_options_navigation_profile), NULL);

	gtk_box_pack_start(GTK_BOX(hbox), cbox, FALSE, FALSE, 0);
	/*******************************************/

	label = gtk_label_new("  or .. ");
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 5);

	button = gtk_button_new_with_label("Customize (Advanced)...");
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(kudu_gui_options_navigation_dialog), cbox);
	gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, FALSE, 0);



	gtk_dialog_add_button(GTK_DIALOG(dialog), "Close", GTK_RESPONSE_CLOSE);

	gtk_widget_show_all(dialog);

	gtk_dialog_run(GTK_DIALOG(dialog));

	gtk_widget_destroy(dialog);
}


