/*
 *	Editor startup and main edit loop
 *	Copyright
 *		(C) 1992 Joseph H. Allen
 *
 * 	This file is part of JOE (Joe's Own Editor)
 */
#include "types.h"

#ifdef MOUSE_GPM
#include <gpm.h>
#endif

#ifdef JOEWIN
#include "jwcomm.h"
#include "jwversion.h"
#endif

unsigned char *exmsg = NULL;		/* Message to display when exiting the editor */
int usexmouse=0;
int xmouse=0;
int nonotice;
int noexmsg = 0;
int help;

Screen *maint;			/* Main edit screen */

/* Make windows follow cursor */

void dofollows(void)
{
	W *w = maint->curwin;

	do {
		if (w->y != -1 && w->watom->follow && w->object)
			w->watom->follow(w->object);
		w = (W *) (w->link.next);
	} while (w != maint->curwin);
}

/* Update screen */

volatile int dostaupd = 1;

void edupd(int flg)
{
	W *w;
	int wid, hei;
	unsigned char *gc = vsmk(1);

	/* dostaupd is set once a second to force status line update */
	if (dostaupd) {
		staupd = 1; /* Flag for status update in disptw */
		dostaupd = 0;
	}
	/* Resize screen if necessary */
	ttgtsz(&wid, &hei);
	if (nresize(maint->t, wid, hei)) {
		sresize(maint);
#ifdef MOUSE_GPM
		gpm_mx = wid;
		gpm_my = hei;
#endif
	}
	/* Move windows so that cursors stays on screen */
	dofollows();
	ttflsh();
	/* Send scrolling commands to terminal */
	nscroll(maint->t, BG_COLOR(bg_text));
	/* Display help */
	help_display(maint);
	/* Update all windows */
	w = maint->curwin;
	do {
		if (w->y != -1) {
			if (w->object && w->watom->disp)
				w->watom->disp(w->object, flg);
			msgout(w);
		}
		w = (W *) (w->link.next);
	} while (w != maint->curwin);
	/* Set cursor position */
	cpos(maint->t, maint->curwin->x + maint->curwin->curx, maint->curwin->y + maint->curwin->cury);
	staupd = 0;
	obj_free(gc);
}

static int ahead = 0;	/* Set when typeahead from before editor started is exhausted */
static int ungot = 0;
static int ungotc = 0;

void nungetc(int c)
{
	if (c != 'C' - '@' && c != 'M' - '@' && c != -1) {
		chmac();
		ungot = 1;
		ungotc = c;
	}
}

/* Execute a macro every nn seconds */

time_t last_timer_time = 0;
time_t cur_time;
time_t timer_macro_delay;
MACRO *timer_macro;

MACRO *timer_play()
{
	cur_time = time(NULL);
	if (timer_macro && timer_macro_delay && cur_time >= last_timer_time + timer_macro_delay) {
		last_timer_time = cur_time;
		return timer_macro;
	}
	return 0;
}

/* Main loop */

int edloop()
{
	int ret = 0;

	/* Here is the loop.  Loop while we're not exiting the editor (or query is not done)... */
	while (!leave) {
		MACRO *m;
		int c;

		/* Free exit message if we're not leaving */
		if (exmsg) {
			obj_free(exmsg);
			exmsg = NULL;
		}
		/* Update the screen */
		edupd(1);
		/* Set ahead when typeahead from before editor startup is done */
		if (!ahead && !have)
			ahead = 1;
		/* Get next character (including nungetc() one) */
		if (ungot) {
			c = ungotc;
			ungot = 0;
		} else
			c = ttgetc();
		/* Deal with typeahead from before editor starting: tty was in
		   cooked mode so it converted carriage returns to line
		   feeds.  Convert them back here. */
		if (!ahead && c == 10)
			c = 13;
		/* Give key to current keyboard handler: it returns a macro to execute when
		   a full sequence is decoded.  */
		m = dokey(maint->curwin->kbd, c);
		/* Make sure main window of group has copy of current key sequence so that it
		   is displayed in the status line (why doesn't status line code figure this out?) */
		if (maint->curwin->main && maint->curwin->main != maint->curwin) {
			int x = maint->curwin->kbd->x;

			maint->curwin->main->kbd->x = x;
			if (x)
				maint->curwin->main->kbd->seq[x - 1] = maint->curwin->kbd->seq[x - 1];
		}
		if (!m)
			m = timer_play();
		/* Execute macro */
		if (m) {
			ret = co_call(exemac, m);
			/* ret = exemac(m); */
		}
	}
	/* prompt can force return of error, which aborts macro */
	return ret;
}

