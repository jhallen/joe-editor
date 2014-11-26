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

/*
** Replacement for i18n.c in the JOE source.  Windows knows the character classes
** and purportedly these functions run quicker than a manual less-than/greater-than
** approach.  More importantly, we don't need to store tables for everything.
*/

#include <wchar.h>
#include "types.h"

int joe_iswupper(struct charmap *foo, int c)
{
	return iswupper((wint_t)c);
}

int joe_iswlower(struct charmap *foo, int c)
{
	return iswlower((wint_t)c);
}

int joe_iswalpha(struct charmap *foo, int c)
{
	return iswalpha((wint_t)c);
}

int joe_iswalpha_(struct charmap *foo, int c)
{
	return (c == 0x5F) || iswalpha((wint_t)c);
}

int joe_iswalnum_(struct charmap *foo, int c)
{
	return (c == 0x5F) || iswalnum((wint_t)c);
}

int joe_iswdigit(struct charmap *foo, int c)
{
	return iswdigit((wint_t)c);
}

int joe_iswspace(struct charmap *foo, int c)
{
	return iswspace((wint_t)c);
}

int joe_iswctrl(struct charmap *foo, int c)
{
	return iswcntrl((wint_t)c);
}

int joe_iswpunct(struct charmap *foo, int c)
{
	return iswpunct((wint_t)c);
}

int joe_iswgraph(struct charmap *foo, int c)
{
	return iswgraph((wint_t)c);
}

int joe_iswprint(struct charmap *foo, int c)
{
	return iswprint((wint_t)c);
}

int joe_iswxdigit(struct charmap *foo, int c)
{
	return iswxdigit((wint_t)c);
}

int joe_iswblank(struct charmap *foo, int c)
{
	// Don't know what this maps to in Windows, so just use the
	// intervals from i18n.c

	return c >= 0x2000
		  ? ((c <= 0x200B && c != 0x2007) || c == 0x205F || c == 0x3000)
		  : (c == 0x0009 || c == 0x0020 || c == 0x1680);
}

int joe_towupper(struct charmap *foo, int c)
{
	return towupper((wint_t)c);
}

int joe_towlower(struct charmap *foo, int c)
{
	return towlower((wint_t)c);
}

int unictrl(int ucs)
{
	/* Control characters are one column wide in JOE */
	if (ucs < 32 || ucs == 0x7F)
		return 1;

	if (ucs >= 0x80 && ucs <= 0x9F)
		return 4;

	/* More control characters... */
	if (ucs>=0x200b && ucs<=0x206f) {
		if (ucs<=0x200f) return 6;
		if (ucs>=0x2028 && ucs<=0x202E) return 6;
		if (ucs>=0x2060 && ucs<=0x2063) return 6;
		if (ucs>=0x206a) return 6;
	}

	/* More control characters... */
	if (ucs>=0xFDD0 && ucs<=0xFDEF)
		return 6;

	if (ucs==0xFEFF)
		return 6;

	if (ucs>=0xFFF9 && ucs<=0xFFFB)
		return 6;

	if (ucs>=0xFFFE && ucs<=0xFFFF)
		return 6;

	return 0;
}

int joe_wcwidth(int wide, int ucs)
{
	/* Use common wcwidth */
	int wcwidth(int);
	int control;

	if (!locale_map->type || !wide)
	{
		return 1;
	}

	control = unictrl(ucs);
	return control ? control : wcwidth(ucs);
}
