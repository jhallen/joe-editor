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
