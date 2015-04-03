/*
 *	Simple hash table
 *	Copyright
 *		(C) 1992 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */
#include "types.h"

static HENTRY *freentry = NULL;

/* Compute hash value of string */

#define hnext(accu, c) (((accu) << 4) + ((accu) >> 28) + (c))

int hash(char *s)
{
	int accu = 0;

	while (*s) {
		accu = hnext(accu, *s++);
	}
	return accu;
}

/* Create hash table */

HASH *htmk(int len)
{
	HASH *t = (HASH *) joe_malloc(SIZEOF(HASH));
	t->nentries = 0;
	t->len = len;
	t->tab = (HENTRY **) joe_calloc(SIZEOF(HENTRY *), len);
	return t;
}

/* Delete hash table.  Only the hash table is deleted, not the names and values */

void htrm(HASH *ht)
{
	int x;
	for (x = 0; x != ht->len; ++x) {
		HENTRY *p, *n;
		for (p = ht->tab[x]; p; p = n) {
			n = p->next;
			p->next = freentry;
			freentry = p;
		}
	}
	joe_free(ht->tab);
	joe_free(ht);
}

/* Expand hash table */

void htexpand(HASH *h)
{
	int x;
	/* Allocate new table */
	int new_size = h->len * 2;
	HENTRY **new_table = joe_calloc(new_size, SIZEOF(HENTRY *));
	/* Copy entries from old table to new */
	for (x = 0; x != h->len; ++x) {
		HENTRY *e;
		while ((e = h->tab[x])) {
			h->tab[x] = e->next;
			e->next = new_table[e->hash_val & (new_size - 1)];
			new_table[e->hash_val & (new_size - 1)] = e;
		}
	}
	/* Replace old table with new */
	free(h->tab);
	h->tab = new_table;
	h->len = new_size;
}

/* Bind a value to a name.  This does not check for duplicate entries.  The
 * name and value are not duplicated: it's up to you to keep them around for
 * the life of the hash table. */

void *htadd(HASH *ht, char *name, void *val)
{
	int hval = hash(name);
	int idx = hval & (ht->len - 1);
	HENTRY *entry;
	int x;

	if (!freentry) {
		entry = (HENTRY *) joe_malloc(SIZEOF(HENTRY) * 64);
		for (x = 0; x != 64; ++x) {
			entry[x].next = freentry;
			freentry = entry + x;
		}
	}
	entry = freentry;
	freentry = entry->next;
	entry->next = ht->tab[idx];
	ht->tab[idx] = entry;
	entry->name = name;
	entry->val = val;
	entry->hash_val = hval;
	if (++ht->nentries == (ht->len >> 1) + (ht->len >> 2))
		htexpand(ht);
	return val;
}

/* Return value associated with name or NULL if there is none */

void *htfind(HASH *ht, char *name)
{
	HENTRY *e;

	for (e = ht->tab[hash(name) & (ht->len - 1)]; e; e = e->next) {
		if (!zcmp(e->name, name)) {
			return e->val;
		}
	}
	return NULL;
}