#ifdef __MSDOS__
extern void setbreak();
extern int breakflg;
#endif

unsigned char **mainenv;

B *startup_log = NULL;
static int logerrors = 0;

unsigned char i_msg[128];

void internal_msg(unsigned char *s)
{
	P *t = pdup(startup_log->eof, USTR "internal_msg");
	binss(t, s);
	prm(t);
}

void setlogerrs(void)
{
	logerrors = 1;
}

/* Opens new bw with startup log */
int ushowlog(BW *bw)
{
	if (startup_log) {
		B *copied;
		BW *newbw;
		void *object;
		W *w;
		
		if (uduptw(bw)) {
			return -1;
		}
		
		copied = bcpy(startup_log->bof, startup_log->eof);
		copied->name = zdup(USTR "* Startup Log *");
		copied->internal = 1;
		
		newbw = (BW *) maint->curwin->object;
		object = newbw->object;
		w = newbw->parent;
		bwrm(newbw);
		w->object = (void *) (newbw = bwmk(w, copied, 0, NULL));
		wredraw(newbw->parent);
		newbw->object = object;
		
		return 0;
	}
	
	return 1;
}

/* Determine name editor was invoked as and process <name>rc file */

CAP *cap;
unsigned char **argv;

int joerc()
{
	struct stat sbuf;
	int c;
	time_t time_rc;
	unsigned char *s;
	unsigned char *t;
	unsigned char *run;
#if defined(__MSDOS__) || defined(JOEWIN)
	unsigned char *rundir;
#endif
	
	/* Figure out name editor was invoked under */
#if defined(__MSDOS__) || defined(JOEWIN)
	_fmode = O_BINARY;
	s = vscpyz(NULL, argv[0]);
	joesep(s);
	run = namprt(s);
	rundir = dirprt(s);
	for (c = 0; run[c]; ++c)
		if (run[c] == '.') {
			run = vstrunc(run, c);
			break;
		}
#else
	run = namprt(argv[0]);
#endif

#ifdef __MSDOS__

	s = vsncpy(NULL, 0, sv(run));
	s = vsncpy(sv(s), sc("rc"));
	c = procrc(cap, s);
	if (c == 0)
		goto donerc;
	if (c == 1) {
		logerror_1((char *)joe_gettext(_("There were errors in '%s'.  Falling back on default.\n")), s);
	}

	s = vsncpy(NULL, 0, sv(rundir));
	s = vsncpy(sv(s), sv(run));
	s = vsncpy(sv(s), sc("rc"));
	c = procrc(cap, s);
	if (c == 0)
		goto donerc;
	if (c == 1) {
		logerror_1((char *)joe_gettext(_("There were errors in '%s'.  Falling back on default.\n")), s);
	}
#else

	/* Name of system joerc file.  Try to find one with matching language... */
	
	/* Try full language: like joerc.de_DE */
#ifdef JOEWIN
	/* Windows port stores configuration files underneath 'conf' directory in same location as exe */
	t = vsncpy(NULL, 0, sz(JOERC));
#else
	t = vsncpy(NULL, 0, sc(JOERC));
#endif
	t = vsncpy(sv(t), sv(run));
	t = vsncpy(sv(t), sc("rc."));
	t = vsncpy(sv(t), sz(locale_msgs));
	if (!stat((char *)t,&sbuf))
		time_rc = sbuf.st_mtime;
	else {
		/* Try generic language: like joerc.de */
		if (locale_msgs[0] && locale_msgs[1] && locale_msgs[2]=='_') {
#ifdef JOEWIN
			t = vsncpy(NULL, 0, sz(JOERC));
#else
			t = vsncpy(NULL, 0, sc(JOERC));
#endif
			t = vsncpy(sv(t), sv(run));
			t = vsncpy(sv(t), sc("rc."));
			t = vsncpy(sv(t), locale_msgs, 2);
			if (!stat((char *)t,&sbuf))
				time_rc = sbuf.st_mtime;
			else
				goto nope;
		} else {
			nope:
			/* Try Joe's bad english */
#ifdef JOEWIN
			t = vsncpy(NULL, 0, sz(JOERC));
#else
			t = vsncpy(NULL, 0, sc(JOERC));
#endif
			t = vsncpy(sv(t), sv(run));
			t = vsncpy(sv(t), sc("rc"));
			if (!stat((char *)t,&sbuf))
				time_rc = sbuf.st_mtime;
			else
				time_rc = 0;
		}
	}

	/* User's joerc file */
	s = (unsigned char *)getenv("HOME");
	if (s) {
		s = vsncpy(NULL, 0, sz(s));
#ifndef JOEWIN
		s = vsncpy(sv(s), sc("/."));
#else
		/* No dot-names in Windows */
		s = vsncpy(sv(s), sc("\\"));
#endif
		s = vsncpy(sv(s), sv(run));
		s = vsncpy(sv(s), sc("rc"));

		if (!stat((char *)s,&sbuf)) {
			if (sbuf.st_mtime < time_rc) {
				logmessage_2((char *)joe_gettext(_("Warning: %s is newer than your %s.\n")),t,s);
			}
		}

		c = procrc(cap, s);
		if (c == 0) {
			goto donerc;
		}
		if (c == 1) {
			logerror_1((char *)joe_gettext(_("There were errors in '%s'.  Falling back on default.\n")), s);
		}
	}

	s = t;
	c = procrc(cap, s);
	if (c == 0)
		goto donerc;
	if (c == 1) {
		logerror_1((char *)joe_gettext(_("There were errors in '%s'.  Falling back on default.\n")), s);
	}

	/* Try built-in joerc */
	s = vsncpy(NULL, 0, sc("*"));
	s = vsncpy(sv(s), sv(run));
	s = vsncpy(sv(s), sc("rc"));
	c = procrc(cap, s);
	if (c != 0 && c != 1) {
		/* If *fancyjoerc not present, use *joerc which is always there */
		s = vstrunc(s, 0);
		s = vsncpy(sv(s),sc("*joerc"));
		c = procrc(cap, s);
	}
	if (c == 0)
		goto donerc;
	if (c == 1) {
		logerror_1((char *)joe_gettext(_("There were errors in '%s'.  Falling back on default.\n")), s);
	}
#endif

	logerror_1((char *)joe_gettext(_("Couldn't open '%s'\n")), s);
	return 1;

	donerc:

	if (validate_rc()) {
		logerror_0((char *)joe_gettext(_("rc file has no :main key binding section or no bindings.  Bye.\n")));
		return 1;
	}

	return 0;
}

