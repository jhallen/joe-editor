/*
 *	Key-map handler
 *	Copyright
 *		(C) 1992 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */
#include "types.h"

struct context *contexts; /* Global list of KMAPs */
char **keymap_list; /* KMAP names array for completion */

/* Create a KBD */

KBD *mkkbd(KMAP *kmap)
{
	KBD *kbd = (KBD *) joe_malloc(SIZEOF(KBD));

	kbd->topmap = kmap;
	kbd->curmap = kmap;
	kbd->x = 0;
	return kbd;
}

/* Eliminate a KBD */

void rmkbd(KBD *k)
{
	joe_free(k);
}

/* Process next key for KBD */

MACRO *dokey(KBD *kbd, int n)
{
	struct bind bind;

	/* If we were passed a negative character */
	if (n < 0)
		n += 256;

	/* If we're starting from scratch, clear the keymap sequence buffer */
	if (kbd->curmap == kbd->topmap)
		kbd->x = 0;

	/* Update cmap if src changed */
	if (kbd->curmap->cmap_version != kbd->curmap->src_version) {
		cmap_build(&kbd->curmap->cmap, kbd->curmap->src, kbd->curmap->dflt);
		kbd->curmap->cmap_version = kbd->curmap->src_version;
	}
	bind = cmap_lookup(&kbd->curmap->cmap, n);
	if (bind.what == 1) {	/* A prefix key was found */
		kbd->seq[kbd->x++] = n;
		kbd->curmap = (KMAP *)bind.thing;
		bind.thing = 0;
	} else {	/* A complete sequence was found */
		kbd->x = 0;
		kbd->curmap = kbd->topmap;
	}

	return (MACRO *)bind.thing;
}

/* Return key code for key name or -1 for syntax error */

static int keyval(char *s)
{
	if (s[0] == 'U' && s[1] == '+') {
		return zhtoi(s + 2);
	} else if (s[0] == '^' && s[1] && !s[2])
		switch (s[1])
		{
		case '?':
			return 127;
		case '#':
			return 0x9B;
		default:
			return s[1] & 0x1F;
		}
	else if ((s[0] == 'S' || s[0] == 's')
		 && (s[1] == 'P' || s[1] == 'p') && !s[2])
		return ' ';
	else if((s[0]=='M'||s[0]=='m') && s[1]) {
		if(!zcmp(s,"MDOWN")) return KEY_MDOWN;
		else if(!zcmp(s,"MWDOWN")) return KEY_MWDOWN;
		else if(!zcmp(s,"MWUP")) return KEY_MWUP;
		else if(!zcmp(s,"MUP")) return KEY_MUP;
		else if(!zcmp(s,"MDRAG")) return KEY_MDRAG;
		else if(!zcmp(s,"M2DOWN")) return KEY_M2DOWN;
		else if(!zcmp(s,"M2UP")) return KEY_M2UP;
		else if(!zcmp(s,"M2DRAG")) return KEY_M2DRAG;
		else if(!zcmp(s,"M3DOWN")) return KEY_M3DOWN;
		else if(!zcmp(s,"M3UP")) return KEY_M3UP;
		else if(!zcmp(s,"M3DRAG")) return KEY_M3DRAG;
		else return s[0];
	} else {
		int ch = utf8_decode_string(s);
		if (ch < 0)
			ch = -1;
		return ch;
	}
/*
	 if (s[1] || !s[0])
		return -1;
	else
		return ((unsigned char *)s)[0];
*/
}

/* Create an empty keymap */

KMAP *mkkmap(void)
{
	KMAP *kmap = (KMAP *) joe_calloc(SIZEOF(KMAP), 1);

	return kmap;
}

/* Eliminate a keymap */

void rmkmap(KMAP *kmap)
{
	struct interval_list *l, *n;
	if (!kmap)
		return;
	for (l = kmap->src; l; l = n) {
		n = l->next;
		if (l->map.what == 1) {
			rmkmap((KMAP *)l->map.thing);
		}
		joe_free(l);
	}
	if (kmap->dflt.what == 1)
		rmkmap((KMAP *)kmap->dflt.thing);
	clr_cmap(&kmap->cmap);
	joe_free(kmap);
}

/* Parse a range */

