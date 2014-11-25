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

#ifndef _JOEWIN_JOEDATA_H
#define _JOEWIN_JOEDATA_H

/*
 * These are normally -Ddefines a la the Makefile, but they cannot
 * be constant in a Windows setup.
 */

const char* joedata_plus(const char* input);

#ifdef JOEWIN
#define JOEDATA_PLUS(x) joedata_plus(x)
#define JOERC jw_joerc
#define JOEDATA jw_joedata
#else
#define JOEDATA_PLUS(x) (JOEDATA x)
#endif

extern unsigned char* JOEDATA;
extern unsigned char* JOERC;

#endif //_JOEWIN_JOEDATA_H