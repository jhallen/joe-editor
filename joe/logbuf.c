/* Log buffer functions */

#include "types.h"

/* Setup log buffer if it isn't already */

void u_log_setup(BW *bw)
{
	B *b = bw->b;
	if (!b->hist) {
		b->hist = bmk(NULL);
		b->hist_p = pdup(b->hist->bof);
	}
}

/* Get previous history line */

int u_log_uparw(BW *bw)
{
	u_log_setup(bw);
	
}

/* Get next history line */

int u_log_dnarw(BW *bw)
{
}

/* User hit return */

int u_log_rtn(BW *bw)
{
	W *w = bw->parent;
	PW *pw = (PW *) bw->object;
	char *s;
	W *win;
	int *notify;
	int (*pfunc) ();
	void *object;
	off_t byte;

	/* Extract entered text from buffer */
	p_goto_eol(bw->cursor);
	byte = bw->cursor->byte;
	p_goto_bol(bw->cursor);
	s = brvs(bw->cursor, (int) (byte - bw->cursor->byte));

	/* Save text into history buffer */
	if (pw->hist) {
		if (bw->b->changed) {
			append_history(pw->hist, sv(s));
		} else {
			promote_history(pw->hist, bw->cursor->line);
		}
	}

	/* Do ~ expansion and set new current directory */
	if (pw->file_prompt&2) {
		set_current_dir(s,1);
	}

	if (pw->file_prompt) {
		s = canonical(s);
	}

	win = w->win;
	pfunc = pw->pfunc;
	object = pw->object;
	bwrm(bw);
	joe_free(pw->prompt);
	joe_free(pw);
	w->object = NULL;
	notify = w->notify;
	w->notify = 0;
	wabort(w);
	dostaupd = 1;

	/* Call callback function */
	if (pfunc) {
		return pfunc(win->object, s, object, notify);
	} else {
		return -1;
	}
}
