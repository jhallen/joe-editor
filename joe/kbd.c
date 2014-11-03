/*
 *	Key-map handler
 *	Copyright
 *		(C) 1992 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */
#include "types.h"

/* Create a KBD */

KBD *mkkbd(KMAP *kmap)
{
	KBD *kbd = (KBD *) joe_malloc(sizeof(KBD));

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

void *dokey(KBD *kbd, int n)
{
	void *bind = NULL;

	/* If we were passed a negative character */
	if (n < 0)
		n += 256;

	/* If we're starting from scratch, clear the keymap sequence buffer */
	if (kbd->curmap == kbd->topmap)
		kbd->x = 0;

	if (kbd->curmap->keys[n].k == 1) {	/* A prefix key was found */
		kbd->seq[kbd->x++] = n;
		kbd->curmap = kbd->curmap->keys[n].value.submap;
	} else {		/* A complete key sequence was entered or an unbound key was found */
		bind = kbd->curmap->keys[n].value.bind;
/*  kbd->seq[kbd->x++]=n; */
		kbd->x = 0;
		kbd->curmap = kbd->topmap;
	}
	return bind;
}

/* Return key code for key name or -1 for syntax error */

static int keyval(unsigned char *s)
{
	if (s[0] == '^' && s[1] && !s[2])
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
		if(!zcmp(s,USTR "MDOWN")) return KEY_MDOWN;
		else if(!zcmp(s,USTR "MWDOWN")) return KEY_MWDOWN;
		else if(!zcmp(s,USTR "MWUP")) return KEY_MWUP;
		else if(!zcmp(s,USTR "MUP")) return KEY_MUP;
		else if(!zcmp(s,USTR "MDRAG")) return KEY_MDRAG;
		else if(!zcmp(s,USTR "M2DOWN")) return KEY_M2DOWN;
		else if(!zcmp(s,USTR "M2UP")) return KEY_M2UP;
		else if(!zcmp(s,USTR "M2DRAG")) return KEY_M2DRAG;
		else if(!zcmp(s,USTR "M3DOWN")) return KEY_M3DOWN;
		else if(!zcmp(s,USTR "M3UP")) return KEY_M3UP;
		else if(!zcmp(s,USTR "M3DRAG")) return KEY_M3DRAG;
		else return s[0];
	} else if (s[1] || !s[0])
		return -1;
	else
		return (unsigned char) s[0];
}

/* Create an empty keymap */

KMAP *mkkmap(void)
{
	KMAP *kmap = (KMAP *) joe_calloc(sizeof(KMAP), 1);

	return kmap;
}

/* Eliminate a keymap */

void rmkmap(KMAP *kmap)
{
	int x;

	if (!kmap)
		return;
	for (x = 0; x != KEYS; ++x)
		if (kmap->keys[x].k == 1)
			rmkmap(kmap->keys[x].value.submap);
	joe_free(kmap);
}

/* Parse a range */

