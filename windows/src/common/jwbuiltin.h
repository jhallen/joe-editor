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

/* Support for built-in config files */

struct jfile {
	FILE *f;		/* Regular file, or NULL for built-in */
	unsigned char *p;	/* Built-in file pointer */
	unsigned int sz;
	void *dealloc;
};

typedef struct jfile JFILE;

JFILE *jwfopen(wchar_t *name, wchar_t *mode);
unsigned char *jwfgets(unsigned char *buf, int size, JFILE *f);
int jwfclose(JFILE *f);
const wchar_t *jwnextbuiltin(const wchar_t *prev, const wchar_t *suffix);

void jwAddResourceHandle(HMODULE module);
