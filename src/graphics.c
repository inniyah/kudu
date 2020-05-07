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
#include "graphics.h"

static int optimise = FALSE, cols_set = FALSE;
static float colour_default[4], colour_selected[4], colour_highlight[4], colour_bone_selected[4], colour_bone_default[4], colour_bone_children[4];

/* This function updates all the local static colour varibles */
/* loading them in from the options database each time could be slow */
void kudu_graphics_colours_reload(void)
{

	kudu_options_get_float(KO_SKIN_COLOUR_SELECTED, 4, colour_selected);
	kudu_options_get_float(KO_SKIN_COLOUR_DEFAULT, 4, colour_default);


	cols_set = TRUE;
}

void kudu_draw_axes(void)
{
	float axes_colours[18];

	kudu_options_get_float(KO_AXES_COLOUR, 18, axes_colours);

	glBegin(GL_LINES);
	glColor3fv(&axes_colours[0]);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex4f(1.0, 0.0, 0.0, 0.0);

	glColor3fv(&axes_colours[3]);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex4f(0.0, 1.0, 0.0, 0.0);

	glColor3fv(&axes_colours[6]);
	glVertex4f(0.0, 0.0, 1.0, 0.0);
	glVertex3f(0.0, 0.0, 0.0);

	glColor3fv(&axes_colours[9]);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex4f(0.0, 0.0, -1.0, 0.0);

	glColor3fv(&axes_colours[12]);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex4f(0.0, -1.0, 0.0, 0.0);

	glColor3fv(&axes_colours[15]);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex4f(-1.0, 0.0, 0.0, 0.0);

	glEnd();
}

int kudu_draw_axes_labels(KuduCamera *camera)
{
	if (camera == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	float axes_colours[9];
	float d = (camera->distance / 2);
	if (d < 0) d = 0.2;

	kudu_options_get_float(KO_AXES_COLOUR, 9, axes_colours);


	glColor3fv(&axes_colours[0]);
	kudu_font_builtin_write_3d(d, 0, 0, "X");
	glColor3fv(&axes_colours[3]);
	kudu_font_builtin_write_3d(0, d, 0, "Y");
	glColor3fv(&axes_colours[6]);
	kudu_font_builtin_write_3d(0, 0, d, "Z");

	return TRUE;
}

int kudu_draw_grid(int x, int z, GLfloat scale, int draw_axis_lines)
{
	int cx, cz;
	float sx, ex, sz, ez, grid_colour[3];

	kudu_options_get_float(KO_GRID_COLOUR, 3, grid_colour);

	x /= 2;
	z /= 2;
	glColor3fv(grid_colour);

	glBegin(GL_LINES);

	for (cx = -x; cx < x+1; cx++) {
		for (cz = -z; cz < z+1; cz++) {
			if ((!draw_axis_lines) && ((cx == 0) || (cz == 0))) continue;

			sx = scale * (GLfloat)cx;
			sz = scale * (GLfloat)cz;
			ex = scale * (GLfloat)x;
			ez = scale * (GLfloat)cz;

			glVertex3f(sx, 0.0, sz);
			glVertex3f(ex, 0.0, ez);
		}
	}

	for (cz = -z; cz < z+1; cz++) {
		for (cx = -x; cx < x+1; cx++) {
			if ((!draw_axis_lines) && ((cx == 0) || (cz == 0))) continue;

			sx = scale * (GLfloat)cx;
			sz = scale * (GLfloat)cz;
			ex = scale * (GLfloat)cx;
			ez = scale * (GLfloat)z;

			glVertex3f(sx, 0.0, sz);
			glVertex3f(ex, 0.0, ez);
		}
	}

	glEnd();

	return TRUE;
}

int kudu_draw_mouse_rectangle(int sx, int sy, int ex, int ey)
{
	int viewport[4];
	int lights = glIsEnabled(GL_LIGHTING);

	glGetIntegerv(GL_VIEWPORT, viewport);
	if (lights) glDisable(GL_LIGHTING);

	glPushMatrix();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0.0, viewport[2], 0.0, viewport[3]);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glBegin(GL_LINE_LOOP);
	glVertex2i(sx, viewport[3] - sy);
	glVertex2i(sx, viewport[3] - ey);
	glVertex2i(ex, viewport[3] - ey);
	glVertex2i(ex, viewport[3] - sy);
	glEnd();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glPopMatrix();

	if (lights) glEnable(GL_LIGHTING);
}


