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
#include "stack.h"


static K_STACK_LIST *stack_list = NULL;

int kudu_stack_list_ready(void)
{
	if (stack_list == NULL) return FALSE;
	else	return TRUE;
}

int kudu_stack_list_init(void)
{
	if (stack_list != NULL) return FALSE;

	stack_list = (K_STACK_LIST*)malloc(sizeof(K_STACK_LIST));
	if (stack_list == NULL) return FALSE;

	stack_list->num_items = 0;
	stack_list->first_item = NULL;
}

K_STACK_ITEM *kudu_stack_item_new(K_STACK_ITEM *previous_item, char *name)
{
	K_STACK_ITEM *stack_item, *old_next_item;

	stack_item = (K_STACK_ITEM*)malloc(sizeof(K_STACK_ITEM));
	if (stack_item == NULL) return NULL;
	stack_item->name = (char*)malloc(strlen(name)+1);
	if (stack_item->name == NULL) {
		free(stack_item);
		return NULL;
	}

	sprintf(stack_item->name, name);
	stack_item->data = NULL;

	if (previous_item == NULL) {
		stack_item->previous_item = NULL;
		stack_item->next_item = NULL;
	} else {
		stack_item->previous_item = previous_item;
		old_next_item = previous_item->next_item;
		previous_item->next_item = stack_item;
		stack_item->next_item = old_next_item;

		if (old_next_item != NULL) old_next_item->previous_item = stack_item;
	}

	return stack_item;
}

int kudu_stack_item_destroy(K_STACK_ITEM *item)
{
	if (item == NULL) return FALSE;

	K_STACK_ITEM *previous_item, *next_item;

	previous_item = item->previous_item;
	next_item = item->next_item;

	if (previous_item != NULL) previous_item->next_item = next_item;
	if (next_item != NULL) next_item->previous_item = previous_item;

	free(item);

	return TRUE;
}

int kudu_stack_list_destroy(void)
{
	if (stack_list == NULL) return FALSE;

	if (stack_list->first_item != NULL) {
		K_STACK_ITEM *current_item = NULL, *destroy_item = NULL;

		do {
			destroy_item = current_item;
			if (current_item == NULL) current_item = stack_list->first_item;
			else	current_item = current_item->next_item;

			if (destroy_item != NULL) kudu_stack_item_destroy(destroy_item);

		} while (current_item->next_item != NULL);
		kudu_stack_item_destroy(current_item);
	}

	free(stack_list);
	stack_list = NULL;
}

K_STACK_ITEM *kudu_stack_list_find_item(char *name)
{
	if (stack_list == NULL) return NULL;
	if (stack_list->first_item == NULL) return NULL;
	if (name == NULL) return NULL;

	K_STACK_ITEM *current_item = NULL;

	do {
		if (current_item == NULL) current_item = stack_list->first_item;
		else	current_item = current_item->next_item;

	} while ((current_item->next_item != NULL) && (strcmp(current_item->name, name) != 0));

	if (strcmp(current_item->name, name) == 0) return current_item;

	return NULL;
}

int kudu_stack_list_item_exist(char *name)
{
	if (kudu_stack_list_find_item(name) == NULL) return FALSE;
	else	return TRUE;
}

int kudu_stack_list_add_item(char *name, void *data)
{
	if (stack_list == NULL) return FALSE;
	if (name == NULL) return FALSE;
	if (kudu_stack_list_item_exist(name)) return FALSE;

	K_STACK_ITEM *stack_item;

	stack_item = kudu_stack_item_new(stack_list->first_item, name);
	if (stack_item == NULL) return FALSE;
	if (stack_list->first_item == NULL) stack_list->first_item = stack_item;
	stack_list->num_items++;

	stack_item->data = data;

	return TRUE;
}

int kudu_stack_list_set_item(char *name, void *data)
{
	if (stack_list == NULL) return FALSE;
	if (name == NULL) return FALSE;

	K_STACK_ITEM *stack_item = kudu_stack_list_find_item(name);

	if (stack_item == NULL) return FALSE;

	stack_item->data = data;

	return TRUE;
}

void *kudu_stack_list_get_item(char *name)
{
	if (stack_list == NULL) return NULL;
	if (stack_list->first_item == NULL) return NULL;

	K_STACK_ITEM *current_item = NULL;

	do {
		if (current_item == NULL) current_item = stack_list->first_item;
		else	current_item = current_item->next_item;

	} while ((current_item->next_item != NULL) && (strcmp(current_item->name, name) != 0));

	if (strcmp(current_item->name, name) == 0) return current_item->data;

	return NULL;
}

int kudu_stack_list_item_destroy(char *name)
{
	if (stack_list == NULL) return FALSE;
	if (name == NULL) return FALSE;

	K_STACK_ITEM *stack_item = kudu_stack_list_find_item(name);

	if (stack_item == NULL) return TRUE;

	kudu_stack_item_destroy(stack_item);

	return TRUE;
}

int kudu_stack_list_widget_show(char *name)
{
	GtkWidget *widget;

	widget = (GtkWidget*)kudu_stack_list_get_item(name);

	if (widget == NULL) return FALSE;

	gtk_widget_show(widget);

	return TRUE;
}

int kudu_stack_list_widget_hide(char *name)
{
	GtkWidget *widget;

	widget = (GtkWidget*)kudu_stack_list_get_item(name);

	if (widget == NULL) return FALSE;

	gtk_widget_hide(widget);

	return TRUE;
}

int kudu_stack_list_widget_check_menu_item_active(char *name, int active)
{
	GtkWidget *widget;

	widget = (GtkWidget*)kudu_stack_list_get_item(name);

	if (widget == NULL) return FALSE;

	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(widget), (gboolean)active);

	return TRUE;
}



