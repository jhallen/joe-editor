/*
 *	Doubly linked list primitives
 *	Copyright
 *		(C) 1992 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */

extern void *ITEM;
extern void *QUEUE;
extern void *LAST;
struct stditem {
	LINK(STDITEM)	link;
};

/* Initialize a doubly-linked list */

#define izque(type,member,item) do { \
	QUEUE = (void *)(item); \
	((type *)QUEUE)->member.prev = (type *)QUEUE; \
	((type *)QUEUE)->member.next = (type *)QUEUE; \
	} while(0)

/* Remove an item from a list */

#define deque(type,member,item) do { \
	ITEM = (void *)(item); \
	((type *)ITEM)->member.prev->member.next = ((type *)ITEM)->member.next; \
	((type *)ITEM)->member.next->member.prev = ((type *)ITEM)->member.prev; \
	} while(0)

/* Remove an item from a list and return it */

#define deque_f(type,member,item) \
	( \
	ITEM=(void *)(item), \
	((type *)ITEM)->member.prev->member.next=((type *)ITEM)->member.next, \
	((type *)ITEM)->member.next->member.prev=((type *)ITEM)->member.prev, \
	(type *)ITEM \
	)

/* Return true if doubly-linked list is empty */

#define qempty(type,member,item) \
	( \
	QUEUE=(void *)(item), \
	(type *)QUEUE==((type *)QUEUE)->member.next \
	)

/* Insert an item at front of list */

#define enquef(type,member,queue,item) do { \
	ITEM = (void *)(item); \
	QUEUE = (void *)(queue); \
	((type *)ITEM)->member.next = ((type *)QUEUE)->member.next; \
	((type *)ITEM)->member.prev = (type *)QUEUE; \
	((type *)QUEUE)->member.next->member.prev = (type *)ITEM; \
	((type *)QUEUE)->member.next = (type *)ITEM; \
	} while(0)

/* Insert an item at back of list */

#define enqueb(type,member,queue,item) do { \
	ITEM = (void *)(item); \
	QUEUE = (void *)(queue); \
	((type *)ITEM)->member.next = (type *)QUEUE; \
	((type *)ITEM)->member.prev = ((type *)QUEUE)->member.prev; \
	((type *)QUEUE)->member.prev->member.next = (type *)ITEM; \
	((type *)QUEUE)->member.prev = (type *)ITEM; \
	} while(0)

/* Insert an item at front of list and return it */

#define enqueb_f(type,member,queue,item) \
	( \
	ITEM=(void *)(item), \
	QUEUE=(void *)(queue), \
	((type *)ITEM)->member.next=(type *)QUEUE, \
	((type *)ITEM)->member.prev=((type *)QUEUE)->member.prev, \
	((type *)QUEUE)->member.prev->member.next=(type *)ITEM, \
	((type *)QUEUE)->member.prev=(type *)ITEM, \
	(type *)ITEM \
	)

/* Promote an item to front of list */

#define promote(type,member,queue,item) \
	enquef(type,member,(queue),deque_f(type,member,(item)))

/* Demote an item to back of list */

#define demote(type,member,queue,item) \
	enqueb(type,member,(queue),deque_f(type,member,(item)))

/* Splice a chain into from of a list */

#define splicef(type,member,queue,chain) do { \
	ITEM = (void *)(chain); \
	LAST = (void *)((type *)ITEM)->member.prev; \
	QUEUE = (void *)(queue); \
	((type *)LAST)->member.next = ((type *)QUEUE)->member.next; \
	((type *)ITEM)->member.prev = (type *)QUEUE; \
	((type *)QUEUE)->member.next->member.prev = (type *)LAST; \
	((type *)QUEUE)->member.next = (type *)ITEM; \
	} while(0)

/* Splice a chain into back of a list */

#define spliceb(type,member,queue,chain) do { \
	ITEM = (void *)(chain); \
	LAST = (void *)((type *)ITEM)->member.prev; \
	QUEUE = (void *)(queue); \
	((type *)LAST)->member.next = (type *)QUEUE; \
	((type *)ITEM)->member.prev = ((type *)QUEUE)->member.prev; \
	((type *)QUEUE)->member.prev->member.next = (type *)ITEM; \
	((type *)QUEUE)->member.prev = (type *)LAST; \
	} while(0)

#define spliceb_f(type,member,queue,chain) \
	( \
	ITEM=(void *)(chain), \
	LAST=(void *)((type *)ITEM)->member.prev, \
	QUEUE=(void *)(queue), \
	((type *)LAST)->member.next=(type *)QUEUE, \
	((type *)ITEM)->member.prev=((type *)QUEUE)->member.prev, \
	((type *)QUEUE)->member.prev->member.next=(type *)ITEM, \
	((type *)QUEUE)->member.prev=(type *)LAST, \
	(type *)ITEM \
	)

/* Remove a range of items from a list and return it as a chain for later splicing */

#define snip(type,member,first,last) \
	( \
	ITEM=(void *)(first), \
	LAST=(void *)(last), \
	((type *)LAST)->member.next->member.prev=((type *)ITEM)->member.prev, \
	((type *)ITEM)->member.prev->member.next=((type *)LAST)->member.next, \
	((type *)ITEM)->member.prev=(type *)LAST, \
	((type *)LAST)->member.next=(type *)ITEM, \
	(type *)ITEM \
	)

/* Allocate an item */

void *alitem(void *list, ptrdiff_t itemsize);

/* Free an item */

void frchn(void *list, void *ch);
