/*
 * 	User file operations
 *	Copyright
 *		(C) 1992 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */
#include "types.h"

#ifdef HAVE_UTIME_H
#include <utime.h>
#else
#ifdef HAVE_SYS_UTIME_H
#include <sys/utime.h>
#endif
#endif

#ifdef WITH_SELINUX
int copy_security_context(const char *from_file, const char *to_file);
#endif

int orphan;
unsigned char *backpath = NULL;		/* Place to store backup files */
B *filehist = NULL;	/* History of file names */
int nobackups = 0;
int exask = 0;
extern int noexmsg;

/* Ending message generator */
/**** message which is shown after closing joe (CTRL+x; CTRL+k) *****/
void genexmsg(BW *bw, int saved, unsigned char *name)
{
	unsigned char *s;
	unsigned char *m;

	if (bw->b->name && bw->b->name[0]) {
		s = bw->b->name;
	} else {
		s = joe_gettext(_("(Unnamed)"));
	}

	if (name) {
		if (saved) {
			m = vsfmt(NULL, 0, joe_gettext(_("File %s saved")), name);
		} else {
			m = vsfmt(NULL, 0, joe_gettext(_("File %s not saved")), name);
		}
	} else if (bw->b->changed && bw->b->count == 1) {
		m = vsfmt(NULL, 0, joe_gettext(_("File %s not saved")), s);
	} else if (saved) {
		m = vsfmt(NULL, 0, joe_gettext(_("File %s saved")), s);
	} else {
		m = vsfmt(NULL, 0, joe_gettext(_("File %s not changed so no update needed")), s);
	}
	if (exmsg)
		obj_free(exmsg);

	exmsg = m;
	obj_perm(exmsg);
	if (!noexmsg)
		msgnw(bw->parent, m);
}

/* For ^X ^C */
void genexmsgmulti(BW *bw, int saved, int skipped)
{
	unsigned char *m;
	if (saved)
		if (skipped)
			m = vsdupz(joe_gettext(_("Some files have not been saved.")));
		else
			m = vsdupz(joe_gettext(_("All modified files have been saved.")));
	else
		m = vsdupz(joe_gettext(_("No modified files, so no updates needed.")));

	msgnw(bw->parent, m);

	if (exmsg)
		obj_free(exmsg);
	exmsg = m;
	obj_perm(exmsg);
}

/* Shell escape */

int ushell(BW *bw)
{
	nescape(bw->parent->t->t);
	ttsusp();
	nreturn(bw->parent->t->t);
	return 0;
}

/* Execute shell command */

int usys(BW *bw)
{
	unsigned char *s = ask(bw->parent, joe_gettext(_("System (^C to abort): ")), NULL, NULL, utypebw, bw->b->o.charmap, 1, 0, NULL);
	if (s) {
		int rtn;
		nescape(bw->parent->t->t);
		rtn=ttshell(s);
		nreturn(bw->parent->t->t);
		return rtn;
	} else {
		return -1;
	}
}

/* Copy a file */

static int cp(unsigned char *from, unsigned char *to)
{
	int f, g, amnt;
	struct stat sbuf;
	char buf[8192];

#ifdef HAVE_UTIME
#ifdef NeXT
	time_t utbuf[2];
#else
	struct utimbuf utbuf;
#endif
#endif

	f = open((char *)from, O_RDONLY);
	if (f < 0) {
		return -1;
	}
	if (fstat(f, &sbuf) < 0) {
		return -1;
	}
	g = creat((char *)to, sbuf.st_mode & ~(S_ISUID | S_ISGID));
	if (g < 0) {
		close(f);
		return -1;
	}
	while ((amnt = read(f, buf, sizeof(buf))) > 0) {
		if (amnt != joe_write(g, buf, amnt)) {
			break;
		}
	}
	close(f);
	close(g);
	if (amnt) {
		return -1;
	}

#ifdef HAVE_UTIME
#ifdef NeXT
	utbuf[0] = (time_t) sbuf.st_atime;
	utbuf[1] = (time_t) sbuf.st_mtime;
#else
	utbuf.actime = sbuf.st_atime;
	utbuf.modtime = sbuf.st_mtime;
#endif
	utime((char *)to, &utbuf);
#endif

#ifdef WITH_SELINUX
	copy_security_context(from,to);
#endif

	return 0;
}