/* Copy environment variables to global variables */

void process_env()
{
	unsigned char *s;

	if ((s = (unsigned char *)getenv("LINES")) != NULL)
		sscanf((char *)s, "%d", &lines);

	if ((s = (unsigned char *)getenv("COLUMNS")) != NULL)
		sscanf((char *)s, "%d", &columns);

	if ((s = (unsigned char *)getenv("BAUD")) != NULL)
		sscanf((char *)s, "%u", (unsigned *)&Baud);

	if (getenv("DOPADDING"))
		dopadding = 1;

	if (getenv("NOXON"))
		noxon = 1;

	if ((s = (unsigned char *)getenv("JOETERM")) != NULL)
		joeterm = s;

}

void setup_mouse()
{
	unsigned char *s;

#ifndef JOEWIN
	/* initialize mouse support */
	if (xmouse && (s=(unsigned char *)getenv("TERM")) && strstr((char *)s,"xterm"))
		usexmouse=1;
#else
	usexmouse = xmouse;
#endif
}

void process_global_options()
{
	int c;
	/* Process global options */
	for (c = 1; argv[c]; ++c) {
		if (argv[c][0] == '-') {
			if (argv[c][1])
				switch (glopt(argv[c] + 1, argv[c + 1], NULL, 1)) {
				case 0:
					logerror_1((char *)joe_gettext(_("Unknown option '%s'\n")), argv[c]);
					break;
				case 1:
					break;
				case 2:
					++c;
					break;
			} else
				idleout = 0;
		}
	}
}

