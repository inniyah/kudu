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
#include "menus.h"

static GtkAccelGroup *accel_group;
static GCallback menu_callback;
static GSList *last_radio_item_group = NULL;
static GtkWidget *sub_menu_item = NULL;

#define MAX_SUBMENU 3

static char *ap_root = NULL, *ap_sp[MAX_SUBMENU];
static GtkWidget *menu_bar = NULL, *first_menu = NULL, *sub_menu[MAX_SUBMENU];
static int current_menu = 0;
static GSList *radio_group = NULL;
static gulong last_signal_id = 0;

/* Return the most recently created itemi's signal id */
gulong kudu_menu_last_signal_id(void)
{
	return last_signal_id;
}

/* Start a new menu */
int kudu_menu_start_new(GtkAccelGroup *group, GtkWidget *bar, char *root_path, GCallback callback)
{
	int a;

	accel_group = group;
	menu_bar = bar;
	menu_callback = callback;

	/* Setup the root accel path for this menu */
	if (ap_root != NULL) free(ap_root);
	ap_root = (char*)malloc(strlen(root_path)+4);
	sprintf(ap_root, "<%s>/\0", root_path);

	first_menu = NULL;
	radio_group = NULL;
	current_menu = 0;

	for (a = 0; a < MAX_SUBMENU; a++) {
		sub_menu[a] = NULL;
		if (ap_sp[a] != NULL) free(ap_sp[a]);
		ap_sp[a] = NULL;
	}

	return TRUE;
}

/* finish a menu */
GtkWidget *kudu_menu_end(void)
{
	int a;

	if (ap_root != NULL) free(ap_root);
	ap_root = NULL;

	for (a = 0; a < MAX_SUBMENU; a++) {
		if (ap_sp[a] != NULL) free(ap_sp[a]);
		ap_sp[a] = NULL;

		sub_menu[a] = NULL;
	}

	return first_menu;
}


/* Create a new sub menu */
GtkWidget *kudu_menu_push(char *label)
{
	if (current_menu >= MAX_SUBMENU) {
		return NULL;
	}

	GtkWidget *menu = gtk_menu_new();
	if (accel_group != NULL) gtk_menu_set_accel_group(GTK_MENU(menu), accel_group);
	sub_menu[current_menu] = gtk_menu_item_new_with_mnemonic(label);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(sub_menu[current_menu]), menu);

	const char *path = gtk_label_get_text(GTK_LABEL(gtk_bin_get_child(GTK_BIN(sub_menu[current_menu]))));
	if (current_menu == 0) {
		ap_sp[current_menu] = (char*)malloc(strlen(ap_root) + strlen(path) + 1);
		sprintf(ap_sp[current_menu], "%s%s\0", ap_root, path);
		if (menu_bar != NULL) gtk_menu_bar_append(GTK_MENU_BAR(menu_bar), sub_menu[current_menu]);
	} else {
		ap_sp[current_menu] = (char*)malloc(strlen(ap_sp[current_menu-1]) + strlen(path) + 2);
		sprintf(ap_sp[current_menu], "%s/%s\0", ap_sp[current_menu-1], path);
		gtk_menu_shell_append(GTK_MENU_SHELL(gtk_menu_item_get_submenu(GTK_MENU_ITEM(sub_menu[current_menu-1]))), sub_menu[current_menu]);
	}

/*	gtk_menu_set_accel_path(GTK_MENU(menu), ap_sp[current_menu]);*/

	if (first_menu == NULL) first_menu = menu;
	gtk_widget_show(sub_menu[current_menu]);

	current_menu++;

	return sub_menu[current_menu-1];
}

/* Push the menu level without atually creating a new submenu */
int kudu_menu_push_fake(char *path)
{
	if (current_menu >= MAX_SUBMENU) {
		return FALSE;
	}

	sub_menu[current_menu] = sub_menu[current_menu-1];

	if (current_menu == 0) {
		ap_sp[current_menu] = (char*)malloc(strlen(ap_root) + strlen(path) + 1);
		sprintf(ap_sp[current_menu], "%s%s\0", ap_root, path);
	} else {
		ap_sp[current_menu] = (char*)malloc(strlen(ap_sp[current_menu-1]) + strlen(path) + 2);
		sprintf(ap_sp[current_menu], "%s/%s\0", ap_sp[current_menu-1], path);
	}

	current_menu++;

	return TRUE;
}

/* Return "pop" back to the previous menu level */
int kudu_menu_pop(void)
{
	if (current_menu == 0) return FALSE;

	current_menu--;

	if (ap_sp[current_menu] != NULL) free(ap_sp[current_menu]);
	ap_sp[current_menu] = NULL;

	sub_menu[current_menu] = NULL;

	return TRUE;
}