void kudu_draw_block(GLfloat X, GLfloat Y, GLfloat Z, GLfloat X2, GLfloat Y2, GLfloat Z2, GLenum TYPE)
{
	glBegin(TYPE);
	glNormal3f(X, Y, Z-1);
	glVertex3f(X, Y, Z);
	glVertex3f(X, Y2, Z);
	glVertex3f(X2, Y2, Z);
	glVertex3f(X2, Y, Z);

	glNormal3f(X, Y, Z2+1);
	glVertex3f(X, Y, Z2);
	glVertex3f(X2, Y, Z2);
	glVertex3f(X2, Y2, Z2);
	glVertex3f(X, Y2, Z2);

	glNormal3f(X-1, Y, Z);
	glVertex3f(X, Y, Z);
	glVertex3f(X, Y2, Z);
	glVertex3f(X, Y2, Z2);
	glVertex3f(X, Y, Z2);

	glNormal3f(X2+1, Y, Z);
	glVertex3f(X2, Y, Z);
	glVertex3f(X2, Y, Z2);
	glVertex3f(X2, Y2, Z2);
	glVertex3f(X2, Y2, Z);

	glNormal3f(X, Y-1, Z);
	glVertex3f(X, Y, Z);
	glVertex3f(X2, Y, Z);
	glVertex3f(X2, Y, Z2);
	glVertex3f(X, Y, Z2);

	glNormal3f(X, Y2+1, Z);
	glVertex3f(X, Y2, Z);
	glVertex3f(X, Y2, Z2);
	glVertex3f(X2, Y2, Z2);
	glVertex3f(X2, Y2, Z);
	glEnd();

}

void kudu_draw_bone_graphic(float bone_length, int options)
{
	float mid_point = (bone_length / 10);

	/* Wire frame bones */
	if (options & K_RENDER_EDGES) {
		glBegin(GL_LINES);
		/* Start at a point, flare out in four directions */
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(-0.1, -0.1, mid_point);

		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(0.1, -0.1, mid_point);

		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(0.1, 0.1, mid_point);

		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(-0.1, 0.1, mid_point);

		/* Square it off */
		glVertex3f(-0.1, -0.1, mid_point);
		glVertex3f(0.1, -0.1, mid_point);

		glVertex3f(0.1, -0.1, mid_point);
		glVertex3f(0.1, 0.1, mid_point);

		glVertex3f(0.1, 0.1, mid_point);
		glVertex3f(-0.1, 0.1, mid_point);

		glVertex3f(-0.1, 0.1, mid_point);
		glVertex3f(-0.1, -0.1, mid_point);

		/* Pull back to center point */
		glVertex3f(-0.1, -0.1, mid_point);
		glVertex3f(0.0, 0.0, bone_length);

		glVertex3f(0.1, -0.1, mid_point);
		glVertex3f(0.0, 0.0, bone_length);

		glVertex3f(0.1, 0.1, mid_point);
		glVertex3f(0.0, 0.0, bone_length);

		glVertex3f(-0.1, 0.1, mid_point);
		glVertex3f(0.0, 0.0, bone_length);
		glEnd();

	} else if (options & K_RENDER_FACES) {	/* Filled poly bones */
		glBegin(GL_TRIANGLE_FAN);
		/* Start at a point, flare out in four directions */
		glVertex3f(0.0, 0.0, 0.0);

		glVertex3f(0.1, 0.1, mid_point);
		glVertex3f(0.1, -0.1, mid_point);
		glVertex3f(-0.1, -0.1, mid_point);
		glVertex3f(-0.1, 0.1, mid_point);
		glVertex3f(0.1, 0.1, mid_point);
		glEnd();

		glBegin(GL_TRIANGLE_FAN);
		/* Pull back to center point */
		glVertex3f(0.0, 0.0, bone_length);

		glVertex3f(0.1, 0.1, mid_point);
		glVertex3f(-0.1, 0.1, mid_point);
		glVertex3f(-0.1, -0.1, mid_point);
		glVertex3f(0.1, -0.1, mid_point);
		glVertex3f(0.1, 0.1, mid_point);
		glEnd();
	}
}

