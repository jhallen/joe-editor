/*
 *	Perfect hash tables which map an unsigned int key to an int.
 *
 *	Copyright
 *		(C) 2015 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */

#include "types.h"

/* sel is initial selector: it's OK to use 0
   len is initial length: it's OK to use 1
 */
struct Phash *mkphash(unsigned sel, unsigned len)
{
	struct Phash *h = (struct Phash *)calloc(1, sizeof(struct Phash));
	h->sel = sel;
	h->len = len;
	h->table = (struct Phash_entry *)calloc(h->len, sizeof(struct Phash_entry));
	return h;
}

void rmphash(struct Phash *h)
{
	free(h->table);
	free(h);
}

/* Select bits in val which are set in sel and compress them all to the right */

unsigned gather(unsigned val,unsigned sel)
{
	unsigned long mk, mp, mv, t;
	int i;
	val &= sel;
	mk = ~sel << 1;
	for (i = 0; i != 5; ++i) {
		mp = mk ^ (mk << 1);
		mp = mp ^ (mp << 2);
		mp = mp ^ (mp << 4);
		mp = mp ^ (mp << 8);
		mp = mp ^ (mp << 16);
		mv = mp & sel;
		sel = sel ^ mv | (mv >> (1 << i));
		t = val & mv;
		val = val ^ t | (t >> (1 << i));
		mk = mk & ~mp;
	}
	return val;
}

int phash_find(struct Phash *h, unsigned key)
{
	unsigned idx = gather(key, h->select);
	struct Phash_entry *e = &h->table[idx];
	unsigned x;
	for (x = 0; x != PHASH_WAYS; ++x)
		if (e->keys[x] == key)
			return e->vals[x];
	return -1;
}

void phash_add(struct Phash *h, unsigned key, int val)
{
	unsigned idx = gather(key, h->select);
	unsigned newbit;
	struct Phash_entry *table = h->table;
	struct Phash_entry *e = &table[idx];
	unsigned len = h->len;
	unsigned x, y, z;

	/* Maybe it fits in current table? */
	for (x = 0; x != PHASH_WAYS; ++x)
		if (!e->vals[x]) {
			e->keys[x] = key;
			e->vals[x] = val;
			++h->count;
			return;
		}

	/* No space in entry: we need to expand */

	/* Find a new select bit */
	newbit = (e->keys[0] ^ key) & ~h->select;
	newbit = newbit & -newbit;

	/* Add it to the set */
	h->select |= newbit;

	/* Double the table size */
	h->len *= 2;
	h->table = (struct Phash_entry *)calloc(h->len, sizeof(struct Phash_entry));
	h->count = 0;

	/* Copy entries into new table */
	for (z = 0; z != len; ++z)
		for (y = 0; y != PHASH_WAYS; ++y)
			if (table[z].vals[y])
				phash_add(h, table[z].keys[y], table[z].vals[y]);

	/* Add new entry */
	phash_add(h, key, val);

	/* Delete old table */
	free(table);
}
