/*
 *  This file is part of Joe's Own Editor for Windows.
 *  Copyright (c) 2014 John J. Jordan.
 *
 *  Joe's Own Editor for Windows is free software: you can redistribute it 
 *  and/or modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation, either version 2 of the 
 *  License, or (at your option) any later version.
 *
 *  Joe's Own Editor for Windows is distributed in the hope that it will
 *  be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Joe's Own Editor for Windows.  If not, see
 *  <http://www.gnu.org/licenses/>.
 */

#include "jwwin.h"
#include <io.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "jwglobals.h"
#include "jwcolors.h"
#include "jwutils.h"
#include "jwbuiltin.h"

/*

EXPLANATION

The code here reads .joecolors files and stores the information in the structures
found in jwcolors.h.  It also contains code that enumerates all the .joecolors
files in the standard locations to populate the list of color schemes available
to the user.

JOECOLORS files are essentially INI files with three sections.  The first section
stores information about the color scheme: the name, the author, and other such
information.  This code only concerns itself with the name as that's the only bit
that is surfaced in the UI (the others are there to ensure that credit is given
somewhere, even though it's mostly hidden).

The second section ("colors") defines the RGB and attribute values for the common
colors (the 16 standard colors and special colors used by both PuTTY and JOE).
This section may also define variables that can be used by the third section to
assist in authoring.  The format is "colorname=#RRGGBB attr1 attr2 attrN"
where the attributes are optional, but can be "bold", "italic", "underline",
and "inverse" (however, only 'bold' is implemented for this section).  The
color names are listed in the colordefaults structure below and are case
insensitive.  If a color is missing from this section, it is defined to its
default value, mapped in the colordefaults structure.

The third section ("syntax") defines colors for syntax tokens from JOE's syntax
highlighting definitions.  The source JSF files typically define their own
colors, but do not always fit the scheme (and are also designed to use a very
limited subset of the 16 colors), so this section allows the scheme to override
those defaults.  Any syntax token that is not overridden is mapped to an RGB
value defined by the second section (according to the default color from the
JSF file).  The format is "[syntax.]token=fg[/bg] attr1 ..." (where text in
brackets is optional).  If the syntax is not specified, it refers to a token
name used regardless of syntax.  fg and bg may either be hex RGB values of the
form "#RRGGBB" or variables defined in the previous section of the form
"$variable".

The jwcolors struct defines a single color scheme.  It consists mainly of two
arrays of other structs: jwcolorentry and jwsyntaxcolor.  jwcolorentry is a key-
value pair from palette indices to RGB values.  These keys and values are
populated into PuTTY's palette when the scheme is loaded.  jwsyntaxcolor is a
key-value pair from syntax token names to palette indices.  The token name is
a 2-tuple, where the syntax name element may be null.  It is sorted so that
lookups can be performed by a binary search (as every token in every syntax
will need to perform a string search).

The jwcolorlist structure is a linked list whose value is a jwcolors struct.
This structure also contains the filepath and name of each scheme.  The list
is populated by loadcolorschemes(), but this does not load every scheme
available.  Instead, the jwcolors pointer will be NULL until the user switches
to the scheme, at which point it will be fully loaded.

The loading algorithm is a bit complicated, so to keep it clear, here is a
rough guide:

First, the output demands arrays, but until we know how many items there will
be, we must use linked lists, which are defined as the tmp_ structures.  Two
lists are created as the file is read: tmp_jwcolorvar list from the "colors"
section and tmp_jwsyntaxcolor from the "syntax" section.  When the colors
section is read, it stores each entry as a variable->color mapping
(tmp_jwcolorvar) -- no color codes are set until the next steps.  The
tmp_jwsyntaxcolor elements from the syntax section contain two tmp_jwcolorspec
structs, which may either be references to color variables (to be resolved
later) or RGB specifications.  These also contain a jwsyntaxcolor struct, which
at first is used for the name, then is used to store the results of a color or
variable lookup, and finally is used to copy directly into the output array.

Once the file is read, a third list is created: tmp_jwcolorlist.  This is
calculated first by mapstandardcolors(), which processes the builtin default
color list.  For each entry, if a variable exists (in tmp_jwcolorvar) then
create an entry with its value, and otherwise use the default value.  As
tmp_jwcolorvar entries are assigned to palette indices, their assignment
pointers are set so that further uses of them (in the next section especially)
already know where to go... unless the 'special' flag is set, which indicates
the color's value is >255 -- in the range of special PuTTY colors, and out of
reach from JOE.

The bit of processing goes over the syntax colors, which assigns colors for
all of the tmp_jwsyntaxcolor entries.  This is handled by mapsyntaxcolors and
assignsyntaxcolor, which loops through each syntax color, resolves any variables
it points to, and assigns a color for the entry.  If a color already exists
(its color code and attributes) on the palette (tmp_jwcolorlist) it is reused
unless it is special.  This process adds colors to the palette for all unique
colors addressed by the syntax, and effectively ignores unused color variables.

The last step is performed by organizecolors, which converts the temporary
linked lists into arrays in a new jwcolors struct, to be consumed by the rest
of the system.

*/


