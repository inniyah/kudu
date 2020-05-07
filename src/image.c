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
#include "image.h"

KuduImage *kudu_image_new(void)
{
	KuduImage *image;

	image = (KuduImage*)malloc(sizeof(KuduImage));
	if (image == NULL) return NULL;

	image->width = 0;
	image->height = 0;
	image->channels = 0;
	image->bpp = 0;
	image->size = 0;
	image->data = NULL;

	return image;
}

int kudu_image_destroy(KuduImage *image)
{
	if (image == NULL) return FALSE;

	if ((image->size != 0) && (image->data != 0)) {
		free(image->data);
		image->size = 0;
	}

	free(image);

	return TRUE;
}

