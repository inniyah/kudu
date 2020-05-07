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
#ifndef OPTIONS_H_INCLUDED
#define OPTIONS_H_INCLUDED

#include "globals.h"
#include "utils.h"
#include "about.h"

#define KO_TYPE_INT 1
#define KO_TYPE_FLOAT 2
#define KO_TYPE_CHAR 3
#define KO_TYPE_STRING 4

typedef enum {
	/* Gui / interface */
	KO_WINDOW_POSITION,
	KO_WINDOW_SIZE,
	KO_WINDOW_MAXED,
	KO_MOUSE_SCROLL_ZOOM,
	KO_USER_ACCEL_KEYS,
	KO_SHOW_SPLASH,

	KO_BONE_DIALOG_POSITION,
	KO_BONE_DIALOG_LOCK_NAME,

	/* Camera viewport */
	KO_VIEW_ORTHOGRAPHIC,

	/* Paths */
	KO_DIR_USER_HOME,
	KO_DIR_LAST_OPEN,
	KO_DIR_LAST_SAVE,
	KO_DIR_USER_DATA,
	KO_DIR_USER_SCRIPTS,

	/* Grid and axes */
	KO_GRID_COLOUR,
	KO_AXES_COLOUR,

	/* Skeleton drawing options */
	KO_BONES_VISIBLE,
	KO_BONES_DETAIL,
	KO_BONES_AXES,
	KO_BONES_COLOUR_DEFAULT,
	KO_BONES_COLOUR_SELECTED,
	KO_BONES_COLOUR_CHILDREN,
	KO_JOINTS_VISIBLE,
	KO_JOINTS_COLOUR_DEFAULT,
	KO_JOINTS_COLOUR_SELECTED,

	/* Skin drawing options */
	KO_SKIN_LIT,
	KO_SKIN_SMOOTH,
	KO_SKIN_REAL_COLOURS,
	KO_SKIN_VISIBLE,
	KO_SKIN_TEXTURED,
	KO_SKIN_DETAIL,
	KO_SKIN_COLOUR_DEFAULT,
	KO_SKIN_COLOUR_SELECTED,
	KO_SKIN_COLOUR_HIGHLIGHT,

	/* Playback options */
	KO_PLAYBACK_LOOP,
	KO_PLAYBACK_USE_CAMERA,

	/* Program use */
	KO_SELECTION_MODE,
	KO_SELECTION_BUFFER,

	KO_OPTIONS_COUNT,	/* Number of options */
} KuduOption;

/* Option names as strings, this is for saving and loading to and */
/* from disk. Names must be in the same order as in the enumarated */
/* list above. Setting a name to NULL will make that particular */
/* option a non-saved option - for e.g. KO_DIR_USER_HOME is set to */
/* NULL as the home dir is resolved on startup and should not be fixed. */
static const char *K_OPTION_NAME[] = {
	/* Gui / interface */
	"KO_WINDOW_POSITION",
	"KO_WINDOW_SIZE",
	"KO_WINDOW_MAXED",
	"KO_MOUSE_SCROLL_ZOOM",
	"KO_USER_ACCEL_KEYS",
	"KO_SHOW_SPLASH",

	"KO_BONE_DIALOG_POSITION",
	NULL,

	/* Camera viewport */
	"KO_VIEW_ORTHOGRAPHIC",

	/* Paths */
	NULL,
	"KO_DIR_LAST_OPEN",
	"KO_DIR_LAST_SAVE",
	NULL,
	NULL,

	/* Grid and axes */
	"KO_GRID_COLOUR",
	"KO_AXES_COLOUR",

	/* Skeleton drawing options */
	"KO_BONES_VISIBLE",
	"KO_BONES_DETAIL",
	"KO_BONES_AXES",
	"KO_BONES_COLOUR_DEFAULT",
	"KO_BONES_COLOUR_SELECTED",
	"KO_BONES_COLOUR_CHILDREN",
	"KO_JOINTS_VISIBLE",
	"KO_JOINTS_COLOUR_DEFAULT",
	"KO_JOINTS_COLOUR_SELECTED",

	/* Skin drawing options */
	"KO_SKIN_LIT",
	"KO_SKIN_SMOOTH",
	"KO_SKIN_REAL_COLOURS",
	"KO_SKIN_VISIBLE",
	"KO_SKIN_TEXTURED",
	"KO_SKIN_DETAIL",
	"KO_SKIN_COLOUR_DEFAULT",
	"KO_SKIN_COLOUR_SELECTED",
	"KO_SKIN_COLOUR_HIGHLIGHT",

	/* Playback options */
	"KO_PLAYBACK_LOOP",
	"KO_PLAYBACK_USE_CAMERA",

	/* Program use */
	"KO_SELECTION_MODE",
	"KO_SELECTION_BUFFER",
};

typedef struct _KuduOptionItem {
	unsigned char guess_type;	/* Last known data type used on this option */
	unsigned char enabled;		/* Enabled flag for simple boolean options */
	int opt_size;			/* Size of option data in bytes */
	void *opt;			/* Pointer to option data */
} KuduOptionItem;

int kudu_options_init(void);
int kudu_options_finish(void);

int kudu_options_enable(KuduOption);
int kudu_options_disable(KuduOption);
int kudu_options_enabled(KuduOption);

int kudu_options_set_int(KuduOption, int, int*);
int kudu_options_set_int_no(KuduOption, int, int);
int kudu_options_set_float(KuduOption, int, float*);
int kudu_options_set_float_no(KuduOption, int, float);
int kudu_options_set_char(KuduOption, int, char*);
int kudu_options_set_char_no(KuduOption, int, char);
int kudu_options_set_string(KuduOption, char*);

int kudu_options_get_int(KuduOption, int, int*);
int kudu_options_get_int_no(KuduOption, int);
int kudu_options_get_float(KuduOption, int, float*);
float kudu_options_get_float_no(KuduOption, int);
int kudu_options_get_char(KuduOption, int, char*);
char kudu_options_get_char_no(KuduOption, int);
char *kudu_options_get_string(KuduOption);
int kudu_options_strlen(KuduOption);

int kudu_options_set(KuduOption, int, int, ...);
int kudu_options_get(KuduOption, int, ...);

int kudu_options_set_defaults(void);
int kudu_options_save_to_file(char*);
int kudu_options_load_from_file(char*);

int kudu_options_save_accel_map(void);
int kudu_options_load_accel_map(void);

int kudu_options_parse(int, char**);

#endif /* OPTIONS_H_INCLUDED */
