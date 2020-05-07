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
#ifndef DUTILS_H_INCLUDED
#define DUTILS_H_INCLUDED
#include "globals.h"

int kudu_util_atoi_strtok(char*);
float kudu_util_strtod_strtok(char*);
int kudu_util_int_bit_flag_get(int, int);
void kudu_util_int_bit_flag_set(int*, int);
void kudu_util_int_bit_flag_unset(int*, int);
char kudu_util_byte_bit_flag_get(char, char);
void kudu_util_byte_bit_flag_set(char*, char);
void kudu_util_byte_bit_flag_unset(char*, char);
char *kudu_util_correct_path(char*);
int kudu_util_mouse_scroll(int, int, float*, float*, float*, float*, GdkModifierType);
int kudu_util_get_endstr(char*, char*, int);

#endif /* DUTILS_H_INCLUDED */