int kudu_draw_bone_axes(float bone_length, int options, float *axes_colours)
{
	float mid_point = (bone_length - 0.3), x, y;
	int a;

	glColor3fv(&axes_colours[0]);

	glBegin(GL_LINES);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(bone_length, 0.0, 0.0);

	glVertex3f(bone_length, 0.0, 0.0);
	glVertex3f(mid_point, 0.0, 0.1);

	glVertex3f(bone_length, 0.0, 0.0);
	glVertex3f(mid_point, 0.0, -0.1);
	glEnd();

/*		glBegin(GL_LINE_LOOP);
		for (a = 0; a < 360; a+=10) {
			x = (bone_length * sin((float)a*RADDEG));
			y = (bone_length * cos((float)a*RADDEG));
			glVertex3f(0.0, x, y);
		}
		glEnd();*/

	glColor3fv(&axes_colours[3]);
	glBegin(GL_LINES);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, bone_length, 0.0);

	glVertex3f(0.0, bone_length, 0.0);
	glVertex3f(0.1, mid_point, 0.0);

	glVertex3f(0.0, bone_length, 0.0);
	glVertex3f(-0.1, mid_point, 0.0);
	glEnd();
/*		glBegin(GL_LINE_LOOP);
		for (a = 0; a < 360; a+=10) {
			x = (bone_length * sin((float)a*RADDEG));
			y = (bone_length * cos((float)a*RADDEG));
			glVertex3f(x, 0.0, y);
		}
		glEnd();*/

	glColor3fv(&axes_colours[6]);
	glBegin(GL_LINES);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, 0.0, 1.0);

	glVertex3f(0.0, 0.0, bone_length);
	glVertex3f(0.1, 0.0, mid_point);

	glVertex3f(0.0, 0.0, bone_length);
	glVertex3f(-0.1, 0.0, mid_point);
	glEnd();
/*		glBegin(GL_LINE_LOOP);
		for (a = 0; a < 360; a+=10) {
			x = (bone_length * sin((float)a*RADDEG));
			y = (bone_length * cos((float)a*RADDEG));
			glVertex3f(x, y, 0.0);
		}
		glEnd();*/

}


int kudu_draw_bone(KuduBone *bone, int options)
{
	KuduBone *current_bone;
	int gone_back, children = FALSE;
	float tp[3], colour_bone_selected[4], colour_bone_default[4], colour_bone_children[4], axes_colours[9];

	if (bone == NULL) return FALSE;

	kudu_options_get_float(KO_BONES_COLOUR_DEFAULT, 4, colour_bone_default);
	kudu_options_get_float(KO_BONES_COLOUR_SELECTED, 4, colour_bone_selected);
	kudu_options_get_float(KO_BONES_COLOUR_CHILDREN, 4, colour_bone_children);
	kudu_options_get_float(KO_AXES_COLOUR, 9, axes_colours);

	current_bone = bone;
	gone_back = FALSE;

	do {
		if (!gone_back) {
			if (!kudu_util_int_bit_flag_get(options, K_RENDER_SHOW_SELECTED)) {
				glColor4fv(colour_bone_default);
			} else {
				if (current_bone->selected) glColor4fv(colour_bone_selected);
				else if (children) glColor4fv(colour_bone_children);
					else glColor4fv(colour_bone_default);
			}

			if (options & K_RENDER_SELECT) glLoadName((unsigned int)current_bone);

			/*if ((current_bone->parent == NULL) || (current_bone->selected)) {
				glPushMatrix();
				glTranslatef(current_bone->posX, current_bone->posY, current_bone->posZ);
				glMultMatrixf(current_bone->matrix[0]);
				glCallList(program.bone_graphic_lists);
				if (current_bone->selected) glCallList(program.bone_graphic_lists+2);
				glPopMatrix();
			}*/

			glPushMatrix();
			/*glTranslatef(current_bone->lineX, current_bone->lineY, current_bone->lineZ);*/
			glTranslatef(current_bone->posX, current_bone->posY, current_bone->posZ);
			glMultMatrixf(current_bone->matrix[0]);

			if (options & K_RENDER_SELECT) kudu_draw_bone_graphic(current_bone->length, K_RENDER_FACES);
			else kudu_draw_bone_graphic(current_bone->length, K_RENDER_EDGES);

			if ((options & K_RENDER_SHOW_AXES) || ((options & K_RENDER_SHOW_SELECTED_AXES) && (current_bone->selected)))
				kudu_draw_bone_axes(current_bone->length, K_RENDER_EDGES, axes_colours);

			/*glCallList(program.bone_graphic_lists+1);
			if (current_bone->selected) glCallList(program.bone_graphic_lists+2);*/
			glPopMatrix();

			/*glBegin(GL_LINES);
			glVertex3f(current_bone->posX, current_bone->posY, current_bone->posZ);
			glVertex3f(current_bone->lineX, current_bone->lineY, current_bone->lineZ);
			glEnd();*/

			if (options & K_RENDER_SHOW_INFO) {
				tp[0] = (current_bone->lineX + current_bone->posX) / 2;
				tp[1] = (current_bone->lineY + current_bone->posY) / 2;
				tp[2] = (current_bone->lineZ + current_bone->posZ) / 2;
				kudu_font_builtin_write_3d(tp[0], tp[1], tp[2], current_bone->name);
			} else
			if ((options & K_RENDER_SHOW_SELECTED_INFO) && (current_bone->selected)) {
				tp[0] = (current_bone->lineX + current_bone->posX) / 2;
				tp[1] = (current_bone->lineY + current_bone->posY) / 2;
				tp[2] = (current_bone->lineZ + current_bone->posZ) / 2;
				kudu_font_builtin_write_3d(tp[0], tp[1], tp[2], current_bone->name);
			}

		}

		if ((current_bone->first_child != NULL) && (!gone_back)) {
			if (current_bone->selected) children++;
			current_bone = current_bone->first_child;
		} else	if ((current_bone->next_sibling != NULL) && (current_bone != bone)) {
				current_bone = current_bone->next_sibling;
				gone_back = FALSE;
			} else {
				if (current_bone->parent != NULL) current_bone = current_bone->parent;
				if (current_bone->selected) children--;
				gone_back = TRUE;
			}

	} while (current_bone->id != bone->id);

	return TRUE;
}

