/*
 *	UNIX Tty and Process interface
 *	Copyright
 *		(C) 1992 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */
#include "types.h"

/* Needed for TIOCGWINSZ detection below */
#ifdef GWINSZ_IN_SYS_IOCTL
#ifdef HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif
#endif

#ifdef HAVE_SYS_WAIT_H
#include <sys/wait.h>
#endif
#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif

#ifdef HAVE_OPENPTY

#ifdef HAVE_PTY_H
#include <pty.h>
#endif

#ifdef HAVE_LIBUTIL_H
#include <libutil.h>
#endif

#endif

#ifdef HAVE_LOGIN_TTY
#ifdef HAVE_UTMP_H
#include <utmp.h>
#endif
#endif

int idleout = 1;

#ifdef __amigaos
#undef SIGTSTP
#endif

/* We use the defines in sys/ioctl to determine what type
 * tty interface the system uses and what type of system
 * we actually have.
 */
#ifdef HAVE_POSIX_TERMIOS
#  include <termios.h>
#else
#  ifdef HAVE_SYSV_TERMIO
#    include <termio.h>
#    include <sys/termio.h>
#  else
#    include <sgtty.h>
#  endif
#endif

#ifdef HAVE_SETITIMER
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#endif

/* I'm not sure if SCO_UNIX and ISC have __svr4__ defined, but I think
   they might */
#ifdef M_SYS5
#ifndef M_XENIX
#include <sys/stream.h>
#include <sys/ptem.h>
#ifndef __svr4__
#define __svr4__ 1
#endif
#endif
#endif

#ifdef ISC
#ifndef __svr4__
#define __svr4__ 1
#endif
#endif

#ifdef __svr4__
#include <stropts.h>
#endif

/** Aliased defines **/

/* O_NDELAY, O_NONBLOCK, and FNDELAY are all synonyms for placing a descriptor
 * in non-blocking mode; we make whichever one we have look like O_NDELAY
 */
#ifndef O_NDELAY
#ifdef O_NONBLOCK
#define O_NDELAY O_NONBLOCK
#endif
#ifdef FNDELAY
#define O_NDELAY FNDELAY
#endif
#endif

/* Some systems define this, some don't */
#ifndef sigmask
#define sigmask(x) (1<<((x)-1))
#endif

/* Some BSDs don't have TILDE */
#ifndef TILDE
#define TILDE 0
#endif

/* Global configuration variables */

int noxon = 0;			/* Set if ^S/^Q processing should be disabled */
int Baud = 0;			/* Baud rate from joerc, cmd line or environment */

/* The terminal */

FILE *termin = NULL;
FILE *termout = NULL;

/* Original state of tty */

#ifdef HAVE_POSIX_TERMIOS
struct termios oldterm;
#else /* HAVE_POSIX_TERMIOS */
#ifdef HAVE_SYSV_TERMIO
static struct termio oldterm;
#else /* HAVE_SYSV_TERMIO */
static struct sgttyb oarg;
static struct tchars otarg;
static struct ltchars oltarg;
#endif /* HAVE_SYSV_TERMIO */
#endif /* HAVE_POSIX_TERMIOS */

/* Output buffer, index and size */

char *obuf = NULL;
ptrdiff_t obufp = 0;
ptrdiff_t obufsiz;

/* The baud rate */

long tty_baud;			/* Bits per second */
long upc;			/* Microseconds per character */

/* TTY Speed code to baud-rate conversion table (this is dumb- is it really
 * too much to ask for them to just use an integer for the baud-rate?)
 */

static speed_t speeds_in[] = {
	B50, B75, B110, B134, B150, B200, B300, B600, B1200, B1800, B2400, B4800, B9600
#ifdef EXTA
	, EXTA
#endif
#ifdef EXTB
	, EXTB
#endif
#ifdef B19200
	, B19200
#endif
#ifdef B38400
	, B38400
#endif
};

static long speeds_out[] = {
	50, 75, 110, 134, 150, 200, 300, 600, 1200, 1800, 2400, 4800, 9600
#ifdef EXTA
	, 19200
#endif
#ifdef EXTB
	, 38400
#endif
#ifdef B19200
	, 19200
#endif
#ifdef B38400
	, 38400
#endif
};

/* Input buffer */

int have = 0;			/* Set if we have pending input */
char havec;	/* Character read in during pending input check */
int leave = 0;			/* When set, typeahead checking is disabled */

/* TTY mode flag.  1 for open, 0 for closed */
static int ttymode = 0;

