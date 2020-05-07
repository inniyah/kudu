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
#ifndef GUI_MATERIALS_H_INCLUDED
#define GUI_MATERIALS_H_INCLUDED

#include "globals.h"
#include "materials.h"

KuduMaterial *kudu_gui_materials_edit_get_picked_material(void);
int kudu_gui_materials_edit(KuduObject*, KuduMaterial*);
int kudu_gui_materials_pick_for_faces(KuduObject*, KuduSelectionList*);

#endif /* GUI_H_MATERIALS_INCLUDED */
