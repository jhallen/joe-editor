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

#include "types.h"
#include "jwcomm.h"

/* Copy block contents to the Windows clipboard */

int uwincopy(BW *bw)
{
	int result = 1;

	if (markv(1)) {
		unsigned char *vscopy, *vscopy8;
		struct charmap *map;
		int len8, len16;

		if (square) {
			B *copy = pextrect(markb, markk->line - markb->line + 1, markk->xcol);
			vscopy = brvs(NULL, copy->bof, copy->eof->byte);
			brm(copy);
		} else {
			vscopy = brvs(NULL, markb, markk->byte - markb->byte);
		}

		/* Convert to UTF-8 and remap line endings if not in CRLF mode */
		map = markb->b->o.charmap;
		if (map->type == 0) {
			/* Byte */
			int i, vlen = vslen(vscopy);
			int crlast = 0;
			int convertlines = !markb->b->o.crlf;

			vscopy8 = vsmk(vlen);
			for (i = 0; i < vlen; i++) {
				int len, c;
				UCHAR buf[16];

				c = map->to_uni(map, vscopy[i]);
				if (c == -1)
				{
					vscopy8 = vsadd(vscopy8, '?');
				}
				else
				{
					if (convertlines && !crlast && c == 10) {
						vscopy8 = vscat(vscopy8, USTR "\r\n", 2);
					} else {
						len = utf8_encode(buf, joe_to_uni(map, vscopy[i]));
						vscopy8 = vscat(vscopy8, buf, len);
					}
				}

				crlast = (c == 13);
			}
		} else {
			/* UTF-8 */
			if (markb->b->o.crlf) {
				/* No need to convert line endings */
				vscopy8 = vscopy;
			} else {
				int i = 0, vlen = vslen(vscopy), last = 0, crlast = 0;
				vscopy8 = vsmk(vlen + markk->line - markb->line + 2);

				for (i = 0; i < vlen; i++) {
					int c = vscopy[i];

					if (c == 10 && !crlast) {
						vscopy8 = vscat(vscopy8, &vscopy[last], i - last);
						vscopy8 = vsadd(vscopy8, '\r');

						last = i;
					} else 
						crlast = (c == 13);
				}

				if (last > 0)
					vscopy8 = vscat(vscopy8, &vscopy[last], vlen - last);
				else
					vscopy8 = vscopy;
			}
		}

		/* Convert to UTF-16 & copy */
		len8 = vslen(vscopy8);
		len16 = MultiByteToWideChar(CP_UTF8, 0, (LPCSTR) vscopy8, len8, NULL, 0);

		if (len16 > 0 && OpenClipboard(0)) {
			HANDLE hMem;

			len16++; /* NUL */
			hMem = GlobalAlloc(GMEM_MOVEABLE, sizeof(wchar_t) * len16);
			MultiByteToWideChar(CP_UTF8, 0, (LPCSTR) vscopy8, len8, (LPWSTR) GlobalLock(hMem), len16);
			GlobalUnlock(hMem);

			EmptyClipboard();
			SetClipboardData(CF_UNICODETEXT, hMem);
			CloseClipboard();

			msgnw(bw->parent, joe_gettext(_("Copied to clipboard")));
			result = 0;
		} else {
			msgnw(bw->parent, joe_gettext(_("Error copying")));
		}

		obj_free(vscopy);
	} else {
		msgnw(bw->parent, joe_gettext(_("No block")));
	}

	return result;
}

/* Gets UTF-8 string from the clipboard, if it's there.  Returns length (0 if no data, <0 if
   error) and allocates a string to store the data.  Caller must free this string. */

