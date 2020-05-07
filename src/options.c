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
#include "options.h"

static KuduOptionItem *option_list = NULL;

int kudu_options_init(void)
{
	if (option_list != NULL) return FALSE;

	option_list = (KuduOptionItem*)malloc((sizeof(KuduOptionItem) * KO_OPTIONS_COUNT));

	return TRUE;
}

int kudu_options_finish(void)
{
	if (option_list == NULL) return FALSE;

	/* First save all current options in the user config file */
	const char *user_data = kudu_options_get_string(KO_DIR_USER_DATA);
	char *user_config = (char*)malloc(strlen(user_data)+10);
	if (user_config != NULL) {
		sprintf(user_config, "%skudu.conf\0", user_data);
		kudu_options_save_to_file(user_config);
		free(user_config);
	}

	KuduOptionItem *opt;
	int a;

	for (a = 0; a < KO_OPTIONS_COUNT; a++) {
		opt = &option_list[a];
		if (opt->opt_size > 0) {
			free(opt->opt);
		}
	}

	free(option_list);
	option_list = NULL;

	return TRUE;
}

/* Set "enabled" flag on an option to TRUE */
int kudu_options_enable(KuduOption option)
{
	if (option_list == NULL) return FALSE;

	KuduOptionItem *opt = &option_list[option];

	opt->enabled = TRUE;

	return TRUE;
}

/* Set "enabled" flag on an option to FALSE */
int kudu_options_disable(KuduOption option)
{
	if (option_list == NULL) return FALSE;

	KuduOptionItem *opt = &option_list[option];

	opt->enabled = FALSE;

	return TRUE;
}

/* Return the status of the "enabled" flag on an option */
int kudu_options_enabled(KuduOption option)
{
	if (option_list == NULL) return FALSE;

	KuduOptionItem *opt = &option_list[option];

	return opt->enabled;
}

/* Allocate, free or reallocate memory block for an option */
int kudu_options_set_data_size(KuduOptionItem *option, int size, int downscale)
{
	if (option == NULL) return FALSE;

	if (size == option->opt_size) return TRUE;
	if ((option->opt_size > size) && (!downscale)) return TRUE;

	if (size == 0) {
		if (option->opt_size > 0) free(option->opt);
		option->opt = NULL;
		option->opt_size = 0;
	} else {
		if (option->opt_size > 0) option->opt = realloc(option->opt, size);
		else	option->opt = malloc(size);
		option->opt_size = size;
	}

	return TRUE;
}

/* Set an integer option using an array */
int kudu_options_set_int(KuduOption option, int num, int *i)
{
	if ((option_list == NULL) || (i == NULL)) return FALSE;

	KuduOptionItem *opt = &option_list[option];
	int a, *si;

	kudu_options_set_data_size(opt, num*sizeof(int), FALSE);

	opt->guess_type = KO_TYPE_INT;
	si = (int*)opt->opt;

	for (a = 0; a < num; a++) si[a] = i[a];

	return TRUE;
}

/* Set an exact int in an option */
int kudu_options_set_int_no(KuduOption option, int num, int i)
{
	if (option_list == NULL) return FALSE;

	KuduOptionItem *opt = &option_list[option];
	int *si;

	kudu_options_set_data_size(opt, (num+1)*sizeof(int), FALSE);

	opt->guess_type = KO_TYPE_INT;
	si = (int*)opt->opt;

	si[num] = i;

	return TRUE;
}

/* Set an option using a float array */
int kudu_options_set_float(KuduOption option, int num, float *f)
{
	if ((option_list == NULL) || (f == NULL)) return FALSE;

	KuduOptionItem *opt = &option_list[option];
	int a;
	float *sf;

	kudu_options_set_data_size(opt, num*sizeof(float), FALSE);

	opt->guess_type = KO_TYPE_FLOAT;
	sf = (float*)opt->opt;

	for (a = 0; a < num; a++) sf[a] = f[a];

	return TRUE;
}

/* Set an exact float in an option */
int kudu_options_set_float_no(KuduOption option, int num, float f)
{
	if (option_list == NULL) return FALSE;

	KuduOptionItem *opt = &option_list[option];
	float *sf;

	kudu_options_set_data_size(opt, (num+1)*sizeof(float), FALSE);

	opt->guess_type = KO_TYPE_FLOAT;
	sf = (float*)opt->opt;

	sf[num] = f;

	return TRUE;
}