/******************************************************** Standard colors */

struct jwcolordefault
{
	char		*name;
	int		paletteidx;
	int		defvalue;
};

struct jwcolordefault colordefaults[] = {
	/* Default colors */
	{"defaultfg",		256,	0x000000},
	{"bolddefaultfg",	257,	0x0000000 | COLOR_FLAG_BOLD},
	{"defaultbg",		258,	0xffffff},
	{"bolddefaultbg",	259,	0xbbbbbb},
	{"cursortext",		260,	0xffffff},
	{"cursorcolor",		261,	0x0080f0},

	/* ANSI colors */
	{"black",		0,	0x000000},
	{"boldblack",		8,	0x000000 | COLOR_FLAG_BOLD},
	{"red",			1,	0xbb0000},
	{"boldred",		9,	0xff3030},
	{"green",		2,	0x00884c},
	{"boldgreen",		10,	0x437d25},
	{"yellow",		3,	0xb07800},
	{"boldyellow",		11,	0xe0e040},
	{"blue",		4,	0x0033cc},
	{"boldblue",		12,	0x5555ff},
	{"magenta",		5,	0xbb00bb},
	{"boldmagenta",		13,	0xb64f90},
	{"cyan",		6,	0x0090a0},
	{"boldcyan",		14,	0x00b7cc},
	{"white",		7,	0x6b6b6b},
	{"boldwhite",		15,	0xffffff},

	/* JOE colors */
	/* These are set to undefined to *reserve* their places in the palette.
	   If the scheme does not specify them, JOE won't find them and will default
	   to using reverse video.  If they're present, they'll get used as expected. */
	{"selectionfg",		SELECTION_FG_CODE,	COLOR_FLAG_UNDEFINED}, /* 16 */
	{"selectionbg",		SELECTION_BG_CODE,	COLOR_FLAG_UNDEFINED}, /* 17 */
	{"statusfg",		STATUS_FG_CODE,		COLOR_FLAG_UNDEFINED}, /* 18 */
	{"statusbg",		STATUS_BG_CODE,		COLOR_FLAG_UNDEFINED}, /* 19 */

	/* Last entry before terminator's color index *must* be the starting index
	   for custom defined colors minus one (so here, the first custom color will
	   have index 20) */

	/* Terminator */
	{NULL,			-1,	0}
};


/******************************************************** Local types */

#define COLOR_NAME_MAX		40

/* List of palette index -> color code mappings */
struct tmp_jwcolorlist
{
	struct tmp_jwcolorlist	*next;
	int			colorcode;
	int			paletteidx;
};

/* List of variable name -> color code mappings */
struct tmp_jwcolorvar
{
	struct tmp_jwcolorvar	*next;
	char			name[COLOR_NAME_MAX];
	int			color;
	struct tmp_jwcolorlist	*assignment;		/* non-null if this has been used, and has a palette index */
	int			special;
};

/* Either a variable reference or a color code */
struct tmp_jwcolorspec
{
	int			colorcode;
	char			reference[COLOR_NAME_MAX];
	int			attrs;
};

/* Syntax color definition */
struct tmp_jwsyntaxcolor
{
	struct tmp_jwsyntaxcolor	*next;
	struct jwsyntaxcolor		color;
	struct tmp_jwcolorspec		fg;
	struct tmp_jwcolorspec		bg;
};

#define SECTION_NONE		0
#define SECTION_SCHEME		1
#define SECTION_COLORS		2
#define SECTION_SYNTAX		3


