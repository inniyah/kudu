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
#include "tga_image.h"

int kudu_image_load_tga(KuduImage *image, char *filename)
{
	if (image == NULL) return FALSE;
	if (filename == NULL) return FALSE;

	FILE *inf;

	inf = fopen(filename, "r");
	if (inf == NULL) return FALSE;

	unsigned char header[12];
	int rv = FALSE;

	if (fread(header, 12, 1, inf) == 0) {
		fclose(inf);
		return FALSE;
	}

	if (memcmp(K_TGA_UHeader, header, 12) == 0) {	/* Uncompressed TGA Image */
		rv = kudu_image_load_tga_uncompressed(image, inf);
	} else if (memcmp(K_TGA_CHeader, header, 12) == 0) {	/* Compressed TGA Image */
		rv = kudu_image_load_tga_compressed(image, inf);
	} else {	/* Not a valid TGA Image */
		fclose(inf);
		return FALSE;
	}

	fclose(inf);

	return rv;
}

/* Read the basic info from the file, height width, bpp ... */
int kudu_image_load_tga_sub_header(KuduImage *image, FILE *inf)
{
	if (image == NULL) return FALSE;
	if (inf == NULL) return FALSE;

	unsigned char header[6];

	if (fread(header, 6, 1, inf) == 0) return FALSE;

	image->width = ((header[1] * 256) + header[0]);
	image->height = ((header[3] * 256) + header[2]);
	image->bpp = header[4];

	if ((image->width <= 0) || (image->height <= 0) || ((image->bpp != 24) && (image->bpp != 32))) return FALSE;

	if (image->bpp == 24) image->channels = 3;
	else	image->channels = 4;

	image->size = (image->channels * image->width * image->height);

	image->data = (unsigned char*)malloc(image->size);
	if (image->data == NULL) return FALSE;

	return TRUE;
}

/* Load an uncompressed TGA image */
int kudu_image_load_tga_uncompressed(KuduImage *image, FILE *inf)
{
	if (image == NULL) return FALSE;
	if (inf == NULL) return FALSE;

	if (!kudu_image_load_tga_sub_header(image, inf)) return FALSE;

	if (fread(image->data, 1, image->size, inf) != image->size) {
		free(image->data);
		return FALSE;
	}

	unsigned int cswap;

	/* Re-order data from BGR to RGB */
	for (cswap = 0; cswap < image->size; cswap += image->channels) {
		image->data[cswap] ^= image->data[cswap+2] ^= image->data[cswap] ^= image->data[cswap+2];
	}

	return TRUE;
}

/* Load a compressed TGA image */
int kudu_image_load_tga_compressed(KuduImage *image, FILE *inf)
{
	if (image == NULL) return FALSE;
	if (inf == NULL) return FALSE;

	if (!kudu_image_load_tga_sub_header(image, inf)) return FALSE;

	unsigned int pixel_count = image->width * image->height;
	unsigned int current_pixel = 0, current_byte = 0;
	unsigned char *buffer = (unsigned char*)malloc(image->channels);

	if (buffer == NULL) {
		free(image->data);
		return FALSE;
	}

	unsigned char chunkheader;
	int cnt;

	do {
		if (fread(&chunkheader, sizeof(unsigned char), 1, inf) == 0) {
			free(image->data);
			free(buffer);
			return FALSE;
		}

		if (chunkheader < 128) {
			chunkheader++;
			for (cnt = 0; cnt < chunkheader; cnt++) {
				if (fread(buffer, 1, image->channels, inf) != image->channels) {
					free(image->data);
					free(buffer);
					return FALSE;
				}

				image->data[current_byte] = buffer[2];		/* R */
				image->data[current_byte+1] = buffer[1];	/* G */
				image->data[current_byte+2] = buffer[0];	/* B */
				if (image->channels == 4) image->data[current_byte + 3] = buffer[3];	/* A */

				current_byte += image->channels;
				current_pixel++;
			}
		} else {
			chunkheader -= 127;

			if (fread(buffer, 1, image->channels, inf) != image->channels) {
				free(image->data);
				free(buffer);
				return FALSE;
			}

			for (cnt = 0; cnt < chunkheader; cnt++) {
				image->data[current_byte] = buffer[2];		/* R */
				image->data[current_byte+1] = buffer[1];	/* G */
				image->data[current_byte+2] = buffer[0];	/* B */
				if (image->channels == 4) image->data[current_byte+3] = buffer[3];	/* A */

				current_byte += image->channels;
				current_pixel++;
			}
		}
	} while (current_pixel < pixel_count);

	free(buffer);

	return TRUE;
}



