/* Set an option using a char array */
int kudu_options_set_char(KuduOption option, int num, char *c)
{
	if ((option_list == NULL) || (c == NULL)) return FALSE;

	KuduOptionItem *opt = &option_list[option];
	int a;
	char *sc;

	kudu_options_set_data_size(opt, num*sizeof(char), FALSE);

	opt->guess_type = KO_TYPE_CHAR;
	sc = (char*)opt->opt;

	for (a = 0; a < num; a++) sc[a] = c[a];

	return TRUE;
}

/* Set an exact char in an option */
int kudu_options_set_char_no(KuduOption option, int num, char c)
{
	if (option_list == NULL) return FALSE;

	KuduOptionItem *opt = &option_list[option];
	char *sc;

	kudu_options_set_data_size(opt, (num+1)*sizeof(char), FALSE);

	opt->guess_type = KO_TYPE_CHAR;
	sc = (char*)opt->opt;

	sc[num] = c;

	return TRUE;
}

/* Set an option using a string */
/* String will be NULL terminated (only if not already) */
int kudu_options_set_string(KuduOption option, char *s)
{
	if ((option_list == NULL) || (s == NULL)) return FALSE;

	KuduOptionItem *opt = &option_list[option];
	int a, nullterm = 0, num = strlen(s);
	char *ss;

	if (s[num-1] != '\0') nullterm = 1;

	kudu_options_set_data_size(opt, (num+nullterm)*sizeof(char), TRUE);

	opt->guess_type = KO_TYPE_STRING;
	ss = (char*)opt->opt;

	for (a = 0; a < num; a++) ss[a] = s[a];
	if (nullterm) ss[num] = '\0';

	return TRUE;
}

/* Retrieve an option in an array of ints */
int kudu_options_get_int(KuduOption option, int num, int *i)
{
	if ((option_list == NULL) || (i == NULL)) return FALSE;

	KuduOptionItem *opt = &option_list[option];

	int a, *si, an = 0;

	if (opt->opt_size > 0) {
		si = (int*)opt->opt;
		an = (opt->opt_size / sizeof(int));
	}

	if (an > 0) {
		if (an >= num) for (a = 0; a < num; a++) i[a] = si[a];
		else {
			for (a = 0; a < an; a++) i[a] = si[a];
			for (a = an; a < num; a++) i[a] = 0;
		}
	} else for (a = 0; a < num; a++) i[a] = 0;

	return TRUE;
}

/* Retrieve a particular int from an option */
int kudu_options_get_int_no(KuduOption option, int num)
{
	if (option_list == NULL) return 0;

	KuduOptionItem *opt = &option_list[option];
	int *si;

	if ((opt->opt_size / sizeof(int)) < num+1) return 0;

	si = (int*)opt->opt;

	return si[num];
}

/* Retrieve an option in an array of floats */
int kudu_options_get_float(KuduOption option, int num, float *f)
{
	if ((option_list == NULL) || (f == NULL)) return FALSE;

	KuduOptionItem *opt = &option_list[option];

	int a, an = 0;
	float *sf;

	if (opt->opt_size > 0) {
		sf = (float*)opt->opt;
		an = (opt->opt_size / sizeof(float));
	}

	if (an > 0) {
		if (an >= num) for (a = 0; a < num; a++) f[a] = sf[a];
		else {
			for (a = 0; a < an; a++) f[a] = sf[a];
			for (a = an; a < num; a++) f[a] = 0.0;
		}
	} else for (a = 0; a < num; a++) f[a] = 0.0;

	return TRUE;
}

/* Retrieve a particular float from an option */
float kudu_options_get_float_no(KuduOption option, int num)
{
	if (option_list == NULL) return 0.0;

	KuduOptionItem *opt = &option_list[option];
	float *sf;

	if ((opt->opt_size / sizeof(float)) < num+1) return 0.0;

	sf = (float*)opt->opt;

	return sf[num];
}

/* Retrieve an option in an array of chars */
int kudu_options_get_char(KuduOption option, int num, char *c)
{
	if ((option_list == NULL) || (c == NULL)) return FALSE;

	KuduOptionItem *opt = &option_list[option];

	int a, an = 0;
	char *sc;

	if (opt->opt_size > 0) {
		sc = (char*)opt->opt;
		an = (opt->opt_size / sizeof(char));
	}

	if (an > 0) {
		if (an >= num) for (a = 0; a < num; a++) c[a] = sc[a];
		else {
			for (a = 0; a < an; a++) c[a] = sc[a];
			for (a = an; a < num; a++) c[a] = 0;
		}
	} else for (a = 0; a < num; a++) c[a] = 0;

	return TRUE;
}

/* Retrieve a particular char from an option */
char kudu_options_get_char_no(KuduOption option, int num)
{
	if (option_list == NULL) return 0;

	KuduOptionItem *opt = &option_list[option];
	char *sc;

	if ((opt->opt_size / sizeof(char)) < num+1) return 0;

	sc = (char*)opt->opt;

	return sc[num];
}

