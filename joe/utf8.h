/*
 *	UTF-8 Utilities
 *	Copyright
 *		(C) 2004 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */
#ifndef _Iutf8
#define _Iutf8 1

/* UTF-8 Encoder
 *
 * c is unicode character.
 * buf is 7 byte buffer- utf-8 coded character is written to this followed by a 0 termination.
 * returns length (not including terminator).
 */

int utf8_encode PARAMS((unsigned char *buf,int c));

/* UTF-8 decoder state machine */

struct utf8_sm {
	unsigned char buf[8];	/* Record of sequence */
	int ptr;		/* Record pointer */
	int state;		/* Current state.  0 = idle, anything else is no. of chars left in sequence */
	int accu;		/* Character accumulator */
};

/* UTF-8 Decoder
 *
 * Returns 0 - 7FFFFFFF: decoded character
 *                   -1: character accepted, nothing decoded yet.
 *                   -2: incomplete sequence
 *                   -3: no sequence started, but character is between 128 - 191, 254 or 255
 */

int utf8_decode PARAMS((struct utf8_sm *utf8_sm,unsigned char c));

int utf8_decode_string PARAMS((unsigned char *s));

int utf8_decode_fwrd PARAMS((unsigned char **p,int *plen));

/* Initialize state machine */

void utf8_init PARAMS((struct utf8_sm *utf8_sm));

void joe_locale();
void to_utf8 PARAMS((struct charmap *map,unsigned char *s,int c));
int from_utf8 PARAMS((struct charmap *map,unsigned char *s));

int unictrl PARAMS((int ucs));
int mk_wcwidth PARAMS((int wide,int c));

extern struct charmap *locale_map;	/* Default bytemap of terminal */
extern unsigned char *locale_lang;	/* Locale language (like de_DE) */
extern unsigned char *locale_msgs;	/* Locale language for editor messages (like de_DE) */

void my_iconv PARAMS((unsigned char *dest, size_t destsiz, struct charmap *dest_map,
              unsigned char *src,struct charmap *src_map));

struct charmap *guess_map PARAMS((unsigned char *buf, int len));

extern int guess_non_utf8;
extern int guess_utf8;

#endif