int kudu_draw_all_bones(KuduBone *bone, int options)
{
	if (bone == NULL) {
		kudu_error(KE_OBJECT_INVALID);
		return FALSE;
	}

	KuduBone *current_bone = NULL;

	do {
		if (current_bone == NULL) current_bone = bone;	
		else	current_bone = current_bone->next_sibling;

		kudu_draw_bone(current_bone, options);

	} while (current_bone->next_sibling != NULL);

	return TRUE;
}

int kudu_draw_point(KuduVertex *vertex, int options)	/* Draws a point vertex */
{
	if (vertex == NULL) return FALSE;
	KuduEdge *edge = NULL;
	KuduFace *face = NULL;
	KuduMaterial *material = NULL;

	if (((options & K_RENDER_SHOW_SELECTED) || (options & K_RENDER_SHOW_SELECTED_POINTS)) && (vertex->selected)) {
		/* vertex is selected, render with selection colour */
		glColor3fv(colour_selected);
	} else
	if ((!(options & K_RENDER_NO_COLOURS)) && (!(options & K_RENDER_POINTS_NO_COLOURS))){
		/* Aquire correct colour for vertex */
		edge = vertex->edge;
		if (edge != NULL) {
			if (edge->right_face != NULL) face = edge->right_face;
			else if (edge->left_face != NULL) face = edge->left_face;
			if (face != NULL) material = face->material;

			if (material != NULL) glColor3fv(material->diffuse);
		}
	} else {
		/* No colours, render with unselected colour */
		glColor3fv(colour_default);
	}

	if (options & K_RENDER_SELECT) glPushName((unsigned int)vertex);

	if (!optimise) glBegin(GL_POINTS);
	glVertex3fv(vertex->av);
	if (!optimise) glEnd();

	if (options & K_RENDER_SELECT) glPopName();

	return TRUE;
}

