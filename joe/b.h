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

B *bmk(B *prop);
void brm(B *b);
void brmall();

B *bfind(unsigned char *s);
B *bfind_scratch(unsigned char *s);
B *bcheck_loaded(unsigned char *s);
B *bfind_reload(unsigned char *s);

P *pdup(P *p, unsigned char *tr);
P *pdupown(P *p, P **o, unsigned char *tr);
P *poffline(P *p);
P *ponline(P *p);
B *bonline(B *b);
B *boffline(B *b);

void prm(P *p);
P *pset(P *n, P *p);

P *p_goto_bof(P *p);		/* move cursor to begging of file */
P *p_goto_eof(P *p);		/* move cursor to end of file */
P *p_goto_bol(P *p);		/* move cursor to begging of line */
P *p_goto_eol(P *p);		/* move cursor to end of line */

P *p_goto_indent(P *p,int c);	/* move cursor to indentation point */

int pisbof(P *p);
int piseof(P *p);
int piseol(P *p);
int pisbol(P *p);
int pisbow(P *p);
int piseow(P *p);

#define piscol(p) ((p)->valcol ? (p)->col : (pfcol(p), (p)->col))

int pisblank(P *p);
int piseolblank(P *p);

long pisindent(P *p);
int pispure(P *p,int c);

int pnext(P *p);
int pprev(P *p);

int pgetb(P *p);
int prgetb(P *p);

int pgetc(P *p);
int prgetc(P *p);

P *pgoto(P *p, long int loc);
P *pfwrd(P *p, long int n);
P *pbkwd(P *p, long int n);

P *pfcol(P *p);

P *pnextl(P *p);
P *pprevl(P *p);

P *pline(P *p, long int line);

P *pcolwse(P *p, long int goalcol);
P *pcol(P *p, long int goalcol);
P *pcoli(P *p, long int goalcol);
void pbackws(P *p);
void pfill(P *p, long int to, int usetabs);

P *pfind(P *p, unsigned char *s, int len);
P *pifind(P *p, unsigned char *s, int len);
P *prfind(P *p, unsigned char *s, int len);
P *prifind(P *p, unsigned char *s, int len);

/* copy text between 'from' and 'to' into new buffer */
B *bcpy(P *from, P *to);	

void pcoalesce(P *p);

void bdel(P *from, P *to);

/* insert buffer 'b' into another at 'p' */
P *binsb(P *p, B *b);
/* insert a block 'blk' of size 'amnt' into buffer at 'p' */
P *binsm(P *p, unsigned char *blk, int amnt); 

/* insert character 'c' into buffer at 'p' */
P *binsc(P *p, int c);

/* insert byte 'c' into buffer at at 'p' */
P *binsbyte(P *p, unsigned char c);

/* insert zero term. string 's' into buffer at 'p' */
P *binss(P *p, unsigned char *s);

/* B *bload(char *s);
 * Load a file into a new buffer
 *
 * Returns with errno set to 0 for success,
 * -1 for new file (file doesn't exist)
 * -2 for read error
 * -3 for seek error
 * -4 for open error
 */
B *bload(unsigned char *s);
B *bread(int fi, long int max);
B *bfind(unsigned char *s);
B *borphan(void);

/* Save 'size' bytes beginning at 'p' into file with name in 's' */
int bsave(P *p, unsigned char *s, off_t size,int flag);
int bsavefd(P *p, int fd, off_t size);

unsigned char *parsens(unsigned char *s, off_t *skip, off_t *amnt);
unsigned char *canonical(unsigned char *s);

/* Get byte at pointer or return NO_MORE_DATA if pointer is at end of buffer */
int brc(P *p);

/* Get character at pointer or return NO_MORE_DATA if pointer is at end of buffer */
int brch(P *p);

/* Copy 'size' bytes from a buffer beginning at p into block 'blk' */
unsigned char *brmem(P *p, unsigned char *blk, int size);

/* Copy 'size' bytes from a buffer beginning at p into a zero-terminated
 * C-string in an malloc block.
 */
unsigned char *brs(P *p, int size);

/* Copy 'size' bytes from a buffer beginning at p into a variable length string. */
unsigned char *brvs(P *p, int size);

/* Copy line into buffer.  Maximum of size bytes will be copied.  Buffer needs
   to be one bigger for NIL */
unsigned char *brzs(P *p, unsigned char *buf, int size);

B *bnext(void);
B *bafter(B *b);
B *bprev(void);

extern int berror;	/* bload error status code (use msgs[-berror] to get message) */

unsigned char **getbufs(void);

int lock_it(unsigned char *path,unsigned char *buf);
void unlock_it(unsigned char *path);
int plain_file(B *b);
int check_mod(B *b);
int file_exists(unsigned char *path);

int udebug_joe(BW *bw);

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