/* Signal state flag.  1 for joe, 0 for normal */
static int ttysig = 0;

/* Stuff for shell windows */

static pid_t kbdpid;		/* PID of kbd client */
static int ackkbd = -1;		/* Editor acks keyboard client to this */

static int mpxfd;		/* Editor reads packets from this fd */
static int mpxsfd;		/* Clients send packets to this fd */

static int nmpx = 0;
static int acceptch = NO_MORE_DATA;	/* =-1 if we have last packet */

struct packet {
	MPX *who;
	ptrdiff_t size;
	int ch;
	char data[1024];
} pack;

MPX asyncs[NPROC];

/* Set signals for JOE */
void sigjoe(void)
{
	if (ttysig)
		return;
	ttysig = 1;
	joe_set_signal(SIGHUP, ttsig);
	joe_set_signal(SIGTERM, ttsig);
	joe_set_signal(SIGABRT, ttsig);
	joe_set_signal(SIGINT, SIG_IGN);
	joe_set_signal(SIGPIPE, SIG_IGN);
}

/* Restore signals for exiting */
void signrm(void)
{
	if (!ttysig)
		return;
	ttysig = 0;
	joe_set_signal(SIGABRT, SIG_DFL);
	joe_set_signal(SIGHUP, SIG_DFL);
	joe_set_signal(SIGTERM, SIG_DFL);
	joe_set_signal(SIGINT, SIG_DFL);
	joe_set_signal(SIGPIPE, SIG_DFL);
}

/* Open terminal and set signals */

void ttopen(void)
{
	sigjoe();
	ttopnn();
}

/* Close terminal and restore signals */

void ttclose(void)
{
	ttclsn();
	signrm();
}

static int winched = 0;
#ifdef SIGWINCH
/* Window size interrupt handler */
static RETSIGTYPE winchd(int unused)
{
	++winched;
	REINSTALL_SIGHANDLER(SIGWINCH, winchd);
}
#endif

/* Second ticker */

int ticked = 0;
static RETSIGTYPE dotick(int unused)
{
	ticked = 1;
}

void tickoff(void)
{
#ifdef HAVE_SETITIMER
	struct itimerval val;
	val.it_value.tv_sec = 0;
	val.it_value.tv_usec = 0;
	val.it_interval.tv_sec = 0;
	val.it_interval.tv_usec = 0;
	setitimer(ITIMER_REAL,&val,NULL);
#else
	alarm(0);
#endif
}

void tickon(void)
{
#ifdef HAVE_SETITIMER
	struct itimerval val;
	val.it_interval.tv_sec = 0;
	val.it_interval.tv_usec = 0;
	if (auto_scroll) {
		long tim = auto_trig_time - mnow();
		if (tim < 0)
			tim = 1;
		tim *= 1000;
		val.it_value.tv_sec = 0;
		val.it_value.tv_usec = tim;
	} else {
		val.it_value.tv_sec = 1;
		val.it_value.tv_usec = 0;
	}
	ticked = 0;
	joe_set_signal(SIGALRM, dotick);
	setitimer(ITIMER_REAL,&val,NULL);
#else
	ticked = 0;
	joe_set_signal(SIGALRM, dotick);
	alarm(1);
#endif
}

/* Open terminal */