static unsigned char *range(unsigned char *seq, int *vv, int *ww)
{
	unsigned char c;
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

static KMAP *kbuild(CAP *cap, KMAP *kmap, unsigned char *seq, void *bind, int *err, unsigned char *capseq, int seql)
{
	int v, w;

	if (!seql && seq[0] == '.' && seq[1]) {
		int x, c;
		unsigned char *s;

		for (x = 0; seq[x] && seq[x] != ' '; ++x) ;
		c = seq[x];
		seq[x] = 0;
#ifdef __MSDOS__
		if (!zcmp(seq + 1, "ku")) {
			capseq = "\0H";
			seql = 2;
		} else if (!zcmp(seq + 1, USTR "kd")) {
			capseq = "\0P";
			seql = 2;
		} else if (!zcmp(seq + 1, USTR "kl")) {
			capseq = "\0K";
			seql = 2;
		} else if (!zcmp(seq + 1, USTR "kr")) {
			capseq = "\0M";
			seql = 2;
		} else if (!zcmp(seq + 1, USTR "kI")) {
			capseq = "\0R";
			seql = 2;
		} else if (!zcmp(seq + 1, USTR "kD")) {
			capseq = "\0S";
			seql = 2;
		} else if (!zcmp(seq + 1, USTR "kh")) {
			capseq = "\0G";
			seql = 2;
		} else if (!zcmp(seq + 1, USTR "kH")) {
			capseq = "\0O";
			seql = 2;
		} else if (!zcmp(seq + 1, USTR "kP")) {
			capseq = "\0I";
			seql = 2;
		} else if (!zcmp(seq + 1, USTR "kN")) {
			capseq = "\0Q";
			seql = 2;
		} else if (!zcmp(seq + 1, USTR "k1")) {
			capseq = "\0;";
			seql = 2;
		} else if (!zcmp(seq + 1, USTR "k2")) {
			capseq = "\0<";
			seql = 2;
		} else if (!zcmp(seq + 1, USTR "k3")) {
			capseq = "\0=";
			seql = 2;
		} else if (!zcmp(seq + 1, USTR "k4")) {
			capseq = "\0>";
			seql = 2;
		} else if (!zcmp(seq + 1, USTR "k5")) {
			capseq = "\0?";
			seql = 2;
		} else if (!zcmp(seq + 1, USTR "k6")) {
			capseq = "\0@";
			seql = 2;
		} else if (!zcmp(seq + 1, USTR "k7")) {
			capseq = "\0A";
			seql = 2;
		} else if (!zcmp(seq + 1, USTR "k8")) {
			capseq = "\0B";
			seql = 2;
		} else if (!zcmp(seq + 1, USTR "k9")) {
			capseq = "\0C";
			seql = 2;
		} else if (!zcmp(seq + 1, USTR "k0")) {
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
		if (s && (s = tcompile(cap, s, 0, 0, 0, 0))
		    && (sLEN(s) > 1 || (signed char)s[0] < 0)) {
			capseq = s;
			seql = sLEN(s);
			for (seq += x; *seq == ' '; ++seq) ;
		}
#endif
		else {
			*err = -2;
			return kmap;
		}
	}

	if (seql) {
		v = w = (unsigned char) *capseq++;
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
	while (v <= w) {
		if (*seq || seql) {
			if (kmap->keys[v].k == 0)
				kmap->keys[v].value.submap = NULL;
			kmap->keys[v].k = 1;
			kmap->keys[v].value.submap = kbuild(cap, kmap->keys[v].value.submap, seq, bind, err, capseq, seql);
			if (!kmap->keys[v].value.submap) {
                        	/* Error during recursion. Prevent crash later. */
                        	kmap->keys[v].k = 0;
			}
		} else {
			if (kmap->keys[v].k == 1)
				rmkmap(kmap->keys[v].value.submap);
			kmap->keys[v].k = 0;
			kmap->keys[v].value.bind =
			    /* This bit of code sticks the key value in the macro */
			    (v == w ? macstk(bind, v) : dupmacro(macstk(bind, v)));
		}
		++v;
	}
	return kmap;
}

int kadd(CAP *cap, KMAP *kmap, unsigned char *seq, void *bind)
{
	int err = 0;

	kbuild(cap, kmap, seq, bind, &err, NULL, 0);
	return err;
}

void kcpy(KMAP *dest, KMAP *src)
{
	int x;

	for (x = 0; x != KEYS; ++x)
		if (src->keys[x].k == 1) {
			if (dest->keys[x].k != 1) {
				dest->keys[x].k = 1;
				dest->keys[x].value.submap = mkkmap();
			}
			kcpy(dest->keys[x].value.submap, src->keys[x].value.submap);
		} else if (src->keys[x].k == 0 && src->keys[x].value.bind) {
			if (dest->keys[x].k == 1)
				rmkmap(dest->keys[x].value.submap);
			dest->keys[x].value.bind = src->keys[x].value.bind;
			dest->keys[x].k = 0;
		}
}

/* Remove a binding from a keymap */

int kdel(KMAP *kmap, unsigned char *seq)
{
	int err = 1;
	int v, w;

	seq = range(seq, &v, &w);
	if (!seq)
		return -1;

	/* Clear bindings between v and w */
	while (v <= w) {
		if (*seq) {
			if (kmap->keys[v].k == 1) {
				int r = kdel(kmap->keys[v].value.submap, seq);

				if (err != -1)
					err = r;
			}
		} else {
			if (kmap->keys[v].k == 1)
				rmkmap(kmap->keys[v].value.submap);
			kmap->keys[v].k = 0;
			kmap->keys[v].value.bind = NULL;
			if (err != -1)
				err = 0;
		}
		++v;
	}

	return err;
}

/* JM */

B *keymaphist=0;

int dokeymap(BW *bw,unsigned char *s,void *object,int *notify)
{
	KMAP *k=ngetcontext(s);
	vsrm(s);
	if(notify) *notify=1;
	if(!k) {
		msgnw(bw->parent,joe_gettext(_("No such keymap")));
		return -1;
	}
	rmkbd(bw->parent->kbd);
	bw->parent->kbd=mkkbd(k);
	return 0;
}

static unsigned char **keymap_list;

static int keymap_cmplt(BW *bw)
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

int ukeymap(BASE *bw)
{
	if (wmkpw(bw->parent,joe_gettext(_("Change keymap: ")),&keymaphist,dokeymap,USTR "keymap",NULL,keymap_cmplt,NULL,NULL,locale_map,0)) return 0;
	else return -1;
}
