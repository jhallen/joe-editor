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
