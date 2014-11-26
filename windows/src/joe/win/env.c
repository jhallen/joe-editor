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
	static char lang[LOCALE_NAME_MAX_LENGTH] = "";
	static char temp[PATH_MAX] = "";

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
		if (!*lang)
		{
			int success = 0;
			
			if (GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SISO639LANGNAME, lang, LOCALE_NAME_MAX_LENGTH))
			{
				int len = strlen(lang);

				lang[len++] = '_';
				if (GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SISO3166CTRYNAME, &lang[len], LOCALE_NAME_MAX_LENGTH - len))
				{
					success = 1;
					strcat(lang, ".UTF-8");
				}
			}

			if (!success)
			{
				/* On error, default to English :-( */
				assert(FALSE);
				strcpy(lang, "en_US.UTF-8");
			}
		}

		return lang;
	}
	else if (!strcmp(env, "TEMP"))
	{
		if (!*temp)
		{
			wchar_t wtemp[MAX_PATH];
			int sz;

			sz = GetTempPathW(MAX_PATH, wtemp);
			if (wcstoutf8(temp, wtemp, PATH_MAX))
			{
				assert(0);
				return NULL;
			}
		}

		return temp;
	}

	return NULL;
}
