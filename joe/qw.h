/*
 *	Single-key query windows
 *	Copyright
 *		(C) 1992 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */
#ifndef _JOE_QW_H
#define _JOE_QW_H 1

/* Single-key Query window */

struct query_result {
	Coroutine t;
	int answer;
};

struct query {
	W *parent;		/* Window we're in */
	struct query_result *result;	/* Who gets the answer */
	unsigned char *prompt;	/* Prompt string */
	int promptlen;		/* Width of prompt string */
	int org_w;
	int org_h;
	int flg;
};

/* Ask the user a question which requires just a single character answer.
 * Returns with the answer.  Returns with -1 if window could not be created.
 *
 * Window is placed at the bottom of 'w'.
 * prompt/len gives the prompt.
 *
 * Flags as defined below:
 */

/* Choose one of: */
/* With no flags: window has type name "query" */
#define QW_STAY 1	/* Cursor stays in original window, window has type name "querya" */
#define QW_SR 2		/* Same as QW_STAY, but window has type name "querysr" (for search/replace) */
/* Type selects which keymap will be used */

/* Plus optionally this: */
#define QW_NOMACRO 4	/* Do not take input from macro for this query */

int query(W *w, unsigned char *prompt, int len, int flg);

#endif