void ttopnn(void)
{
	ptrdiff_t x;
	speed_t bbaud;

#ifdef HAVE_POSIX_TERMIOS
	struct termios newterm;
#else
#ifdef HAVE_SYSV_TERMIO
	struct termio newterm;
#else
	struct sgttyb arg;
	struct tchars targ;
	struct ltchars ltarg;
#endif
#endif

	if (!termin) {
		if (idleout ? (!(termin = stdin) || !(termout = stdout)) : (!(termin = fopen("/dev/tty", "r")) || !(termout = fopen("/dev/tty", "w")))) {
			fputs(joe_gettext(_("Couldn\'t open /dev/tty\n")), stderr);
			exit(1);
		} else {
#ifdef SIGWINCH
			joe_set_signal(SIGWINCH, winchd);
#endif
		}
	}

	if (ttymode)
		return;
	ttymode = 1;
	fflush(termout);

#ifdef HAVE_POSIX_TERMIOS
	tcgetattr(fileno(termin), &oldterm);
	newterm = oldterm;
	newterm.c_lflag = 0;
	if (noxon)
		newterm.c_iflag &= ~(unsigned)(ICRNL | IGNCR | INLCR | IXON | IXOFF);
	else
		newterm.c_iflag &= ~(unsigned)(ICRNL | IGNCR | INLCR);
	newterm.c_oflag = 0;
	newterm.c_cc[VMIN] = 1;
	newterm.c_cc[VTIME] = 0;
	tcsetattr(fileno(termin), TCSADRAIN, &newterm);
	bbaud = cfgetospeed(&newterm);
#else
#ifdef HAVE_SYSV_TERMIO
	joe_ioctl(fileno(termin), TCGETA, &oldterm);
	newterm = oldterm;
	newterm.c_lflag = 0;
	if (noxon)
		newterm.c_iflag &= ~(ICRNL | IGNCR | INLCR | IXON | IXOFF);
	else
		newterm.c_iflag &= ~(ICRNL | IGNCR | INLCR);
	newterm.c_oflag = 0;
	newterm.c_cc[VMIN] = 1;
	newterm.c_cc[VTIME] = 0;
	joe_ioctl(fileno(termin), TCSETAW, &newterm);
	bbaud = (newterm.c_cflag & CBAUD);
#else
	joe_ioctl(fileno(termin), TIOCGETP, &arg);
	joe_ioctl(fileno(termin), TIOCGETC, &targ);
	joe_ioctl(fileno(termin), TIOCGLTC, &ltarg);
	oarg = arg;
	otarg = targ;
	oltarg = ltarg;
	arg.sg_flags = ((arg.sg_flags & ~(ECHO | CRMOD | XTABS | ALLDELAY | TILDE)) | CBREAK);
	if (noxon) {
		targ.t_startc = -1;
		targ.t_stopc = -1;
	}
	targ.t_intrc = -1;
	targ.t_quitc = -1;
	targ.t_eofc = -1;
	targ.t_brkc = -1;
	ltarg.t_suspc = -1;
	ltarg.t_dsuspc = -1;
	ltarg.t_rprntc = -1;
	ltarg.t_flushc = -1;
	ltarg.t_werasc = -1;
	ltarg.t_lnextc = -1;
	joe_ioctl(fileno(termin), TIOCSETN, &arg);
	joe_ioctl(fileno(termin), TIOCSETC, &targ);
	joe_ioctl(fileno(termin), TIOCSLTC, &ltarg);
	bbaud = arg.sg_ospeed;
#endif
#endif

	tty_baud = 9600;
	upc = 0;
	for (x = 0; x != sizeof(speeds_in)/sizeof(speed_t); ++x)
		if (bbaud == speeds_in[x]) {
			tty_baud = speeds_out[x];
			break;
		}
	if (Baud)
		tty_baud = Baud;
	upc = DIVIDEND / tty_baud;
	if (obuf)
		joe_free(obuf);
	if (!(TIMES * upc))
		obufsiz = 4096;
	else {
		obufsiz = 1000000 / (TIMES * upc);
		if (obufsiz > 4096)
			obufsiz = 4096;
	}
	if (!obufsiz)
		obufsiz = 1;
	obuf = (char *)joe_malloc(obufsiz);
}

/* Close terminal */

void ttclsn(void)
{
	int oleave;

	if (ttymode)
		ttymode = 0;
	else
		return;

	oleave = leave;
	leave = 1;

	ttflsh();

#ifdef HAVE_POSIX_TERMIOS
	tcsetattr(fileno(termin), TCSADRAIN, &oldterm);
#else
#ifdef HAVE_SYSV_TERMIO
	joe_ioctl(fileno(termin), TCSETAW, &oldterm);
#else
	joe_ioctl(fileno(termin), TIOCSETN, &oarg);
	joe_ioctl(fileno(termin), TIOCSETC, &otarg);
	joe_ioctl(fileno(termin), TIOCSLTC, &oltarg);
#endif
#endif

	leave = oleave;
}

/* Timer interrupt handler */

static int yep;
static RETSIGTYPE dosig(int unused)
{
	yep = 1;
}

/* FLush output and check for typeahead */

#ifdef HAVE_SETITIMER
#ifdef SIG_SETMASK
static void maskit(void)
{
	sigset_t set;

	sigemptyset(&set);
	sigaddset(&set, SIGALRM);
	sigprocmask(SIG_SETMASK, &set, NULL);
}

static void unmaskit(void)
{
	sigset_t set;

	sigemptyset(&set);
	sigprocmask(SIG_SETMASK, &set, NULL);
}

static void pauseit(void)
{
	sigset_t set;

	sigemptyset(&set);
	sigsuspend(&set);
}

