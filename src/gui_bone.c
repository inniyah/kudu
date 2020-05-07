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
#include "gui_bone.h"

/*void kudu_gui_bone_set_h_angle(GtkWidget *widget, KuduBone *bone)
{
	bone->laxis_rot[0] = gtk_spin_button_get_value(GTK_SPIN_BUTTON(widget));
	kudu_bone_update(bone);
}

void kudu_gui_bone_set_v_angle(GtkWidget *widget, KuduBone *bone)
{
	bone->laxis_rot[1] = gtk_spin_button_get_value(GTK_SPIN_BUTTON(widget));
	kudu_bone_update(bone);
}

void kudu_gui_bone_set_r_angle(GtkWidget *widget, KuduBone *bone)
{
	bone->laxis_rot[2] = gtk_spin_button_get_value(GTK_SPIN_BUTTON(widget));
	kudu_bone_update(bone);
}*/

void kudu_gui_bone_set_name(GtkWidget *widget, KuduBone *bone)
{
	sprintf(bone->name, gtk_entry_get_text(GTK_ENTRY(widget)));
}

void kudu_gui_bone_set_rotation(GtkWidget *widget, KuduBone *bone)
{
	bone->rotation = gtk_spin_button_get_value(GTK_SPIN_BUTTON(widget));
	kudu_bone_update(bone);
}

void kudu_gui_bone_set_length(GtkWidget *widget, KuduBone *bone)
{
	bone->length = gtk_spin_button_get_value(GTK_SPIN_BUTTON(widget));
	kudu_bone_update(bone);
}

void kudu_gui_bone_clear_lmatrix(GtkWidget *widget, KuduBone *bone)
{
	kudu_math_matrix_set_identity(bone->lmatrix);
	kudu_bone_update(bone);
}

void kudu_gui_bone_clear_gmatrix(GtkWidget *widget, KuduBone *bone)
{
	kudu_math_matrix_set_identity(bone->gmatrix);
	kudu_bone_update(bone);
}

void kudu_gui_bone_edit_request_refresh(GtkWidget *widget, gpointer data)
{
	program.request_refresh = TRUE;
}

int kudu_gui_bone_properties_move(GtkWidget *widget, gpointer data)
{
	GtkWindow *window = GTK_WINDOW(widget);

	if (kudu_options_enabled(KO_BONE_DIALOG_POSITION)) {
		int pos[2];
		gtk_window_get_position(window, &pos[0], &pos[1]);
		kudu_options_set_int(KO_BONE_DIALOG_POSITION, 2, pos);
	}

}

int kudu_gui_bone_properties_edit(KuduBone *bone)
{
	if (bone == NULL) return FALSE;

	GtkWidget *dialog;
	GtkWidget *bone_name, *bone_v_angle, *bone_h_angle, *bone_r_angle, *bone_length, *bone_rotation, *button;
	int lock_name = FALSE;
	GtkWidget *hbox;
	GtkWidget *label;
	GtkObject *h_angle_adjustment, *v_angle_adjustment, *r_angle_adjustment, *length_adjustment, *rotation;

	lock_name = (kudu_options_enabled(KO_BONE_DIALOG_LOCK_NAME));

/*	h_angle_adjustment = gtk_adjustment_new(bone->laxis_rot[0], 0, 360, 1.0, 0.01, 0);
	v_angle_adjustment = gtk_adjustment_new(bone->laxis_rot[1], 0, 360, 1.0, 0.01, 0);
	r_angle_adjustment = gtk_adjustment_new(bone->laxis_rot[2], 0, 360, 1.0, 0.01, 0);*/
	rotation = gtk_adjustment_new(bone->rotation, 0, 360, 1.0, 0.01, 0);
	length_adjustment = gtk_adjustment_new(bone->length, 0, 1000000, 0.1, 0.01, 0);

	dialog = gtk_dialog_new();
	gtk_dialog_set_has_separator(GTK_DIALOG(dialog), FALSE);
	gtk_window_set_title(GTK_WINDOW(dialog), "Selected Bone Properties");

	hbox = gtk_hbox_new(FALSE, 10);
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), hbox, TRUE, TRUE, 5);
	gtk_widget_show(hbox);

	label = gtk_label_new("Bone Name:");
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);
	gtk_widget_show(label);

	if (!lock_name) {
		bone_name = gtk_entry_new();
		gtk_entry_set_max_length(GTK_ENTRY(bone_name), 32);
		gtk_box_pack_start(GTK_BOX(hbox), bone_name, TRUE, TRUE, 0);
		gtk_entry_set_text(GTK_ENTRY(bone_name), bone->name);
		g_signal_connect(G_OBJECT(bone_name), "changed", G_CALLBACK(kudu_gui_bone_set_name), bone);
		g_signal_connect_after(G_OBJECT(bone_name), "changed", G_CALLBACK(kudu_gui_bone_edit_request_refresh), NULL);
		gtk_widget_show(bone_name);
	} else {
		bone_name = gtk_label_new(bone->name);
		gtk_box_pack_start(GTK_BOX(hbox), bone_name, FALSE, FALSE, 0);
		gtk_widget_show(bone_name);
	}

