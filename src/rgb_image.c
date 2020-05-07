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
#include "rgb_image.h"

int kudu_image_load_rgb(KuduImage *image, char *filename)
{
	if ((image == NULL) || (filename == NULL)) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	FILE *inf;
	unsigned char header[12];
	short int swap_bytes, image_type;

	inf = fopen(filename, "r");
	if (inf == NULL) return FALSE;

	if (fread(header, 12, 1, inf) == 0) {
		fclose(inf);
		kudu_error(KE_FILE_INVALID);
		return FALSE;
	}

	swap_bytes = ((header[0] * 256) + header[1]);

	if (swap_bytes == IMAGIC_SWAP) {
		image_type = ((header[3] * 256) + header[2]);
		image->width = ((header[7] * 256) + header[6]);
		image->height = ((header[9] * 256) + header[8]);
		image->channels = ((header[11] * 256) + header[10]);
	} else {
		image_type = ((header[2] * 256) + header[3]);
		image->width = ((header[6] * 256) + header[7]);
		image->height = ((header[8] * 256) + header[9]);
		image->channels = ((header[10] * 256) + header[11]);
	}

	/* Allocate memory for the image */
	image->data = (unsigned char*)malloc((image->width * image->height) * image->channels);
	if (image->data == NULL) {
		fclose(inf);
		kudu_error(KE_NO_MEM);
		return FALSE;
	}

	printf("%d X %d  -- %d\n", image->width, image->height, image->channels);

	if ((image_type & 0xFF00) == 0x0100) {  /* RLE Image */
	} else {				/* Raw, uncompressed image */
	}

	return TRUE;
}

/*void ImageWriteRGBData(FILE *outFile, KuduImage *image)
{
	GLint a, b, c = 0;
	unsigned char *buf;
*/
	/* Write RAW RGB image */
	/* We are assuming that the data was stored with 1 byte alignment */
	/* i.e. in OpenGL: glPixelStorei(GL_PACK_ALIGNMENT, 1); */

/*	buf = (unsigned char*) malloc((image->width*image->height)*image->channels);
	if (buf == NULL) return;

	for (a = 0; a < image->channels; a++)
		for (b = a; b < ((image->width*image->height)*image->channels); b+= image->channels)
			buf[c++] = image->image[b];

	fwrite(buf, ((image->width*image->height)*image->channels), 1, outFile);
	free(buf);
}

GLint SAVE_RGB_IMAGE(KuduImage *image, char *filename){
	INTERNAL_IMAGE *iImage;
	char header[488];
	GLint i;

	iImage = (INTERNAL_IMAGE*)malloc(sizeof(INTERNAL_IMAGE));
	if (iImage == NULL) return -1;
	if ((iImage->file = fopen(filename, "w")) == NULL) return -1;
*/
	/* I have no idea what these values are for so I'm setting them up like */
	/* every rgb image I've ever opened - they're seemingly always the same */
/*	iImage->min = 0;
	iImage->max = -16777216;
	iImage->wasteBytes = 0;
	for (i = 0; i <= 488; i++) header[i] = 0;
*/
	/* Swap Bytes */
/*	iImage->swapBytes = IMAGIC_SWAP;
	iImage->type = SWAP_SHORT_BYTES((GLushort)1);
	iImage->dim = SWAP_SHORT_BYTES((GLushort)3);
	iImage->width = SWAP_SHORT_BYTES(image->width);
	iImage->height = SWAP_SHORT_BYTES(image->height);
	iImage->channels = SWAP_SHORT_BYTES(image->channels);

	fwrite(iImage, 1, 24, iImage->file);
	fwrite(header, 488, 1, iImage->file);
	ImageWriteRGBData(iImage->file, image);

	fclose(iImage->file);
	free(iImage);
}

INTERNAL_IMAGE *IMAGE_OPEN(char *fileName)
{
	INTERNAL_IMAGE *image;
	GLuint *rowStart, *rowSize, ulTmp;
	GLint x, i;

	image = (INTERNAL_IMAGE*)malloc(sizeof(INTERNAL_IMAGE));
	if (image == NULL) {
		return NULL;
	}

	if ((image->file = fopen(fileName, "rb")) == NULL) 
	{
		free( image );
		return NULL;
	}*/
	/*
	*	Read the image header
	*/
	/*fread(image, 1, 12, image->file);*/
	/*
	*	Check byte order
	*/
/*	if (image->swapBytes == IMAGIC_SWAP) {
		image->type = SWAP_SHORT_BYTES(image->type);
		image->dim = SWAP_SHORT_BYTES(image->dim);
		image->width = SWAP_SHORT_BYTES(image->width);
		image->height = SWAP_SHORT_BYTES(image->height);
		image->channels = SWAP_SHORT_BYTES(image->channels);
	}

	for (i = 0 ; i <= image->channels; i++) {
		image->tmp[i] = (unsigned char*)malloc(image->width*256);
		if (image->tmp[i] == NULL ) return NULL;
	}

	if ((image->type & 0xFF00) == 0x0100) {*/  /* RLE Image */