void process_args()
{
	int c;
	int backopt;
	int omid;
	int opened = 0;

	/* The business with backopt is to load the file first, then apply file
	 * local options afterwords */

	/* orphan is not compatible with exemac()- macros need a window to exist */
	for (c = 1, backopt = 0; argv[c]; ++c)
		if (argv[c][0] == '+' && argv[c][1]>='0' && argv[c][1]<='9') {
			if (!backopt)
				backopt = c;
		} else if (argv[c][0] == '-' && argv[c][1]) {
			if (!backopt)
				backopt = c;
			if (glopt(argv[c] + 1, argv[c + 1], NULL, 0) == 2)
				++c;
		} else {
			B *b = bfind(argv[c]);
			BW *bw = NULL;
			int er = berror;

			/* This is too annoying */
			/* set_current_dir(argv[c],1); */

			setup_history(&filehist);
			append_history(filehist,sz(argv[c]));

			/* wmktw inserts the window before maint->curwin */
			if (!orphan || !opened) {
				bw = wmktw(maint, b);
				if (er)
					msgnwt(bw->parent, joe_gettext(msgs[-er]));
			} else {
				long line;
				b->orphan = 1;
				b->oldcur = pdup(b->bof, USTR "main");
				pline(b->oldcur, get_file_pos(b->name));
				p_goto_bol(b->oldcur);
				line = b->oldcur->line - (maint->h - 1) / 2;
				if (line < 0)
					line = 0;
				b->oldtop = pdup(b->oldcur, USTR "main");
				pline(b->oldtop, line);
				p_goto_bol(b->oldtop);
			}
			if (bw) {
				long lnum = 0;

				bw->o.readonly = bw->b->rdonly;
				if (backopt) {
					while (backopt != c) {
						if (argv[backopt][0] == '+') {
							sscanf((char *)(argv[backopt] + 1), "%ld", &lnum);
							++backopt;
						} else {
							if (glopt(argv[backopt] + 1, argv[backopt + 1], &bw->o, 0) == 2)
								backopt += 2;
							else
								backopt += 1;
							lazy_opts(bw->b, &bw->o);
						}
					}
				}
				bw->b->o = bw->o;
				bw->b->rdonly = bw->o.readonly;
				/* Put cursor in window, so macros work properly */
				maint->curwin = bw->parent;
				/* Execute macro */
				if (er == -1 && bw->o.mnew)
					exmacro(bw->o.mnew,1);
				if (er == 0 && bw->o.mold)
					exmacro(bw->o.mold,1);
				/* Hmm... window might not exist any more... depends on what macro does... */
				if (lnum > 0)
					pline(bw->cursor, lnum - 1);
				else
					pline(bw->cursor, get_file_pos(bw->b->name));
				p_goto_bol(bw->cursor);
				/* Go back to first window so windows are in same order as command line  */
				if (opened)
					wnext(maint);
				
			}
			opened = 1;
			backopt = 0;
		}
	if (opened) {
		/* Show all files on the screen */
		wshowall(maint);
		/* Temporarily set 'mid' option so that cursor is centered
		   in each window */
		omid = mid;
		mid = 1;
		dofollows();
		mid = omid;
	} else {
		/* Create empty window */
		BW *bw = wmktw(maint, bfind(USTR ""));

		if (bw->o.mnew)
			exmacro(bw->o.mnew,1);
	}
	/* Set window with cursor to first window on screen */
	maint->curwin = maint->topwin;
#ifdef JOEWIN
	notify_selection();
#endif
}

/* Show startup log if there were any messages */

void show_startup_log()
{
	if (logerrors) {
		B *copied = bcpy(startup_log->bof, startup_log->eof);
		BW *bw = wmktw(maint, copied);
		copied->name = zdup(startup_log->name);
		copied->internal = 1;
		maint->curwin = bw->parent;
#ifdef JOEWIN
		notify_selection();
#endif
		wshowall(maint);
	}
}

/* Set up piping into JOE */

