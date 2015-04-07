/*
 *	Simple hash table
 *	Copyright
 *		(C) 1992 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */
#ifndef _JOE_HASH_H
#define _JOE_HASH_H 1

struct entry {
	HENTRY *next;
	unsigned char *name;
	unsigned hash_val;
	void *val;
};

struct hash {
	unsigned len;
	HENTRY **tab;
	unsigned nentries;
};

/* Compute hash code for a string */
unsigned long hash(unsigned char *s);

/* Create a hash table of specified size, which must be a power of 2 */
HASH *htmk(int len);

/* Delete a hash table.  HENTRIES get freed, but name/vals don't. */
void htrm(HASH *ht);

/* Add an entry to a hash table.
  Note: 'name' is _not_ strdup()ed */
void *htadd(HASH *ht, unsigned char *name, void *val);

/* Look up an entry in a hash table, returns NULL if not found */
void *htfind(HASH *ht, unsigned char *name);

#endif
