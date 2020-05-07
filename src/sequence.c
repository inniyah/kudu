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
#include "sequence.h"

int kudu_sequence_set_frame(KuduFrameList *frame_list, KuduObject *object, KuduSelectionList *selection_list,
			    KuduSelectionList *selected_bones_list, K_FRAME_SETS what)
{
	if (frame_list == NULL) return FALSE;

	KuduBone *current_bone = NULL, *siblings = NULL;
	int gone_back = FALSE;
	int succeeded = FALSE;

	switch (what) {
		case SET_ALL_SCENE:
			break;
		case SET_ALL_OBJECT:
			break;
		case SET_BONES_SCENE:
			break;
		case SET_BONES_OBJECT:
			if (object == NULL) break;
			if (object->bone == NULL) break;
			do {
				if (siblings == NULL) siblings = object->bone;
				else	siblings = siblings->next_sibling;

				current_bone = siblings;
				gone_back = FALSE;

				do {
					if (!gone_back) kudu_frame_list_set_frame_operation(frame_list, object,
									current_bone, frame_list->current_frame, OPERATION_BONE_SET);
					if ((current_bone->first_child != NULL) && (!gone_back))
						current_bone = current_bone->first_child;
					else	if (current_bone->next_sibling != NULL) {
						current_bone = current_bone->next_sibling;
						gone_back = FALSE;
					} else {
						if (current_bone->parent != NULL) current_bone = current_bone->parent;
						gone_back = TRUE;
					}
				} while ((current_bone != siblings));
			} while (siblings->next_sibling != NULL);

			succeeded = TRUE;
			break;
		case SET_BONES_SELECTED:
			if (object == NULL) break;
			if (selected_bones_list == NULL) break;
			if (!kudu_selection_list_anything_selected(selected_bones_list)) break;
			if (!kudu_selection_list_for_each_do(selected_bones_list)) break;
			while ((current_bone = (KuduBone*)kudu_selection_list_next_do()) != NULL) {
				kudu_frame_list_set_frame_operation(frame_list, object, current_bone,
							   frame_list->current_frame, OPERATION_BONE_SET);
			}
			succeeded = TRUE;
			break;
		case SET_VERTICES_SCENE:
			break;
		case SET_VERTICES_OBJECT:
			break;
		case SET_VERTICES_SELECTED:
			break;
		case SET_POSITION_SCENE:
			break;
		case SET_POSITION_OBJECT:
			if (object == NULL) break;
			kudu_frame_list_set_frame_operation(frame_list, object, object, frame_list->current_frame, OPERATION_OBJECT_SET);
			succeeded = TRUE;
			break;
	}

}