/* Retrieve an option as a string */
char *kudu_options_get_string(KuduOption option)
{
	if (option_list == NULL) return FALSE;

	KuduOptionItem *opt = &option_list[option];

	return (char*)opt->opt;
}

/* Return the string length of an option */
int kudu_options_strlen(KuduOption option)
{
	if (option_list == NULL) return 0;

	KuduOptionItem *opt = &option_list[option];

	return (opt->opt_size / sizeof(char));
}

int kudu_options_set(KuduOption option, int type, int num, ...)
{
	if ((option_list == NULL) || (option > KO_OPTIONS_COUNT)) return FALSE;

	va_list ap;
	int a, i;
	char c;
	float f;

	va_start(ap, num);

	for (a = 0; a < num; a++) {
		switch (type) {
			case KO_TYPE_CHAR:
				c = (char)va_arg(ap, int);
				kudu_options_set_char_no(option, a, c);
				break;
			case KO_TYPE_INT:
				i = va_arg(ap, int);
				kudu_options_set_int_no(option, a, i);
				break;
			case KO_TYPE_FLOAT:
				f = (float)va_arg(ap, double);
				kudu_options_set_float_no(option, a, f);
				break;

		}
	}

	return TRUE;
}

int kudu_options_get(KuduOption option, int type, ...)
{

	if ((option_list == NULL) || (option > KO_OPTIONS_COUNT)) return FALSE;

	va_list ap;
	char *c;
	int *i, busy = TRUE, num = 0;
	float *f;

	va_start(ap, type);

	while (busy) {
		num++;
		switch (type) {
			case KO_TYPE_CHAR:
				c = (char*)va_arg(ap, char*);
				if (c == NULL) {
					busy = FALSE;
					break;
				}
				*c = kudu_options_get_char_no(option, num);
				break;
			case KO_TYPE_INT:
				i = (int*)va_arg(ap, int*);
				if (i == NULL) {
					busy = FALSE;
					break;
				}
				*i = kudu_options_get_int_no(option, num);
				break;
			case KO_TYPE_FLOAT:
				f = (float*)va_arg(ap, float*);
				if (f == NULL) {
					busy = FALSE;
					break;
				}
				*f = kudu_options_get_float_no(option, num);
				break;

		}
	}

	return TRUE;
}


/* Setup the paths to the user dirs */
/* or create them if they don't exist */
int kudu_options_configure_user_dirs(void)
{
	if (option_list == NULL) return FALSE;

	const char *user_dir = kudu_options_get_string(KO_DIR_USER_HOME);
	int hlen = strlen(user_dir);
	char *ad, *data_dir, *script_dir;
	struct stat file_stat;

	data_dir = (char*)malloc(hlen + 6);
	sprintf(data_dir, "%s.kudu\0", user_dir);

	script_dir = (char*)malloc(hlen + 6 + 6);
	sprintf(script_dir, "%s.kudu/scripts\0", user_dir);

	if (stat(data_dir, &file_stat) < 0) {
		mkdir(data_dir, 0755);
	}

	if (stat(script_dir, &file_stat) < 0) {
		mkdir(script_dir, 0755);
	}

	ad = kudu_util_correct_path(data_dir);
	kudu_options_set_string(KO_DIR_USER_DATA, ad);
	free(ad);
	free(data_dir);

	ad = kudu_util_correct_path(script_dir);
	kudu_options_set_string(KO_DIR_USER_SCRIPTS, ad);
	free(ad);
	free(script_dir);

	return TRUE;
}


