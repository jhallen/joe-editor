/*
 *	Single-key query windows
 *	Copyright
 *		(C) 1992 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */

/* Single-key Query window */

struct query {
	W	*parent;	/* Window we're in */
	int	(*func) ();	/* Func. which gets called when key is hit */
	int	(*abrt) ();
	void	*object;
	char	*prompt;	/* Prompt string */
	ptrdiff_t	promptlen;	/* Width of prompt string */
	ptrdiff_t	org_w;
	ptrdiff_t	org_h;
};

/* QW *mkqw(W *w, char *prompt, int (*func)(), int (*abrt)(), void *object);
 * Create a query window for the given window
 */
/* FIXME: ??? ----> */
QW *mkqw(W *w, char *prompt, ptrdiff_t len, int (*func) (/* ??? */), int (*abrt) (/* ??? */), void *object, int *notify);
QW *mkqwna(W *w, char *prompt, ptrdiff_t len, int (*func) (/* ??? */), int (*abrt) (/* ??? */), void *object, int *notify);
QW *mkqwnsr(W *w, char *prompt, ptrdiff_t len, int (*func) (/* ??? */), int (*abrt) (/* ??? */), void *object, int *notify);
