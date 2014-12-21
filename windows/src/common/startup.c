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
#include <shlobj.h>
#include <wchar.h>
#include <assert.h>
#include "jwglobals.h"
#include "jwutils.h"
#include "jwcomm.h"

static DWORD jw_threadid;

static void setjoedata()
{
	wchar_t tmp1[MAX_PATH], tmp2[MAX_PATH];
	wchar_t *s;
	int n;

	// Get running module filename (full path to joe.exe)
	GetModuleFileName(NULL, tmp1, MAX_PATH - 1);

	// JOEDATA = startup dir
	s = wcsrchr(tmp1, L'\\');
	if (!s)
	{
		// Must have been started up in local directory.
		wcscpy(tmp1, L".\\");
	}
	else
	{
		*s = 0;
	}

	GetFullPathNameW(tmp1, MAX_PATH, tmp2, NULL);
	n = wcstoutf8len(tmp2);
	jw_joedata = (unsigned char*)malloc(n + 2);
	wcstoutf8(jw_joedata, tmp2, n);
	strcat(jw_joedata, "\\");

	// JOERC = startup dir + \conf
	jw_joerc = (unsigned char*)malloc(strlen(jw_joedata) + 6);
	strcpy(jw_joerc, jw_joedata);
	strcat(jw_joerc, "conf\\");
}

static void setjoehome()
{
	wchar_t appdata[MAX_PATH + 50];
	int gotit = 0;

	if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, appdata)))
	{
		if (appdata[wcslen(appdata) - 1] != L'\\')
		{
			wcscat(appdata, L"\\");
		}

		wcscat(appdata, L"JoeEditor\\");

		// Does it exist?
		if (GetFileAttributesW(appdata) == INVALID_FILE_ATTRIBUTES)
		{
			// Create it.
			if (SUCCEEDED(CreateDirectoryW(appdata, NULL)))
			{
				gotit = 1;
			}
		}
		else
		{
			gotit = 1;
		}

	}

	if (gotit)
	{
		size_t len = wcstoutf8len(appdata);
		jw_home = (unsigned char*)malloc(len + 1);
		wcstoutf8(jw_home, appdata, len);
	}
	else
	{
		jw_home = NULL;
	}
}

static int setjoeargs(int argc, wchar_t **argv)
{
	wchar_t tmp[MAX_PATH];
	int i;

	jw_argc = argc;
	jw_argv = (char**)malloc(sizeof(char*) * (argc + 1));

	if (!argv)
	{
		return 1;
	}

	/* Handle first argument special: we need to fix the path and get the personality */
	{
		size_t arglen;
		wchar_t *r, *d;

		wcscpy(tmp, argv[0]);
		if (fixpath(tmp, MAX_PATH))
		{
			assert(FALSE);
			return 1;
		}

		arglen = wcstoutf8len(tmp) + 1;
		jw_argv[0] = (char*)malloc(arglen);
		if (wcstoutf8(jw_argv[0], tmp, arglen))
		{
			assert(FALSE);
			return 1;
		}

		r = wcsrchr(tmp, L'\\');
		if (!r)
		{
			r = tmp;
		}
		else
		{
			r++;
		}

		d = wcschr(r, L'.');
		if (d)
		{
			*d = 0;
		}

		jw_personality = wcsdup(r);
	}

	/* Copy the rest of the arguments */
	for (i = 1; i < argc; i++)
	{
		size_t arglen = wcstoutf8len(argv[i]) + 1;
		jw_argv[i] = (char*)malloc(arglen);
		if (wcstoutf8(jw_argv[i], argv[i], arglen))
		{
			assert(FALSE);
			return 1;
		}
	}

	jw_argv[i] = NULL;
	return 0;
}

/* If started in joe's binary directory, change to user's home directory */
static void setjoedir()
{
	wchar_t exepath[MAX_PATH], curpath[MAX_PATH];

	if (GetModuleFileName(NULL, exepath, MAX_PATH - 1) && _wgetcwd(curpath, MAX_PATH - 1)) {
		int curlen = wcslen(curpath);
		wchar_t *p;

		p = wcsrchr(exepath, L'\\');
		if (p)
			*p = 0;

		if (curpath[curlen - 1] == L'\\')
			curpath[curlen - 1] = 0;

		if (!wcsicmp(exepath, curpath)) {
			if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_PROFILE, NULL, 0, exepath))) {
				_wchdir(exepath);
			}
		}
	}
}

int jwInitJoe(int argc, wchar_t **argv)
{
	void jwAddResourceHandle(HMODULE module);

	int result = setjoeargs(argc, argv);
	if (!result)
	{
		setjoedata();
		setjoehome();
		setjoedir();

		/* Set up builtin resource loader */
		{
			HMODULE current = NULL;
			HMODULE exeModule = GetModuleHandle(NULL);
			jwAddResourceHandle(exeModule);

			if (GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCTSTR)jwInitJoe, &current) && current != exeModule) {
				jwAddResourceHandle(current);
			}
		}
	}

	return result;
}

static DWORD WINAPI joethread(LPVOID threadParam)
{
	int joe_main(int, char **, char **);
	
	static char *env[] = { NULL, NULL };

	joe_main(jw_argc, jw_argv, env);

	jwexit(0);
	return 0; /* appease compiler */
}

HANDLE jwStartJoe(unsigned long* threadId)
{
	HANDLE result = CreateThread(NULL, 0, joethread, NULL, 0, &jw_threadid);

	*threadId = jw_threadid;
	return result;
}

void jwexit(int code)
{
	DWORD tid = GetCurrentThreadId();
	assert(GetCurrentThreadId() == jw_threadid);

	// Tell the client we want to exit.
	jwSendComm1(JW_SIDE_EDITOR, COMM_EXIT, code);

	ExitThread(code);
}