/******************************************************** Forward decls */

static void loadschemesindir(wchar_t *, struct jwcolorlist **);
static void loadbuiltinschemes(struct jwcolorlist **list);
static void loadschemeinfo(wchar_t *, struct jwcolorlist **);
static struct jwcolorlist *loaddefaultcolors();

static struct jwcolors *doloadcolors(wchar_t *file);
static struct tmp_jwcolorvar *addcolorvar(char *line, struct tmp_jwcolorvar* head);
static struct tmp_jwsyntaxcolor *addsyntaxcolor(char *line, struct tmp_jwsyntaxcolor* head);
static void mapstandardcolors(struct tmp_jwcolorvar *vars, struct tmp_jwcolorlist **tail);
static void mapsyntaxcolors(struct tmp_jwsyntaxcolor **slist, struct tmp_jwcolorvar *vars, struct tmp_jwcolorlist *clist);
static int assignsyntaxcolor(int ccode, char *varname, struct tmp_jwcolorvar *vars, struct tmp_jwcolorlist *clist);
static struct jwcolors *organizecolors(struct tmp_jwsyntaxcolor *slist, struct tmp_jwcolorlist* clist);
static struct tmp_jwcolorlist *getorcreatecolor(struct tmp_jwcolorlist *clist, int colorcode);
static struct tmp_jwcolorvar *lookupcolorvar(struct tmp_jwcolorvar *vlist, char *name);
static void stripws(char *line);
static int detectsection(char *line, int *section);
static int readcolorspec(char *line, struct tmp_jwcolorspec *fgspec, struct tmp_jwcolorspec *bgspec);
static int readcolorval(char *token, struct tmp_jwcolorspec *color);


/******************************************************** Scheme list/info loading */

/* Load all color scheme info from user & install directories */
struct jwcolorlist *loadcolorschemes()
{
	struct jwcolorlist* ret = NULL, *p;
	wchar_t path[MAX_PATH + 40];
	int i;

	if (utf8towcs(path, jw_home, MAX_PATH))
	{
		assert(FALSE);
		return NULL;
	}

	wcscat(path, L"\\schemes\\");
	loadschemesindir(path, &ret);

	if (utf8towcs(path, jw_joedata, MAX_PATH))
	{
		assert(FALSE);
		return NULL;
	}

	wcscat(path, L"\\schemes\\");
	loadschemesindir(path, &ret);

	loadbuiltinschemes(&ret);

	/* Stick the default scheme at the head of the list */
	p = loaddefaultcolors();
	p->next = ret;
	ret = p;

	/* Number the schemes */
	for (i = 0, p = ret; p; p = p->next, i++)
	{
		p->idx = i;
	}

	return ret;
}

/* Load a scheme found in a specific file */
struct jwcolorlist *loadexternalscheme(wchar_t *file)
{
	struct jwcolorlist *result;

	result = (struct jwcolorlist*)malloc(sizeof(struct jwcolorlist));
	wcscpy(result->file, file);
	strcpy(result->name, "");
	result->next = NULL;
	result->colors = doloadcolors(file);
	result->idx = -999;

	return result;
}

/* Load all schemes from a single directory */
static void loadschemesindir(wchar_t *path, struct jwcolorlist **list)
{
	HANDLE search;
	WIN32_FIND_DATAW finddata;
	wchar_t searchpath[MAX_PATH + 40];

	wcscpy(searchpath, path);
	wcscat(searchpath, L"*.joecolor");

	search = FindFirstFileW(searchpath, &finddata);
	if (search != INVALID_HANDLE_VALUE)
	{
		do
		{
			wchar_t filepath[MAX_PATH + 40];

			wcscpy(filepath, path);
			wcscat(filepath, finddata.cFileName);
			fixpath(filepath, MAX_PATH + 40);

			loadschemeinfo(filepath, list);
		} while (FindNextFileW(search, &finddata));

		FindClose(search);
	}
}

/* Load embedded schemes */
static void loadbuiltinschemes(struct jwcolorlist **list)
{
	const wchar_t *ptr = NULL;
	wchar_t tmp[80];

	while ((ptr = jwnextbuiltin(ptr, L".joecolor"))) {
		wcscpy(tmp, L"*");
		wcscat(tmp, ptr);
		loadschemeinfo(tmp, list);
	}
}

