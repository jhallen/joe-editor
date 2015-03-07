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

#ifndef __JOEWIN_JWUTILS_H
#define __JOEWIN_JWUTILS_H

int fixpath(wchar_t *path, size_t sz);

#ifdef _JOEWIN_DO_GLOBALS
DWORD jw_wc_flags = -1;
#else
extern DWORD jw_wc_flags;
#endif

// 0x80 == WC_ERR_INVALID_CHARS, which is not available on XP.  Do the best we can do...
#define JW_GET_WC_FLAGS		(jw_wc_flags != -1 ? jw_wc_flags : (jw_wc_flags = ((GetVersion() & 0xff) >= 6 ? 0x80 : 0)))

/********************************************************** UTF-16/UTF-8 conversion macros */

#define utf8towcs(out,in,sz)	(!MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, ((LPCSTR)(in)), -1, (out), (sz)))
#define wcstoutf8(out,in,sz)	(!WideCharToMultiByte(CP_UTF8, JW_GET_WC_FLAGS, ((LPCSTR)(in)), -1, (out), (sz), NULL, NULL))
#define utf8towcslen(s)		(MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, ((LPCSTR)(s)), -1, NULL, 0))
#define wcstoutf8len(s)		(WideCharToMultiByte(CP_UTF8, JW_GET_WC_FLAGS, (s), -1, NULL, 0, NULL, NULL))

#endif
