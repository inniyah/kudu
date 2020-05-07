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
#ifndef RGB_IMAGE_H_INCLUDED
#define RGB_IMAGE_H_INCLUDED

#include "globals.h"

#define IMAGIC      0x01da
#define IMAGIC_SWAP 0xda01

#define KuduImage_RGB_IMAGIC 0x01da
#define KuduImage_RGB_IMAGIC_SWAP 0xda01

#define SWAP_SHORT_BYTES(x) ((((x) & 0xff) << 8) | (((x) & 0xff00) >> 8))
#define SWAP_LONG_BYTES(x) (((((x) & 0xff) << 24) | (((x) & 0xff00) << 8)) | \
((((x) & 0xff0000) >> 8) | (((x) & 0xff000000) >> 24)))

typedef struct {
	GLushort swapBytes;
	GLushort type;
	GLushort dim;
	GLushort width;
	GLushort height;
	GLushort channels;
	GLuint min;
	GLuint max;
	GLuint wasteBytes;
	char name[80];
	GLuint colorMap;
	FILE *file;
	unsigned char *tmp[5];
	GLuint rleEnd;
	GLuint *rowStart;
	GLuint *rowSize;
	} INTERNAL_IMAGE;

void ImageWriteRGBData(FILE*, KuduImage*);
GLint SAVE_RGB_IMAGE(KuduImage*, char*);
void ImageGetRow(INTERNAL_IMAGE*, unsigned char*, GLint, GLint);
void ImageGetRawData(INTERNAL_IMAGE*, unsigned char*);
GLint LOAK_RGB_IMAGE(KuduImage*, char*);
void RGB_VALUE(KuduImage*, GLint, GLint, unsigned char[3]);

#endif /* RGB_IMAGE_H_INCLUDED */