/* Make backup file if it needs to be made
 * Returns 0 if backup file was made or didn't need to be made
 * Returns 1 for error
 */

static int backup(BW *bw)
{
	if (!bw->b->backup && !nobackups && !bw->o.nobackup && bw->b->name && bw->b->name[0]) {
		unsigned char *name = 0;

#ifdef __MSDOS__
		int x;

		if (backpath) {
			unsigned char *t = vsncpy(NULL,0,sz(backpath));
			t = canonical(t);
			name = vsfmt(name, 0, USTR "%s/%s", t, namepart(tmp, dequote(bw->b->name)));
		} else {
			name = vsfmt(name, 0, USTR "%s", dequote(bw->b->name));
		}

		for (x = vslen(name); name[--x] != '.';) {
			if (name[x] == '\\' || (name[x] == ':' && x == 1) || x == 0) {
				x = vslen(name);
				break;
			}
		}

		name = vsncpy(name, x, sc(".bak"));
		name = vstrunc(name, x + 4);

#else

		/* Create backup file name */
		unsigned char *simple_backup_suffix = (unsigned char *)getenv("SIMPLE_BACKUP_SUFFIX");
		
		if (simple_backup_suffix == NULL) {
			simple_backup_suffix = USTR "~";
		}
		if (backpath) {
			unsigned char *t = vsncpy(NULL, 0, sz(backpath));
			t = canonical(t);
			name = vsfmt(name, 0, USTR "%s/%s%s", t, namprt(bw->b->name), simple_backup_suffix);
		} else {
			name = vsfmt(name, 0, USTR "%s%s", bw->b->name, simple_backup_suffix);
		}
		
		/* Attempt to delete backup file first */
		unlink((char *)name);

#endif

		/* Copy original file to backup file */
		if (cp(dequote(bw->b->name), name)) {
			return 1;
		} else {
			bw->b->backup = 1;
			return 0;
		}
	} else {
		return 0;
	}
}

/* Write file */

/* Continuation structure */

struct savereq {
	int (*callback) ();
	unsigned char *name;
	B *first;
	int not_saved;	/* Set if a modified file was not saved */
	int rename;	/* Set if we're renaming the file during save */
	int block_save; /* Flag, if we want to save a block# */
	unsigned char *message; /* String for messages to be shown to the user */
};

struct savereq *mksavereq(int (*callback)(), unsigned char *name, B *first,int rename, int block_save)
{
	struct savereq *req = (struct savereq *) joe_malloc(sizeof(struct savereq));
	req->callback = callback;
	req->name = name;
	req->first = first;
	req->not_saved = 0;
	req->rename = rename;
	req->block_save = block_save;
	return req;
}

static void rmsavereq(struct savereq *req)
{
	obj_free(req->name);
	joe_free(req);
}

/* Check if character 'c' is in the set.
 * 'c' should be unicode if the locale is UTF-8, otherwise it's
 * an 8-bit character.  'set' should be of this format: "xxxx<>yyyy".  xxxx
 * is a list of 8-bit characters. yyyy is a list of UTF-8 characters.
 */

unsigned char *yes_key = (unsigned char *) _("|yes|yY");
unsigned char *no_key = (unsigned char *) _("|no|nN");

int yncheck(unsigned char *key_set, int c)
{
	unsigned char *set = joe_gettext(key_set);
	if (locale_map->type) {
		/* 'c' is unicode */
		while (*set) {
			if (c == utf8_decode_fwrd(&set, NULL))
				return 1;
		}
		return 0;
	} else {
		/* 'c' is 8-bit */
		while (set[0]) {
			if (set[0] == c)
				return 1;
			++set;
		}
		return 0;
	}
}

int ynchecks(unsigned char *set, unsigned char *s)
{
	if (locale_map->type)
		return yncheck(set, utf8_decode_fwrd(&s, NULL));
	else
		return yncheck(set, s[0]);
}

