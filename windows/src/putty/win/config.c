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

#include <string.h>
#include <assert.h>

#include "putty.h"
#include "jwconfig.h"
#include "jwcolors.h"
#include "jwutils.h"
#include "jwglobals.h"

#define JW_ROOT_KEY L"Software\\JoeEditor\\"

static wchar_t *getkeyname()
{
	static wchar_t keyname[256];

	wcscpy(keyname, JW_ROOT_KEY);
	wcscat(keyname, jw_personality);

	return keyname;
}

void jwLoadConfig(Config *cfg)
{
	wchar_t cscheme[MAX_PATH];
	HKEY hkey;

	/* Basics -- not overridable via registry settings */

	strcpy(cfg->host, "");
	cfg->port = 666;
	cfg->close_on_exit = FORCE_ON;
	cfg->warn_on_close = 0;
	cfg->blink_cur = 1;
	cfg->beep = 1;
	cfg->fullscreenonaltenter = 1;
	strcpy(cfg->line_codepage, "utf-8");

	cfg->savelines = 0;
	cfg->scrollbar = 0;
	cfg->scrollbar_in_fullscreen = 0;

	cfg->window_border = 2;

	strcpy(cfg->wintitle, "Joe's Own Editor");
	cfg->font_quality = FQ_CLEARTYPE;

	cfg->colorlist = NULL;
	cfg->currentcolors = NULL;
	cfg->externalcolors = NULL;

	/* Defaults */

	cfg->cursor_type = 0;

	cfg->height = 45;
	cfg->width = 100;

	strcpy(cfg->font.name, "Consolas");
	cfg->font.isbold = 0;
	cfg->font.height = 10;
	wcscpy(cscheme, L"");

	/* Load from registry... */

	if (RegOpenKeyExW(HKEY_CURRENT_USER, getkeyname(), 0, KEY_ALL_ACCESS, &hkey) == ERROR_SUCCESS)
	{
		DWORD sz = MAX_PATH;
		DWORD type;
		DWORD val;

		sz = MAX_PATH;
		if (ERROR_SUCCESS != RegQueryValueExW(hkey, L"ColorScheme", NULL, &type, (LPBYTE)cscheme, &sz) && type == REG_SZ)
		{
			wcscpy(cscheme, L"");
		}

		sz = sizeof(DWORD);
		if (ERROR_SUCCESS == RegQueryValueExW(hkey, L"Rows", NULL, &type, (LPBYTE)&val, &sz) && type == REG_DWORD)
		{
			cfg->height = val;
		}

		sz = sizeof(DWORD);
		if (ERROR_SUCCESS == RegQueryValueExW(hkey, L"Columns", NULL, &type, (LPBYTE)&val, &sz) && type == REG_DWORD)
		{
			cfg->width = val;
		}

		sz = sizeof(DWORD);
		if (ERROR_SUCCESS == RegQueryValueExW(hkey, L"FontSize", NULL, &type, (LPBYTE)&val, &sz) && type == REG_DWORD)
		{
			cfg->font.height = val;
		}

		sz = sizeof(DWORD);
		if (ERROR_SUCCESS == RegQueryValueExW(hkey, L"FontBold", NULL, &type, (LPBYTE)&val, &sz) && type == REG_DWORD)
		{
			cfg->font.isbold = val;
		}

		sz = sizeof(DWORD);
		if (ERROR_SUCCESS == RegQueryValueExW(hkey, L"Cursor", NULL, &type, (LPBYTE)&val, &sz) && type == REG_DWORD)
		{
			cfg->cursor_type = val;
		}

		sz = 64;
		RegQueryValueExA(hkey, "FontName", NULL, &type, (LPBYTE)cfg->font.name, &sz);

		RegCloseKey(hkey);
	}

	jwReloadColors(cfg, cscheme);

	/* Cheat: preset the window x and y coordinates so that JOE doesn't get a resize and kill the copyright banner */
	jw_initialcols = cfg->width;
	jw_initialrows = cfg->height;
	jw_initialcolorscheme = dupcolorscheme(cfg->currentcolors->colors);
}

void jwSaveSettings(Config *cfg)
{
	HKEY hkey;
	DWORD tmp;
	wchar_t fonttmp[64];

	if (ERROR_SUCCESS == RegCreateKeyExW(HKEY_CURRENT_USER, getkeyname(), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hkey, NULL))
	{
		RegSetValueExW(hkey, L"ColorScheme", 0, REG_SZ, (LPBYTE)cfg->currentcolors->file, sizeof(wchar_t) * (wcslen(cfg->currentcolors->file) + 1));
		
		tmp = cfg->font.height;
		RegSetValueExW(hkey, L"FontSize", 0, REG_DWORD, (LPBYTE)&tmp, sizeof(DWORD));

		tmp = cfg->font.isbold;
		RegSetValueExW(hkey, L"FontBold", 0, REG_DWORD, (LPBYTE)&tmp, sizeof(DWORD));

		RegSetValueExA(hkey, "FontName", 0, REG_SZ, (LPBYTE)cfg->font.name, sizeof(char) * (strlen(cfg->font.name) + 1));
		
		tmp = cfg->cursor_type;
		RegSetValueExW(hkey, L"Cursor", 0, REG_DWORD, (LPBYTE)&tmp, sizeof(DWORD));

		RegCloseKey(hkey);
	}
}