/* Create and insert a jwcolorlist cell with a name and filepath given the file */
void loadschemeinfo(wchar_t *file, struct jwcolorlist **list)
{
	struct jwcolorlist *newentry;
	int cursection;
	char buffer[160];
	JFILE *fp;

	newentry = (struct jwcolorlist *)malloc(sizeof(struct jwcolorlist));
	strcpy(newentry->name, "(Unnamed scheme)");

	fp = jwfopen(file, L"r");

	while (jwfgets((unsigned char *)buffer, 160, fp)) {
		stripws(buffer);

		if (!buffer[0]) { /* Empty line */ }
		else if (detectsection(buffer, &cursection)) { }
		else if (cursection == SECTION_SCHEME) {
			char *p = strchr(buffer, '=');

			if (p) {
				*p++ = 0;

				stripws(buffer);
				stripws(p);

				if (!stricmp("name", buffer)) {
					/* Found the name */
					strncpy(newentry->name, p, JWSCHEMENAME_MAX);
					break;
				}
			}
		}
	}

	jwfclose(fp);

	wcscpy(newentry->file, file);

	/* Insert to list (sorted order by name) */
	for (; *list; list = &(*list)->next)
	{
		int c = stricmp(newentry->name, (*list)->name);
		if (c < 0)
		{
			/* Found insertion point */
			break;
		}
		else if (c == 0)
		{
			/* Duplicate?  Forget it. */
			free(newentry);
			return;
		}
	}

	newentry->colors = NULL;
	newentry->next = *list;
	*list = newentry;
}

/* Load the builtin default color scheme */
static struct jwcolorlist* loaddefaultcolors()
{
	struct tmp_jwcolorlist *clist = NULL;
	struct jwcolors *scheme = NULL;
	struct jwcolorlist *ret = NULL;

	mapstandardcolors(NULL, &clist);
	scheme = organizecolors(NULL, clist);

	while (clist)
	{
		struct tmp_jwcolorlist *next = clist->next;
		free(clist);
		clist = next;
	}

	ret = (struct jwcolorlist*)malloc(sizeof(struct jwcolorlist));
	ret->colors = scheme;
	ret->idx = 0;
	strcpy(ret->name, "(Default)");
	ret->next = NULL;
	wcscpy(ret->file, L"");

	return ret;
}

/******************************************************** Actual scheme loading */

/* Load color scheme if it hasn't been loaded already */
struct jwcolors *loadcolorscheme(struct jwcolorlist *entry)
{
	if (entry->colors == NULL)
	{
		entry->colors = doloadcolors(entry->file);
	}
	
	return entry->colors;
}

/* Worker function to actually load the scheme from a file */
static struct jwcolors *doloadcolors(wchar_t *file)
{
	JFILE *fp;
	char buffer[160];
	int cursection = SECTION_NONE;
	struct tmp_jwcolorlist *clist = NULL;
	struct tmp_jwcolorvar *vlist = NULL;
	struct tmp_jwsyntaxcolor *slist = NULL;
	struct jwcolors *ret = NULL;
	int errors = 0;
	
	fp = jwfopen(file, L"r");

	if (!fp)
	{
		return NULL;
	}

	while (jwfgets((unsigned char *)buffer, 160, fp))
	{
		/* Strip comments, leading and trailing whitespace */
		stripws(buffer);
		
		/* Handle empty lines */
		if (!buffer[0])
		{
		}
		/* Handle section changes */
		else if (detectsection(buffer, &cursection))
		{
		}
		/* Handle key/value pairs */
		else if (cursection == SECTION_COLORS)
		{
			struct tmp_jwcolorvar* newhead;
			newhead = addcolorvar(buffer, vlist);
			if (!newhead)
			{
				errors = 1;
				break;
			}
			vlist = newhead;
		}
		else if (cursection == SECTION_SYNTAX)
		{
			struct tmp_jwsyntaxcolor* newhead;
			newhead = addsyntaxcolor(buffer, slist);
			if (!newhead)
			{
				errors = 1;
				break;
			}
			slist = newhead;
		}
	}

	jwfclose(fp);
	
	if (!errors)
	{
		/* Resolve all colors */
		mapstandardcolors(vlist, &clist);
		mapsyntaxcolors(&slist, vlist, clist);
	
		/* Build result */
		ret = organizecolors(slist, clist);
	}