static int saver(BW *bw, int c, struct savereq *req)
{
	int fl;
	again:
	if (c == NO_CODE || yncheck(no_key, c)) {
		msgnw(bw->parent, joe_gettext(_("Couldn't make backup file... file not saved")));
		if (req->callback) {
			return req->callback(bw, req, -1);
		} else {
			rmsavereq(req);
			return -1;
		}
	}
	if (c != YES_CODE && !yncheck(yes_key, c)) {
		c = query(bw->parent, sz(joe_gettext(_("Could not make backup file.  Save anyway (y,n,^C)? "))), 0);
		if (c == -1) {
			rmsavereq(req);
			return -1;
		} else
			goto again;
	}
	if (bw->b->er == -1 && bw->o.msnew) {
		exmacro(bw->o.msnew,1);
		bw->b->er = -3;
	}
	if (bw->b->er == 0 && bw->o.msold) {
		exmacro(bw->o.msold,1);
	}
	if ((fl = bsave(bw->b->bof, req->name, bw->b->eof->byte, req->rename ? 2 : 1)) != 0) {
		msgnw(bw->parent, joe_gettext(msgs[-fl]));
		if (req->callback) {
			return req->callback(bw, req, -1);
		} else {
			rmsavereq(req);
			return -1;
		}
	} else {
		if (req->rename) {
			bw_unlock(bw);
			joe_free(bw->b->name);
			bw->b->name = 0;
		}
		if (!bw->b->name && req->name[0]!='!' && req->name[0]!='>')
			bw->b->name = joesep(zdup(req->name));
		if (bw->b->name && !zcmp(bw->b->name, req->name)) {
			bw_unlock(bw);
			bw->b->changed = 0;
			saverr(bw->b->name);
		}
		{
			/* Last UNDOREC which wasn't modified will be changed
			 * to modified. And because this block is
			 * executed after each 'save', there can't be more
			 * than one record which is not modified
			 *		24 Apr 2001, Marx
			 */
			UNDO *u = bw->b->undo;
			UNDOREC *rec, *rec_start;

			rec = rec_start = &u->recs;

			do {
				rec = rec->link.prev;
			} while (rec != rec_start && rec->changed);
			if(rec->changed == 0)
				rec->changed = 1;

		}
		genexmsg(bw, 1, req->name);
		if (req->callback) {
			return req->callback(bw, req, 0);
		} else {
			rmsavereq(req);
			return 0;
		}
	}
}

static int dosave(BW *bw, struct savereq *req)
{
	if (req->block_save)
	{
		if (markv(1)) {
			if (square) {
				int fl;
				int ret = 0;
				B *tmp = pextrect(markb,
						  markk->line - markb->line + 1,
						  markk->xcol);
						  
				if ((fl = bsave(tmp->bof, req->name, tmp->eof->byte, 0)) != 0) {
					msgnw(bw->parent, joe_gettext(msgs[-fl]));
					ret = -1;
				}
				brm(tmp);
				if (!ret) {
					msgnw(bw->parent, vsfmt(NULL, 0, joe_gettext(_("Block written to file %s")), req->name));
				}
				if (lightoff)
					unmark(bw);
				rmsavereq(req);
				return ret;
			} else {
				int fl;
				int ret = 0;

				if ((fl = bsave(markb, req->name, markk->byte - markb->byte, 0)) != 0) {
					msgnw(bw->parent, joe_gettext(msgs[-fl]));
					ret = -1;
				}
				if (!ret) {
					msgnw(bw->parent, vsfmt(NULL, 0, joe_gettext(_("Block written to file %s")), req->name));
				}
				if (lightoff)
					unmark(bw);
				rmsavereq(req);
				return ret;
			}
		} else {
			rmsavereq(req);
			msgnw(bw->parent, joe_gettext(_("No block")));
			return -1;
		}
	}
	else
	{
		if (backup(bw)) {
			return saver(bw, 0, req);
		} else {
			return saver(bw, YES_CODE, req);
		}
	}
}

static int dosave2(BW *bw, int c, struct savereq *req)
{
	again:
	if (c == YES_CODE || yncheck(yes_key, c)) {
		return dosave(bw, req);
	} else if (c == NO_CODE || yncheck(no_key, c)) {
		genexmsg(bw, 0, req->name);
		rmsavereq(req);
		return -1;
	} else if (-1 != (c = query(bw->parent, sz(req->message), 0))) {
		goto again;
	} else {
		/* Should be in abort function */
		rmsavereq(req);
		return -1;
	}
}

/* Checks if file exists. */

