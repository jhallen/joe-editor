#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "jwwin.h"

#include "jwglue.h"
#include "jwglobals.h"

char* glue_getenv(const char* env)
{
	static char lang[LOCALE_NAME_MAX_LENGTH] = "";

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

	return NULL;
}
