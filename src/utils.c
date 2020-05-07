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
#include "utils.h"

int kudu_util_atoi_strtok(char *str)
{
	GLint rv;

	char *tmp = strtok(NULL, str);
	if ((tmp != NULL) && (strcmp(tmp, "") != 0)) rv = (GLint)atoi(tmp);
	else rv = 0;

	return rv;
}

float kudu_util_strtod_strtok(char *str)
{
	GLfloat rv;

	char *tmp = strtok(NULL, str);
	if ((tmp != NULL) && (strcmp(tmp, "") != 0)) rv = (GLfloat)strtod(tmp, NULL);
	else rv = 0;

	return rv;
}

int kudu_util_int_bit_flag_get(int VALUE, int BIT)
{
	if ((VALUE & BIT) == BIT) return GL_TRUE;
	else return GL_FALSE;
}

void kudu_util_int_bit_flag_set(int *VALUE, int BIT)
{
	if (!kudu_util_int_bit_flag_get(*VALUE, BIT))
		*VALUE += BIT;
}

void kudu_util_int_bit_flag_unset(int *VALUE, int BIT)
{
	if (kudu_util_int_bit_flag_get(*VALUE, BIT))
		*VALUE -= BIT;
}

char kudu_util_byte_bit_flag_get(char VALUE, char BIT)
{
	if ((VALUE & BIT) == BIT) return GL_TRUE;
	else return GL_FALSE;
}

void kudu_util_byte_bit_flag_set(char *VALUE, char BIT)
{
	if (!kudu_util_byte_bit_flag_get(*VALUE, BIT))
		*VALUE += BIT;
}

void kudu_util_byte_bit_flag_unset(char *VALUE, char BIT)
{
	if (kudu_util_byte_bit_flag_get(*VALUE, BIT))
		*VALUE -= BIT;
}

/* Creates a new string, ensuring that it ends with a "/" character */
/* Return value must be freed with free() after use */
char *kudu_util_correct_path(char *cpath)
{
	if (cpath == NULL) return NULL;

	char *path;
	int a;

	a = strlen(cpath);

	if (cpath[a-1] == '/') {
		path = (char*)malloc(a);
		sprintf(path, cpath);
	} else {
		path = (char*)malloc(a+1);
		sprintf(path, "%s/", cpath);
	}

	return path;
}

int kudu_util_mouse_scroll(int mx, int my, float *hscroll, float *vscroll, float *opt_h, float *opt_v, GdkModifierType mods)
{
	if ((hscroll == NULL) || (vscroll == NULL) || (opt_h == NULL) || (opt_v == NULL)) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	float hs = 0, vs = 0;
	static float rhs = 0, rvs = 0, step;
	int shift = (mods & GDK_SHIFT_MASK), ctrl = (mods & GDK_CONTROL_MASK), opt = FALSE;
	static int old_shift = -1, old_ctrl = -1;

	if ((old_shift != shift) || (old_ctrl != ctrl)) {
		old_shift = shift;
		old_ctrl = ctrl;
		*opt_h = 0;
		*opt_v = 0;
	}

	hs = ((float)(((int)program.windowWidth / 2) - mx) / 10);
	vs = ((float)(((int)program.windowHeight / 2) - my) / 10);

	/*printf("mx:%d  my:%d  hs:%4.2f  vs:%4.2f\n", mx, my, hs, vs);
	printf("pgw: %d   pgh: %d\n", (int)(program.windowWidth / 2), (int)(program.windowHeight / 2));*/

	if ((shift) && (ctrl)) {
		hs /= 10;
		vs /= 10;
	} else
	if ((shift) || (ctrl)) {
		opt = TRUE;
		if (shift) step = 15.0;
		else	step = 1.0;
		rhs += hs;
		rvs += vs;
		hs = 0;
		vs = 0;

		if ((rhs >= 45) || (rhs <= -45)) {
			if (rhs > 0) hs = step;
			else hs = -step;
			rhs = 0;
		}
		if ((rvs >= 425) || (rvs <= -45)) {
			if (rvs > 0) vs = step;
			else vs = -step;
			rvs = 0;
		}
	}

	if (opt) {
		*opt_h += hs;
		*opt_v += vs;
	} else {
		*opt_h += (hs / 10);
		*opt_v += (vs / 10);
	}

	*hscroll = hs;
	*vscroll = vs;

	if ((hs == 0) && (vs == 0)) return FALSE;

	return TRUE;
}

int kudu_util_get_endstr(char *src, char *dest, int pos)
{
	if ((src == NULL) || (dest == NULL) || (pos <= 0)) return FALSE;
	int a, b;

	b = pos;
	a = strlen(src)-b;

	if (src[b] == ' ') {
		do {
			b++;
			a--;
		} while ((src[b] == ' ') && (a >= 0));
	}

	strncpy(dest, &src[b], a);
	dest[a] = '\0';

	return TRUE;
}


