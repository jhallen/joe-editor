#define PHASH_WAYS 4

struct Phash_entry {
	unsigned keys[PHASH_WAYS];
	int vals[PHASH_WAYS];
};

struct Phash {
	unsigned select;	/* Bits to select from input */
	unsigned len;	/* Size of hash table */
	unsigned count;	/* Number of items in table */
	struct Phash_entry *table;	/* Hash table */
};

struct Phash *mkphash(unsigned sel, unsigned len);

void rmphash(struct Phash *h);

int phash_find(struct Phash *h, unsigned key);

void phash_add(struct Phash *h, unsigned key, int val);
