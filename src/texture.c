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
#include "texture.h"

static int TEX_ID = 1;

KuduTexture *kudu_texture_new(KuduTexture *previous_texture)
{
	KuduTexture *tex, *old_next_tex;

	tex = (KuduTexture*)malloc(sizeof(KuduTexture));
	if (tex == NULL) return NULL;

	if (previous_texture == NULL) {
		tex->previous_texture = NULL;
		tex->next_texture = NULL;
	} else {
		old_next_tex = previous_texture->next_texture;
		previous_texture->next_texture = tex;
		tex->previous_texture = previous_texture;
		tex->next_texture = old_next_tex;

		if (old_next_tex != NULL) old_next_tex->previous_texture = tex;
	}

	tex->id = TEX_ID++;
	tex->tex = 0;
	tex->width = 0;
	tex->height = 0;

	return tex;
}

int kudu_texture_destroy(KuduTexture *tex)
{
	if (tex == NULL) return FALSE;

	KuduTexture *previous_tex = tex->previous_texture;
	KuduTexture *next_tex = tex->next_texture;

	if (previous_tex != NULL) previous_tex->next_texture = next_tex;
	if (next_tex != NULL) next_tex->previous_texture = previous_tex;

	if (glIsTexture(tex->tex)) {
		glDeleteTextures(1, &tex->tex);
		tex->tex = 0;
	}

	free(tex);

	return TRUE;
}

int kudu_texture_assign_image(KuduTexture *tex, KuduImage *image)
{
	if (tex == NULL) return FALSE;
	if (image == NULL) return FALSE;

	/* Make sure that image is valid and at least has reasonable values */
	if (!((image->width > 0) && (image->height > 0) && (image->size > 0) &&
	    (image->channels > 0) && (image->data != NULL))) return FALSE;

	/* Has Another image been previously assigned to this texture ? */
	if (glIsTexture(tex->tex)) {
		glDeleteTextures(1, &tex->tex);	/* Erase it */
		tex->tex = 0;
	}

	glGenTextures(1, &tex->tex);
	glBindTexture(GL_TEXTURE_2D, tex->tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, image->channels == 3 ? GL_RGB : GL_RGBA, image->width,
			image->height, 0, image->channels == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, image->data);

	tex->width = image->width;
	tex->height = image->height;

	return TRUE;
}
























