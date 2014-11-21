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

#define utf8towcs(out,in,sz)	(!MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, (in), -1, (out), (sz)))
#define wcstoutf8(out,in,sz)	(!WideCharToMultiByte(CP_UTF8, JW_GET_WC_FLAGS, (in), -1, (out), (sz), NULL, NULL))
#define utf8towcslen(s)		(MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, (s), -1, NULL, 0))
#define wcstoutf8len(s)		(WideCharToMultiByte(CP_UTF8, JW_GET_WC_FLAGS, (s), -1, NULL, 0, NULL, NULL))

#endif