/* Set the accelerator for this item, must be in current menu level */
int kudu_menu_set_accelerator(GtkWidget *item, char *accelerator)
{
	const char *path = gtk_label_get_text(GTK_LABEL(gtk_bin_get_child(GTK_BIN(item))));
	char *sp = (char*)malloc(strlen(ap_sp[current_menu-1]) + strlen(path) + 2);
	sprintf(sp, "%s/%s\0", ap_sp[current_menu-1], path);

	/* Setup an accelerator path for this item, even if it doesn't */
	/* have a default accelerator - user might want to set one */
	gtk_menu_item_set_accel_path(GTK_MENU_ITEM(item), sp);

	/* Accelerator string == NULL don't go further */
	if (accelerator == NULL) {
		free(sp);
		return FALSE;
	}

	guint accel_key;
	GdkModifierType accel_mods;
	gtk_accelerator_parse(accelerator, &accel_key, &accel_mods);
	gtk_accel_map_add_entry(sp, accel_key, accel_mods);
	free(sp);

	return TRUE;
}

/* Add a new menu item at the current level */
GtkWidget *kudu_menu_add_item(char *label, int value, char *accelerator)
{
	if (current_menu == 0) return NULL;

	GtkWidget *menu = gtk_menu_item_get_submenu(GTK_MENU_ITEM(sub_menu[current_menu-1]));

	GtkWidget *item = gtk_menu_item_new_with_mnemonic(label);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
	last_signal_id = g_signal_connect(G_OBJECT(item), "activate", G_CALLBACK(menu_callback), GINT_TO_POINTER(value));

	kudu_menu_set_accelerator(item, accelerator);

	gtk_widget_show(item);

	return item;
}

/* Add a new menu check item at the current level */
GtkWidget *kudu_menu_add_check(char *label, int value, char *accelerator)
{
	if (current_menu == 0) return NULL;

	GtkWidget *menu = gtk_menu_item_get_submenu(GTK_MENU_ITEM(sub_menu[current_menu-1]));

	GtkWidget *item = gtk_check_menu_item_new_with_mnemonic(label);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
	last_signal_id = g_signal_connect(G_OBJECT(item), "activate", G_CALLBACK(menu_callback), GINT_TO_POINTER(value));

	kudu_menu_set_accelerator(item, accelerator);

	gtk_widget_show(item);

	return item;
}

/* Prepare for a new radio group */
/* must be called between all individual radio groups */
int kudu_menu_new_radio_group(void)
{
	radio_group = NULL;
}

/* Add a new menu radio item at the current level */
GtkWidget *kudu_menu_add_radio(char *label, int value, char *accelerator)
{
	if (current_menu == 0) return NULL;

	GtkWidget *menu = gtk_menu_item_get_submenu(GTK_MENU_ITEM(sub_menu[current_menu-1]));

	GtkWidget *item = gtk_radio_menu_item_new_with_mnemonic(radio_group, label);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
	last_signal_id = g_signal_connect(G_OBJECT(item), "activate", G_CALLBACK(menu_callback), GINT_TO_POINTER(value));

	kudu_menu_set_accelerator(item, accelerator);

	radio_group = gtk_radio_menu_item_get_group(GTK_RADIO_MENU_ITEM(item));

	gtk_widget_show(item);

	return item;
}

/* Add a new menu tearoff item at the current level */
GtkWidget *kudu_menu_add_tearoff(void)
{
	if (current_menu == 0) return NULL;

	GtkWidget *menu = gtk_menu_item_get_submenu(GTK_MENU_ITEM(sub_menu[current_menu-1]));

	GtkWidget *item = gtk_tearoff_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);

	gtk_widget_show(item);

	return item;
}

/* Add a new menu seperator item at the current level */
GtkWidget *kudu_menu_add_separator(void)
{
	if (current_menu == 0) return NULL;

	GtkWidget *menu = gtk_menu_item_get_submenu(GTK_MENU_ITEM(sub_menu[current_menu-1]));

	GtkWidget *item = gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);

	gtk_widget_show(item);

	return item;
}










/*********************/
/* Older menu system */
/*********************/

int kudu_gui_menu_set_callback(GCallback callback)
{
	menu_callback = callback;
	return TRUE;
}

int kudu_gui_menu_set_accel_group(GtkAccelGroup* ag)
{
	if (ag == NULL) return FALSE;
	accel_group = ag;
	return TRUE;
}

GSList *kudu_gui_menu_get_last_radio_item_group(void)
{
	return last_radio_item_group;
}