void setup_pipein()
{
	if (!idleout && modify_logic(maint->curwin->object, ((BW *)maint->curwin->object)->b)) {
		/* Start shell going in first window */
		/* This is silly- mpx should be able to just read from stdin */

		unsigned char **a = vamk(3);
		a = vaadd(a, vsncpy(NULL, 0, sc("/bin/sh")));
		a = vaadd(a, vsncpy(NULL, 0, sc("-c")));
		a = vaadd(a, vsncpy(NULL, 0, sc("/bin/cat")));

		cstart(maint->curwin->object, USTR "/bin/sh", a, NULL, 0, 1);
	}
}

/* Scheduler wants us to get some work */

SCRN *main_scrn;

unsigned char *startup_gc;

#ifdef JOEWIN
int joe_main(int argc, char **real_argv, char **envv)
#else
int main(int argc, char **real_argv, char **envv)
#endif
{
	/* Save arguments */
	argv = (unsigned char **)real_argv;

	/* Remember environment vector for sub-shells */
	mainenv = (unsigned char **)envv;

	/* Garbage collect startup process */
	startup_gc = vsmk(1);

	/* Set up locale (determines character set of terminal) */
	joe_locale();
	fdefault.charmap = locale_map;
	pdefault.charmap = locale_map;

	/* Setup software virtual memory */
	vmem = vtmp();
	
	/* Create startup log buffer */
	startup_log = bfind_scratch(USTR "* Startup Log *");
	startup_log->internal = 1;

	/* Copy some environment variables to global variables */
	process_env();

	/* Try to get termcap entry before we get too far */
	if (!(cap = getcap(NULL, 9600, NULL, NULL))) {
		fprintf(stderr, (char *)joe_gettext(_("Couldn't load termcap/terminfo entry\n")));
		goto exit_errors;
	}

	/* Process JOERC file */
	if (joerc()) 
		goto exit_errors;

#ifndef JOEWIN
	/* Is somebody piping something into JOE, or is stdin the tty? */
	if (!isatty(fileno(stdin)))
		/* If stdin is not /dev/tty, set flag so that
		   nopen opens /dev/tty instead of using stdin/stdout */
		idleout = 0;
#endif

	/* First scan of argv: process global options on command line */
	process_global_options();

	/* Enable mouse if we're an xterm and mouse option was given in rc
	 * file or as a command line option */
	setup_mouse();

	/* Setup tty handler (sets cbreak mode, turns off cooked) */
	if (!(main_scrn = nopen(cap)))
		goto exit_errors;

	/* Initialize windowing system */
	maint = screate(main_scrn);

	/* Read in ~/.joe_state file */
	load_state();

	/* Read files given on command line and process local options */
	process_args();

	/* Show startup log if there were any messages */
	show_startup_log();

	/* Turn on help if requested by global option */
	if (help) {
		help_on(maint);
	}

	/* Display startup message unless disabled by global option */
	if (!nonotice) {
		msgnw(((BASE *)lastw(maint)->object)->parent,
#ifndef JOEWIN
		  vsfmt(NULL, 0, joe_gettext(_("\\i** Joe's Own Editor v%s ** (%s) ** Copyright %s 2008 **\\i")),VERSION,locale_map->name,(locale_map->type ? "©" : "(C)")));
#else
		  USTR("\\i" JW_VERSION_BANNER "\\i"));
#endif
	}

	/* Setup reading in from stdin to first window if something was
	   piped into JOE */
	setup_pipein();

	/* Clean up startup gargbage */
	obj_free(startup_gc);

#ifdef JOEWIN
	/* Rendezvous.  Wait until the UI end is ready before we get going. */
	jwRendezvous(JW_TO_EDITOR, JW_TO_UI);
#endif

	/* Run the editor */
	edloop();

	/* Write ~/.joe_state file */
	save_state();

	/* Delete all buffers so left over locks get eliminated */
	brmall();

	/* Delete temporary software virtual memory file */
	vclose(vmem);

	/* Close terminal (restores mode) */
	nclose(main_scrn);

	if (exmsg && !noexmsg) {
		fprintf(stderr, "\n%s\n", exmsg);
	} else if (notite) {
		fprintf(stderr, "\n");
	}

	return 0;

exit_errors:

	/* Write out error log to console if we are exiting with errors. */
	if (startup_log && startup_log->eof->byte)
		bsavefd(startup_log->bof, 2, startup_log->eof->byte);
	
	return 1;
}
