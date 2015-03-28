/*
 *	Editor engine
 *	Copyright
 *		(C) 1992 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */
#ifndef _JOE_B_H
#define _JOE_B_H 1

/* A buffer is made up of a doubly-linked list of gap buffer.  These are the
 * buffer headers.  The buffers themselves can be swapped out.  A buffer with
 * point referring to it is guaranteed to be swapped in.
 */

struct header {
	LINK(H)	link;		/* Doubly-linked list of gap buffer headers */
	long	seg;		/* Swap file offset to gap buffer */
	int	hole;		/* Offset to gap */
	int	ehole;		/* Offset to after gap */
	int	nlines;		/* No. '\n's in this buffer */
};

/* A pointer to some location within a buffer.  After an insert or delete,
 * all of the pointers following the insertion or deletion point are
 * adjusted so that they keep pointing to the same character. */

struct point {
	LINK(P)	link;		/* Doubly-linked list of pointers for a particular buffer */

	B	*b;		/* Buffer */
	int	ofst;		/* Gap buffer offset */
	unsigned char	*ptr;	/* Gap buffer address */
	H	*hdr;		/* Gap buffer header */

	off_t	byte;		/* Buffer byte offset */
	long	line;		/* Line number */
	long	col;		/* current column */
	long	xcol;		/* cursor column (can be different from actual column) */
	int	valcol;		/* bool: is col valid? */
	int	end;		/* set if this is end of file pointer */
	int	attr;		/* current ansi attribute */
	int	valattr;	/* set if attr is still valid */

	P	**owner;	/* owner of this pointer.  owner gets cleared if pointer is deleted. */
	unsigned char *tracker;	/* Name of function who pdup()ed me */
};

/* Options: both BWs and Bs have one of these */

struct options {
	OPTIONS	*next;
	unsigned char	*name_regex;
	unsigned char	*contents_regex;
	int	overtype;
	int	lmargin;
	int	rmargin;
	int	autoindent;
	int	wordwrap;
	int	nobackup;
	int	tab;
	int	indentc;
	int	istep;
	unsigned char	*context;
	unsigned char	*lmsg;
	unsigned char	*rmsg;
	unsigned char	*smsg;
	unsigned char	*zmsg;
	int	linums;
	int	readonly;
	int	french;
	int	flowed;
	int	spaces;
	int	crlf;
	int	highlight;	/* Set to enable highlighting */
	unsigned char *syntax_name;	/* Name of syntax to use */
	struct high_syntax *syntax;	/* Syntax for highlighting (load_syntax() from syntax_name happens in setopt()) */
	unsigned char *map_name;	/* Name of character set */
	struct charmap *charmap;	/* Character set */
	unsigned char *language;	/* Language of this buffer (for spell) */
	int	smarthome;	/* Set for smart home key */
	int	indentfirst;	/* Smart home goes to indentation point first */
	int	smartbacks;	/* Set for smart backspace key */
	int	purify;		/* Purify indentation */
	int	picture;	/* Picture mode */
	int	highlighter_context;	/* Use the context annotations from the highlighter for ^G */
	int	single_quoted;	/* Ignore '  ' for ^G */
	int	no_double_quoted;	/* Don't ignore " " for ^G */
	int	c_comment;	/* Ignore text in C comments */
	int	cpp_comment;	/* Ignore text after // comments */
	int	pound_comment;	/* Ignore text after # comments */
	int	vhdl_comment;	/* Ignore text after -- comments */
	int	semi_comment;	/* Ignore text after ; comments */
	int	tex_comment;	/* Ignore text after % comments */
	int	hex;		/* Hex edit mode */
	int	ansi;		/* Hide ansi sequences mode */
	unsigned char *text_delimiters;	/* Define word delimiters */
	unsigned char *cpara;	/* Characters which can indent paragraphcs */
	unsigned char *cnotpara;/* Characters which begin non-paragraph lines */
	MACRO	*mnew;		/* Macro to execute for new files */
	MACRO	*mold;		/* Macro to execute for existing files */
	MACRO	*msnew;		/* Macro to execute before saving new files */
	MACRO	*msold;		/* Macro to execute before saving existing files */
	MACRO	*mfirst;	/* Macro to execute on first change */
};

/* A buffer */

