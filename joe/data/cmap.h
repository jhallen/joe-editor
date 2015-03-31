/* Character maps */

/* An interval */

struct interval {
	int first;
	int last;
};

/* An interval list item */

struct interval_list {
	struct interval_list *next;
	void *map;
	int first;
	int last;
};

/* An interval map item */

struct interval_map {
	void *map;
	int first;
	int last;
};

/* A character map */

struct cmap {
	void *direct_map[128];		/* Direct mapping for ASCII range */
	void *dflt_map;			/* Matches when none of the above do */
	int size;			/* No. items in range_map */
	struct interval_map *range_map;	/* Sorted range map */
};


struct interval_list *mkinterval(struct interval_list *next, int first, int last, void *map);

void rminterval(struct interval_list *item);

/* Add a single interval to an interval list */
struct interval_list *interval_add(struct interval_list *interval_list, int first, int last, void *map);

/* Add set of intervals (a character class) to an interval list */
struct interval_list *interval_set(struct interval_list *list, struct interval *array, int size, void *map);

/* Build character map from interval list */
void cmap_build(struct cmap *cmap, struct interval_list *list, void *dflt_map);

/* Look up single character in a character map, return what it's mapped to */
void *cmap_lookup(struct cmap *cmap, int ch);