static int getpastedata(unsigned char **result)
{
	int ret = -1;
	*result = NULL;

	if (OpenClipboard(0)) {
		HANDLE cdata;
		unsigned char *paste8;

		/* CF_OEMTEXT and CF_TEXT are coerced to UTF-16 automatically */
		cdata = GetClipboardData(CF_UNICODETEXT);
		if (cdata != NULL) {
			int len8;
			wchar_t *paste16;

			paste16 = (wchar_t*) GlobalLock(cdata);
			len8 = wcstoutf8len(paste16);
			if (len8 > 0) {
				paste8 = USTR joe_malloc(len8);
				if (!wcstoutf8((LPSTR) paste8, paste16, len8)) {
					*result = paste8;
					ret = len8 - 1;
				} else {
					assert(0);
					joe_free(paste8);
				}
			}

			GlobalUnlock(cdata);
		} else {
			ret = 0;
		}

		CloseClipboard();
	}

	return ret;
}

/* Paste windows clipboard contents to cursor position */

static int dowinpaste(BW *bw, int mark)
{
	unsigned char *paste8;
	int presult;
	
	presult = getpastedata(&paste8);
	if (presult < 0) {
		msgnw(bw->parent, joe_gettext(_("Error pasting")));
	} else if (presult == 0) {
		msgnw(bw->parent, joe_gettext(_("Clipboard empty")));
	} else {
		struct charmap *map;
		int crlf;

		/* Paste in local charmap and reformat CRLF's to buffer's line ending */

		map = bw->b->o.charmap;
		crlf = bw->b->o.crlf;
		if (map->type == 0) {
			/* Byte */
			int i = 0, crlast = 0;
			struct utf8_sm m;

			utf8_init(&m);

			if (mark) umarkb(bw);

			for (i = 0; i < presult; i++) {
				int u;
				
				u = utf8_decode(&m, paste8[i]);
				if (u >= 0) {
					int c;

					c = joe_from_uni(map, u);
					if (c == 13) {
						crlast = 1;
					} else if (c == 10 && crlast) {
						binss(bw->cursor, crlf ? USTR "\r\n" : USTR "\n");
						pnextl(bw->cursor);
						crlast = 0;
					} else {
						if (crlast) {
							binsc(bw->cursor, 13);
							pfwrd(bw->cursor, 1);
						}

						if (c < 0) {
							c = joe_from_uni(map, '?');
						}

						if (c >= 0) {
							binsc(bw->cursor, c);
							pfwrd(bw->cursor, 1);
						}

						crlast = 0;
					}
				}
			}

			if (crlast) {
				binsc(bw->cursor, 13);
				pfwrd(bw->cursor, 1);
			}

			if (mark) umarkk(bw);
		} else {
			/* UTF-8 */
			int i = 0, last = 0, crlast = 0;

			if (mark) umarkb(bw);

			for (i = 0; i < presult; i++) {
				int c = paste8[i];

				if (c == 10 && crlast) {
					int amt = i - last - 1;

					binsm(bw->cursor, &paste8[last], amt);
					pfwrd(bw->cursor, amt);
					binss(bw->cursor, crlf ? USTR "\r\n" : USTR "\n");
					pnextl(bw->cursor);

					last = i + 1;
				}

				crlast = (c == 13);
			}

			binsm(bw->cursor, &paste8[last], presult - last);
			pfwrd(bw->cursor, presult - last);

			if (mark) umarkk(bw);
		}

		joe_free(paste8);

		return 0;
	}

	return 1;
}

int uwinpaste(BW *bw)
{
	return dowinpaste(bw, 0);
}

/* Copy block if there's one selected, otherwise paste Windows clipboard contents */

int uwinblkcpy(BW *bw)
{
	if (markv(1)) {
		/* If there's a block, copy it */
		return ublkcpy(bw);
	} else {
		/* If no block, paste from the clipboard */
		int result;
		
		result = dowinpaste(bw, 1);
		if (!result) utomarkb(bw);
		return result;
	}
}

/* Show the context menu */

int uwincontext(BW *bw)
{
	int hasblock = markv(1);
	jwSendComm1(JW_FROM_EDITOR, COMM_CONTEXTMENU, hasblock);
	return 0;
}

/* Turn off echo in a shell window */
void vtraw(int fd);

int uwinrawvt(BW *bw)
{
	if (bw->b->vt) {
		vtraw(bw->b->out);
	}
}
