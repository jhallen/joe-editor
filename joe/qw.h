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

struct query {
	W	*parent;	/* Window we're in */
	int	(*func) ();	/* Func. which gets called when key is hit */
	int	(*abrt) ();
	void	*object;
	unsigned char	*prompt;	/* Prompt string */
	int	promptlen;	/* Width of prompt string */
	int	org_w;
	int	org_h;
};

/* QW *mkqw(W *w, char *prompt, int (*func)(), int (*abrt)(), void *object);
 * Create a query window for the given window
 */
/* FIXME: ??? ----> */
QW *mkqw PARAMS((W *w, unsigned char *prompt, int len, int (*func) (/* ??? */), int (*abrt) (/* ??? */), void *object, int *notify));
QW *mkqwna PARAMS((W *w, unsigned char *prompt, int len, int (*func) (/* ??? */), int (*abrt) (/* ??? */), void *object, int *notify));
QW *mkqwnsr PARAMS((W *w, unsigned char *prompt, int len, int (*func) (/* ??? */), int (*abrt) (/* ??? */), void *object, int *notify));

#endif
