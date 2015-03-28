/*
 *	TTY interface header file
 *	Copyright
 *		(C) 1992 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */
#ifndef _JOE_TTY_H
#define _JOE_TTY_H 1

struct mpx {
	int	ackfd;		/* Packetizer response descriptor */
	int	kpid;		/* Packetizer process id */
	int	pid;		/* Client process id */
	void	(*func) ();	/* Function to call when read occures */
	void	*object;	/* First arg to pass to function */
	void	(*die) ();	/* Function: call when client dies or closes */
	void	*dieobj;
};

/* void ttopen(void);  Open the tty (attached to stdin) for use inside of JOE
 *
 * (0) Call sigjoe()
 *     There is also 'void ttopnn(void)' which does not do this step.
 *
 * (1) fflush(stdout)
 *
 * (2) Save the current state of the tty
 *
 * (3) Disable CR/LF/NL input translations,
 *     Disable all output processing,
 *     Disable echo and line editing, and
 *     Place tty in character at a time mode.
 *     (basically, disable all processing except for XON/XOFF if it's set)
 *
 * (4) Set this new tty state without losing any typeahead (by using the
 *     proper ioctl).
 *
 * (5) Store the baud rate in the global variable 'baud'
 *
 * (6) Divide the baud rate into the constant DIVIDEND and store the result
 *     in the global variable 'upc'.  This should come out to the number
 *     of microseconds needed to send each character.  The constant 'DIVIDEND'
 *     should be chosen so that 'upc' reflects the real throughput of the
 *     tty, not the theoretical best throughput.
 *
 * (7) Create an output buffer of a size which depends on 'upc' and the
 *     constant 'TIMES'.  'TIMES' is the number of times per second JOE
 *     should check for typeahead.  Since we only check for typehead after
 *     the output buffer is flushed, 'upc' and the size of the output buffer
 *     determine how often this occurs.  So for example if 'upc'==1000 (~9600
 *     baud) and 'TIMES'==3, the output buffer size is set to 333 characters.
 *     Each time this buffer is completely flushed, 1/3 of a second will go by.
 */
void ttopen PARAMS((void));
void ttopnn PARAMS((void));
extern unsigned long upc; /* Microseconds per character */
extern unsigned baud; /* Baud rate */

#define TIMES 3
#define DIVIDEND 10000000

/* void ttclose(void);  Restore the tty back to its original mode.
 *
 * (1) ttyflsh()
 *
 * (2) Restore the original tty mode which aopen() had saved.  Do this without
 *     losing any typeahead.
 *
 * (3) Call signrm().  There is also 'void ttyclsn(void)' which does not do
 *     the this step.
 */
void ttclose PARAMS((void));
void ttclsn PARAMS((void));

/* int ttgetc(void);  Flush the output and get the next character from the tty
 *
 * (1) ttflsh()
 *
 * (2) Read the next input character
 *     If the input closed, call 'ttsig' with 0 as its argument.
 *
 * (3) Clear 'have'
 */
int ttgetc PARAMS((void));

int ttcheck PARAMS((void));

/* void ttputc(char c);  Write a character to the output buffer.  If it becomes
 * full, call ttflsh()
 */
extern int obufp; /* Output buffer index */
extern int obufsiz; /* Output buffer size */
extern unsigned char *obuf; /* Output buffer */

#define ttputc(c) { obuf[obufp++] = (c); if(obufp == obufsiz) ttflsh(); }

/* void ttputs(char *s);  Write a string to the output buffer.  Any time the
 * output buffer gets full, call ttflsh()
 */
void ttputs PARAMS((unsigned char *s));

/* int ttshell(char *s);  Run a shell command or if 's' is zero, run a
 * sub-shell
 */
int ttshell PARAMS((unsigned char *cmd));

/* void ttsusp(void);  Suspend the process, or if the UNIX can't do it, call
 * ttshell(NULL)
 */
void ttsusp PARAMS((void));

