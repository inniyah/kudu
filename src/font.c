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
#include "font.h"
#include "defaultfont.h"

void kudu_font_builtin_init(void)
{
	GLint i, j;
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	FONT_OFFSET = glGenLists(128);

	for (i = 0; i < 128; i++) {
		glNewList(FONT_OFFSET+i, GL_COMPILE);
		glBitmap(8, 13, 0.0, 2.0, 10.0, 0.0, unknown);
		glEndList();
	}

	for (i = 0, j = 'A'; i < 26; i++, j++) {
		glNewList(FONT_OFFSET+j, GL_COMPILE);
		glBitmap(8, 13, 0.0, 2.0, 10.0, 0.0, defaultLetters[i]);
		glEndList();
	}

	for (i = 0, j = 'a'; i < 26; i++, j++) {
		glNewList(FONT_OFFSET+j, GL_COMPILE);
		glBitmap(8, 13, 0.0, 2.0, 10.0, 0.0, defaultLetters[i]);
		glEndList();
	}

	for (i = 0, j = '0'; i < 10; i++, j++) {
		glNewList(FONT_OFFSET+j, GL_COMPILE);
		glBitmap(8, 13, 0.0, 2.0, 10.0, 0.0, defaultNumbers[i]);
		glEndList();
	}

	glNewList(FONT_OFFSET+' ', GL_COMPILE);
	glBitmap(8, 13, 0.0, 2.0, 10.0, 0.0, space);
	glEndList();

	glNewList(FONT_OFFSET+'.', GL_COMPILE);
	glBitmap(8, 13, 0.0, 2.0, 6.0, 0.0, point);
	glEndList();

	glNewList(FONT_OFFSET+':', GL_COMPILE);
	glBitmap(8, 13, 0.0, 2.0, 6.0, 0.0, colon);
	glEndList();

	glNewList(FONT_OFFSET+'%', GL_COMPILE);
	glBitmap(8, 13, 0.0, 2.0, 10.0, 0.0, percent);
	glEndList();

	glNewList(FONT_OFFSET+'-', GL_COMPILE);
	glBitmap(8, 13, 0.0, 2.0, 10.0, 0.0, minus);
	glEndList();

	glNewList(FONT_OFFSET+'/', GL_COMPILE);
	glBitmap(8, 13, 0.0, 2.0, 10.0, 0.0, forwardslash);
	glEndList();

	glNewList(FONT_OFFSET+'\\', GL_COMPILE);
	glBitmap(8, 13, 0.0, 2.0, 10.0, 0.0, backslash);
	glEndList();

	glNewList(FONT_OFFSET+1, GL_COMPILE);
	glBitmap(8, 13, 0.0, 2.0, 10.0, 0.0, cursor);
	glEndList();
}

void kudu_font_builtin_write_2d(GLint X, GLint Y, char *TEXT)
{
	glPushAttrib(GL_LIST_BIT);

	/* Older versions of OpenGL (pre 1.4) don't have the glWindowPos2i function...
	   use a workaround in this case */
	#ifdef HAVE_GLWINDOWPOS
	glWindowPos2i(X, Y);
	#else
	glPushMatrix();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0.0, (GLfloat)program.windowWidth, 0.0, (GLfloat)program.windowHeight);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glRasterPos2i(X, Y);
	#endif

	glListBase(FONT_OFFSET);
	glCallLists(strlen(TEXT), GL_UNSIGNED_BYTE, (GLubyte *) TEXT);
	glPopAttrib();

	#ifndef HAVE_GLWINDOWPOS
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	#endif
}

void kudu_font_builtin_write_3d(GLfloat X, GLfloat Y, GLfloat Z, char *TEXT)
{
	glPushAttrib(GL_LIST_BIT);
	glRasterPos3f(X, Y, Z);
	glListBase(FONT_OFFSET);
	glCallLists(strlen(TEXT), GL_UNSIGNED_BYTE, (GLubyte *) TEXT);
	glPopAttrib();
}