static int dosave1(BW *bw, unsigned char *s, struct savereq *req)
{
	int f;

	if (req->name)
		obj_free(req->name);
	req->name = s;
	obj_perm(req->name);

	if (s[0] != '!' && !(s[0] == '>' && s[1] == '>')) {
		/* It's a normal file: not a pipe or append */
		if (!bw->b->name || zcmp(s, bw->b->name)) {
			/* Newly named file or name is different than buffer */
			f = open((char *)dequote(s), O_RDONLY);
			if (f != -1) {
				close(f);
				req->message = joe_gettext(_("File exists. Overwrite (y,n,^C)? "));
				return dosave2(bw, 0, req);
			}
		}
		else {
			/* We're saving a newer version of the same file */
			if (check_mod(bw->b)) {
				req->message = joe_gettext(_("File on disk is newer. Overwrite (y,n,^C)? "));
				return dosave2(bw, 0, req);
			}
		}
	}

	return dosave(bw, req);
}

/* User command: ^K D */

int usave(BW *bw)
{
	unsigned char *s = ask(bw->parent, joe_gettext(_("Name of file to save (^C to abort): ")), &filehist, USTR "Names", cmplt,
	                       locale_map, bw->b->name ? 1 : 7, 0, bw->b->name);
	if (s) {
		return dosave1(bw, s, mksavereq(NULL,NULL,NULL,1, 0));
	} else {
		return -1;
	}
}

int usavenow(BW *bw)
{
	if (bw->b->name) {
		return dosave1(bw,vsncpy(NULL,0,sz(bw->b->name)),mksavereq(NULL,NULL,NULL,0,0));
	} else
		return usave(bw);
}

/* Write highlighted block to a file */

int ublksave(BW *bw)
{
	if (markb && markk && markb->b == markk->b && (markk->byte - markb->byte) > 0 && (!square || piscol(markk) > piscol(markb))) {
		unsigned char *s = ask(bw->parent, joe_gettext(_("Name of file to write (^C to abort): ")), &filehist, USTR "Names", cmplt,
				       locale_map, 3, 0, bw->b->name);
		if (s) {
			return dosave1(bw, s, mksavereq(NULL,NULL,NULL,0, 1));
		} else {
			return -1;
		}
	} else {
		return usave(bw);
	}
}


/* Load file to edit */

int doedit(BW *bw,int c,unsigned char *s)
{
	int omid;
	int ret = 0;
	int er;
	void *object;
	W *w;
	B *b;
	unsigned char *current_dir;
	again:
	if (c == YES_CODE || yncheck(yes_key, c)) {
		/* Reload from file */

		b = bfind_reload(s);
		er = berror;
		current_dir = vsdup(bw->b->current_dir);
		/* Try to pop scratch window */
		if (bw->b->scratch) {
			W *w = bw->parent;
			upopabort(bw);
			bw = (BW *)w->object;
		}
		if (bw->b->count == 1 && (bw->b->changed || bw->b->name)) {
			if (orphan) {
				orphit(bw);
			} else {
				if (uduptw(bw)) {
					brm(b);
					return -1;
				}
				bw = (BW *) maint->curwin->object;
			}
		}
		if (er) {
			msgnwt(bw->parent, joe_gettext(msgs[-er]));
			if (er != -1) {
				ret = -1;
			}
		}
		object = bw->object;
		w = bw->parent;
		bwrm(bw);
		w->object = (void *) (bw = bwmk(w, b, 0, NULL));
		/* Propogate current directory to newly loaded buffer */
		if (!b->current_dir)
			b->current_dir = current_dir;
		obj_perm(b->current_dir);
		wredraw(bw->parent);
		bw->object = object;
		if (er == -1 && bw->o.mnew) {
			exmacro(bw->o.mnew,1);
		}
		if (er == 0 && bw->o.mold) {
			exmacro(bw->o.mold,1);
		}
		/* Restore cursor line */
		pline(bw->cursor, get_file_pos(bw->b->name));
		omid = mid;
		mid = 1;
		dofollows();
		mid = omid;
		
		return ret;
	} else if (c == NO_CODE || yncheck(no_key, c)) {
		/* Edit already loaded buffer */

		b = bfind(s);
		er = berror;
		/* Try to pop scratch window */
		if (bw->b->scratch) {
			W *w = bw->parent;
			upopabort(bw);
			bw = (BW *)w->object;
		}
		if (bw->b->count == 1 && (bw->b->changed || bw->b->name)) {
			if (orphan) {
				orphit(bw);
			} else {
				if (uduptw(bw)) {
					brm(b);
					return -1;
				}
				bw = (BW *) maint->curwin->object;
			}
		}
		if (er) {
			msgnwt(bw->parent, joe_gettext(msgs[-er]));
			if (er != -1) {
				ret = -1;
			}
		}
		object = bw->object;
		w = bw->parent;
		bwrm(bw);
		w->object = (void *) (bw = bwmk(w, b, 0, NULL));
		wredraw(bw->parent);
		bw->object = object;
		if (er == -1 && bw->o.mnew) {
			exmacro(bw->o.mnew,1);
		}
		if (er == 0 && bw->o.mold) {
			exmacro(bw->o.mold,1);
		}
		/* Restore cursor line */
		pline(bw->cursor, get_file_pos(bw->b->name));
		omid = mid;
		mid = 1;
		dofollows();
		mid = omid;
		return ret;
	} else {
		/* FIXME: need abort handler to prevent leak */
		c = query(bw->parent, sz(joe_gettext(_("Load original file from disk (y,n,^C)? "))), 0);
		if (c == -1)
			return -1;
		goto again;
	}
}

