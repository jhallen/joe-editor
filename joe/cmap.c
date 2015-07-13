/*
 *	Characters maps
 *	Copyright
 *		(C) 1992 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */

#include "types.h"

struct bind mkbinding(void *thing, int what)
{
	struct bind bind;
	bind.thing = thing;
	bind.what = what;
	return bind;
}

struct interval_list *mkinterval(struct interval_list *next, int first, int last, struct bind map)
{
	struct interval_list *interval_list = (struct interval_list *)malloc(sizeof(struct interval_list));
	interval_list->next = next;
	interval_list->interval.first = first;
	interval_list->interval.last = last;
	interval_list->map = map;
	return interval_list;
}

void rminterval(struct interval_list *interval_list)
{
	while (interval_list) {
		struct interval_list *n = interval_list->next;
		free(interval_list);
		interval_list = n;
	}
}

/* Add a single character range to an interval list */

struct interval_list *interval_add(struct interval_list *interval_list, int first, int last, struct bind map)
{
	struct interval_list *e, **p;
	for (p = &interval_list; *p;) {
		e = *p;
		if (first > e->interval.last + 1 || (first > e->interval.last && (map.thing != e->map.thing))) {
			/* e is below new range, skip it */
			p = &e->next;
		} else if (e->interval.first > last + 1 || (e->interval.first > last && (map.thing != e->map.thing))) {
			/* e is fully above new range, so insert new one here */
			break;
		} else if (e->map.thing == map.thing) {
			/* merge e into new */
			if (e->interval.first <= first) {
				if (e->interval.last >= last) { /* && e->interval.first <= first */
					/* Existing covers new: we're done */
					return interval_list;
				} else { /* e->interval.last < last && e->interval.first <= first */
					/* Enlarge new, delete existing */
					first = e->interval.first;
					*p = e->next;
					e->next = 0;
					rminterval(e);
				}
			} else { /* e->interval.first > first */
				if (e->interval.last <= last) { /* && e->interval.first > first */
					/* New fully covers existing, delete existing */
					*p = e->next;
					e->next = 0;
					rminterval(e);
				} else { /* e->interval.last > last && e->interval.first > first */
					/* Extend existing */
					e->interval.first = first;
					return interval_list;
				}
			}
		} else {
			/* replace e with new */
			if (e->interval.first < first) {
				if (e->interval.last <= last) { /* && e->interval.first < first */
					/* Top part of existing get cut-off by new */
					e->interval.last = first - 1;
				} else { /* e->interval.last > last && e->interval.first < first */
					int org = e->interval.last;
					struct bind orgmap = e->map;
					e->interval.last = first - 1;
					p = &e->next;
					*p = mkinterval(*p, first, last, map);
					p = &(*p)->next;
					*p = mkinterval(*p, last + 1, org, orgmap);
					return interval_list;
				}
			} else { /* e->interval.first >= first */
				if (e->interval.last <= last) { /* && e->interval.first >= first */
					/* Delete existing */
					*p = e->next;
					e->next = 0;
					rminterval(e);
				} else { /* e->interval.last > last && e->interval.first >= first */
					e->interval.first = last + 1;
					break;
				}
			}
		}
	}
	*p = mkinterval(*p, first, last, map);
	return interval_list;
}

/* Add a list of intervals (typically representing a character class) to an interval list */

struct interval_list *interval_set(struct interval_list *interval_list, struct interval *list, int size, struct bind map)
{
	int x;
	for (x = 0; x != size; ++x)
		interval_list = interval_add(interval_list, list[x].first, list[x].last, map);
	return interval_list;
}

struct bind interval_lookup(struct interval_list *list, int item)
{
	while (list) {
		if (item >= list->interval.first && item <= list->interval.last)
			return list->map;
		list = list->next;
	}
	return mkbinding(NULL, 0);
}

/* Build a cmap from an interval list */

void cmap_build(struct cmap *cmap, struct interval_list *list, struct bind dflt_map)
{
	struct interval_list *l;
	int x;
	/* Record default */
	cmap->dflt_map = dflt_map;
	for (x = 0; x != 128; ++x)
		cmap->direct_map[x] = dflt_map;
	/* Fill in direct map */
	for (l = list; l; l = l->next)
		if (l->interval.first >= 128)
			break;
		else {
			int ch;
			for (ch = l->interval.first; ch < 128 && ch <= l->interval.last; ++ch)
				cmap->direct_map[ch] = l->map;
		}
	/* Skip over list items which are wholly in the direct map */
	while (list && list->interval.last < 128)
		 list = list -> next;
	/* Calculate size of array */
	cmap->size = 0;
	for (l = list; l; l = l->next)
		++cmap->size;
	if (cmap->size) {
		/* Allocate and populate array */
		cmap->range_map = (struct interval_map *)malloc(sizeof(struct interval_map) * cmap->size);
		x = 0;
		for (l = list; l; l = l->next) {
			cmap->range_map[x].interval.first = l->interval.first;
			cmap->range_map[x].interval.last = l->interval.last;
			cmap->range_map[x].map = l->map;
			if (cmap->range_map[x].interval.first < 128)
				cmap->range_map[x].interval.first = 128;
			++x;
		}
	} else
		cmap->range_map = 0;
}

/* Clear cmap, free space */

void clr_cmap(struct cmap *cmap)
{
	if (cmap->range_map)
		joe_free(cmap->range_map);
	cmap->range_map = 0;
	cmap->size = 0;
}

/* Lookup a character in a cmap, return its assigned mapping */

struct bind cmap_lookup(struct cmap *cmap, int ch)
{
	if (ch < 128)
		return cmap->direct_map[ch];
	if (cmap->size) {
		int min = 0;
		int mid;
		int max = cmap->size - 1;
		if (ch < cmap->range_map[min].interval.first || ch > cmap->range_map[max].interval.last)
			goto no_match;
		while (max >= min) {
			mid = (min + max) / 2;
			if (ch > cmap->range_map[mid].interval.last)
				min = mid + 1;
			else if (ch < cmap->range_map[mid].interval.first)
				max = mid - 1;
			else
				return cmap->range_map[mid].map;
		}
	}
	no_match:
	return cmap->dflt_map;
}

#ifdef junk
struct slist {
	struct slist *next;
	char *s;
} *slist;

char *find(char *s)
{
	struct slist *l;
	for (l = slist; l; l = l->next)
		if (!strcmp(l->s, s))
			return l->s;
	l = (struct slist *)malloc(sizeof(struct slist));
	l->next = slist;
	slist = l;
	l->s = strdup(s);
	return l->s;
}

int main(int argc, char *argv)
{
	char buf[100];
	struct interval_list *list = 0;
	while (gets(buf)) {
		if (buf[0] == 'a') {
			int first, last;
			char buf1[100];
			sscanf(buf + 1," %d %d %s",&first,&last,buf1);
			printf("a %d %d %s\n", first, last, buf1);
			list = interval_add(list, first, last, mkbind(find(buf1), 0));
		} else if (buf[0] == 's') {
			struct interval_list *l;
			for (l = list; l; l = l->next)
				printf("%d %d %s\n",l->interval.first,l->interval.last,l->map.thing);
		}
	}
}
#endif