void jwReloadColors(Config *cfg, wchar_t *currentfile)
{
	wchar_t curcolors[MAX_PATH];
	struct jwcolorlist *clist;
	struct jwcolors *col = NULL;

	if (currentfile)
	{
		wcscpy(curcolors, currentfile);

		/* Don't fix path for builtins */
		if (*currentfile != L'*')
		{
			fixpath(curcolors, MAX_PATH);
		}
	}
	else
	{
		wcscpy(curcolors, L"");
	}

	cfg->currentcolors = NULL;
	if (cfg->colorlist)
	{
		freecolorlist(cfg->colorlist);
	}
	if (cfg->externalcolors)
	{
		freecolorlist(cfg->externalcolors);
		cfg->externalcolors = NULL;
	}

	cfg->colorlist = loadcolorschemes();
	
	for (clist = cfg->colorlist; clist; clist = clist->next)
	{
		if (!wcsicmp(curcolors, clist->file))
		{
			cfg->currentcolors = clist;
			break;
		}
	}

	if (!cfg->currentcolors)
	{
		cfg->externalcolors = loadexternalscheme(curcolors);
		cfg->currentcolors = cfg->externalcolors;
	}

	if (cfg->currentcolors)
	{
		col = loadcolorscheme(cfg->currentcolors);
	}

	if (!col)
	{
		/* The head of cfg->colorlist ought to be the default colors */

		for (clist = cfg->colorlist; clist; clist = clist->next)
		{
			col = loadcolorscheme(clist);
			if (col)
			{
				cfg->currentcolors = clist;
				break;
			}
		}

		assert(col);
	}
}

int jwLoadWindowLoc(POINT *point)
{
	HKEY hkey;
	int got = 0;

	if (RegOpenKeyExW(HKEY_CURRENT_USER, getkeyname(), 0, KEY_ALL_ACCESS, &hkey) == ERROR_SUCCESS)
	{
		DWORD sz = MAX_PATH;
		DWORD type;
		DWORD val;

		sz = sizeof(DWORD);
		if (ERROR_SUCCESS == RegQueryValueExW(hkey, L"X", NULL, &type, (LPBYTE)&val, &sz) && type == REG_DWORD)
		{
			point->x = val;
			got++;
		}

		sz = sizeof(DWORD);
		if (ERROR_SUCCESS == RegQueryValueExW(hkey, L"Y", NULL, &type, (LPBYTE)&val, &sz) && type == REG_DWORD)
		{
			point->y = val;
			got++;
		}

		RegCloseKey(hkey);
	}

	if (got != 2)
	{
		return 1;
	}
	
	/* Make sure the point is actually on the screen */

#ifndef NO_MULTIMON
	if (!MonitorFromPoint(*point, MONITOR_DEFAULTTONULL))
	{
		return 1;
	}
#else
	if (point->x < GetSystemMetrics(SM_XVIRTUALSCREEN) || point->x >= GetSystemMetrics(SM_CXVIRTUALSCREEN) ||
	    point->y < GetSystemMetrics(SM_YVIRTUALSCREEN) || point->y >= GetSystemMetrics(SM_CYVIRTUALSCREEN))
	{
		return 1;
	}
#endif

	return 0;
}

void jwSaveWindowLoc(POINT point)
{
	HKEY hkey;

	if (ERROR_SUCCESS == RegCreateKeyExW(HKEY_CURRENT_USER, getkeyname(), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hkey, NULL))
	{
		DWORD tmp;

		tmp = point.x;
		RegSetValueExW(hkey, L"X", 0, REG_DWORD, (LPBYTE)&tmp, sizeof(DWORD));

		tmp = point.y;
		RegSetValueExW(hkey, L"Y", 0, REG_DWORD, (LPBYTE)&tmp, sizeof(DWORD));

		RegCloseKey(hkey);
	}
}

void jwSaveWindowSize(int rows, int cols)
{
	HKEY hkey;

	if (ERROR_SUCCESS == RegCreateKeyExW(HKEY_CURRENT_USER, getkeyname(), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hkey, NULL))
	{
		DWORD tmp;

		tmp = rows;
		RegSetValueExW(hkey, L"Rows", 0, REG_DWORD, (LPBYTE)&tmp, sizeof(DWORD));

		tmp = cols;
		RegSetValueExW(hkey, L"Columns", 0, REG_DWORD, (LPBYTE)&tmp, sizeof(DWORD));

		RegCloseKey(hkey);
	}
}

void jwSaveWindowCoords(HWND hwnd, int rows, int cols)
{
	RECT rect;

	/* Don't save if window is maximized */
	if (!IsZoomed(hwnd) && GetWindowRect(hwnd, &rect))
	{
		POINT p;
		p.x = rect.left;
		p.y = rect.top;

		jwSaveWindowLoc(p);
		jwSaveWindowSize(rows, cols);
	}
}