/* Set default options */
/* configure directories */
/* and finally load user config file */
int kudu_options_set_defaults(void)
{
	if (option_list == NULL) return FALSE;

	char *home;
	int temp[8];
	float tempf[18];

	/* Determine current user home path */
	home = (char*)kudu_util_correct_path(getenv("HOME"));
	kudu_options_set_string(KO_DIR_USER_HOME, home);
	free(home);

	kudu_options_set_string(KO_DIR_LAST_OPEN, KUDU_EXAMPLES_DIR);

	kudu_options_enable(KO_WINDOW_POSITION);
	kudu_options_set(KO_WINDOW_POSITION, KO_TYPE_INT, 2, 50, 50);

	kudu_options_enable(KO_WINDOW_SIZE);
	kudu_options_set(KO_WINDOW_SIZE, KO_TYPE_INT, 2, 320, 240);

	kudu_options_enable(KO_WINDOW_MAXED);

	kudu_options_set(KO_BONES_VISIBLE, KO_TYPE_INT, 8, 1, 0, 1, 0, 1, 1, 0, 0);

	kudu_options_set(KO_BONES_DETAIL, KO_TYPE_INT, 8, 1, 3, 1, 3, 3, 3, 3, 3);

	kudu_options_set(KO_SKIN_VISIBLE, KO_TYPE_INT, 8, 1, 1, 1, 1, 1, 1, 0, 1);

	kudu_options_set(KO_SKIN_DETAIL, KO_TYPE_INT, 8, 2, 1, 4, 1, 2, 2, 2, 2);

	kudu_options_set(KO_SKIN_LIT, KO_TYPE_INT, 8, 1, 1, 0, 1, 0, 0, 0, 0);

	kudu_options_set(KO_SKIN_REAL_COLOURS, KO_TYPE_INT, 8, 0, 1, 0, 1, 1, 1, 0, 1);

	kudu_options_set(KO_SKIN_SMOOTH, KO_TYPE_INT, 8, 0, 0, 0, 1, 0, 0, 0, 0);

	kudu_options_set(KO_SKIN_COLOUR_DEFAULT, KO_TYPE_FLOAT, 4, 0.7, 0.7, 0.7, 1.0);

	kudu_options_set(KO_SKIN_COLOUR_SELECTED, KO_TYPE_FLOAT, 4, 1.0, 0.0, 0.0, 1.0);

	kudu_options_set(KO_BONES_COLOUR_DEFAULT, KO_TYPE_FLOAT, 4, 1.0, 0.0, 1.0, 1.0);

	kudu_options_set(KO_BONES_COLOUR_SELECTED, KO_TYPE_FLOAT, 4, 1.0, 0.0, 0.0, 1.0);

	kudu_options_set(KO_BONES_COLOUR_CHILDREN, KO_TYPE_FLOAT, 4, 1.0, 1.0, 1.0, 1.0);

	kudu_options_set(KO_AXES_COLOUR, KO_TYPE_FLOAT, 18, 0.6, 0.0, 0.0,  0.0, 0.6, 0.0,  0.0, 0.0, 0.6,
							    0.6, 0.6, 0.6,  0.6, 0.6, 0.6,  0.6, 0.6, 0.6);

	kudu_options_set(KO_GRID_COLOUR, KO_TYPE_FLOAT, 3, 0.7, 0.7, 0.7);

	kudu_options_set(KO_BONES_AXES, KO_TYPE_INT, 8, 2, 3, 3, 3, 3, 3, 3, 3);

	kudu_options_set(KO_SELECTION_MODE, KO_TYPE_INT, 8, 9, 6, 6, 7, 4, 4, 6, 7);

	kudu_options_enable(KO_MOUSE_SCROLL_ZOOM);
	kudu_options_set_float_no(KO_MOUSE_SCROLL_ZOOM, 0, 10.0);


	kudu_options_configure_user_dirs();

	const char *user_data = kudu_options_get_string(KO_DIR_USER_DATA);
	char *user_config = (char*)malloc(strlen(user_data)+10);
	if (user_config != NULL) {
		sprintf(user_config, "%skudu.conf\0", user_data);
		kudu_options_load_from_file(user_config);
		free(user_config);
	}

	return TRUE;
}

int kudu_options_save_to_file(char *filename)
{
	if (filename == NULL) return FALSE;
	if (option_list == NULL) return FALSE;

	KuduOptionItem *opt;
	FILE *file;
	int a, oc, num, *ip;
	float *fp;
	char *cp;

	file = fopen(filename, "w");
	if (file == NULL) return FALSE;

	for (oc = 0; oc < KO_OPTIONS_COUNT; oc++) {
		opt = &option_list[oc];

		if (K_OPTION_NAME[oc] == NULL) continue;

		/* Option name */
		fprintf(file, "[%s]\n", K_OPTION_NAME[oc]);

		/* Enabled flag */
		if (opt->enabled) fprintf(file, "enabled = true\n");

		/* If option buffer is empty, then thats all for this option */
		if (opt->opt_size == 0) {
			fprintf(file, "\n");
			continue;
		}

		switch (opt->guess_type) {
			case KO_TYPE_INT:
				ip = (int*)opt->opt;
				num = (opt->opt_size / sizeof(int));
				fprintf(file, "ints = ");
				for (a = 0; a < num; a++) {
					fprintf(file, "%d", ip[a]);
					if (a < (num-1)) fprintf(file, ", ");
				}
				fprintf(file, "\n");
				break;
			case KO_TYPE_FLOAT:
				fp = (float*)opt->opt;
				num = (opt->opt_size / sizeof(float));
				fprintf(file, "floats = ");
				for (a = 0; a < num; a++) {
					fprintf(file, "%f", fp[a]);
					if (a < (num-1)) fprintf(file, ", ");
				}
				fprintf(file, "\n");
				break;
			case KO_TYPE_CHAR:
				cp = (char*)opt->opt;
				num = (opt->opt_size / sizeof(char));
				fprintf(file, "chars = ");
				for (a = 0; a < num; a++) {
					fprintf(file, "%d", cp[a]);
					if (a < (num-1)) fprintf(file, ", ");
				}
				fprintf(file, "\n");
				break;
			case KO_TYPE_STRING:
				cp = (char*)opt->opt;
				fprintf(file, "string = \"%s\"\n", cp);
				break;
		}
		fprintf(file, "\n");

	}

	fclose(file);

	return TRUE;
}