int kudu_draw_edge(KuduEdge *edge, int options)	/* Draws an edge */
{
	if (edge == NULL) return FALSE;
	KuduVertex *s_vertex, *e_vertex;
	KuduFace *face = NULL;
	KuduMaterial *material = NULL;

	s_vertex = edge->start_vertex;
	e_vertex = edge->end_vertex;

	if ((s_vertex == NULL) || (e_vertex == NULL)) return FALSE;

	if (((options & K_RENDER_SHOW_SELECTED) || (options & K_RENDER_SHOW_SELECTED_EDGES)) && (edge->selected)) {
		/* Edge is selected, render with selection colour */
		glColor3fv(colour_selected);
	} else
	if ((!(options & K_RENDER_NO_COLOURS)) && (!(options & K_RENDER_EDGES_NO_COLOURS))) {
		/* Aquire correct colour for edge */
		if (edge->right_face != NULL) face = edge->right_face;
		else if (edge->left_face != NULL) face = edge->left_face;
		if (face != NULL) material = face->material;

		glColor3fv(material->diffuse);
	} else {
		/* No colours, render edge with unselected colour */
		glColor3fv(colour_default);
	}

	if (options & K_RENDER_SELECT) glPushName((unsigned int)edge);

	if (!optimise) glBegin(GL_LINES);
	glVertex3fv(s_vertex->av);
	glVertex3fv(e_vertex->av);
	if (!optimise) glEnd();

	if (options & K_RENDER_SELECT) glPopName();

	return TRUE;
}

int kudu_draw_face(KuduFace *face, int options)
{
	if (face == NULL) return FALSE;

	KuduEdge *current_edge = face->edge;
	if (current_edge == NULL) return FALSE;
	float mat[] = {0.0, 0.0, 0.0, 0.0};
	int lights_on = (options & K_RENDER_LIGHTING);
	int flat_shading = (options & K_RENDER_FLAT_SHADING);
	int show_selected = (options & K_RENDER_SHOW_SELECTED) + (options & K_RENDER_SHOW_SELECTED_FACES);
	int lights = glIsEnabled(GL_LIGHTING);

	KuduMaterial *material = face->material;
	KuduVertex *current_vertex;

	/* No lighting was requested, but gl lights are still on.. switch them off */
	if ((!lights_on) && (lights)) glDisable(GL_LIGHTING);

	if ((show_selected) && ((face->selected) || (options & K_RENDER_SHAPE_SELECTED))) {
		if (lights) glDisable(GL_LIGHTING);
		glColor3fv(colour_selected);
		lights_on = FALSE;
	} else
	if ((!(options & K_RENDER_NO_COLOURS)) && (material != NULL)) {
		if (lights_on) {
			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, material->diffuse);
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, material->ambient);
			glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, material->specular);
			glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, material->emission);
			glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, material->shininess);
		} else {
			glColor3fv(material->diffuse);
		}
	} else if (options & K_RENDER_NO_COLOURS) {
		if (lights_on) {
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, colour_selected);
			glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat);
			glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, mat);
			glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 50.0);
		} else {
			glColor3fv(colour_default);
		}
	}

	if (options & K_RENDER_SELECT) glPushName((unsigned int)face);
	glBegin(GL_POLYGON);
	if ((lights_on) && (flat_shading)) glNormal3fv(face->fn);

	do {
		if (current_edge->right_face == face) {
			current_vertex = current_edge->start_vertex;
			current_edge = current_edge->right_succ;
		} else
		if (current_edge->left_face == face) {
			current_vertex = current_edge->end_vertex;
			current_edge = current_edge->left_succ;
		} else	break;

		if (current_vertex != NULL) {
			if ((lights_on) && (!flat_shading)) glNormal3fv(current_vertex->n);
			glVertex3fv(current_vertex->av);
		}

	} while ((current_edge != face->edge) && (current_edge != NULL));

	glEnd();
	if (options & K_RENDER_SELECT) glPopName();
	/* Turn gl lights back on if they were */
	if ((!lights_on) && (lights)) glEnable(GL_LIGHTING);

	return TRUE;
}