/*		x = image->height * image->channels * sizeof(GLuint);
		image->rowStart = (GLuint*)malloc(x);
		image->rowSize = (GLuint*)malloc(x);
		if (image->rowStart == NULL || image->rowSize == NULL) return NULL;

		image->rleEnd = 512 + (2 * x);
		fseek(image->file, 512, SEEK_SET);
		fread(image->rowStart, 1, x, image->file);
		fread(image->rowSize, 1, x, image->file);
		if (image->swapBytes == IMAGIC_SWAP) {
			x /= sizeof(GLint);
			rowStart = image->rowStart;
			rowSize = image->rowSize;
			while (x--) {
				ulTmp = *rowStart;
				*rowStart++ = SWAP_LONG_BYTES(ulTmp);
				ulTmp = *rowSize;
				*rowSize++ = SWAP_LONG_BYTES(ulTmp);
			}
		}
	}

	return image;
}*/

int kudu_image_rgb_rle_get_row(KuduImage *image, FILE *file, unsigned char *tmp[4], unsigned int *row_size,
					unsigned int *row_start, int y, int z)
{
	if ((image == NULL) || (file == NULL) || (tmp == NULL) || (row_size == NULL) || (row_start == NULL)) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	unsigned char *iPtr, *oPtr, pixel;
	int count;

	fseek(file, row_start[y+z*image->height], SEEK_SET);
	fread(tmp, 1, row_size[y+z*image->height], file);

	iPtr = tmp[0];
	oPtr = image->data;

	while (1) {
		pixel = *iPtr++;
		count = (int)(pixel & 0x7F);
		if (!count) return TRUE;

		if (pixel & 0x80) {
			while (count--) {
				*oPtr++ = *iPtr++;
			}
		} else {
			pixel = *iPtr++;
			while (count--) {
				*oPtr++ = pixel;
			}
		}
	}
}

int kudu_image_rgb_rle_get_data(KuduImage *image, FILE *file, unsigned char *tmp[4], unsigned int *row_size,
						unsigned int *row_start)
{
	if ((image == NULL) || (file == NULL) || (tmp == NULL) || (row_size == NULL) || (row_start == NULL)) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	int i, j, k, remain = 0;

	switch (image->channels) {
		case 1:
			remain = image->width % 4;
			break;
		case 2:
			remain = image->width % 2;
			break;
		case 3:
			remain = (image->width * 3) & 0x3;
			if (remain) remain = 4 - remain;
			break;
		case 4:
			remain = 0;
			break;
	}

	for (i = 0; i < (int)image->height; i++) {
		for ( k = 0; k < (int)image->channels; k++) kudu_image_rgb_rle_get_row(image, file, tmp[k+1], row_size, row_start, i, k);
		for (j = 0; j < (int)image->width; j++)
			for ( k = 1; k <= (int)image->channels; k++) *image->data++ = *(tmp[k] + j);
		image->data += remain;
	}

	return TRUE;
}


/*
	}  else {*/	/* verbatim image */
/*		fseek(image->file, 512+(y*image->width)+(z*image->width*image->height), SEEK_SET);
		fread(buf, 1, image->width, image->file);
	}
void ImageGetRawData(INTERNAL_IMAGE *image, unsigned char *data)
{
	GLint i, j, k;
	GLint remain = 0;

	switch (image->channels) {
		case 1:
			remain = image->width % 4;
			break;
		case 2:
			remain = image->width % 2;
			break;
		case 3:
			remain = (image->width * 3) & 0x3;
			if (remain) remain = 4 - remain;
			break;
		case 4:
			remain = 0;
			break;
	}

	for (i = 0; i < image->height; i++) {
		for ( k = 0; k < image->channels ; k++ ) ImageGetRow(image, image->tmp[k+1], i, k);
		for (j = 0; j < image->width; j++)
			for ( k = 1; k <= image->channels ; k++ ) *data++ = *(image->tmp[k] + j);
		data += remain;
	}
}

void ImageClose(INTERNAL_IMAGE *image)
{
	GLint i;

	fclose(image->file);
	for (i = 0 ; i <= image->channels; i++) free(image->tmp[i]);
	free(image);
}

GLint LOAK_RGB_IMAGE(KuduImage *final, char *fileName)
{
	INTERNAL_IMAGE *image;
	GLint sx;

	if ((image = IMAGE_OPEN(fileName)) == NULL) return -1;

	final->width = (GLint)image->width; 
	final->height = (GLint)image->height;
	final->channels = (GLint)image->channels;
*/
	/* 
	* Round up so rows are long-word aligned 
	*/
/*	sx = ((image->width) * (image->channels) + 3) >> 2;

	final->image = (unsigned char*)malloc((sx * image->width * sizeof(GLuint)));

	if (final->image == NULL) return -1;

	ImageGetRawData(image, final->image);
	ImageClose(image);
	return 0;
}

void RGB_VALUE(KuduImage *image, GLint width, GLint height, unsigned char rgb[3])
{
	GLint a, b, nh;

	nh = ((image->height-1) - height);

	a = (nh * (image->width * image->channels));
	a = (a + (width * image->channels));

	rgb[0] = image->image[a];
	rgb[1] = image->image[a+1];
	rgb[2] = image->image[a+2];
}
*/
