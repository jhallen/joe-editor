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

/*
 * Miscellaneous stubs for PuTTY files removed from the project.
 */

#include "putty.h"


/* cmdline.c */

int cmdline_get_passwd_input(prompts_t *p, unsigned char *in, int inlen)
{
	return -1;
}


/* winhandl.c */

void handle_got_event(HANDLE event)
{
}

HANDLE *handle_get_events(int *nevents)
{
	*nevents = 0;
	return NULL;
}


/* winhelp.c */

void shutdown_help(void)
{
}

void launch_help(HWND hwnd, const char *topic)
{
}

int has_help(void)
{
	return 0;
}

void init_help(void)
{
}

void quit_help(HWND hwnd)
{
}


/* logging.c */

void *log_init(void *frontend, Config *cfg)
{
	return NULL;
}

void logtraffic(void *handle, unsigned char c, int logmode)
{
}

void logflush(void *handle)
{
}

void log_eventlog(void *handle, const char *event)
{
}


/* winnet.c */

void net_pending_errors(void)
{
}

void sk_cleanup(void)
{
}

void sk_init(void)
{
}