int kudu_draw_shape(KuduShape *shape, int options)
{
	if (shape == NULL) return FALSE;
	if ((options & K_RENDER_SELECT) || (options & K_RENDER_SELECT_SHAPES)) glPushName((unsigned int)shape);

	KuduFace *current_face = NULL;
	KuduEdge *current_edge = NULL;
	KuduVertex *current_vertex = NULL;
	int first_edge_id, opts, lights_on = glIsEnabled(GL_LIGHTING);

/*	if ((program.edit_mode != EDIT_MODE_NONE) && (shape->selected)) {
		glPushMatrix();
		glTranslatef(program.offset[0], program.offset[1], program.offset[2]);
	}*/

	if (options & K_RENDER_POINTS) {
		if (lights_on) glDisable(GL_LIGHTING);
		if (!(options & K_RENDER_SELECT)) {
			optimise = TRUE;
			glBegin(GL_POINTS);
		}
		do {
			if (current_vertex == NULL) current_vertex = shape->vertex;
			else	current_vertex = current_vertex->next_vertex;

			kudu_draw_point(current_vertex, options);
		} while (current_vertex->next_vertex != NULL);
		if (optimise) {
			glEnd();
			optimise = FALSE;
		}
		if (lights_on) glEnable(GL_LIGHTING);
	}

	if (options & K_RENDER_EDGES) {
		if (lights_on) glDisable(GL_LIGHTING);
		if (!(options & K_RENDER_SELECT))  {
			optimise = TRUE;
			glBegin(GL_LINES);
		}
		do {
			if (current_edge == NULL) current_edge = shape->edge;
			else	current_edge = current_edge->next_edge;

			kudu_draw_edge(current_edge, options);
		} while (current_edge->next_edge != NULL);
		if (optimise) {
			glEnd();
			optimise = FALSE;
		}
		if (lights_on) glEnable(GL_LIGHTING);
	}

	if (options & K_RENDER_FACES) {
		opts = options;
		if ((options & K_RENDER_SHOW_SELECTED_SHAPES) && (shape->selected))
			if (!(opts & K_RENDER_SHAPE_SELECTED))
				opts += K_RENDER_SHAPE_SELECTED;
		do {
			if (current_face == NULL) current_face = shape->face;
			else	current_face = current_face->next_face;

			kudu_draw_face(current_face, opts);

		} while (current_face->next_face != NULL);
	}

	if ((options & K_RENDER_SELECT) || (options & K_RENDER_SELECT_SHAPES)) glPopName();

/*	if ((program.edit_mode != EDIT_MODE_NONE) && (shape->selected)) {
		glPopMatrix();
	}*/

	return TRUE;
}

int kudu_draw_shapes(KuduShape *shape, int options)
{
	if (shape == NULL) return FALSE;

	if (!cols_set) kudu_graphics_colours_reload();

	KuduShape *current_shape = NULL;

	do {
		if (current_shape == NULL) current_shape = shape;
		else current_shape = current_shape->next_shape;

		kudu_draw_shape(current_shape, options);

	} while (current_shape->next_shape != NULL);

	return TRUE;
}

int kudu_draw_object(KuduObject *object, int options)
{
	if (object == NULL) return FALSE;

	if (options & K_RENDER_SELECT_OBJECTS) glPushName(object->id);
	if (options & K_RENDER_TRANSLATE_POS) {
		glPushMatrix();
		glTranslatef(object->position[0], object->position[1], object->position[2]);
	}
	if (object->skin != NULL) kudu_draw_shapes(object->skin, options);
	if (options & K_RENDER_SELECT_OBJECTS) glPopName();
	if (options & K_RENDER_TRANSLATE_POS) glPopMatrix();

	return TRUE;
}

int kudu_draw_objects(KuduObject *object, int options)
{
	if (object == NULL) return FALSE;

	KuduObject *current_object = NULL;
	int parse_backwards, eol;

	for (parse_backwards = 0; parse_backwards < 2; parse_backwards++) {
		current_object = NULL;
		eol = FALSE;
		do {
			if (current_object == NULL) current_object = object;
			else if (!parse_backwards) current_object = current_object->next_object;
				else	current_object = current_object->previous_object;

			if ((options & K_RENDER_OMIT_SELECTED_OBJECT) && (current_object == program.selected_object))
				goto eol_test;

			kudu_draw_object(current_object, options);

			eol_test:
			if (!parse_backwards) {
				if (current_object->next_object == NULL) eol = TRUE;
			} else {
				if (current_object->previous_object == NULL) eol = TRUE;
			}
		} while (!eol);
	}

	return TRUE;
}

/* Actual graphical representation of a bone */
int kudu_graphics_init_bone_lists(void)
{
	program.bone_graphic_lists = glGenLists(3);

	/* This list holds the begining end of the bone */
	glNewList(program.bone_graphic_lists, GL_COMPILE);
	gdk_gl_draw_sphere(FALSE, 0.06, 8, 8);
	glEndList();

	/* This list holds the end of the bone */
	glNewList(program.bone_graphic_lists+1, GL_COMPILE);
	gdk_gl_draw_sphere(FALSE, 0.03, 8, 8);
	glEndList();

	/* This list holds the selection drawn around the */
	/* begining and end of the bone (when selected) */
	glNewList(program.bone_graphic_lists+2, GL_COMPILE);
	gdk_gl_draw_cube(FALSE, 0.2);
	glEndList();

}