int okrepl(BW *bw)
{
	if (bw->b->count == 1 && bw->b->changed) {
		msgnw(bw->parent, joe_gettext(_("Can't replace modified file")));
		return -1;
	} else {
		return 0;
	}
}

/* Edit a file.  If file is already loaded, ask the original should be loaded or to just switch to buffer */

int uedit(BW *bw)
{
	unsigned char *s = ask(bw->parent, joe_gettext(_("Name of file to edit (^C to abort): ")), &filehist,
	                       USTR "Names", cmplt, locale_map, 7, 0, NULL);
	if (s) {
		B *b;

		b = bcheck_loaded(s);

		if (b) {
			if (b->changed && !b->scratch)
				/* Modified buffer exists, so ask */
				return doedit(bw, 0, s);
			else
				/* Buffer not modified- just use it as is */
				return doedit(bw, NO_CODE, s);
		} else
			/* File not in buffer: don't ask */
			return doedit(bw, YES_CODE, s);
	} else {
		return -1;
	}
}

int usetcd(BW *bw)
{
	unsigned char *s;
	
	s = ask(bw->parent, joe_gettext(_("Set current directory (^C to abort): ")), &filehist, USTR "Names", cmplt, locale_map, 7, 0, NULL);
	if (!s)
		return -1;
	
	if (s[0])
		set_current_dir(bw, s, 1);
	
	msgnw(bw->parent, vsfmt(NULL, 0, joe_gettext(_("Directory prefix set to %s")), s));
	return 0;
}

/* Switch to another buffer.  If it doesn't exist, load file. */

int doswitch(BW *bw, unsigned char *s)
{
	return doedit(bw, NO_CODE, s);
}

int uswitch(BW *bw)
{
	unsigned char *s = ask(bw->parent, joe_gettext(_("Name of buffer to edit (^C to abort): ")), &filehist,
	                       USTR "Names", cmplt, locale_map, 1, 0, NULL);
	if(s) {
		return doswitch(bw, s);
	} else {
		return -1;
	}
}

void wpush(BW *bw)
{
	struct bstack *e;
	e = (struct bstack *)malloc(sizeof(struct bstack));
	e->b = bw->b;
	++bw->b->count;
	e->cursor = 0;
	e->top = 0;
	pdupown(bw->cursor, &e->cursor, USTR "wpush");
	pdupown(bw->top, &e->top, USTR "wpush");
	e->next = bw->parent->bstack;
	bw->parent->bstack = e;
}

int uscratch(BW *bw)
{
	int ret = 0;
	int er;
	void *object;
	W *w;
	B *b;
	unsigned char *current_dir;
	unsigned char *s;

	s = ask(bw->parent, joe_gettext(_("Name of scratch buffer to edit (^C to abort): ")), &filehist, USTR "Names", cmplt, locale_map, 1, 0, NULL);
	if (!s)
		return -1;

	current_dir = vsdup(bw->b->current_dir);

	b = bfind_scratch(s);
	/* Propogate current directory to scratch buffer */
	if (!b->current_dir) {
		b->current_dir = current_dir;
		obj_perm(current_dir);
	}

	er = berror;

	if (bw->b->count == 1 && (bw->b->changed || bw->b->name)) { /* Last reference on dirty buffer */
		if (orphan || bw->b->scratch) {
			orphit(bw);
		} else {
			if (uduptw(bw)) {
				brm(b);
				return -1;
			}
			bw = (BW *) maint->curwin->object;
		}
	}
	
	if (er) {
		msgnwt(bw->parent, joe_gettext(msgs[-er]));
		if (er != -1) {
			ret = -1;
		}
	}

	object = bw->object;
	w = bw->parent;
	bwrm(bw);
	w->object = (void *) (bw = bwmk(w, b, 0, NULL));
	wredraw(bw->parent);
	bw->object = object;

	if (er == -1 && bw->o.mnew)
		exmacro(bw->o.mnew,1);
	if (er == 0 && bw->o.mold)
		exmacro(bw->o.mold,1);

	return ret;
}

