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
	int	(*func)(W *w, int k, void *object, int *notify);	/* Func. which gets called when key is hit */
	int	(*abrt)(W *w, void *object);
	void	*object;
	char	*prompt;	/* Prompt string */
	ptrdiff_t	promptlen;	/* Width of prompt string */
	ptrdiff_t	org_w;
	ptrdiff_t	org_h;
};

/* QW *mkqw(W *w, char *prompt, int (*func)(), int (*abrt)(), void *object);
 * Create a query window for the given window
 */

QW *mkqw(W *w, const char *prompt, ptrdiff_t len, int (*func)(W *w, int k, void *object, int *notify), int (*abrt)(W *w, void *object), void *object, int *notify);
QW *mkqwna(W *w, const char *prompt, ptrdiff_t len, int (*func)(W *w, int k, void *object, int *notify), int (*abrt)(W *w, void *object), void *object, int *notify);
QW *mkqwnsr(W *w, const char *prompt, ptrdiff_t len, int (*func)(W *w, int k, void *object, int *notify), int (*abrt)(W *w, void *object), void *object, int *notify);