#else
static void maskit(void)
{
	sigsetmask(sigmask(SIGALRM));
}

static void unmaskit(void)
{
	sigsetmask(0);
}

static void pauseit(void)
{
	sigpause(0);
}

#endif
#endif

/* Check for type-ahead */

int ttcheck()
{
	/* Ack previous packet */
	if (ackkbd != -1 && acceptch != NO_MORE_DATA && !have) {
		char c = 0;

		if (pack.who && pack.who->func)
			joe_write(pack.who->ackfd, &c, 1);
		else
			joe_write(ackkbd, &c, 1);
		acceptch = NO_MORE_DATA;
	}

	/* Check for typeahead or next packet */

	if (!have && !leave) {
		if (ackkbd != -1) {
			fcntl(mpxfd, F_SETFL, O_NDELAY);
			if (read(mpxfd, &pack, pack.data - (char *)&pack) > 0) {
				fcntl(mpxfd, F_SETFL, 0);
				joe_read(mpxfd, pack.data, pack.size);
				have = 1;
				acceptch = pack.ch;
			} else
				fcntl(mpxfd, F_SETFL, 0);
		} else {
			/* Set terminal input to non-blocking */
			fcntl(fileno(termin), F_SETFL, O_NDELAY);

			/* Try to read */
			if (read(fileno(termin), &havec, 1) == 1)
				have = 1;

			/* Set terminal back to blocking */
			fcntl(fileno(termin), F_SETFL, 0);
		}
	}
	return have;
}

/* Flush output and check for type ahead */

int ttflsh(void)
{
	/* Flush output */
	if (obufp) {
		long usec = obufp * upc;	/* No. usecs this write should take */
 
#ifdef HAVE_SETITIMER
		if (usec >= 50000 && tty_baud < 9600) {
			struct itimerval a, b;

			a.it_value.tv_sec = usec / 1000000;
			a.it_value.tv_usec = usec % 1000000;
			a.it_interval.tv_usec = 0;
			a.it_interval.tv_sec = 0;
			alarm(0);
			joe_set_signal(SIGALRM, dosig);
			yep = 0;
			maskit();
			setitimer(ITIMER_REAL, &a, &b);
			joe_write(fileno(termout), obuf, obufp);
			while (!yep)
				pauseit();
			unmaskit();
		} else
			joe_write(fileno(termout), obuf, obufp);

#else

		joe_write(fileno(termout), obuf, obufp);

#ifdef FIORDCHK
		if (tty_baud < 9600 && usec / 1000)
			nap(usec / 1000);
#endif

#endif

		obufp = 0;
	}

	/* Check for typeahead or next packet */
	ttcheck();
	return 0;
}

/* Read next character from input */

void mpxdied(MPX *m);

time_t last_time;

char ttgetc(void)
{
        MACRO *m;
	ptrdiff_t stat;
	time_t new_time;
	int flg;


	tickon();

      loop:
      	flg = 0;
      	/* Status line clock */
	new_time = time(NULL);
	if (new_time != last_time) {
		last_time = new_time;
		dostaupd = 1;
		ticked = 1;
	}
	/* Autoscroller */
	if (auto_scroll && mnow() >= auto_trig_time) {
		do_auto_scroll();
		ticked = 1;
		flg = 1;
	}
	ttflsh();
	m = timer_play();
	if (m) {
	        exemac(m, NO_MORE_DATA);
	        edupd(1);
	        ttflsh();
	}
	while (winched) {
		winched = 0;
		dostaupd = 1;
		edupd(1);
		ttflsh();
	}
	if (ticked) {
		edupd(flg);
		ttflsh();
		tickon();
	}
	if (ackkbd != -1) {
		if (!have) {	/* Wait for input */
			stat = read(mpxfd, &pack, pack.data - (char *)&pack);

			if (pack.size && stat > 0) {
				joe_read(mpxfd, pack.data, pack.size);
			} else if (stat < 1) {
				if (winched || ticked)
					goto loop;
				else
					ttsig(0);
			}
			acceptch = pack.ch;
		}
		have = 0;
		if (pack.who) {	/* Got bknd input */
			if (acceptch != NO_MORE_DATA) {
				if (pack.who->func) {
					pack.who->func(pack.who->object, pack.data, pack.size);
					edupd(1);
				}
			} else
				mpxdied(pack.who);
			goto loop;
		} else {
			if (acceptch != NO_MORE_DATA) {
				tickoff();
				return TO_CHAR_OK(acceptch);
			}
			else {
				tickoff();
				ttsig(0);
				return 0;
			}
		}
	}
	if (have) {
		have = 0;
	} else {
		if (read(fileno(termin), &havec, 1) < 1) {
			if (winched || ticked)
				goto loop;
			else
				ttsig(0);
		}
	}
	tickoff();
	return havec;
}

