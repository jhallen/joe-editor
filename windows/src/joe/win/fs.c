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

#include <direct.h>
#include "types.h"
#include <assert.h>

void *opendir(unsigned char *path)
{
	static struct dirent the_struct;
	wchar_t c;

	if (utf8towcs(the_struct._startname, path, MAX_PATH))
	{
		assert(FALSE);
		return NULL;
	}

	c = the_struct._startname[wcslen(the_struct._startname) - 1];
	if (c != L'/' && c != L'\\')
	{
		wcscat(the_struct._startname, L"\\");
	}

	wcscat(the_struct._startname, L"*.*");

	the_struct.find_handle = INVALID_HANDLE_VALUE;
	return &the_struct;
}

void closedir(void* handle)
{
	struct dirent* de = (struct dirent*)handle;
	if (de)
	{
		if (de->find_handle != INVALID_HANDLE_VALUE)
		{
			FindClose(de->find_handle);
			de->find_handle = INVALID_HANDLE_VALUE;
			strcpy(de->d_name, "");
			wcscpy(de->_startname, L"");
		}
	}
}

struct dirent *readdir(void* handle)
{
	struct dirent* de = (struct dirent*)handle;
	if (de)
	{
		WIN32_FIND_DATAW find_data;

		if (de->find_handle == INVALID_HANDLE_VALUE)
		{
			de->find_handle = FindFirstFileW(de->_startname, &find_data);
			if (de->find_handle == INVALID_HANDLE_VALUE)
			{
				return NULL;
			}
		}
		else
		{
			if (!FindNextFileW(de->find_handle, &find_data))
			{
				return NULL;
			}
		}

		if (wcstoutf8(de->d_name, find_data.cFileName, PATH_MAX))
		{
			assert(FALSE);
			return NULL;
		}

		return de;
	}

	return de;
}

int __cdecl glue_open(const char *filename, int oflag, ...)
{
	wchar_t wfilename[MAX_PATH + 1];
	int pmode;
	va_list ap;

	va_start(ap, oflag);
	pmode = va_arg(ap, int);
	va_end(ap);

	if (utf8towcs(wfilename, filename, MAX_PATH))
	{
		assert(FALSE);
		return -1;
	}

	return _wopen(wfilename, oflag, pmode);
}

int glue_stat(const char *filename, struct stat *buffer)
{
	wchar_t wfilename[MAX_PATH + 1];

	if (utf8towcs(wfilename, filename, MAX_PATH))
	{
		assert(FALSE);
		return -1;
	}

	return jwstatfunc(wfilename, buffer);
}

FILE *glue_fopen(const char *filename, const char *mode)
{
	wchar_t wfilename[MAX_PATH + 1];
	wchar_t wmode[20];

	if (utf8towcs(wfilename, filename, MAX_PATH) || utf8towcs(wmode, mode, 20))
	{
		assert(FALSE);
		return NULL;
	}

	return _wfopen(wfilename, wmode);
}

int glue_creat(const char *filename, int pmode)
{
	wchar_t wfilename[MAX_PATH + 1];

	if (utf8towcs(wfilename, filename, MAX_PATH))
	{
		assert(FALSE);
		return -1;
	}

	return _wcreat(wfilename, pmode);
}

char *glue_getcwd(char *output, size_t outsz)
{
	wchar_t wworkingdir[MAX_PATH + 1];

	if (!_wgetcwd(wworkingdir, MAX_PATH))
	{
		return NULL;
	}

	if (wcstoutf8(output, wworkingdir, outsz))
	{
		return NULL;
	}

	return output;
}

int glue_chdir(const char *path)
{
	wchar_t wpath[MAX_PATH + 1];

	if (utf8towcs(wpath, path, MAX_PATH))
	{
		assert(FALSE);
		return -1;
	}

	return _wchdir(wpath);
}

int glue_unlink(const char *path)
{
	wchar_t wpath[MAX_PATH + 1];

	if (utf8towcs(wpath, path, MAX_PATH))
	{
		assert(FALSE);
		return -1;
	}

	return _wunlink(wpath);
}

int glue_fprintf(FILE *fp, const char *fmt, ...)
{
	int result;
	va_list args;

	va_start(args, fmt);

#ifdef DEBUG
	if (fp == stderr) {
		char tmp[1024];

		result = vsnprintf(tmp, 1024, fmt, args);
		va_end(args);

		OutputDebugStringA(tmp);

		return result;
	}
#endif

	result = vfprintf(fp, fmt, args);
	va_end(args);

	return result;
}