int uscratch_push(BW *bw)
{
	int ret = 0;
	int er;
	void *object;
	W *w;
	B *b;
	unsigned char *current_dir;
	unsigned char *s;

	s = ask(bw->parent, joe_gettext(_("Name of scratch buffer to edit (^C to abort): ")), &filehist, USTR "Names", cmplt, locale_map, 1, 0, NULL);
	if (!s)
		return -1;

	current_dir = vsdup(bw->b->current_dir);

	b = bfind_scratch(s);
	/* Propogate current directory to scratch buffer */
	if (!b->current_dir) {
		b->current_dir = current_dir;
		obj_perm(current_dir);
	}

	er = berror;

	if (!bw->b->scratch)
		wpush(bw);

	if (bw->b->count == 1 && (bw->b->changed || bw->b->name)) { /* Last reference on dirty buffer */
		if (orphan || bw->b->scratch) {
			orphit(bw);
		} else {
			if (uduptw(bw)) {
				brm(b);
				return -1;
			}
			bw = (BW *) maint->curwin->object;
		}
	}

	if (er) {
		msgnwt(bw->parent, joe_gettext(msgs[-er]));
		if (er != -1) {
			ret = -1;
		}
	}

	object = bw->object;
	w = bw->parent;
	bwrm(bw);
	w->object = (void *) (bw = bwmk(w, b, 0, NULL));
	wredraw(bw->parent);
	bw->object = object;
	
	if (er == -1 && bw->o.mnew)
		exmacro(bw->o.mnew,1);
	if (er == 0 && bw->o.mold)
		exmacro(bw->o.mold,1);
	
	return ret;
}

/* Load file into buffer: can result in an orphaned buffer */

static int dorepl(BW *bw, unsigned char *s, void *obj)
{
	void *object = bw->object;
	int omid;
	int ret = 0;
	int er;
	W *w = bw->parent;
	B *b;
	unsigned char *current_dir = vsdup(bw->b->current_dir);

	b = bfind(s);
	er = berror;
	if (berror) {
		msgnwt(bw->parent, joe_gettext(msgs[-berror]));
		if (berror != -1) {
			ret = -1;
		}
	}
	if (bw->b->count == 1 && (bw->b->changed || bw->b->name)) {
		orphit(bw);
	}
	bwrm(bw);
	w->object = (void *) (bw = bwmk(w, b, 0, NULL));
	/* Propogate current directory into new buffer */
	if (!b->current_dir) {
		b->current_dir = current_dir;
		obj_perm(current_dir);
	}
	wredraw(bw->parent);
	bw->object = object;
	if (er == -1 && bw->o.mnew) {
		exmacro(bw->o.mnew,1);
	}
	if (er == 0 && bw->o.mold) {
		exmacro(bw->o.mold,1);
	}
	/* Restore cursor line */
	pline(bw->cursor, get_file_pos(bw->b->name));
	omid = mid;
	mid = 1;
	dofollows();
	mid = omid;
	return ret;
}

/* Switch to a particular buffer */

int get_buffer_in_window(BW *bw, B *b)
{
	void *object = bw->object;
	W *w = bw->parent;
	if (b == bw->b) {
		return 0;
		/* return -1; this helps with querysave (no error when only one buffer) */
	}
	if (!b->orphan) {
		++b->count;
	} else {
		b->orphan = 0;
	}
	if (bw->b->count == 1) {
		orphit(bw);
	}
	bwrm(bw);
	w->object = (void *) (bw = bwmk(w, b, 0, NULL));
	wredraw(bw->parent);
	bw->object = object;
	return 0;
}

/* Switch to next buffer in window */

int unbuf(BW *bw)
{
	B *b;
	b = bnext(); /* bnext() returns NULL if there are no non-internal buffers */
	if (!b)
		return -1; /* No non-internal buffer to switch to */
	if (b == bw->b) {
		b = bnext();
	}
	return get_buffer_in_window(bw,b);
}

