/*
 *	Key-map handler
 *	Copyright
 *		(C) 1992 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */

/* A map of keycode to command/sub-map bindings */
struct kmap {
	struct cmap cmap;	/* Character map: (optimized version of src) */
	struct interval_list *src;	/* Interval list which is source of cmap */
	struct bind dflt;		/* Default binding which is source of cmap */
	int cmap_version;	/* When cmap_version != src_version, we update cmap */
	int src_version;	/* Increment this whenever src is changed */
};

/** A keyboard handler **/
struct kbd {
	KMAP	*curmap;	/* Current keymap */
	KMAP	*topmap;	/* Top-level keymap */
	int	seq[16];	/* Current sequence of keys */
	ptrdiff_t	x;	/* What we're up to */
};

/* KMAP *mkkmap(void);
 * Create an empty keymap
 */
KMAP *mkkmap(void);

/* void rmkmap(KMAP *kmap);
 * Free a key map
 */
void rmkmap(KMAP *kmap);

/* int kadd(KMAP *kmap,char *seq,void *bind);
 * Add a key sequence binding to a key map
 *
 * Returns 0 for success
 *        -1 for for invalid key sequence
 *
 * A valid key sequence is one or more keys seperated with spaces.  A key
 * is a single character or one of the following strings:
 *
 *    ^?	                   127 (DEL)
 *
 *    ^@   -   ^_                  Control characters
 *
 *    SP                           32 (space character)
 *
 *    UP, DOWN, LEFT, RIGHT,
 *    F0 - F10, DEL, INS, HOME,
 *    END, PGUP, PGDN              termcap special characters
 *
 * In addition, the last key of a key sequence may be replaced with
 * a range-fill of the form: <KEY> TO <KEY>
 *
 * So for example, if the sequence: ^K A TO Z
 * is speicified, then the key sequences
 * ^K A, ^K B, ^K C, ... ^K Z are all bound.
 */
int kadd(CAP *cap, KMAP *kmap, char *seq, MACRO *bind);

/* void kcpy(KMAP *dest,KMAP *src);
 * Copy all of the entries in the 'src' keymap into the 'dest' keymap
 */
void kcpy(KMAP *dest, KMAP *src);

/* int kdel(KMAP *kmap,char *seq);
 * Delete a binding from a keymap
 *
 * Returns 0 for success
 *        -1 if the given key sequence was invalid
 *         1 if the given key sequence did not exist
 */
int kdel(KMAP *kmap, char *seq);

/* KBD *mkkbd(KMAP *kmap);
   Create a keyboard handler which uses the given keymap
*/
KBD *mkkbd(KMAP *kmap);

/* void rmkbd(KBD *);
 *
 * Eliminate a keyboard handler
 */
void rmkbd(KBD *k);

/* void *dokey(KBD *kbd,int k);
   Handle a key for a KBD:

     Returns 0 for invalid or prefix keys

     Returns binding for a completed key sequence
*/
MACRO *dokey(KBD *kbd, int n);

/* A list of named KMAPs */
struct context {
	struct context *next;
	char *name;
	KMAP *kmap;
};

/* JM - user command handler */
int ukeymap(W *w, int k);

/* True is KMAP is empty */
int kmap_empty(KMAP *k);

/* KMAP *kmap_getcontext(char *name);
 * Find and return the KMAP for a given context name.  If none is found, an
 * empty kmap is created, bound to the context name, and returned.
 */
KMAP *kmap_getcontext(const char *name);

/* KMAP *ngetcontext(char *name);
 * JM - Find and return the KMAP for a given context name.  If none is found,
 * NULL is returned.
 */
KMAP *ngetcontext(const char *name);
