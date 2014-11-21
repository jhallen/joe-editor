
#include "jwwin.h"
#include <string.h>

int fixpath(wchar_t *path, size_t sz)
{
	wchar_t tmp[MAX_PATH];

	if (!GetFullPathNameW(path, MAX_PATH, tmp, NULL))
	{
		return 1;
	}

	if (sz <= wcslen(tmp))
	{
		return 1;
	}

	wcscpy(path, tmp);
	return 0;
}