int upbuf(BW *bw)
{
	B *b;
	b = bprev();
	if (!b)
		return -1;
	if (b == bw->b) {
		b = bprev();
	}
	return get_buffer_in_window(bw, b);
}

/* Save and exit */

static int exdone(BW *bw, struct savereq *req,int flg)
{
	rmsavereq(req);
	if (flg) {
		return -1;
	} else {
		bw_unlock(bw);
		bw->b->changed = 0;
		saverr(bw->b->name);
		return uabort1(bw, -1);
	}
}

int uexsve(BW *bw)
{
	if (!bw->b->changed || bw->b->scratch) {
		/* It didn't change or it's just a scratch buffer: don't save */
		uabort(bw, -1);
		return 0;
	} else if (bw->b->name && !exask) {
		/* It changed, it's not a scratch buffer and it's named */
		return dosave1(bw, vsncpy(NULL, 0, sz(bw->b->name)), mksavereq(exdone,NULL,NULL,0,0));
	} else {
		unsigned char *s = ask(bw->parent, joe_gettext(_("Name of file to save (^C to abort): ")), &filehist,
				       USTR "Names", cmplt, locale_map, 1, 0, bw->b->name);
		if (s) {
			return dosave1(bw, s, mksavereq(NULL, NULL, NULL, 0, 0));
		} else {
			return -1;
		}
	}
}

/* If buffer is modified, prompt for saving: if user types 'n', uabort(), otherwise just return. */
/* If buffer is not modified, just return. */

int uask(BW *bw)
{
	if (bw->b->count == 1 && bw->b->changed && !bw->b->scratch) {
		for (;;) {
			int c = query(bw->parent, sz(joe_gettext(_("Save changes to this file (y,n,^C)? "))), 0);
			/* what happens when ^C is hit? */
			if (c == -1) {
				return -1;
			} else if (c == YES_CODE || yncheck(yes_key, c)) {
				return 0;
			} else if (c == NO_CODE || yncheck(no_key, c)) {
				genexmsg(bw, 0, NULL);
				abortit(bw);
				return -1;
			}
		}
	} else {
		return 0;
	}
}

/* Kill a buffer: any windows which have it get their buffer replaced with a
 * a scratch buffer */

int ulose(BW *bw)
{
	W *w;
	B *b, *new_b;
	int cnt;
	msgnw(bw->parent, NULL);
	if (bw->b->count==1 && bw->b->pid) {
		return ukillpid(bw);
	}
	if (bw->b->changed && !bw->b->scratch) {
		int c = query(bw->parent, sz(joe_gettext(_("Lose changes to this file (y,n,^C)? "))), 0);
		if (c != YES_CODE && !yncheck(yes_key, c)) {
			return -1;
		}
	}
	b=bw->b;
	cnt = b->count;
	b->count = 1;
	genexmsg(bw, 0, NULL);
	b->count = cnt;
	if ((w = maint->topwin) != NULL) {
		do {
			if ((w->watom->what&TYPETW) && ((BW *)w->object)->b==b) {
				if ((new_b = borphan()) != NULL) {
					BW *bw = (BW *)w->object;
					void *object = bw->object;
					/* FIXME: Shouldn't we wabort() and wcreate here to kill
					   any prompt windows? */

					bwrm(bw);
					w->object = (void *) (bw = bwmk(w, new_b, 0, NULL));
					wredraw(w);
					bw->object = object;
				} else {
					BW *bw = (BW *)w->object;
					void *object = bw->object;
					bwrm(bw);
					w->object = (void *) (bw = bwmk(w, bfind(USTR ""), 0, NULL));
					wredraw(w);
					bw->object = object;
					if (bw->o.mnew)
						exmacro(bw->o.mnew,1);
				}
			}
		w = w->link.next;
		} while (w != maint->topwin);
	}
	return 0;
}

/* Buffer list */

unsigned char **sbufs = NULL;

static int bufedcmplt(BW *bw)
{
	if (sbufs) {
		varm(sbufs);
		sbufs = 0;
	}
	if (!sbufs) {
		sbufs = getbufs();
		vaperm(sbufs);
	}
	return simple_cmplt(bw,sbufs);
}

B *bufhist = NULL;