/* Get character from input: convert whatever we get to Unicode */

static struct utf8_sm main_utf8_sm;

int ttgetch()
{
	if (locale_map->type) {
		int utf8_char;
		do {
			char c = ttgetc();
			utf8_char = utf8_decode(&main_utf8_sm, c);
		} while (utf8_char < 0);
		return utf8_char;
	} else {
		char c = ttgetc();
		int utf8_char = to_uni(locale_map, c);
		if (utf8_char == -1)
			utf8_char = '?'; /* Maybe we should ignore it? */
		return utf8_char;
	}
}

/* Write string to output */

void ttputs(const char *s)
{
	while (*s) {
		obuf[obufp++] = *s++;
		if (obufp == obufsiz)
			ttflsh();
	}
}

/* Get window size */

void ttgtsz(ptrdiff_t *x, ptrdiff_t *y)
{
#ifdef TIOCGSIZE
	struct ttysize getit;
#else
#ifdef TIOCGWINSZ
	struct winsize getit;
#endif
#endif

	*x = 0;
	*y = 0;

#ifdef TIOCGSIZE
	if (joe_ioctl(fileno(termout), TIOCGSIZE, &getit) != -1) {
		*x = getit.ts_cols;
		*y = getit.ts_lines;
	}
#else
#ifdef TIOCGWINSZ
	if (joe_ioctl(fileno(termout), TIOCGWINSZ, &getit) != -1) {
		*x = getit.ws_col;
		*y = getit.ws_row;
	}
#endif
#endif
}

/* Set window size */

void ttstsz(int fd, ptrdiff_t w, ptrdiff_t h)
{
#ifdef TIOCSSIZE
	struct ttysize getit;
	getit.ts_cols = w;
	getit.ts_lines = h;
	joe_ioctl(fd, TIOCSSIZE, &getit);
#else
#ifdef TIOCSWINSZ
	struct winsize getit;
	getit.ws_col = (unsigned short)w;
	getit.ws_row = (unsigned short)h;
	joe_ioctl(fd, TIOCSWINSZ, &getit);
#endif
#endif
}

