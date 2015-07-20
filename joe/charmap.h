/*
 *	Character sets
 *	Copyright
 *		(C) 2004 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */

/* For sorted from_map entries */

struct pair {
	int first;			/* Unicode */
	int last;			/* Byte */
};

/* A character set */

struct charmap {
	struct charmap *next;		/* Linked list of loaded character maps */
	const char *name;		/* Name of this one */

	int type;			/* 0=byte, 1=UTF-8 */

	/* Character predicate functions */

	int (*is_punct)(struct charmap *map,int c);
	int (*is_print)(struct charmap *map,int c);
	int (*is_space)(struct charmap *map,int c);
	int (*is_alpha_)(struct charmap *map,int c);
	int (*is_alnum_)(struct charmap *map,int c);

	/* Character conversion functions */

	int (*to_lower)(struct charmap *map,int c);
	int (*to_upper)(struct charmap *map,int c);
	int (*to_uni)(struct charmap *map,int c);
	int (*from_uni)(struct charmap *map,int c);

	/* Information for byte-oriented character sets */

	int *to_map;			/* Convert byte to unicode */

	char lower_map[256];	/* Convert to lower case */
	char upper_map[256];

	struct pair from_map[256];	/* Convert from unicode to byte */

	ptrdiff_t from_size;			/* No. paris in from_map */

	char print_map[32];	/* Bit map of printable characters */
	char alpha__map[32];	/* Bit map of alphabetic characters and _ */
	char alnum__map[32];	/* Bit map of alphanumeric characters and _ */
};

/* Predicates */

#define joe_ispunct(map,c) ((map)->is_punct((map),(c)))
#define joe_isprint(map,c) ((map)->is_print((map),(c)))
#define joe_isspace(map,c) ((map)->is_space((map),(c)))
#define joe_isalpha_(map,c) ((map)->is_alpha_((map),(c)))
#define joe_isalnum_(map,c) ((map)->is_alnum_((map),(c)))
int joe_isblank(struct charmap *map,int c);
int joe_isspace_eof(struct charmap *map,int c);

/* Conversion functions */

#define joe_tolower(map,c) ((map)->to_lower((map),(c)))
#define joe_toupper(map,c) ((map)->to_upper((map),(c)))
#define joe_to_uni(map,c) ((map)->to_uni((map),(c)))
#define joe_from_uni(map,c) ((map)->from_uni((map),(c)))

/* Find (load if necessary) a character set */
struct charmap *find_charmap(const char *name);

/* Get available encodings */
char **get_encodings(void);

int from_uni(struct charmap *cset, int c);
int to_uni(struct charmap *cset, int c);
