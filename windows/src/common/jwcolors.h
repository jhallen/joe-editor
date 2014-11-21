#ifndef _JOEWIN_COLORS_H
#define _JOEWIN_COLORS_H

#include "jwglobals.h"

#define COLOR_FLAG_BOLD		0x01000000
#define COLOR_FLAG_ITALIC	0x02000000
#define COLOR_FLAG_UNDEFINED	0x04000000

#define SELECTION_FG_CODE	16
#define SELECTION_BG_CODE	17
#define STATUS_FG_CODE		18
#define STATUS_BG_CODE		19

#define SYNTAX_FLAG_UNDERLINE	0x01
#define SYNTAX_FLAG_INVERSE	0x02

#define JWSYNTAXFILE_MAX	16
#define JWTOKENNAME_MAX		32
#define JWSCHEMENAME_MAX	40

#define JWC_RED(x)		(((x) & 0xff0000) >> 16)
#define JWC_GREEN(x)		(((x) & 0x00ff00) >> 8)
#define JWC_BLUE(x)		 ((x) & 0x0000ff)

struct jwsyntaxcolor
{
	char			syntaxfile[JWSYNTAXFILE_MAX];
	char			token[JWTOKENNAME_MAX];
	int			fgcoloridx;
	int			bgcoloridx;
	int			attrs;
};

struct jwcolorentry
{
	int			paletteidx;
	int			color;
};

struct jwcolors
{
	int			ncolors;
	int			nsyntaxcolors;
	struct jwcolorentry	*colors;
	struct jwsyntaxcolor	*syntaxcolors;
};

struct jwcolorlist
{
	struct jwcolorlist	*next;
	char			name[JWSCHEMENAME_MAX];
	wchar_t			file[MAX_PATH];
	int			idx;
	struct jwcolors		*colors;
};

struct jwcolorlist *loadcolorschemes();
struct jwcolors *loadcolorscheme(struct jwcolorlist *entry);
struct jwcolorlist *loadexternalscheme(wchar_t *file);
int iscolordefined(struct jwcolors *colors, int color);
struct jwsyntaxcolor *searchsyntaxcolor(struct jwcolors *colors, const char *syntax, const char *token);
struct jwcolors *dupcolorscheme(struct jwcolors *colors);
void freecolorscheme(struct jwcolors *colors);
void freecolorlist(struct jwcolorlist *colorlist);

JOEWIN_GLOBAL struct jwcolors *jw_initialcolorscheme;

#endif
