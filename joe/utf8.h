/*
 *	UTF-8 Utilities
 *	Copyright
 *		(C) 2004 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */

/* UTF-8 Encoder
 *
 * c is unicode character.
 * buf is 7 byte buffer- utf-8 coded character is written to this followed by a 0 termination.
 * returns length (not including terminator).
 */

ptrdiff_t utf8_encode(char *buf,int c);

/* UTF-8 decoder state machine */

struct utf8_sm {
	char buf[8];	/* Record of sequence */
	ptrdiff_t ptr;	/* Record pointer */
	int state;	/* Current state.  0 = idle, anything else is no. of chars left in sequence */
	int accu;	/* Character accumulator */
};

/* UTF-8 Decoder
 *
 * Returns 0 - 7FFFFFFF: decoded character
 *                   -1: character accepted, nothing decoded yet.
 *                   -2: incomplete sequence
 *                   -3: no sequence started, but character is between 128 - 191, 254 or 255
 */

int utf8_decode(struct utf8_sm *utf8_sm,char c);

int utf8_decode_string(const char *s);

int utf8_decode_fwrd(const char **p,ptrdiff_t *plen);

/* Initialize state machine */

void utf8_init(struct utf8_sm *utf8_sm);

void joe_locale();
void to_utf8(struct charmap *map,char *s,int c);
int from_utf8(struct charmap *map,const char *s);

int unictrl(int ucs);
int mk_wcwidth(int wide,int c);

extern struct charmap *locale_map;	/* Default bytemap of terminal */
extern const char *locale_lang;	/* Locale language (like de_DE) */
extern const char *locale_msgs;	/* Locale language for editor messages (like de_DE) */

void my_iconv(char *dest, ptrdiff_t destsiz, struct charmap *dest_map,
              const char *src,struct charmap *src_map);

struct charmap *guess_map(const char *buf, ptrdiff_t len);

extern int guess_non_utf8;
extern int guess_utf8;
