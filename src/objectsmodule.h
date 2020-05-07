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
#ifndef OBJECTSMODULE_H_INCLUDED
#define OBJECTSMODULE_H_INCLUDED


#include "globals.h"
#include "frames.h"
#include "bones.h"
#include "vertices.h"
#include "wingedge.h"
#include "mathematics.h"
#include "selection.h"

int kudu_script_init_module_objects(KuduObject*, KuduFrameList *frame_list, KuduSelectionList*, KuduSelectionList*);

#endif /* OBJECTSMODULE_H_INCLUDED */