int ttshell(char *cmd)
{
	int x, omode = ttymode;
	int stat= -1;
	const char *s = getenv("SHELL");

	if (!s) {
		s = "/bin/sh";
		/* return; */
	}
	ttclsn();
#ifdef HAVE_FORK
	if ((x = fork()) != 0) {
#else
	if ((x = vfork()) != 0) { /* For AMIGA only  */
#endif
		if (x != -1)
			wait(&stat);
		if (omode)
			ttopnn();
		return stat;
	} else {
		signrm();
		if (cmd)
			execl(s, s, "-c", cmd, NULL);
		else {
			fputs(joe_gettext(_("You are at the command shell.  Type 'exit' to return\n")), stderr);
			execl(s, s, NULL);
		}
		_exit(0);
		return 0;
	}
}

/* Create keyboard task */

static int mpxresume(void)
{
	int fds[2];
	if (-1 == pipe(fds))
		return -1;
	acceptch = NO_MORE_DATA;
	have = 0;
	if (!(kbdpid = fork())) {
		close(fds[1]);
		do {
			char c;
			ptrdiff_t sta;

			pack.who = 0;
			sta = joe_read(fileno(termin), &c, 1);
			if (sta == 0)
				pack.ch = NO_MORE_DATA;
			else
				pack.ch = c;
			pack.size = 0;
			joe_write(mpxsfd, &pack, pack.data - (char *)&pack);
		} while (joe_read(fds[0], &pack, 1) == 1);
		_exit(0);
	}
	close(fds[0]);
	ackkbd = fds[1];
	return 0;
}

/* Kill keyboard task */

static void mpxsusp(void)
{
	if (ackkbd!=-1) {
		kill(kbdpid, 9);
		while (wait(NULL) < 0 && errno == EINTR)
			/* do nothing */;
		close(ackkbd);
	}
}

/* We used to leave the keyboard copy task around during suspend, but
   Cygwin gets confused when two processes are waiting for input and you
   change the tty from raw to cooked (on the call to ttopnn()): the keyboard
   process was stuck in cooked until he got a carriage return- then he
   switched back to raw (he's supposed to switch to raw without waiting for
   the end of line). Probably this should be done for ttshell() as well. */

void ttsusp(void)
{
	int omode;

#ifdef SIGTSTP
	omode = ttymode;
	mpxsusp();
	ttclsn();
	fputs(joe_gettext(_("You have suspended the program.  Type 'fg' to return\n")), stderr);
	kill(0, SIGTSTP);
#ifdef junk
	/* Hmmm... this should not have been necessary */
	if (ackkbd != -1)
		kill(kbdpid, SIGCONT);
#endif
	if (omode)
		ttopnn();
	if (ackkbd!= -1)
		mpxresume();
#else
	ttshell(NULL);
#endif
}

/* Stuff for asynchronous I/O multiplexing.  We do not use streams or
   select() because joe needs to work on versions of UNIX which predate
   these calls.  Instead, when there is multiple async sources, we use
   helper processes which packetize data from the sources.  A header on each
   packet indicates the source.  There is no guarentee that packets getting
   written to the same pipe don't get interleaved, but you can reasonable
   rely on it with small packets. */

static int mpxstart(void)
{
	int fds[2];
	if (-1 == pipe(fds))
		return -1;
	mpxfd = fds[0];
	mpxsfd = fds[1];
	return mpxresume();
}

static void mpxend(void)
{
	mpxsusp();
	ackkbd = -1;
	close(mpxfd);
	close(mpxsfd);
	if (have)
		havec = (char)pack.ch;
}

/* Get a pty/tty pair.  Returns open pty in 'ptyfd' and returns tty name
 * string in static buffer or NULL if couldn't get a pair.
 */

#ifdef __svr4__
#define USEPTMX 1
#else
#ifdef __CYGWIN__
#define USEPTMX 1
#endif
#endif

#ifdef sgi

/* Newer sgi machines can do it the __svr4__ way, but old ones can't */

extern char *_getpty();

static char *getpty(int *ptyfd, int *ttyfd)
{
	return _getpty(ptyfd, O_RDWR, 0600, 0);
}

#else
#ifdef USEPTMX

/* Strange streams way */

extern char *ptsname();

static char *getpty(int *ptyfd, int *ttyfd)
{
	int fdm;
	*ptyfd = fdm = open("/dev/ptmx", O_RDWR);
	grantpt(fdm);
	unlockpt(fdm);
	return ptsname(fdm);
}

#else
#ifdef HAVE_OPENPTY

/* BSD function, present in libc5 and glibc2 */

static char *getpty(int *ptyfd, int *ttyfd)
{
	static char name[32];

	if (openpty(ptyfd, ttyfd, name, NULL, NULL) == 0) {
        	return(name);
        } else {
        	return (NULL);
	}
}

#else
/* The normal way: for each possible pty/tty pair, try to open the pty and
 * then the corresponding tty.  If both could be opened, close them both and
 * then re-open the pty.  If that succeeded, return with the opened pty and the
 * name of the tty.
 *
 * Logically you should only have to succeed in opening the pty- but the
 * permissions may be set wrong on the tty, so we have to try that too.
 * We close them both and re-open the pty because we want the forked process
 * to open the tty- that way it gets to be the controlling tty for that
 * process and the process gets to be the session leader.
 */

static char *getpty(int *ptyfd, int *ttyfd)
{
	int x, fd;
	char *orgpwd = pwd();
	static char **ptys = NULL;
	static char *ttydir;
	static char *ptydir;
	static char ttyname[32];

	if (!ptys) {
		ttydir = "/dev/pty/";
		ptydir = "/dev/ptym/";	/* HPUX systems */
		if (chpwd(ptydir) || !(ptys = rexpnd("pty*")))
			if (!ptys) {
				ttydir = ptydir = "/dev/";	/* Everyone else */
				if (!chpwd(ptydir))
					ptys = rexpnd("pty*");
			}
	}
	chpwd(orgpwd);

	if (ptys)
		for (fd = 0; ptys[fd]; ++fd) {
			zlcpy(ttyname, SIZEOF(ttyname), ptydir);
			zlcat(ttyname, SIZEOF(ttyname), ptys[fd]);
			if ((*ptyfd = open(ttyname, O_RDWR)) >= 0) {
				ptys[fd][0] = 't';
				zlcpy(ttyname, SIZEOF(ttyname), ttydir);
				zlcat(ttyname, SIZEOF(ttyname), ptys[fd]);
				ptys[fd][0] = 'p';
				x = open(ttyname, O_RDWR);
				if (x >= 0) {
					close(x);
					close(*ptyfd);
					zlcpy(ttyname, SIZEOF(ttyname), ptydir);
					zlcat(ttyname, SIZEOF(ttyname), ptys[fd]);
					*ptyfd = open(ttyname, O_RDWR);
					ptys[fd][0] = 't';
					zlcpy(ttyname, SIZEOF(ttyname), ttydir);
					zlcat(ttyname, SIZEOF(ttyname), ptys[fd]);
					ptys[fd][0] = 'p';
					return ttyname;
				} else
					close(*ptyfd);
			}
		}
	return NULL;
}

#endif
#endif
#endif

/* Shell dies signal handler.  Puts pty in non-block mode so
 * that read returns with <1 when all data from process has
 * been read. */
int dead = 0;
int death_fd;
static RETSIGTYPE death(int unused)
{
	fcntl(death_fd,F_SETFL,O_NDELAY);
	wait(NULL);
	dead = 1;
}

#ifndef SIGCHLD
#define SIGCHLD SIGCLD
#endif

/* Build a new environment, but replace one variable */

static const char **newenv(const char * const *old, const char *s)
{
	const char **newv;
	int x, y, z;

	for (x = 0; old[x]; ++x) ;
	newv = (const char **) joe_malloc((x + 2) * SIZEOF(char *));

	for (x = 0, y = 0; old[x]; ++x) {
		for (z = 0; s[z] != '='; ++z)
			if (s[z] != old[x][z])
				break;
		if (s[z] == '=') {
			if (s[z + 1])
				newv[y++] = s;
		} else
			newv[y++] = old[x];
	}
	if (x == y)
		newv[y++] = s;
	newv[y] = 0;
	return newv;
}

/* Create a shell process */

/* If out_only is set, leave program's stdin attached to JOE's stdin */

MPX *mpxmk(int *ptyfd, const char *cmd, char **args, void (*func)(void *object, char *data, ptrdiff_t len), void *object, void (*die) (void *object), void *dieobj, int out_only,
           ptrdiff_t w, ptrdiff_t h)
{
	char buf[80];
	int fds[2];
	int comm[2];
	pid_t pid;
	int x;
	MPX *m = 0;
	char *name;
	int ttyfd = -1;

	/* Find free slot */
	for (x = 0; x != NPROC; ++x)
		if (!asyncs[x].func) {
			m = asyncs + x;
			break;
		}
	if (x==NPROC)
		return NULL;

	/* Get pty/tty pair */
	if (!(name = getpty(ptyfd, &ttyfd)))
		return NULL;

	/* Fixes cygwin console bug: if you fork() with inverse video he assumes you want
	 * ESC [ 0 m to keep it in inverse video from then on. */
	set_attr(maint->t,0);

	/* Flush output */
	ttflsh();

	/* Start input multiplexer */
	if (ackkbd == -1)
		if (mpxstart())
			return NULL;

	/* Remember callback function */
	m->func = func;
	m->object = object;
	m->die = die;
	m->dieobj = dieobj;

	/* Acknowledgement pipe */
	if (-1 == pipe(fds))
		return NULL;
	m->ackfd = fds[1];

	/* PID number pipe */
	if (-1 == pipe(comm))
		return NULL;

	/* Bump no. current async inputs to joe */
	++nmpx;


	/* Create processes... */
	if (!(m->kpid = fork())) {
		/* This process copies data from shell to joe */
		/* After each packet it sends to joe it waits for
		   an acknowledgement from joe so that it can not get
		   too far ahead with buffering */

		/* Close joe side of pipes */
		close(fds[1]);
		close(comm[0]);

		/* Flag which indicates child died */
		dead = 0;
		death_fd = *ptyfd;
		joe_set_signal(SIGCHLD, death);

		if (!(pid = fork())) {
			/* This process becomes the shell */
			signrm();

			/* Close pty (we only need tty) */
			close(*ptyfd);

			/* All of this stuff is for disassociating ourself from
			   controlling tty (session leader) and starting a new
			   session.  This is the most non-portable part of UNIX- second
			   only to pty/tty pair creation. */
#ifndef HAVE_LOGIN_TTY

#ifdef TIOCNOTTY
			x = open("/dev/tty", O_RDWR);
			joe_ioctl(x, TIOCNOTTY, 0);
#endif

			setsid();	/* I think you do setprgp(0,0) on systems with no setsid() */
#ifndef SETPGRP_VOID
			setpgrp(0, 0);
#else
			setpgrp();
#endif

#endif

			/* Open the TTY (if we didn't already get it from openpty() */
			if (ttyfd == -1)
				ttyfd = open(name, O_RDWR);

			if (ttyfd != -1) {
				const char **enva;
				const char **env;

				if (!out_only) {			/* Standard input */
					dup2(ttyfd, 0);
				}
				dup2(ttyfd, 1);
				dup2(ttyfd, 2);
				/* (yes, stdin, stdout, and stderr must all be open for reading and
				 * writing.  On some systems the shell assumes this */

				for (x = 2; x != 32; ++x)
					close(x);/* Yes, this is quite a kludge... all in the name of portability */

				if (w == -1)
					enva = newenv(mainenv, "TERM=");
				else
					enva = newenv(mainenv, "TERM=linux");
				env = newenv(enva, "JOE=1");


				if (!out_only) {
#ifdef HAVE_LOGIN_TTY
					login_tty(1);

#else
				/* This tells the fd that it's a tty (I think) */
#ifdef __svr4__
					joe_ioctl(1, I_PUSH, "ptem");
					joe_ioctl(1, I_PUSH, "ldterm");
#endif

#endif

#ifdef HAVE_POSIX_TERMIOS
					tcsetattr(1, TCSADRAIN, &oldterm);
#else
#ifdef HAVE_SYSV_TERMIO
					joe_ioctl(1, TCSETAW, &oldterm);
#else
					joe_ioctl(1, TIOCSETN, &oarg);
					joe_ioctl(1, TIOCSETC, &otarg);
					joe_ioctl(1, TIOCSLTC, &oltarg);
#endif
#endif
					/* We could probably have a special TTY set-up for JOE, but for now
					 * we'll just use the TTY setup for the TTY was was run on */
					 if (w != -1)
					         ttstsz(1, w, h);

					/* Execute the shell */
					execve(cmd, args, (char * const *)env);

					/* If shell didn't execute */
					joe_snprintf_1(buf,SIZEOF(buf),joe_gettext(_("Couldn't execute shell '%s'\n")),cmd);
					if (-1 == joe_write(1, buf, zlen(buf)))
						sleep(2);
					else
						sleep(1);

				} else {
					char ibuf[1024];
					ptrdiff_t len;
					for (;;) {
						len = read(0, ibuf, SIZEOF(ibuf));
						if (len > 0) {
							if (-1 == write(1, ibuf, (size_t)len))
								break;
						} else
							break;
					}
				}


			}

			_exit(0);
		}

		/* Tell JOE PID of shell */
		joe_write(comm[1], &pid, SIZEOF(pid));

		/* sigpipe should be ignored here. */

		/* This process copies data from shell to JOE until EOF.  It creates a packet
		   for each data */


		/* We don't really get EOF from a pty- it would just wait forever
		   until someone else writes to the tty.  So: when the shell
		   dies, the child died signal handler death() puts pty in non-block
		   mode.  This allows us to read any remaining data- then
		   read returns 0 and we know we're done. */

	      loop:
		pack.who = m;
		pack.ch = 0;

		/* Read data from process */
		pack.size = joe_read(*ptyfd, pack.data, SIZEOF(pack.data));

		/* On SUNOS 5.8, the very first read from the pty returns 0 for some reason */
		if (!pack.size)
			pack.size = joe_read(*ptyfd, pack.data, SIZEOF(pack.data));

		if (pack.size > 0) {
			/* Send data to JOE, wait for ack */
			joe_write(mpxsfd, &pack, pack.data - (char *)&pack + pack.size);

			joe_read(fds[0], &pack, 1);
			goto loop;
		} else {
			/* Shell died: return */
			pack.ch = NO_MORE_DATA;
			pack.size = 0;
			joe_write(mpxsfd, &pack, pack.data - (char *)&pack);

			_exit(0);
		}
	}
	joe_read(comm[0], &m->pid, SIZEOF(m->pid));

	/* Close tty side of pty (if we used openpty) */
	if (-1 != ttyfd)
		close(ttyfd);

	/* We only need comm once */
	close(comm[0]);
	close(comm[1]);

	/* Close other side of copy process pipe */
	close(fds[0]);
	return m;
}

void mpxdied(MPX *m)
{
	if (!--nmpx)
		mpxend();
	while (wait(NULL) < 0 && errno == EINTR)
		/* do nothing */;
	if (m->die)
		m->die(m->dieobj);
	m->func = NULL;
	close(m->ackfd);
	edupd(1);
}
