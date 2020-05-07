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
#ifndef TGA_IMAGE_H_INCLUDED
#define TGA_IMAGE_H_INCLUDED

#include "globals.h"


const unsigned char K_TGA_UHeader[12] = {0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0};
const unsigned char K_TGA_CHeader[12] = {0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0};


int kudu_image_load_tga(KuduImage*, char*);
int kudu_image_load_tga_sub_header(KuduImage*, FILE*);
int kudu_image_load_tga_uncompressed(KuduImage*, FILE*);
int kudu_image_load_tga_compressed(KuduImage*, FILE*);

#endif /* TGA_IMAGE_H_INCLUDED */