	/* Free temporary data */
	while (clist)
	{
		struct tmp_jwcolorlist *next = clist->next;
		free(clist);
		clist = next;
	}
	
	while (vlist)
	{
		struct tmp_jwcolorvar *next = vlist->next;
		free(vlist);
		vlist = next;
	}
	
	while (slist)
	{
		struct tmp_jwsyntaxcolor *next = slist->next;
		free(slist);
		slist = next;
	}
	
	return ret;
}

/* Read a line from the "colors" section and create a new tmp_jwcolorvar */
static struct tmp_jwcolorvar* addcolorvar(char *line, struct tmp_jwcolorvar* head)
{
	struct tmp_jwcolorspec fgspec, bgspec;
	struct tmp_jwcolorvar *ret;
	char *eq;
	
	eq = strchr(line, '=');
	if (!eq)
	{
		return NULL;
	}
	
	*eq = 0;
	eq++;

	stripws(eq);
	if (!readcolorspec(eq, &fgspec, &bgspec))
	{
		return NULL;
	}

	if (fgspec.colorcode & COLOR_FLAG_UNDEFINED)
	{
		/* Wasn't specified or pointed to a variable. */

		/* What if it pointed to a variable? We can handle that. */
		if (*fgspec.reference) 
		{
			struct tmp_jwcolorvar *other = lookupcolorvar(head, fgspec.reference);
			if (other) {
				fgspec.colorcode = other->color;
			} else {
				/* Can't find it */
				return NULL;
			}
		} else {
			/* No, it just really wasn't specified. */
			return NULL;
		}
	}

	/* Everything is OK, go ahead and create the variable */
	ret = (struct tmp_jwcolorvar*)malloc(sizeof(struct tmp_jwcolorvar));

	/* Load variable name */
	stripws(line);
        strncpy(ret->name, line, COLOR_NAME_MAX - 1);
        ret->name[COLOR_NAME_MAX - 1] = 0;

	ret->assignment = NULL;
	ret->next = head;
	ret->color = fgspec.colorcode;
	ret->special = 0; /* we don't know yet... */
        
	return ret;
}

/* Read a line from the "syntax" section and create a new tmp_jwsyntaxcolor */
static struct tmp_jwsyntaxcolor* addsyntaxcolor(char *line, struct tmp_jwsyntaxcolor* head)
{
	struct tmp_jwsyntaxcolor *ret;
	struct tmp_jwcolorspec fgspec, bgspec;
	char *eq, *dot;
	
	eq = strchr(line, '=');
	if (!eq)
	{
		return NULL;
	}
	
	*eq = 0;
	eq++;
	
	/* Parse color */
	stripws(eq);
	if (!readcolorspec(eq, &fgspec, &bgspec))
	{
		return NULL;
	}

	/* Load in data */
	ret = (struct tmp_jwsyntaxcolor *)malloc(sizeof(struct tmp_jwsyntaxcolor));
	memcpy(&ret->bg, &bgspec, sizeof(struct tmp_jwcolorspec));
	memcpy(&ret->fg, &fgspec, sizeof(struct tmp_jwcolorspec));
	ret->color.attrs = fgspec.attrs;

	stripws(line);
	dot = strchr(line, '.');
	if (dot)
	{
		*dot = 0;
		line[JWSYNTAXFILE_MAX - 1] = 0;
		strcpy(ret->color.syntaxfile, line);
		
		dot[JWTOKENNAME_MAX] = 0;
		strcpy(ret->color.token, dot + 1);
	}
	else
	{
		strcpy(ret->color.syntaxfile, "");
		line[JWTOKENNAME_MAX - 1] = 0;
		strcpy(ret->color.token, line);
	}
	
	ret->next = head;
	return ret;
}