static char *range(char *seq, int *vv, int *ww)
{
	char c;
	int x, v, w;

	for (x = 0; seq[x] && seq[x] != ' '; ++x) ;	/* Skip to a space */
	c = seq[x];
	seq[x] = 0;		/* Zero terminate the string */
	v = keyval(seq);	/* Get key */
	w = v;
	if (w < 0)
		return NULL;
	seq[x] = c;		/* Restore the space or 0 */
	for (seq += x; *seq == ' '; ++seq) ;	/* Skip over spaces */

	/* Check for 'TO ' */
	if ((seq[0] == 'T' || seq[0] == 't') && (seq[1] == 'O' || seq[1] == 'o') && seq[2] == ' ') {
		for (seq += 2; *seq == ' '; ++seq) ;	/* Skip over spaces */
		for (x = 0; seq[x] && seq[x] != ' '; ++x) ;	/* Skip to space */
		c = seq[x];
		seq[x] = 0;	/* Zero terminate the string */
		w = keyval(seq);	/* Get key */
		if (w < 0)
			return NULL;
		seq[x] = c;	/* Restore the space or 0 */
		for (seq += x; *seq == ' '; ++seq) ;	/* Skip over spaces */
	}

	if (v > w)
		return NULL;

	*vv = v;
	*ww = w;
	return seq;
}

/* Add a binding to a keymap */

static KMAP *kbuild(CAP *cap, KMAP *kmap, char *seq, MACRO *bind, int *err, const char *capseq, ptrdiff_t seql)
{
	int v, w;

	if (!seql && seq[0] == '.' && seq[1]) {
		int x;
		char c;
		const char *s;
		char *v;

		for (x = 0; seq[x] && seq[x] != ' '; ++x) ;
		c = seq[x];
		seq[x] = 0;
#ifdef __MSDOS__
		if (!zcmp(seq + 1, "ku")) {
			capseq = "\0H";
			seql = 2;
		} else if (!zcmp(seq + 1, "kd")) {
			capseq = "\0P";
			seql = 2;
		} else if (!zcmp(seq + 1, "kl")) {
			capseq = "\0K";
			seql = 2;
		} else if (!zcmp(seq + 1, "kr")) {
			capseq = "\0M";
			seql = 2;
		} else if (!zcmp(seq + 1, "kI")) {
			capseq = "\0R";
			seql = 2;
		} else if (!zcmp(seq + 1, "kD")) {
			capseq = "\0S";
			seql = 2;
		} else if (!zcmp(seq + 1, "kh")) {
			capseq = "\0G";
			seql = 2;
		} else if (!zcmp(seq + 1, "kH")) {
			capseq = "\0O";
			seql = 2;
		} else if (!zcmp(seq + 1, "kP")) {
			capseq = "\0I";
			seql = 2;
		} else if (!zcmp(seq + 1, "kN")) {
			capseq = "\0Q";
			seql = 2;
		} else if (!zcmp(seq + 1, "k1")) {
			capseq = "\0;";
			seql = 2;
		} else if (!zcmp(seq + 1, "k2")) {
			capseq = "\0<";
			seql = 2;
		} else if (!zcmp(seq + 1, "k3")) {
			capseq = "\0=";
			seql = 2;
		} else if (!zcmp(seq + 1, "k4")) {
			capseq = "\0>";
			seql = 2;
		} else if (!zcmp(seq + 1, "k5")) {
			capseq = "\0?";
			seql = 2;
		} else if (!zcmp(seq + 1, "k6")) {
			capseq = "\0@";
			seql = 2;
		} else if (!zcmp(seq + 1, "k7")) {
			capseq = "\0A";
			seql = 2;
		} else if (!zcmp(seq + 1, "k8")) {
			capseq = "\0B";
			seql = 2;
		} else if (!zcmp(seq + 1, "k9")) {
			capseq = "\0C";
			seql = 2;
		} else if (!zcmp(seq + 1, "k0")) {
			capseq = "\0D";
			seql = 2;
		}
		seq[x] = c;
		if (seql) {
			for (seq += x; *seq == ' '; ++seq) ;
		}
#else
		s = jgetstr(cap, seq + 1);
		seq[x] = c;
		if (s && (v = tcompile(cap, s, 0, 0, 0, 0))
		    && (sLEN(v) > 1 || (signed char)v[0] < 0)) {
			capseq = v;
			seql = sLEN(v);
			for (seq += x; *seq == ' '; ++seq) ;
		}
#endif
		else {
			*err = -2;
			return kmap;
		}
	}

	if (seql) {
		v = w = *capseq++;
		--seql;
	} else {
		seq = range(seq, &v, &w);
		if (!seq) {
			*err = -1;
			return kmap;
		}
	}

	if (!kmap)
		kmap = mkkmap();	/* Create new keymap if 'kmap' was NULL */

	/* Make bindings between v and w */
	if (v <= w) {
		if (*seq || seql) {
			struct bind old = interval_lookup(kmap->src, v);
			if (!old.thing || !old.what) {
				kmap->src = interval_add(kmap->src, v, w, mkbinding(kbuild(cap, NULL, seq, bind, err, capseq, seql), 1));
				++kmap->src_version;
			} else
				kbuild(cap, (KMAP *)old.thing, seq, bind, err, capseq, seql);
		} else {
			kmap->src = interval_add(kmap->src, v, w, mkbinding(bind, 0));
			++kmap->src_version;
		}
	}
	return kmap;
}