int ubufed(BW *bw)
{
	unsigned char *s;
	s = ask(bw->parent, joe_gettext(_("Name of buffer to edit (^C to abort): ")), &bufhist, USTR "bufed", bufedcmplt, locale_map, 0, 0, NULL);
	if (s) {
		return dorepl(bw, s, NULL);
	} else {
		return -1;
	}
}


/* Query save loop */

static int doquerysave(BW *bw,int c,struct savereq *req)
{
	W *w = bw->parent;
	again:
	if (c == YES_CODE || yncheck(yes_key, c)) {
		if (bw->b->name && bw->b->name[0])
			return dosave1(bw, vsncpy(NULL,0,sz(bw->b->name)), req);
		else {
			unsigned char *s = ask(bw->parent, joe_gettext(_("Name of file to save (^C to abort): ")),
			                       &filehist, USTR "Names", cmplt, locale_map, 7, 0, NULL);

			if (s) {
				return dosave1(bw, s, req);
			} else {
				joe_free(req);
				return -1;
			}
		}
	} else if (c == NO_CODE || yncheck(no_key, c)) {
		/* Find next buffer to save */
		if (bw->b->changed)
			req->not_saved = 1;
		next:
		if (unbuf(bw)) {
			rmsavereq(req);
			genexmsgmulti(bw,1,req->not_saved);
			return 0;
		}
		bw = w->object;
		if (bw->b==req->first) {
			rmsavereq(req);
			genexmsgmulti(bw,1,req->not_saved);
			return 0;
		}
		if (!bw->b->changed || bw->b->scratch)
			goto next;

		return doquerysave(bw,0,req);
	} else {
		unsigned char *buf = 0;
		buf = vsfmt(buf, 0, joe_gettext(_("File %s has been modified.  Save it (y,n,^C)? ")),bw->b->name ? bw->b->name : USTR "(Unnamed)" );
		c = query(bw->parent, sv(buf), 0);
		if (c == -1) {
			rmsavereq(req);
			return -1;
		}
		goto again;
	}
}

static int query_next(BW *bw, struct savereq *req,int flg)
{
	if (flg) {
		rmsavereq(req);
		return -1;
	} else
		return doquerysave(bw,NO_CODE,req);
}

int uquerysave(BW *bw)
{
	W *w = bw->parent;
	B *first;

	/* Synchronize current window with buffer ring so that first is correct */
	unbuf(bw);
	bw = w->object;
	first = bw->b;

	/* Find a modified buffer */
	do {
		if (bw->b->changed && !bw->b->scratch)
			return doquerysave(bw,0,mksavereq(query_next,NULL,first,0,0));
		else if (unbuf(bw))
			break;
		bw = w->object;
	} while(bw->b!=first);

	genexmsgmulti(bw,0,0);

	return 0;
}

int ukilljoe(BW *bw)
{
	/* Save file positions */
	set_file_pos_all(bw->parent->t);
	/* FIXME: emacs checks for unsaved modified buffers at this point */
	leave = 1;
	return 0;
}

int ureload(BW *bw)
{
	B *n;
	if (!plain_file(bw->b)) {
		msgnw(bw->parent, joe_gettext(_("Can only reload plain files")));
		return -1;
	}
	if (bw->b->changed) {
		int c = query(bw->parent, sz(joe_gettext(_("Lose changes to this file (y,n,^C)? "))), 0);
		if (c != YES_CODE && !yncheck(yes_key, c)) {
			return -1;
		}
	}
	n = bload(bw->b->name);
	if (berror) {
		brm(n);
		msgnw(bw->parent, joe_gettext(msgs[-berror]));
		return -1;
	}
	breplace(bw->b, n);
	nredraw(bw->parent->t->t);
	msgnw(bw->parent, joe_gettext(_("File reloaded")));
	return 0;
}

int ureload_all(BW *bw)
{
	int count = 0;
	int er = 0;
	B *b;
	for (b = bufs.link.next; b != &bufs; b = b->link.next)
		if (!b->changed && plain_file(b)) {
			B *n = bload(b->name);
			if (berror) {
				msgnw(bw->parent, joe_gettext(msgs[-berror]));
				er = -1;
				brm(n);
			} else {
				breplace(b, n);
				++count;
			}
		}
	nredraw(bw->parent->t->t);
	if (!er) {
		msgnw(bw->parent, vsfmt(NULL, 0, joe_gettext(_("%d files reloaded")), count));
	}
	return er;
}