/* Create a menu item */
GtkWidget *kudu_gui_menu_item_new(GtkWidget *menu_shell, char *label, int value, guint accel_key,
			GdkModifierType accel_mods)
{
	GtkWidget *menu_item;

	menu_item = gtk_menu_item_new_with_mnemonic(label);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu_shell), menu_item);
	g_signal_connect(G_OBJECT(menu_item), "activate", G_CALLBACK(menu_callback), GINT_TO_POINTER(value));

	char al[256];

/*	if (accel_key > 0)
		gtk_widget_add_accelerator(menu_item, "activate", accel_group, accel_key, accel_mods, GTK_ACCEL_VISIBLE);*/

	gtk_widget_show(menu_item);

	return menu_item;
}

/* Create a check menu item */
GtkWidget *kudu_gui_check_menu_item_new(GtkWidget *menu_shell, char *label, int value, guint accel_key,
			GdkModifierType accel_mods)
{
	GtkWidget *menu_item;

	menu_item = gtk_check_menu_item_new_with_mnemonic(label);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu_shell), menu_item);
	g_signal_connect(G_OBJECT(menu_item), "activate", G_CALLBACK(menu_callback), GINT_TO_POINTER(value));
/*	if (accel_key > 0)
		gtk_widget_add_accelerator(menu_item, "activate", accel_group, accel_key, accel_mods, GTK_ACCEL_VISIBLE);*/

	gtk_widget_show(menu_item);

	return menu_item;
}

/* Create a radio menu item */
GtkWidget *kudu_gui_radio_menu_item_new(int group, GtkWidget *menu_shell, char *label, int value, guint accel_key,
			GdkModifierType accel_mods)
{
	GtkWidget *menu_item;

	if (group) menu_item = gtk_radio_menu_item_new_with_mnemonic(NULL, label);
	else	menu_item = gtk_radio_menu_item_new_with_mnemonic(last_radio_item_group, label);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu_shell), menu_item);
	g_signal_connect(G_OBJECT(menu_item), "activate", G_CALLBACK(menu_callback), GINT_TO_POINTER(value));
/*	if (accel_key > 0)
		gtk_widget_add_accelerator(menu_item, "activate", accel_group, accel_key, accel_mods, GTK_ACCEL_VISIBLE);*/

	gtk_widget_show(menu_item);
	last_radio_item_group = gtk_radio_menu_item_get_group (GTK_RADIO_MENU_ITEM (menu_item));

	return menu_item;
}

/* Create a menu separator */
GtkSeparatorMenuItem *kudu_gui_menu_separator_new(GtkWidget *menu_shell)
{
	GtkWidget *menu_separator;

	menu_separator = gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu_shell), menu_separator);
	gtk_widget_show(menu_separator);

	return GTK_SEPARATOR_MENU_ITEM(menu_separator);
}

/* Create a menu tearoff */
GtkTearoffMenuItem *kudu_gui_menu_tearoff_new(GtkWidget *menu_shell)
{
	GtkWidget *menu_tearoff;

	menu_tearoff = gtk_tearoff_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu_shell), menu_tearoff);
	gtk_widget_show(menu_tearoff);

	return GTK_TEAROFF_MENU_ITEM(menu_tearoff);
}

/* Create a root menu*/
GtkWidget *kudu_gui_menu_new(GtkWidget *menu_bar, char *label)
{
	GtkWidget *menu;
	char sp[128];

	menu = gtk_menu_new();
	gtk_menu_set_accel_group(GTK_MENU(menu), accel_group);
	sub_menu_item = gtk_menu_item_new_with_mnemonic(label);
	gtk_widget_show(sub_menu_item);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(sub_menu_item), menu);
	gtk_menu_bar_append(GTK_MENU_BAR(menu_bar), sub_menu_item);

	sprintf(sp, "<kudu>/%s", label);
	gtk_menu_set_accel_path(GTK_MENU(menu), sp);

	return menu;
}

/* Create a sub menu*/
GtkWidget *kudu_gui_sub_menu_new(GtkWidget *menu_shell, char *label)
{
	GtkWidget *menu;
	char sp[128];

	menu = gtk_menu_new();
	sub_menu_item = gtk_menu_item_new_with_mnemonic(label);
	gtk_widget_show(sub_menu_item);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu_shell), sub_menu_item);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(sub_menu_item), menu);

	/*sprintf(sp, "<kudu>/%s", label);
	gtk_menu_set_accel_path(GTK_MENU(menu), sp);*/

	return menu;
}

GtkWidget *kudu_gui_sub_menu_get_item(void)
{
	return sub_menu_item;
}

