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
#ifndef JOINTS_H_INCLUDED
#define JOINTS_H_INCLUDED

#include "globals.h"


/*KuduJointList *kudu_joint_list_new(void);
int kudu_joint_list_add_joint(KuduJointList*);*/
KuduJoint *kudu_joint_new(KuduObject*);
int kudu_joint_destroy(KuduJoint*);
int kudu_joint_clone_values(KuduJoint*, KuduJoint*);
int kudu_joint_list(KuduJoint*);
int kudu_joint_list(KuduJoint*);
/*KuduJoint *kudu_joint_list_get_joint(KuduJointList*, int);*/

#endif /* JOINTS_H_INCLUDED */