/* Create initial color mapping list, tmp_jwcolorlist, based on default colors and variables from the file */
static void mapstandardcolors(struct tmp_jwcolorvar *vars, struct tmp_jwcolorlist **tail)
{
	int i;
	
	/* Map default values */
	
	for (i = 0; colordefaults[i].name; i++)
	{
		struct tmp_jwcolorvar *var;
		struct tmp_jwcolorlist *newcolor;
		
		newcolor = (struct tmp_jwcolorlist*)malloc(sizeof(struct tmp_jwcolorlist));
		newcolor->paletteidx = colordefaults[i].paletteidx;
		newcolor->next = NULL;

		var = lookupcolorvar(vars, colordefaults[i].name);
		if (var)
		{
			newcolor->colorcode = var->color;
			var->assignment = newcolor;

			if (colordefaults[i].paletteidx > 255)
			{
				/* This is a special color: outside of JOE's ability to use it, at *this* palette index.
				   (it's local to PuTTY's palette.) */
				var->special = 1;
			}
		}
		else
		{
			newcolor->colorcode = colordefaults[i].defvalue;
		}
		
		*tail = newcolor;
		tail = &newcolor->next;
	}
}

/* Resolve all entries from tmp_jwsyntaxcolor and assign palette indices */
static void mapsyntaxcolors(struct tmp_jwsyntaxcolor **slist, struct tmp_jwcolorvar *vars, struct tmp_jwcolorlist *clist)
{
	struct tmp_jwsyntaxcolor *s;
	struct tmp_jwsyntaxcolor *next;
	struct tmp_jwsyntaxcolor **tail = slist;

	for (s = *slist; s; s = next)
	{
		next = s->next;

		s->color.fgcoloridx = assignsyntaxcolor(s->fg.colorcode, s->fg.reference, vars, clist);
		s->color.bgcoloridx = assignsyntaxcolor(s->bg.colorcode, s->bg.reference, vars, clist);
		s->color.attrs = s->fg.attrs;

		if (s->color.fgcoloridx == -1 && s->color.bgcoloridx == -1 && s->color.attrs == 0)
		{
			/* Axe it */
			*tail = next;
			free(s);
		}
		else
		{
			tail = &s->next;
		}
	}
}

/* Assigns a single tmp_jwcolorspec: looks up any variable and assigns to a palette index */
static int assignsyntaxcolor(int ccode, char *varname, struct tmp_jwcolorvar *vars, struct tmp_jwcolorlist *clist)
{
	if (!*varname)
	{
		if (!(ccode & COLOR_FLAG_UNDEFINED))
		{
			return getorcreatecolor(clist, ccode)->paletteidx;
		}
		else
		{
			return -1;
		}
	}
	else
	{
                struct tmp_jwcolorvar *var = lookupcolorvar(vars, varname);
                if (var)
                {
			if (!var->assignment || var->special)
			{
				var->assignment = getorcreatecolor(clist, var->color);
			}
			return var->assignment->paletteidx;
                }
                else
                {
                        return -1;
                }
	}
}

/* Find an existing palette index for the specified color code or creates one, and returns the tmp_jwcolorlist */
static struct tmp_jwcolorlist *getorcreatecolor(struct tmp_jwcolorlist *clist, int colorcode)
{
	struct tmp_jwcolorlist *p = clist;
	struct tmp_jwcolorlist **tail = NULL;
	int idx = 0;
	
	for (; p; p = p->next)
	{
		if (p->colorcode == colorcode && p->paletteidx <= 255)
		{
			return p;
		}
		
		idx = p->paletteidx;
		tail = &p->next;
	}
	
	assert(tail != NULL);
	
	p = (struct tmp_jwcolorlist*)malloc(sizeof(struct tmp_jwcolorlist));
	p->colorcode = colorcode;
	p->next = NULL;
	p->paletteidx = idx + 1;
	*tail = p;
	
	return p;
}

static int __cdecl syntaxcmp(const void *x, const void *y)
{
	struct jwsyntaxcolor *s1 = (struct jwsyntaxcolor *)x;
	struct jwsyntaxcolor *s2 = (struct jwsyntaxcolor *)y;
	
	int c = stricmp(s1->syntaxfile, s2->syntaxfile);
	return (c == 0) ? stricmp(s1->token, s2->token) : c;
}

/* Creates jwcolors structure given tmp_jwsyntaxcolor and tmp_jwcolorlist lists */
static struct jwcolors *organizecolors(struct tmp_jwsyntaxcolor *slist, struct tmp_jwcolorlist* clist)
{
	int ccount = 0, scount = 0, i;
	struct tmp_jwsyntaxcolor *s;
	struct tmp_jwcolorlist *c;
	
	struct jwcolors *ret;
	
	for (s = slist; s; s = s->next)
	{
		scount++;
	}
	