/*	hbox = gtk_hbox_new(FALSE, 10);
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), hbox, TRUE, TRUE, 5);
	gtk_widget_show(hbox);

	label = gtk_label_new("Bone H-Angle:");
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);
	gtk_widget_show(label);

	bone_h_angle = gtk_spin_button_new(GTK_ADJUSTMENT(h_angle_adjustment), 1.0, 2);
	gtk_box_pack_start(GTK_BOX(hbox), bone_h_angle, TRUE, TRUE, 0);
	g_signal_connect(G_OBJECT(bone_h_angle), "value_changed", G_CALLBACK(kudu_gui_bone_set_h_angle), bone);
	g_signal_connect_after(G_OBJECT(bone_h_angle), "value_changed",
			G_CALLBACK(kudu_gui_bone_edit_request_refresh), NULL);
	gtk_spin_button_set_wrap(GTK_SPIN_BUTTON(bone_h_angle), TRUE);
	gtk_widget_show(bone_h_angle);


	hbox = gtk_hbox_new(FALSE, 10);
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), hbox, TRUE, TRUE, 5);
	gtk_widget_show(hbox);

	label = gtk_label_new("Bone V-Angle:");
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);
	gtk_widget_show(label);

	bone_v_angle = gtk_spin_button_new(GTK_ADJUSTMENT(v_angle_adjustment), 1.0, 2);
	gtk_box_pack_start(GTK_BOX(hbox), bone_v_angle, TRUE, TRUE, 0);
	g_signal_connect(G_OBJECT(bone_v_angle), "value_changed", G_CALLBACK(kudu_gui_bone_set_v_angle), bone);
	g_signal_connect_after(G_OBJECT(bone_v_angle), "value_changed",
		G_CALLBACK(kudu_gui_bone_edit_request_refresh), NULL);
	gtk_spin_button_set_wrap(GTK_SPIN_BUTTON(bone_v_angle), TRUE);
	gtk_widget_show(bone_v_angle);


	hbox = gtk_hbox_new(FALSE, 10);
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), hbox, TRUE, TRUE, 5);
	gtk_widget_show(hbox);

	label = gtk_label_new("Bone R-Angle:");
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);
	gtk_widget_show(label);

	bone_r_angle = gtk_spin_button_new(GTK_ADJUSTMENT(r_angle_adjustment), 1.0, 2);
	gtk_box_pack_start(GTK_BOX(hbox), bone_r_angle, TRUE, TRUE, 0);
	g_signal_connect(G_OBJECT(bone_r_angle), "value_changed", G_CALLBACK(kudu_gui_bone_set_r_angle), bone);
	g_signal_connect_after(G_OBJECT(bone_r_angle), "value_changed",
		G_CALLBACK(kudu_gui_bone_edit_request_refresh), NULL);
	gtk_spin_button_set_wrap(GTK_SPIN_BUTTON(bone_r_angle), TRUE);
	gtk_widget_show(bone_r_angle);*/

	hbox = gtk_hbox_new(FALSE, 10);
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), hbox, TRUE, TRUE, 5);

	label = gtk_label_new("Bone Rotation:");
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);

	bone_rotation = gtk_spin_button_new(GTK_ADJUSTMENT(rotation), 1.0, 2);
	gtk_box_pack_start(GTK_BOX(hbox), bone_rotation, TRUE, TRUE, 0);
	g_signal_connect(G_OBJECT(bone_rotation), "value_changed", G_CALLBACK(kudu_gui_bone_set_rotation), bone);
	g_signal_connect_after(G_OBJECT(bone_rotation), "value_changed", G_CALLBACK(kudu_gui_bone_edit_request_refresh), NULL);
	gtk_spin_button_set_wrap(GTK_SPIN_BUTTON(bone_rotation), TRUE);


	hbox = gtk_hbox_new(FALSE, 10);
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), hbox, TRUE, TRUE, 5);
	gtk_widget_show(hbox);

	label = gtk_label_new("Bone Length:");
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);
	gtk_widget_show(label);

	bone_length = gtk_spin_button_new(GTK_ADJUSTMENT(length_adjustment), 1.0, 2);
	gtk_box_pack_start(GTK_BOX(hbox), bone_length, TRUE, TRUE, 0);
	g_signal_connect(G_OBJECT(bone_length), "value_changed", G_CALLBACK(kudu_gui_bone_set_length), bone);
	g_signal_connect_after(G_OBJECT(bone_length), "value_changed",
		G_CALLBACK(kudu_gui_bone_edit_request_refresh), NULL);
	gtk_widget_show(bone_length);


	hbox = gtk_hbox_new(FALSE, 10);
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), hbox, TRUE, TRUE, 5);

	button = gtk_button_new_with_label("Clear Local Rotations");
	gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(kudu_gui_bone_clear_lmatrix), bone);
	g_signal_connect_after(G_OBJECT(button), "clicked", G_CALLBACK(kudu_gui_bone_edit_request_refresh), NULL);

	button = gtk_button_new_with_label("Clear Global Rotations");
	gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(kudu_gui_bone_clear_gmatrix), bone);
	g_signal_connect_after(G_OBJECT(button), "clicked", G_CALLBACK(kudu_gui_bone_edit_request_refresh), NULL);


	g_signal_connect(G_OBJECT(dialog), "configure_event", G_CALLBACK(kudu_gui_bone_properties_move), NULL);

	if (kudu_options_enabled(KO_BONE_DIALOG_POSITION)) {
		int pos[2];
		kudu_options_get_int(KO_BONE_DIALOG_POSITION, 2, pos);
		gtk_window_move(GTK_WINDOW(dialog), pos[0], pos[1]);
	}

	gtk_dialog_add_button(GTK_DIALOG(dialog), "Ok", 0);

	gtk_widget_show_all(dialog);
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
}

