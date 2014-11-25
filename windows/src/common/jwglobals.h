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

#ifndef _JOEWIN_GLOBALS_H
#define _JOEWIN_GLOBALS_H

#ifdef _JOEWIN_DO_GLOBALS
#define JOEWIN_GLOBAL
#else
#define JOEWIN_GLOBAL extern
#endif

JOEWIN_GLOBAL int jw_argc;
JOEWIN_GLOBAL unsigned char** jw_argv;
JOEWIN_GLOBAL unsigned char* jw_joedata;
JOEWIN_GLOBAL unsigned char* jw_joerc;
JOEWIN_GLOBAL unsigned char* jw_home;
JOEWIN_GLOBAL wchar_t* jw_personality;

JOEWIN_GLOBAL int jw_initialrows, jw_initialcols;

int jwInitJoe(int, wchar_t**);
HANDLE jwStartJoe(unsigned long*);

void jwexit(int);

#endif