	for (c = clist; c; c = c->next)
	{
		/* Do not include undefined colors in the output */
		if (!(c->colorcode & COLOR_FLAG_UNDEFINED))
			ccount++;
	}
	
	ret = (struct jwcolors*)malloc(sizeof(struct jwcolors));
	ret->ncolors = ccount;
	ret->nsyntaxcolors = scount;

	ret->colors = (struct jwcolorentry*)malloc(sizeof(struct jwcolorentry) * ccount);
	for (i = 0, c = clist; c; i++, c = c->next)
	{
		if (c->colorcode & COLOR_FLAG_UNDEFINED)
		{
			/* Do not include undefined colors in the output */
			i--;
		}
		else
		{
			ret->colors[i].color = c->colorcode;
			ret->colors[i].paletteidx = c->paletteidx;
		}
	}
	
	if (scount > 0)
	{
		ret->syntaxcolors = (struct jwsyntaxcolor*)malloc(sizeof(struct jwsyntaxcolor) * scount);
		for (i = 0, s = slist; s; i++, s = s->next)
		{
			memcpy(&ret->syntaxcolors[i], &s->color, sizeof(struct jwsyntaxcolor));
		}

		qsort(ret->syntaxcolors, scount, sizeof(struct jwsyntaxcolor), syntaxcmp);
	}
	else
	{
		ret->syntaxcolors = NULL;
	}
	
	return ret;
}

/* Resolves a tmp_jwcolorvar by name */
static struct tmp_jwcolorvar *lookupcolorvar(struct tmp_jwcolorvar *vlist, char *name)
{
	for (; vlist; vlist = vlist->next)
	{
		if (!stricmp(vlist->name, name))
		{
			return vlist;
		}
	}
	
	return NULL;
}


/******************************************************** Scheme file parsing */

#define ISWS(c) ((c)==' '||(c)=='\t'||(c)=='\r'||(c)=='\n'||(c)=='\v'||(c)=='\f')

static void stripws(char* line)
{
	char *w, *r;
	
	r = w = line;

	/* Skip leading whitespace */
	while (*r && ISWS(*r)) r++;

	/* Read the line */
	if (r != w)
	{
		while (*r && *r != ';') *w++ = *r++;
	}
	else
	{
		while (*r && *r != ';') r++;
		w = r;
	}

	/* Walk back trailing whitespace */
	for (w--; w >= line && ISWS(*w); w--) {}
	*++w = 0;
}

static int detectsection(char *line, int *section)
{
	if (line[0] != '[')
	{
		return 0;
	}
	
	if (!stricmp(line, "[scheme]"))
	{
		*section = SECTION_SCHEME;
		return 1;
	}
	else if (!stricmp(line, "[colors]"))
	{
		*section = SECTION_COLORS;
		return 1;
	}
	else if (!stricmp(line, "[syntax]"))
	{
		*section = SECTION_SYNTAX;
		return 1;
	}
	
	return 0;
}

static int readcolorspec(char *line, struct tmp_jwcolorspec *fgspec, struct tmp_jwcolorspec *bgspec)
{
	char *tokctx;
	char *tok, *slash;
	int skipfirst = 1;

	if (!*line)
	{
		/* This case is reported as an error below, and really shouldn't be. */
		readcolorval("", fgspec);
		readcolorval("", bgspec);
		return 1;
	}

	tok = strtok_s(line, " \t", &tokctx);

	/* First token should be color */
	slash = strchr(tok, '/');
	if (!slash)
	{
		/* Don't ignore. If no color specified, then use '/'. */
		if (!readcolorval(tok, fgspec))
		{
			return 0;
		}

		/* Just initialize */
		readcolorval("", bgspec);
	}
	else
	{
		*slash = 0;
		slash++;

		/* Can be empty. */
		if (!readcolorval(tok, fgspec) && *tok)
		{
			return 0;
		}

		/* Can be an attribute instead.  We need to keep it in that case. */
		skipfirst = readcolorval(slash, bgspec) || !*tok;
		tok = slash;
	}

	/* We may want to read the right slide of the '/' in some cases, so next loop is uninitialized, and we do it here. */
	if (skipfirst) {
		tok = strtok_s(NULL, " \t", &tokctx);
	}

	/* The rest of the tokens should be attributes */
	for (; tok; tok = strtok_s(NULL, " \t", &tokctx))
	{
		if (!stricmp(tok, "bold"))
		{
			fgspec->colorcode |= COLOR_FLAG_BOLD;
		}
		else if (!stricmp(tok, "italic"))
		{
			fgspec->colorcode |= COLOR_FLAG_ITALIC;
		}
		else if (!stricmp(tok, "underline") || !stricmp(tok, "underlined"))
		{
			fgspec->attrs |= SYNTAX_FLAG_UNDERLINE;
		}
		else if (!stricmp(tok, "inverse"))
		{
			fgspec->attrs |= SYNTAX_FLAG_INVERSE;
		}
		else
		{
			return 0;
		}
	}

	return 1;
}

