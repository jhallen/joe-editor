/*
 *	Fast block move/copy subroutines
 *
 *      These exist because memmove on older UNIXs was really slow.
 *
 *	Copyright
 *		(C) 1992 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */

/* char *mmove(char *d,char *s,int sz); Copy 'sz' bytes from 's' to 'd'.
 * Chooses either mbkwd or mfwrd to do this such that the data won't get
 * clobbered.
 */
void *mmove(void *d, const void *s, ptrdiff_t sz);

/* char *mset(char *d,char c,int sz); Set 'sz' bytes at 'd' to 'c'.
 * If 'sz'==0 nothing happens
 * Return original value of 'd'
 */
char *mset(char *dest, char c, ptrdiff_t sz);

/* int *msetI(int *d,int c,int sz); Set 'sz' ints at 'd' to 'c'.
 * If 'sz'==0 nothing happens
 * Returns orininal value of 'd'
 */
int *msetI(int *dest, int c, ptrdiff_t sz);

/* int *msetI(int *d,int c,int sz); Set 'sz' ints at 'd' to 'c'.
 * If 'sz'==0 nothing happens
 * Returns orininal value of 'd'
 */
ptrdiff_t *msetD(ptrdiff_t *dest, ptrdiff_t c, ptrdiff_t sz);

/* void **msetP(void **d,void *c,int sz); Set 'sz' pointers at 'd' to 'c'.
 * If 'sz'==0 nothing happens
 * Returns orininal value of 'd'
 */
void **msetP(void **dest, void *c, ptrdiff_t sz);

/* int mcnt(char *blk,char c,ptrdiff_t size);
 *
 * Count the number of occurances a character appears in a block
 */
ptrdiff_t mcnt(const char *blk, char c, ptrdiff_t size);

#ifdef junk
/* char *mchr(char *s,char c);
 *
 * Return address of first 'c' following 's'.
 */
char *mchr(char *s, char c);
#endif