int kudu_options_load_from_file(char *filename)
{
	if ((filename == NULL) || (option_list == NULL)) return FALSE;

	KuduOptionItem *opt;
	KuduOption name;
	FILE *file;
	int a, num, os = FALSE;
	float f;
	char data[1024], rname[1024], tmp[1024], *st, c;

	file = fopen(filename, "r");
	if (file == NULL) return FALSE;

	while (fgets(data, 1024, file) != NULL) {
		if (strncmp(data, "#", 1) == 0) continue;

		if (strncmp(data, "[", 1) == 0) {
			if (data[strlen(data)-2] != ']') continue;
			strncpy(rname, &data[1], strlen(data)-3);
			rname[strlen(data)-3] = '\0';

			os = FALSE;
			for (a = 0; a < KO_OPTIONS_COUNT; a++) {
				if (K_OPTION_NAME[a] == NULL) continue;
				if (strcmp(K_OPTION_NAME[a], rname) == 0) {
					name = (unsigned int)a;
					os = TRUE;
					a = KO_OPTIONS_COUNT;
				}
			}

			continue;
		}

		if (strncasecmp(data, "enabled =", 9) == 0) {
			if (!os) continue;
			strncpy(tmp, &data[10], strlen(data)-2);
			tmp[strlen(data)-11] = 0;
			if (strcasecmp(tmp, "true") == 0) kudu_options_enable(name);
			if (strcasecmp(tmp, "false") == 0) kudu_options_disable(name);
			continue;
		}

/*		if (strncasecmp(data, "forsaken =", 10) == 0) {
			if (!os) continue;
			strncpy(tmp, &data[11], strlen(data)-2);
			tmp[strlen(data)-12] = 0;
			if (strcasecmp(tmp, "true") == 0) kudu_options_forsaken(name);
			if (strcasecmp(tmp, "false") == 0) kudu_options_not_forsaken(name);
			continue;
		}*/

		if (strncasecmp(data, "ints =", 6) == 0) {
			if (!os) continue;
			strncpy(tmp, &data[7], strlen(data)-1);
			st = strtok(tmp, ",");
			if (st == NULL) continue;
			num = 0;
			do {
				a = atoi(st);
				kudu_options_set_int_no(name, num, a);
				st = strtok(NULL, ",");
				num++;
			} while (st != NULL);
			continue;
		}

		if (strncasecmp(data, "floats =", 8) == 0) {
			if (!os) continue;
			strncpy(tmp, &data[9], strlen(data)-1);
			st = strtok(tmp, ",");
			if (st == NULL) continue;
			num = 0;
			do {
				f = strtod(st, NULL);
				kudu_options_set_float_no(name, num, f);
				st = strtok(NULL, ",");
				num++;
			} while (st != NULL);
			continue;
		}

		if (strncasecmp(data, "chars =", 7) == 0) {
			if (!os) continue;
			strncpy(tmp, &data[9], strlen(data)-1);
			st = strtok(tmp, ",");
			if (st == NULL) continue;
			num = 0;
			do {
				c = (char)atoi(st);
				kudu_options_set_char_no(name, num, c);
				st = strtok(NULL, ",");
				num++;
			} while (st != NULL);
			continue;
		}

		if (strncasecmp(data, "string =", 8) == 0) {
			if (!os) continue;
			strncpy(tmp, &data[9], strlen(data)-1);
			num = strlen(data)-10;
			tmp[num] = 0;
			if ((tmp[0] != '"') || (tmp[num-1] != '"')) continue;
			strncpy(tmp, &tmp[1], num-2);
			tmp[num-2] = 0;
			kudu_options_set_string(name, tmp);
			if (st == NULL) continue;
		}

	}

	fclose(file);

	return TRUE;
}