struct buffer {
	LINK(B)	link;		/* Doubly-linked list of all buffers */
	P	*bof;		/* Beginning of file pointer */
	P	*eof;		/* End of file pointer */
	unsigned char	*name;	/* File name */
	int locked;		/* Set if we created a lock for this file */
	int ignored_lock;	/* Set if we didn't create a lock and we don't care (locked set in this case) */
	int didfirst;		/* Set after user attempted first change */
	long    mod_time;	/* Last modification time for file */
	long	check_time;	/* Last time we checked the file on disk */
	int	gave_notice;	/* Set if we already gave file changed notice for this file */
	int	orphan;		/* Set if buffer is orphaned: refcount is bumped up by one in this case */
	int	count;		/* Reference count.  Buffer is deleted if brm decrements count to 0 */
	int	changed;
	int	backup;
	void	*undo;
	P	*marks[11];	/* Bookmarks */
	OPTIONS	o;		/* Options */
	P	*oldcur;	/* Last cursor position before orphaning */
	P	*oldtop;	/* Last top screen position before orphaning */
	P	*err;		/* Last error line */
	unsigned char *current_dir;
	int shell_flag;		/* Set if last cursor position is same as vt cursor: if it is we keep it up to date */
	int	rdonly;		/* Set for read-only */
	int	internal;	/* Set for internal buffers */
	int	scratch;	/* Set for scratch buffers */
	int	er;		/* Error code when file was loaded */
	pid_t	pid;		/* Process id */
	int	out;		/* fd to write to process */
	VT	*vt;		/* video terminal emulator */
	struct lattr_db *db;	/* Linked list of line attribute databases */
	void (*parseone)(struct charmap *map,unsigned char *s,unsigned char **rtn_name,
	                 long *rtn_line);
	                        /* Error parser for this buffer */
};

extern B bufs;

/* 31744 */
extern unsigned char stdbuf[stdsiz];	/* Convenient global buffer */

extern int force;		/* Set to have final '\n' added to file */
extern int tabwidth;		/* Default tab width */

extern VFILE *vmem;		/* Virtual memory file used for buffer system */

extern unsigned char *msgs[];	/* File access status messages */

B *bmk PARAMS((B *prop));
void brm PARAMS((B *b));
void brmall();

B *bfind PARAMS((unsigned char *s));
B *bfind_scratch PARAMS((unsigned char *s));
B *bcheck_loaded PARAMS((unsigned char *s));
B *bfind_reload PARAMS((unsigned char *s));

P *pdup PARAMS((P *p, unsigned char *tr));
P *pdupown PARAMS((P *p, P **o, unsigned char *tr));
P *poffline PARAMS((P *p));
P *ponline PARAMS((P *p));
B *bonline PARAMS((B *b));
B *boffline PARAMS((B *b));

void prm PARAMS((P *p));
P *pset PARAMS((P *n, P *p));

P *p_goto_bof PARAMS((P *p));		/* move cursor to begging of file */
P *p_goto_eof PARAMS((P *p));		/* move cursor to end of file */
P *p_goto_bol PARAMS((P *p));		/* move cursor to begging of line */
P *p_goto_eol PARAMS((P *p));		/* move cursor to end of line */

P *p_goto_indent PARAMS((P *p,int c));	/* move cursor to indentation point */

int pisbof PARAMS((P *p));
int piseof PARAMS((P *p));
int piseol PARAMS((P *p));
int pisbol PARAMS((P *p));
int pisbow PARAMS((P *p));
int piseow PARAMS((P *p));

#define piscol(p) ((p)->valcol ? (p)->col : (pfcol(p), (p)->col))

int pisblank PARAMS((P *p));
int piseolblank PARAMS((P *p));

long pisindent PARAMS((P *p));
int pispure PARAMS((P *p,int c));

int pnext PARAMS((P *p));
int pprev PARAMS((P *p));

int pgetb PARAMS((P *p));
int prgetb PARAMS((P *p));

int pgetc PARAMS((P *p));
int prgetc PARAMS((P *p));

P *pgoto PARAMS((P *p, long int loc));
P *pfwrd PARAMS((P *p, long int n));
P *pbkwd PARAMS((P *p, long int n));

P *pfcol PARAMS((P *p));

