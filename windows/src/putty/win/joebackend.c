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
 * JoeWin backend.
 *
 * Hijacks PROT_TELNET, which is the default protocol for
 * our build of PuTTY and instead pipes it through a middle
 * layer to a running version of JOE on the other side.
 */

#include <stdio.h>
#include <stdlib.h>
#include "putty.h"
#include "jwglobals.h"
#include "jwcomm.h"
#include "jwbentries.h"
#include "jwuserfuncs.h"


/********************************************************** Context data */

typedef struct joe_backend_data {
	void *frontend;
	int bufsize;
	int exitcode;
	int active;

	HANDLE thread;
	DWORD threadId;
} *JoeData;


/********************************************************** Forward declarations */

static const char *joe_init(void *, void **, Config *, char *, int, char **, int, int);
static void joe_free(void *);
static void joe_reconfig(void *, Config *);
static int joe_send(void *, char *, int);
static int joe_sendbuffer(void *);
static void joe_size(void *, int, int);
static void joe_special(void *, Telnet_Special);
static const struct telnet_special *joe_get_specials(void *);
static int joe_connected(void *);
static int joe_sendok(void *);
static void joe_unthrottle(void *, int);
static int joe_ldisc(void *, int);
static void joe_provide_ldisc(void *, void *);
static void joe_provide_logctx(void *, void *);
static int joe_exitcode(void *);
static int joe_cfg_info(void *);
static void joe_shutdown(JoeData);


/********************************************************** Backend definition */

Backend joe_backend = {
    joe_init,
    joe_free,
    joe_reconfig,
    joe_send,
    joe_sendbuffer,
    joe_size,
    joe_special,
    joe_get_specials,
    joe_connected,
    joe_exitcode,
    joe_sendok,
    joe_ldisc,
    joe_provide_ldisc,
    joe_provide_logctx,
    joe_unthrottle,
    joe_cfg_info,
    "joe",
    PROT_RAW,
    999 /* port */
};


/********************************************************** Backend Implementation */

static const char *joe_init(void *frontend_handle, void **backend_handle,
			    Config *cfg,
			    char *host, int port, char **realhost, int nodelay,
			    int keepalive)
{
	JoeData data = snew(struct joe_backend_data);
	data->frontend = frontend_handle;
	data->bufsize = 0;

	/* Spin up thread, execute joe */
	data->thread = jwStartJoe(&data->threadId);
	data->active = 1;

	*realhost = _strdup("localhost");
	*backend_handle = data;

	return NULL; /* no error */
}

static void joe_free(void *handle)
{
	JoeData data = (JoeData) handle;

	/* Kill the thread */

	/* Free up PuTTY's handles */
	joe_shutdown(data);
	sfree(data);
}

static void joe_shutdown(JoeData data)
{
	data->active = 0;
	jwShutdownComm();
}

static void joe_reconfig(void *handle, Config *cfg)
{ /* Stub routine (we don't have any need to reconfigure this backend). */ }

static int joe_send(void *handle, char *buf, int len)
{
	jwWriteIO(JW_SIDE_UI, buf, len);
	return 0;
}

static int joe_sendbuffer(void *handle)
{
	JoeData data = (JoeData) handle;
	return data->bufsize;
}

static void joe_size(void *handle, int width, int height)
{
	jwSendComm2(JW_SIDE_UI, COMM_WINRESIZE, width, height);
}

static void joe_special(void *handle, Telnet_Special code)
{ return; }

static const struct telnet_special *joe_get_specials(void *handle)
{ return NULL; }

static int joe_connected(void *handle)
{
	/* Always connected */
	return 1;
}

static int joe_sendok(void *handle)
{
	return 1;
}

static void joe_unthrottle(void *handle, int backlog)
{
}

static int joe_ldisc(void *handle, int option)
{
	//return (option == LD_EDIT || option == LD_ECHO) ? 1 : 0;
	return 0;
}

static void joe_provide_ldisc(void *handle, void *ldisc)
{ }

static void joe_provide_logctx(void *handle, void *logctx)
{ }

static int joe_exitcode(void *handle)
{
	JoeData data = (JoeData) handle;
	if (!data->active)
	{
		return INT_MAX;
	}
	else
	{
		return -1;
	}
}

static int joe_cfg_info(void *handle)
{ return 0; }


/********************************************************** Shutdown handler */

void jwShutdownBackend(void *dataptr, int exitcode)
{
	JoeData data = (JoeData) dataptr;
	data->exitcode = exitcode;
	data->threadId = 0;
	data->thread = INVALID_HANDLE_VALUE;

	joe_shutdown(data);
	notify_remote_exit(data->frontend);
}

