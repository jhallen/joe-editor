/*
 *	Syntax highlighting DFA interpreter
 *	Copyright
 *		(C) 2004 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */

/* Color definition */

struct high_color {
	struct high_color *next;
	const char *name;		/* Symbolic name of color */
	int color;			/* Color value */
};

/* State */

struct high_state {
	ptrdiff_t no;			/* State number */
	const char *name;			/* Highlight state name */
	int color;			/* Color for this state */
	/* struct high_cmd *cmd[256]; */	/* Character table */
	struct cmap cmap;		/* Character map (character -> struct high_cmd *) */
	struct interval_list *src;	/* Source for character map */
	struct high_cmd *dflt;		/* Default for no match */
	struct high_cmd *delim;		/* Matching delimiter */
};

/* Parameter list */

struct high_param {
	struct high_param *next;
	char *name;
};

/* Command (transition) */

struct high_cmd {
	unsigned noeat : 1;		/* Set to give this character to next state */
	unsigned start_buffering : 1;	/* Set if we should start buffering */
	unsigned stop_buffering : 1;	/* Set if we should stop buffering */
	unsigned save_c : 1;		/* Save character */
	unsigned save_s : 1;		/* Save string */
	unsigned ignore : 1;		/* Set to ignore case */
	unsigned start_mark : 1;	/* Set to begin marked area including this char */
	unsigned stop_mark : 1;		/* Set to end marked area excluding this char */
	unsigned recolor_mark : 1;	/* Set to recolor marked area with new state */
	unsigned rtn : 1;		/* Set to return */
	unsigned reset : 1;		/* Set to reset the call stack */
	ptrdiff_t recolor;		/* No. chars to recolor if <0. */
	struct high_state *new_state;	/* The new state */
	HASH *keywords;			/* Hash table of keywords */
	struct high_cmd *delim;		/* Matching delimiter */
	struct high_syntax *call;	/* Syntax subroutine to call */
};

/* Call stack frame */

struct high_frame {
	struct high_frame *parent;		/* Caller's frame */
	struct high_frame *child;		/* First callee's frame */
	struct high_frame *sibling;		/* Caller's next callee's frame */
	struct high_syntax *syntax;		/* Current syntax subroutine */
	struct high_state *return_state;	/* Return state in the caller's subroutine */
};

/* Loaded form of syntax file or subroutine */

struct high_syntax {
	struct high_syntax *next;	/* Linked list of loaded syntaxes */
	char *name;			/* Name of this syntax */
	char *subr;			/* Name of the subroutine (or NULL for whole file) */
	struct high_param *params;	/* Parameters defined */
	struct high_state **states;	/* The states of this syntax.  states[0] is idle state */
	HASH *ht_states;		/* Hash table of states */
	ptrdiff_t nstates;		/* No. states */
	ptrdiff_t szstates;		/* Malloc size of states array */
	struct high_color *color;	/* Linked list of color definitions */
	struct high_cmd default_cmd;	/* Default transition for new states */
	struct high_frame *stack_base;  /* Root of run-time call tree */
};

/* Find a syntax.  Load it if necessary. */

struct high_syntax *load_syntax(const char *name);

/* Parse a lines.  Returns new state. */

extern int *attr_buf;
HIGHLIGHT_STATE parse(struct high_syntax *syntax,P *line,HIGHLIGHT_STATE state);

#define clear_state(s) (((s)->saved_s[0] = 0), ((s)->state = 0), ((s)->stack = 0))
#define invalidate_state(s) ((s)->state = -1)
#define move_state(to,from) (*(to)= *(from))
#define eq_state(x,y) ((x)->state == (y)->state && (x)->stack == (y)->stack && !strcmp((x)->saved_s, (y)->saved_s))

extern struct high_color *global_colors;
void parse_color_def(struct high_color **color_list,const char *p,char *name,int line);

void dump_syntax(BW *bw);
