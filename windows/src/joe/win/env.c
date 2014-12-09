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

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "jwwin.h"

#include "jwglue.h"
#include "jwglobals.h"
#include "jwutils.h"

char* glue_getenv(const char* env)
{
	static char *lang = NULL, *temp = NULL;

	if (!strcmp(env, "HOME"))
	{
		return (char*)jw_home;
	}
	else if (!strcmp(env, "TERM"))
	{
		return "putty";
	}
	else if (!strcmp(env, "LANG"))
	{
		if (!lang)
		{
			char langtmp[LOCALE_NAME_MAX_LENGTH];
			
			if (GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SISO639LANGNAME, langtmp, LOCALE_NAME_MAX_LENGTH))
			{
				int len = strlen(langtmp);

				langtmp[len++] = '_';
				if (GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SISO3166CTRYNAME, &langtmp[len], LOCALE_NAME_MAX_LENGTH - len))
				{
					strcat(langtmp, ".UTF-8");
					lang = strdup(langtmp);
				}
			}

			if (!lang)
			{
				/* On error, default to English :-( */
				assert(FALSE);
				lang = "en_US.UTF-8";
			}
		}

		return lang;
	}
	else if (!strcmp(env, "TEMP"))
	{
		if (!temp)
		{
			wchar_t wtemp[MAX_PATH];
			int sz;

			GetTempPathW(MAX_PATH, wtemp);
			sz = wcstoutf8len(wtemp) + 1;
			temp = (char *)malloc(sz);

			if (wcstoutf8(temp, wtemp, sz))
			{
				assert(0);
				return NULL;
			}
		}

		return temp;
	}

	return NULL;
}