P *pnextl PARAMS((P *p));
P *pprevl PARAMS((P *p));

P *pline PARAMS((P *p, long int line));

P *pcolwse PARAMS((P *p, long int goalcol));
P *pcol PARAMS((P *p, long int goalcol));
P *pcoli PARAMS((P *p, long int goalcol));
void pbackws PARAMS((P *p));
void pfill PARAMS((P *p, long int to, int usetabs));

P *pfind PARAMS((P *p, unsigned char *s, int len));
P *pifind PARAMS((P *p, unsigned char *s, int len));
P *prfind PARAMS((P *p, unsigned char *s, int len));
P *prifind PARAMS((P *p, unsigned char *s, int len));

/* copy text between 'from' and 'to' into new buffer */
B *bcpy PARAMS((P *from, P *to));	

void pcoalesce PARAMS((P *p));

void bdel PARAMS((P *from, P *to));

/* insert buffer 'b' into another at 'p' */
P *binsb PARAMS((P *p, B *b));
/* insert a block 'blk' of size 'amnt' into buffer at 'p' */
P *binsm PARAMS((P *p, unsigned char *blk, int amnt)); 

/* insert character 'c' into buffer at 'p' */
P *binsc PARAMS((P *p, int c));

/* insert byte 'c' into buffer at at 'p' */
P *binsbyte PARAMS((P *p, unsigned char c));

/* insert zero term. string 's' into buffer at 'p' */
P *binss PARAMS((P *p, unsigned char *s));

/* B *bload(char *s);
 * Load a file into a new buffer
 *
 * Returns with errno set to 0 for success,
 * -1 for new file (file doesn't exist)
 * -2 for read error
 * -3 for seek error
 * -4 for open error
 */
B *bload PARAMS((unsigned char *s));
B *bread PARAMS((int fi, long int max));
B *bfind PARAMS((unsigned char *s));
B *borphan PARAMS((void));

/* Save 'size' bytes beginning at 'p' into file with name in 's' */
int bsave PARAMS((P *p, unsigned char *s, off_t size,int flag));
int bsavefd PARAMS((P *p, int fd, off_t size));

unsigned char *parsens PARAMS((unsigned char *s, off_t *skip, off_t *amnt));
unsigned char *canonical PARAMS((unsigned char *s));

/* Get byte at pointer or return NO_MORE_DATA if pointer is at end of buffer */
int brc PARAMS((P *p));

/* Get character at pointer or return NO_MORE_DATA if pointer is at end of buffer */
int brch PARAMS((P *p));

/* Copy 'size' bytes from a buffer beginning at p into block 'blk' */
unsigned char *brmem PARAMS((P *p, unsigned char *blk, int size));

/* Copy 'size' bytes from a buffer beginning at p into a zero-terminated
 * C-string in an malloc block.
 */
unsigned char *brs PARAMS((P *p, int size));

/* Copy 'size' bytes from a buffer beginning at p into a variable length string. */
unsigned char *brvs PARAMS((P *p, int size));

/* Copy line into buffer.  Maximum of size bytes will be copied.  Buffer needs
   to be one bigger for NIL */
unsigned char *brzs PARAMS((P *p, unsigned char *buf, int size));

B *bnext PARAMS((void));
B *bafter PARAMS((B *b));
B *bprev PARAMS((void));

extern int berror;	/* bload error status code (use msgs[-berror] to get message) */

unsigned char **getbufs PARAMS((void));

int lock_it PARAMS((unsigned char *path,unsigned char *buf));
void unlock_it PARAMS((unsigned char *path));
int plain_file PARAMS((B *b));
int check_mod PARAMS((B *b));
int file_exists PARAMS((unsigned char *path));

int udebug_joe PARAMS((BW *bw));

extern int guesscrlf; /* Try to guess line ending when set */
extern int guessindent; /* Try to guess indent character and step when set */
extern int break_links; /* Break hard links on write */
extern int break_symlinks; /* Break symbolic links on write */
extern int nodeadjoe; /* Prevent creation of DEADJOE files */

void set_file_pos_orphaned();

void breplace(B *b, B *n);

unsigned char *dequote(unsigned char *);

#define ANSI_BIT 0x40000000
int ansi_code(unsigned char *s);
unsigned char *ansi_string(int code);

#endif
