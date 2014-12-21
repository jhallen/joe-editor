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
#ifndef _JOE_BLOCKS_H
#define _JOE_BLOCKS_H 1

/* char *mmove(char *d,char *s,int sz); Copy 'sz' bytes from 's' to 'd'.
 * Chooses either mbkwd or mfwrd to do this such that the data won't get
 * clobbered.
 */
void *mmove PARAMS((void *d, void *s, int sz));

/* char *mset(char *d,char c,int sz); Set 'sz' bytes at 'd' to 'c'.
 * If 'sz'==0 nothing happens
 * Return original value of 'd'
 */
unsigned char *mset PARAMS((void *dest, unsigned char c, int sz));

/* int *msetI(int *d,int c,int sz); Set 'sz' ints at 'd' to 'c'.
 * If 'sz'==0 nothing happens
 * Returns orininal value of 'd'
 */
int *msetI PARAMS((void *dest, int c, int sz));

/* void **msetP(void **d,void *c,int sz); Set 'sz' pointers at 'd' to 'c'.
 * If 'sz'==0 nothing happens
 * Returns orininal value of 'd'
 */
void **msetP PARAMS((void **dest, void *c, int sz));

/* int mcnt(char *blk,char c,int size);
 *
 * Count the number of occurances a character appears in a block
 */
int mcnt PARAMS((unsigned char *blk, unsigned char c, int size));

#ifdef junk
/* char *mchr(char *s,char c);
 *
 * Return address of first 'c' following 's'.
 */
unsigned char *mchr PARAMS(());
#endif
#endif