int kadd(CAP *cap, KMAP *kmap, char *seq, MACRO *bind)
{
	int err = 0;

	kbuild(cap, kmap, seq, bind, &err, NULL, 0);
	return err;
}

void kcpy(KMAP *dest, KMAP *src)
{
	struct interval_list *l;
	for (l = src->src; l; l = l->next) {
		if (l->map.what == 1) {
			KMAP *k = mkkmap();
			kcpy(k, (KMAP *)l->map.thing);
			dest->src = interval_add(dest->src, l->interval.first, l->interval.last, mkbinding(k, 1));
			++dest->src_version;
		} else {
			dest->src = interval_add(dest->src, l->interval.first, l->interval.last, l->map);
			++dest->src_version;
		}
	}
}

/* Remove a binding from a keymap */

int kdel(KMAP *kmap, char *seq)
{
	int err = 1;
	int v, w;

	seq = range(seq, &v, &w);
	if (!seq)
		return -1;

	/* Clear bindings between v and w */
	if (v <= w) {
		if (*seq) {
			struct bind old = interval_lookup(kmap->src, v);
			if (old.what == 1) {
				kdel((KMAP *)old.thing, seq);
			} else {
				kmap->src = interval_add(kmap->src, v, w, mkbinding(NULL, 0));
				++kmap->src_version;
				
			}
		} else {
			kmap->src = interval_add(kmap->src, v, w, mkbinding(NULL, 0));
			++kmap->src_version;
		}
	}

	return err;
}

/* Find a context of a given name- if not found, one with an empty kmap
 * is created.
 */

KMAP *kmap_getcontext(const char *name)
{
	struct context *c;

	for (c = contexts; c; c = c->next)
		if (!zcmp(c->name, name))
			return c->kmap;
	c = (struct context *) joe_malloc(SIZEOF(struct context));

	c->next = contexts;
	c->name = zdup(name);
	contexts = c;
	return c->kmap = mkkmap();
}

/* JM - ngetcontext(name) - like getcontext, but return NULL if it
 * doesn't exist, instead of creating a new one.
 */

KMAP *ngetcontext(const char *name)
{
	struct context *c;
	for(c=contexts;c;c=c->next)
		if(!zcmp(c->name,name))
			return c->kmap;
	return 0;
}

/* True if KMAP is empty */

int kmap_empty(KMAP *k)
{
	return k->src == NULL && k->dflt.thing == NULL;
}

/* JM */

B *keymaphist=0;

static int dokeymap(W *w,char *s,void *object,int *notify)
{
	KMAP *k=ngetcontext(s);
	vsrm(s);
	if(notify) *notify=1;
	if(!k) {
		msgnw(w,joe_gettext(_("No such keymap")));
		return -1;
	}
	rmkbd(w->kbd);
	w->kbd=mkkbd(k);
	return 0;
}

static char **get_keymap_list()
{
	char **lst = 0;
	struct context *c;
	for (c=contexts; c; c=c->next)
		lst = vaadd(lst, vsncpy(NULL,0,sz(c->name)));

	return lst;
}

static int keymap_cmplt(BW *bw, int k)
{
	/* Reload every time: we should really check date of tags file...
	  if (tag_word_list)
	  	varm(tag_word_list); */

	if (!keymap_list)
		keymap_list = get_keymap_list();

	if (!keymap_list) {
		ttputc(7);
		return 0;
	}

	return simple_cmplt(bw,keymap_list);
}

int ukeymap(W *w, int k)
{
	if (wmkpw(w,joe_gettext(_("Change keymap: ")),&keymaphist,dokeymap,"keymap",NULL,keymap_cmplt,NULL,NULL,locale_map,0)) return 0;
	else return -1;
}