static int readcolorval(char *token, struct tmp_jwcolorspec *color)
{
	/* Initialize */
	color->colorcode = COLOR_FLAG_UNDEFINED;
	color->attrs = 0;
	strcpy(color->reference, "");

	if (!*token)
	{
		return 0;
	}
	else if (*token == '$')
	{
		/* Reference */
		strncpy(color->reference, &token[1], COLOR_NAME_MAX - 1);
		color->reference[COLOR_NAME_MAX - 1] = 0;
		return 1;
	}
	else if (*token == '#')
	{
		/* Read hex value */
		int code = 0;
		int i = 0;

		for (i = 0; i < 6 && token[1 + i]; i++)
		{
			char c = token[1 + i];

			code <<= 4;
			if (c >= '0' && c <= '9')
			{
				code |= (int)(c - '0');
			}
			else if (c >= 'a' && c <= 'f')
			{
				code |= (int)(c - 'a') + 10;
			}
			else if (c >= 'A' && c <= 'F')
			{
				code |= (int)(c - 'A') + 10;
			}
			else
			{
				return 0;
			}
		}

		color->colorcode = code;
		return 1;
	}

	return 0;
}


/******************************************************** Search/access functions */

struct jwsyntaxcolor* searchsyntaxcolor(struct jwcolors *colors, const char *syntax, const char *token)
{
	int left = 0;
	int right = colors->nsyntaxcolors - 1;

	while (left <= right)
	{
		int mid = (left + right) / 2;
		struct jwsyntaxcolor *midv = &colors->syntaxcolors[mid];

		int c = stricmp(midv->syntaxfile, syntax);
		c = (c == 0) ? stricmp(midv->token, token) : c;

		if (c < 0)
		{
			left = mid + 1;
		}
		else if (c > 0)
		{
			right = mid - 1;
		}
		else
		{
			/* Found! */
			return midv;
		}
	}

	return NULL;
}

int iscolordefined(struct jwcolors *colors, int color)
{
	int i;

	for (i = 0; i < colors->ncolors; i++)
	{
		if (colors->colors[i].paletteidx == color) return 1;
	}

	return 0;
}


/******************************************************** Object management */

struct jwcolors* dupcolorscheme(struct jwcolors *colors)
{
	struct jwcolors *newcolors;
	int sz;

	newcolors = (struct jwcolors*)malloc(sizeof(struct jwcolors));
	newcolors->ncolors = colors->ncolors;
	newcolors->nsyntaxcolors = colors->nsyntaxcolors;

	sz = sizeof(struct jwcolorentry) * colors->ncolors;
	newcolors->colors = (struct jwcolorentry*)malloc(sz);
	memcpy(newcolors->colors, colors->colors, sz);

	if (colors->nsyntaxcolors > 0)
	{
		sz = sizeof(struct jwsyntaxcolor) * colors->nsyntaxcolors;
		newcolors->syntaxcolors = (struct jwsyntaxcolor*)malloc(sz);
		memcpy(newcolors->syntaxcolors, colors->syntaxcolors, sz);
	}
	else
	{
		newcolors->syntaxcolors = NULL;
	}

	return newcolors;
}

void freecolorscheme(struct jwcolors *colors)
{
	free(colors->colors);
	if (colors->syntaxcolors)
	{
		free(colors->syntaxcolors);
	}
	free(colors);
}

void freecolorlist(struct jwcolorlist *colorlist)
{
	while (colorlist)
	{
		struct jwcolorlist *next = colorlist->next;
		if (colorlist->colors)
		{
			freecolorscheme(colorlist->colors);
		}
		free(colorlist);
		colorlist = next;
	}
}
