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
#ifndef MATERIALS_H_INCLUDED
#define MATERIALS_H_INCLUDED

#include "globals.h"

int kudu_material_force_next_id(KuduObject*, int);
int kudu_material_next_id(KuduObject*);
int kudu_material_id_counter_reset(KuduObject*);
KuduMaterial *kudu_material_new(KuduObject*, KuduMaterial*);
KuduMaterial *kudu_material_new_with_id(KuduObject*, KuduMaterial*, int);
int kudu_material_destroy(KuduMaterial*);
int kudu_material_destroy_all(KuduMaterial*);
int kudu_material_for_each_do(KuduMaterial*);
KuduMaterial *kudu_material_next_do(void);
KuduMaterial *kudu_material_find_with_id(KuduMaterial*, int);
KuduMaterial *kudu_material_find_with_same_values(KuduMaterial*, KuduMaterial*);
int kudu_material_num_after(KuduMaterial*);
KuduMaterial *kudu_material_find_with_name(KuduMaterial*, char*);

#endif /* MATERIALS_H_INCLUDED */
