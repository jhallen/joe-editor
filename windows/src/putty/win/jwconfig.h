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

#ifndef _JOEWIN_JWCONFIG_H
#define _JOEWIN_JWCONFIG_H

void jwLoadConfig(Config*);
void jwReloadColors(Config*, wchar_t*);

void jwSaveSettings(Config *cfg);
int jwLoadWindowLoc(POINT *point);
void jwSaveWindowCoords(HWND hwnd, int rows, int cols);

#endif // _JOEWIN_JWCONFIG_H
