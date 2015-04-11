/*
 *	Character maps
 *	Copyright
 *		(C) 1992 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */

/* An interval.  A character matches if it's in the range.
   An array of these can be used to define a character class. */

struct interval {
	int first;
	int last;
};

/* Something bound to a character */

struct bind {
	void *thing;	/* Address of thing */
	int what;	/* What is it? (we could use LSBs for thing for this, but not portable) */
};

/* An interval list item (for lists of interval to bind mappings) */

struct interval_list {
	struct interval_list *next; /* Next item in list */
	struct interval interval; /* Range of characters */
	struct bind map; /* What it's bound to */
};

/* An interval map item (for arrays of interval to bind mappings) */

struct interval_map {
	struct interval interval; /* Range of characters */
	struct bind map; /* What it's bound to */
};

/* A character map */

struct cmap {
	struct bind direct_map[128];	/* Direct mapping for ASCII range */
	int size;			/* No. items in range_map */
	struct interval_map *range_map;	/* Sorted range map */
	struct bind dflt_map;		/* Matches when none of the above do */
};

struct bind mkbinding(void *thing, int what);

struct interval_list *mkinterval(struct interval_list *next, int first, int last, struct bind map);

void rminterval(struct interval_list *item);

/* Add a single interval to an interval list */
struct interval_list *interval_add(struct interval_list *interval_list, int first, int last, struct bind map);

/* Add set of intervals (a character class) to an interval list */
struct interval_list *interval_set(struct interval_list *list, struct interval *array, int size, struct bind map);

/* Look up single character in an interval list, return what it's mapped to */
struct bind interval_lookup(struct interval_list *list, int item);

/* Build character map from interval list */
void cmap_build(struct cmap *cmap, struct interval_list *list, struct bind map);

/* Clear a cmap */
void clr_cmap(struct cmap *cmap);

/* Look up single character in a character map, return what it's mapped to */
struct bind cmap_lookup(struct cmap *cmap, int ch);
