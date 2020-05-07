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

/* Create a "default" simple check image, can be used as a placeholder for unsupported image formats */
KuduImage *kudu_image_new_default(void)
{
	KuduImage *image;

	image = kudu_image_new();
	if (image == NULL) {
		kudu_error(KE_NO_MEM);
		return NULL;
	}

	image->width = 32;
	image->height = 32;
	image->channels = 3;
	image->bpp = 8;
	image->size = ((image->width * image->height) * image->channels);

	image->data = (unsigned char*)malloc(image->size);
	if (image->data == NULL) {
		kudu_image_destroy(image);
		kudu_error(KE_NO_MEM);
		return NULL;
	}

	int x, y, i;

	/* Maybe not the most elegant way to do this ... but it does work */
	for (y = 0; y < image->height; y++) {
		for (x = 0; x < image->width; x++) {
			i = (x*image->channels);
			i += (y * (image->width * image->channels));

			if (((x % 2) && (y % 2)) || ((!(x % 2)) && (!(y % 2)))) {
				image->data[i] = 255;
				image->data[i+1] = 255;
				image->data[i+2] = 255;
			} else {
				image->data[i] = 0;
				image->data[i+1] = 0;
				image->data[i+2] = 255;
			}
		}
	}

	return image;
}

/* Print out a list of all supported image formats */
int kudu_image_print_formats(void)
{
	printf("\nImage formats supported by Kudu:\n\n");
	printf("Builtin support:\n\n");
	printf("tga - The Targa Image Format\n");

	printf("\n\nFormats supported via the GdkPixbuf library:\n\n");

	GSList *list, *it;
	GdkPixbufFormat *format;

	list = gdk_pixbuf_get_formats();

	if (list != NULL) {
		for (it = list; it->next != NULL; it = it->next) {
			format = (GdkPixbufFormat*)it->data;
			printf("%s - %s\n", gdk_pixbuf_format_get_name(format), gdk_pixbuf_format_get_description(format));
		}
		g_slist_free(list);
	}

}


/* Convert a GdkPixbuf into a KuduImage */
KuduImage *kudu_image_new_from_gdkpixbuf(GdkPixbuf *pixbuf)
{
	if (pixbuf == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	KuduImage *image;
	int x, y, rowstride, i, k;
	unsigned char *pixels, *p;

	image = kudu_image_new();
	if (image == NULL) {
		kudu_error(KE_NO_MEM);
		return NULL;
	}

	/* Get all relevant info */
	image->bpp = gdk_pixbuf_get_bits_per_sample(pixbuf);
	image->width = gdk_pixbuf_get_width(pixbuf);
	image->height = gdk_pixbuf_get_height(pixbuf);
	image->channels = gdk_pixbuf_get_n_channels(pixbuf);

	/* To the best of my knowledge the gdk-pixbuf library only supports 8 bpp images with
	   RGB channels and optionally an alpha (A) channel... just in case though run this check */
	if ((image->bpp != 8) || (image->channels < 3) || (image->channels > 4)) {
		kudu_image_destroy(image);
		return NULL;
	}

	/* Allocate memory */
	image->size = ((image->width * image->height) * image->channels);
	image->data = (unsigned char*)malloc(image->size);

	pixels = gdk_pixbuf_get_pixels(pixbuf);
	rowstride = gdk_pixbuf_get_rowstride(pixbuf);

	/* Copy image data from the pixbuf into the allocated memory */
	for (y = 0; y < image->height; y++) {
		for (x = 0; x < image->width; x++) {
			/* Point p to the pixel at x,y */
			p = pixels + y * rowstride + x * image->channels;

			/* Find the equivilent point in the KuduImage memory block */			
			i = (x*image->channels);
			i += ((image->height-1 - y) * (image->width * image->channels));

			image->data[i] = p[0];
			image->data[i+1] = p[1];
			image->data[i+2] = p[2];
			/* 4 Channels mean's this image has an alpha channel */
			if (image->channels == 4) image->data[i+3] = p[3];
		}
	}

	return image;
}

/* Create a new KuduImage by first creating a GdkPixbuf, and then calling
   kudu_image_new_from_gdkpixbuf to convert it into a KuduImage */
KuduImage *kudu_image_new_via_gdkpixbuf(char *filename)
{
	KuduImage *image = NULL;
	GdkPixbuf *pixbuf;
	GError *error = NULL;

	pixbuf = gdk_pixbuf_new_from_file(filename, &error);

	if (pixbuf == NULL) return NULL;

	image = kudu_image_new_from_gdkpixbuf(pixbuf);

	g_object_unref(pixbuf);

	return image;
}

/* Create a new KuduImage from an image file,
   first try to load it using any builtin image file loaders, otherwise try using
   the gdk-pixbuf library to load it, finaly if the format cannot be loaded create
   a default "check" image */
KuduImage *kudu_image_new_autoload(char *filename)
{
	if (filename == NULL) return NULL;

	char *ext;
	int a, b, c;
	KuduImage *image = NULL;

	b = strlen(filename);
	c = b+1;
	for (a = b-1; a > 0; a--) {
		if (filename[a] == '.') {
			c = a;
			a = 0;
		}
	}

	if (c >= b) return NULL;

	a = (b-c)+1;
	ext = (char*)malloc(a);
	strncpy(ext, &filename[c+1], a);
	ext[a] = '\0';

	/* If we have any specially written image file loaders then use them
	   Otherwise try use gdkpixbuf routines */

	if (strcasecmp(ext, "tga") == 0) {	/* TGA Image */
		image = kudu_image_new();
		kudu_image_load_tga(image, filename);
	} else {				/* Unrecognized image format.. try using gdkpixbuf */
		image = kudu_image_new_via_gdkpixbuf(filename);

		if (image == NULL) {
			printf("Unknown file extension\n");
		}
	}

	free(ext);

	/* If image is NULL then we have a unsupported file format,
	   in this case create a default "check" image */
	if (image == NULL) image = kudu_image_new_default();

	return image;
}