/*void kudu_draw_terrain(K_TERRAIN *terrain, K_TEXTURE_LIST *textureList, GLint DETAIL)
{
	if (!GET_BIT(terrain->flags, TERRAIN_LOADED)) return;

	GLint a, b, c, v[6], curPos;
	GLfloat *normals, *vertices;
	GLfloat mat1[4], mat2[4], mat3[4], mat4[4], mat5[4], mat6[4];
	GLubyte rgb[3], texno;
	GLint do_mat = 0, do_norm = 0, do_tex = 0;

	if ((terrain->type == TERRAIN_TEXTURED) ||
	    (terrain->type == TERRAIN_TEXTURE_MAPPED)) {
		glEnable(GL_TEXTURE_2D);
		do_tex = GL_TRUE;
		do_mat = GL_FALSE;
	}

	if ((terrain->type == TERRAIN_COLOUR_MAPPED) || (terrain->type == TERRAIN_COLOUR_COPY_MAPPED)) {
		do_mat = GL_TRUE;
	}

	if (DETAIL < 3) {
		glDisable(GL_LIGHTING);
		glPolygonMode(GL_FRONT_ANK_BACK, GL_LINE);
		do_norm = GL_FALSE;
	} else {
		do_norm = GL_TRUE;
		normals = (GLfloat*)malloc((((terrain->width*terrain->length)*3)*sizeof(GLfloat)));

		for (a = 0; a < terrain->width; a++)
			for (b = 0; b < terrain->length; b++) {
				curPos = ((terrain->width * b)*3)+(a*3);
				v[0] = ((terrain->width * b)*3)+((a+1)*3);
				v[1] = ((terrain->width * b)*3)+(a*3);
				v[2] = ((terrain->width *(b+1))*3)+(a*3);

				NORMAL_VECTOR(&terrain->terrainv[v[0]], &terrain->terrainv[v[1]], &terrain->terrainv[v[2]],
					&normals[curPos]);

			}

	}

	vertices = (GLfloat*)malloc((((terrain->width*terrain->length)*3)*sizeof(GLfloat)));

	for (a = 0; a < terrain->width; a++)
		for (b = 0; b < terrain->length; b++) {
			curPos = ((terrain->width * b)*3)+(a*3);
			vertices[curPos] = (terrain->terrainv[curPos] * terrain->widthM);
			vertices[curPos+1] = (terrain->terrainv[curPos+1] * terrain->heightM);
			vertices[curPos+2] = (terrain->terrainv[curPos+2] * terrain->lengthM);
	}

	if (DETAIL > 2) {
		glMaterialf(GL_FRONT_ANK_BACK, GL_SHININESS, 50.0);
	}

		for (a = 0; a < terrain->width-1; a++)
			for (b = 0; b < terrain->length-1; b++) {
				v[0] = ((terrain->width * b)*3)+((a+1)*3);
				v[1] = ((terrain->width * b)*3)+(a*3);
				v[2] = ((terrain->width *(b+1))*3)+(a*3);
				v[3] = ((terrain->width *(b+1))*3)+((a+1)*3);
				v[4] = v[0];
				v[5] = v[2];


				if ((DETAIL > 2) && (terrain->type == TERRAIN_TEXTURE_MAPPED)) {
					rgb[0] = terrain->rgbcols[v[0]];
					rgb[1] = terrain->rgbcols[v[0]+1];
					rgb[2] = terrain->rgbcols[v[0]+2];
					BINK_TEXTURE_RGB(textureList, rgb);
				}*/
		/*		printf("%d %d %d\n", rgb[0], rgb[1], rgb[2]);
				printf("%4.2f %4.2f %4.2f\n", terrain->rgbcols[v[0]], terrain->rgbcols[v[0]+1],
						terrain->rgbcols[v[0]+2]);*/
