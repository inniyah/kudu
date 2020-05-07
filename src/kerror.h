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
#ifndef KERROR_H_INCLUDED
#define KERROR_H_INCLUDED

#include "globals.h"

typedef enum {
	KE_UNKNOWN,
	KE_NO_MEM,

	KE_OBJECT_INVALID,
	KE_OBJECT_NOT_FOUND,

	KE_FILE_NAME_INVALID,
	KE_FILE_NOT_EXIST,
	KE_FILE_INVALID,
	KE_FILE_CORRUPT,
	KE_FILE_CANNOT_OPEN,
	KE_FILE_NEWER_VERSION,
} KuduError;

extern KuduError d_error;


#endif /* KERROR_H_INCLUDED */