/* int ttflsh(void);  Flush the output buffer and check for typeahead.
 *
 * (1) write() any characters in the output buffer to the tty and then sleep
 *     for the amount of time it should take for the written characters to get
 *     to the tty.  This is so that any buffering between the editor and the
 *     tty is defeated.  If this is not done, the screen update will not be
 *     able to defer for typeahead.
 *
 *     The best way to do the sleep (possible only on systems with the
 *     setitimer call) is to set a timer for the necessary amount, write the
 *     characters to the tty, and then sleep until the timer expires.
 *
 *     If this can't be done, it's usually ok to 'write' and then to sleep for
 *     the necessary amount of time.  However, you will notice delays in the
 *     screen update if the 'write' actually takes any significant amount of
 *     time to execute (it usually takes none since all it usually does is
 *     write to an operating system output buffer).
 *
 * (2) The way we check for typeahead is to put the TTY in nonblocking mode
 *     and attempt to read a character.  If one could be read, the global
 *     variable 'have' is set to indicate that there is typeahead pending and
 *     the character is stored in a single character buffer until ttgetc
 *     is called.  If the global variable 'leave' is set, the check for
 *     typeahead is disabled.  This is so that once the program knows that it's
 *     about to exit, it doesn't eat the first character of your typeahead if
 *     ttflsh gets called.  'leave' should also be set before shell escapes and
 *     suspends.
 */
int ttflsh PARAMS((void));

extern int have; /* Set if we have typeahead */
extern unsigned char havec; /* typeahead character */
extern int leave; /* Set if we're exiting (so don't check for typeahead) */

#ifdef __MSDOS__
#define ifhave bioskey(1)
#else
#define ifhave have
#endif

/* void ttsig(int n);  Signal handler you provide.  This is called if the
 * editor gets a hangup signal, termination signal or if the input closes.
 * It is called with 'n' set to the number of the caught signal or 0 if the
 * input closed.
 */
void ttsig PARAMS((int sig));

/* void ttgtsz(int *x,int *y);  Get size of screen from ttsize/winsize
 * structure */
void ttgtsz PARAMS((int *x, int *y));

/* You don't have to call these: ttopen/ttclose does it for you.  These
 * may be needed to make your own shell escape sequences.
 */

/* void sigjoe(void);  Set the signal handling for joe.  I.E., ignore all
 * signals the user can generate from the keyboard (SIGINT, SIGPIPE)
 * and trap the software terminate and hangup signals (SIGTERM, SIGHUP) so
 * that 'ttsig' gets called.
 */
void sigjoe PARAMS((void));

/* void signrm(void);  Set above signals back to their default values.
 */
void signrm PARAMS((void));

/* MPX *mpxmk(int fd,int pid,
 *             void (*func)(),void *object,
 *             void (*die)(),void *dieobj,
 *            );
 *
 * Create an asynchronous input source handler for a process
 *   Child process id in 'pid'
 *   File descriptor to get input from in 'fd'
 *   Function to call with received characters in 'func'
 *   Function to call when process dies in 'die'
 *   The first arg passed to func and die is object and dieobj
 */
MPX *mpxmk PARAMS((int *ptyfd, unsigned char *cmd, unsigned char **args, void (*func) (/* ??? */), void *object, void (*die) (/* ??? */), void *dieobj, int out_only, int w, int h));

/* int subshell(int *ptyfd);
 * Execute a subshell.  Returns 'pid' of shell or zero if there was a
 * problem.  Returns file descriptor for the connected pty in 'ptyfd'.
 */
int subshell PARAMS(());

extern int noxon;			/* Set if ^S/^Q processing should be disabled */
extern int Baud;			/* Baud rate from joerc, cmd line or environment */

void tickoff PARAMS((void));
void tickon PARAMS((void));

extern long last_time; /* Current time in seconds */
extern int idleout; /* Clear to use /dev/tty for screen */

void ttstsz(int fd, int w, int h); /* Set window size */
extern int nodeadjoe; /* Flag to prevent creation of DEADJOE files */

#endif