/*
				if ((DETAIL > 2) && (do_mat)) {
					for (c = 0; c < 3; c++) {
						mat1[c] = (terrain->rgbcols[v[0]+c] * (1.0 / 256));
						mat2[c] = (terrain->rgbcols[v[1]+c] * (1.0 / 256));
						mat3[c] = (terrain->rgbcols[v[2]+c] * (1.0 / 256));

						mat4[c] = (terrain->rgbcols[v[3]+c] * (1.0 / 256));
						mat5[c] = (terrain->rgbcols[v[4]+c] * (1.0 / 256));
						mat6[c] = (terrain->rgbcols[v[5]+c] * (1.0 / 256));
					}

					mat1[3] = 1.0;
					mat2[3] = 1.0;
					mat3[3] = 1.0;
					mat4[3] = 1.0;
					mat5[3] = 1.0;
					mat6[3] = 1.0;
				}

				if (DETAIL == 0) glBegin(GL_POINTS); else
				if (DETAIL == 1) glBegin(GL_QUADS); else
				if (DETAIL >= 2) glBegin(GL_TRIANGLES);

				if (DETAIL > 2) {
					if (do_mat) glMaterialfv(GL_FRONT_ANK_BACK, GL_AMBIENT_ANK_DIFFUSE, mat1);
					if (do_norm) glNormal3fv(&normals[v[0]]);
					if (do_tex) glTexCoord2f(1.0, 0.0);
				}
				glVertex3fv(&vertices[v[0]]);

				if (DETAIL == 0) continue;

				if (DETAIL > 2) {
					if (do_mat) glMaterialfv(GL_FRONT_ANK_BACK, GL_AMBIENT_ANK_DIFFUSE, mat2);
					if (do_norm) glNormal3fv(&normals[v[1]]);
					if (do_tex) glTexCoord2f(0.0, 0.0);
				}
				glVertex3fv(&vertices[v[1]]);

				if (DETAIL > 2) {
					if (do_mat) glMaterialfv(GL_FRONT_ANK_BACK, GL_AMBIENT_ANK_DIFFUSE, mat3);
					if (do_norm) glNormal3fv(&normals[v[2]]);
					if (do_tex) glTexCoord2f(0.0, 1.0);
				}
				glVertex3fv(&vertices[v[2]]);

				if (DETAIL > 2) {
					if (do_mat) glMaterialfv(GL_FRONT_ANK_BACK, GL_AMBIENT_ANK_DIFFUSE, mat4);
					if (do_norm) glNormal3fv(&normals[v[3]]);
					if (do_tex) glTexCoord2f(1.0, 1.0);
				}
				glVertex3fv(&vertices[v[3]]);

				if (DETAIL == 1) continue;

				if (DETAIL > 2) {
					if (do_mat) glMaterialfv(GL_FRONT_ANK_BACK, GL_AMBIENT_ANK_DIFFUSE, mat5);
					if (do_norm) glNormal3fv(&normals[v[4]]);
					if (do_tex) glTexCoord2f(1.0, 0.0);
				}
				glVertex3fv(&vertices[v[4]]);

				if (DETAIL > 2) {
					if (do_mat) glMaterialfv(GL_FRONT_ANK_BACK, GL_AMBIENT_ANK_DIFFUSE, mat6);
					if (do_norm) glNormal3fv(&normals[v[5]]);
					if (do_tex) glTexCoord2f(0.0, 1.0);
				}
				glVertex3fv(&vertices[v[5]]);

				glEnd();
			}


	if (DETAIL < 3) {
		glEnable(GL_LIGHTING);
		glPolygonMode(GL_FRONT_ANK_BACK, GL_FILL);
	} else {
		free(normals);
	}

	free(vertices);

	if ((terrain->type == TERRAIN_TEXTURED) ||
	    (terrain->type == TERRAIN_TEXTURE_MAPPED)) glDisable(GL_TEXTURE_2D);*/
			/*glDisable(GL_TEXTURE_2D);*/

		/*	tf[3] = (GLfloat)(tf[0])-(image.width*(tf[0]/2));
			tf[4] = (GLfloat)(tf[1])-(image.height*(tf[1]/2));
			tf[5] = (GLfloat)(image.width*tf[0])-(image.width*(tf[0]/2));
			tf[6] = (GLfloat)(image.height*tf[1])-(image.height*(tf[1]/2));

			glDisable(GL_LIGHTING);
			glBegin(GL_QUADS);
			glVertex3f(tf[3], 0.0, tf[4]);
			glVertex3f(tf[3], 0.0, tf[6]);
			glVertex3f(tf[5], 0.0, tf[4]);
			glVertex3f(tf[5], 0.0, tf[6]);
			glEnd();
			glEnable(GL_LIGHTING);



			glEndList();

			free(terrainv);
			free(normals);
			free(rgbcols);
		}*/
/*}*/

