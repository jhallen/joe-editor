/*
 *	User text formatting functions
 *	Copyright
 *		(C) 1992 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */
#include "types.h"

/* Center line cursor is on and move cursor to beginning of next line */

int ucenter(BW *bw)
{
	P *p = bw->cursor, *q;
	long endcol, begcol, x;
	int c;

	p_goto_eol(p);
	while (joe_isblank(bw->b->o.charmap, (c = prgetc(p))))
		/* do nothing */;
	if (c == '\n') {
		pgetc(p);
		goto done;
	}
	if (c == NO_MORE_DATA)
		goto done;
	pgetc(p);
	endcol = piscol(p);

	p_goto_bol(p);
	while (joe_isblank(bw->b->o.charmap, (c = pgetc(p))))
		/* do nothing */;
	if (c == '\n') {
		prgetc(p);
		goto done;
	}
	if (c == NO_MORE_DATA)
		goto done;
	prgetc(p);
	begcol = piscol(p);

	if (endcol - begcol > bw->o.rmargin + bw->o.lmargin)
		goto done;

	q = pdup(p, USTR "ucenter");
	p_goto_bol(q);
	bdel(q, p);
	prm(q);

	for (x = 0; x != (bw->o.lmargin + bw->o.rmargin) / 2 - (endcol - begcol) / 2; ++x)
		binsc(p, ' ');

      done:
	if (!pnextl(p)) {
		binsc(p, '\n');
		pgetc(p);
		return -1;
	} else
		return 0;
}

/* Return true if c is a character which can indent a paragraph */

/*   > is for mail/news
 *   * is for C comments
 *   / is for C++ comments
 *   # is for shell script comments
 *   % is for TeX comments
 */

static int cpara(BW *bw, int c)
{
	int x;
	if (c == ' ' || c == '\t')
		return 1;
	if (bw->o.cpara)
		for (x = 0; bw->o.cpara[x]; ++x)
			if (bw->o.cpara[x] == c)
				return 1;
	return 0;
#ifdef junk
	if (c == ' ' || c == '\t' || c == '\\' ||
	    c == '>' || c == '|' || c == ':' || c == '*' || c == '/' ||
	    c == ',' || c == '.' || c == '?' || c == ';' || c == ']' ||
	    c == '}' || c == '=' || c == '+' || c == '-' || c == '_' ||
	    c == ')' || c == '&' || c == '^' || c == '%' || c == '$' ||
	    c == '#' || c == '@' || c == '!' || c == '~')
		return 1;
	else
		return 0;
#endif
}

/* Return true if this first non-whitespace character means
   we are not a paragraph for sure (for example, '.' in nroff) */

static int cnotpara(BW *bw, int c)
{
	int x;
	if (bw->o.cnotpara)
		for (x = 0; bw->o.cnotpara[x]; ++x)
			if (bw->o.cnotpara[x] == c)
				return 1;
	return 0;
}

/* Return true if line is definitly not a paragraph line.
 * Lines which arn't paragraph lines:
 *  1) Blank lines
 *  2) Lines which begin with '.'
 */

static int pisnpara(BW *bw, P *p)
{
	P *q;
	int c;

	q = pdup(p, USTR "pisnpara");
	p_goto_bol(q);
	while (cpara(bw, c = pgetc(q)))
		/* do nothing */;
	prm(q);
	if (cnotpara(bw, c) || c == '\r' || c == '\n')
		return 1;
	else
		return 0;
}

/* Determine amount of indentation on current line.  Set first
   to include '-' and '*' bullets. */

static long nindent(BW *bw, P *p, int first)
{
	P *q = pdup(p, USTR "nindent");
	long col;
	int c;

	p_goto_bol(q);
	do {
		col = q->col;
	} while (cpara(bw, (c = pgetc(q))));
	if (first && (c == '-' || c == '*')) {
		c = pgetc(q);
		if (c == ' ') {
			col = q->col;
		}
	}
	prm(q);
	return col;
}

/* Get indentation prefix column */

static long prefix(BW *bw, P *p,int up)
{
	long len;
	P *q = pdup(p, USTR "prefix");

	p_goto_bol(q);
	while (cpara(bw, brch(q)))
		pgetc(q);
	while (!pisbol(q)) {
		/* int c; */
		if (!joe_isblank(p->b->o.charmap, ( /* c = */ prgetc(q)))) {
		/*
			if (up && (c == '*' || c == '-')) {
				if (!pisbol(q)) {
					c = prgetc(q);
					pgetc(q);
					if (c == ' ' || c == '\t')
						goto skip;
				} else
					goto skip;
			}
			pgetc(q);
		*/
			break;
		/* 	skip:; */
		}
	}
	len = piscol(q);
	prm(q);
	return len;
}

/* Move pointer to beginning of paragraph
 *
 * This function simply moves backwards until it sees:
 *  0) The beginning of the file
 *  1) A blank line
 *  2) A line with a different indentation prefix
 *  3) A line with indentation greater than that of the line we started with
 *  4) A line with indentation less than that of the starting line, but with
 *     a blank line (or beginning of file) preceeding it.
 */

int within = 0;

P *pbop(BW *bw, P *p)
{
	long indent;
	long prelen;
	P *last;

	p_goto_bol(p);
	indent = nindent(bw, p, 0);
	prelen = prefix(bw, p, 0);
	last = pdup(p, USTR "pbop");
	while (!pisbof(p) && (!within || !markb || p->byte > markb->byte)) {
		long ind;
		long len;

		pprevl(p);
		p_goto_bol(p);
		ind = nindent(bw, p, 0);
		len = prefix(bw, p, 0);
		if (pisnpara(bw, p) || len != prelen) {
			pset(p, last);
			break;
		}
		if (ind > indent) {
/*
			int ok = 1;
			P *q = pdup(p, USTR "pbop");
			if (pprevl(q)) {
				p_goto_bol(q);
				if (nindent(bw, q, 0) == ind)
					ok = 0;
			}
			prm(q);
			if (!ok)
				pnextl(p);
*/
			break;
		}
		if (ind < indent) {
			pset(p, last);
			break;
			/* if (pisbof(p)) {
				break;
			}
			pprevl(p);
			p_goto_bol(p);
			if (pisnpara(bw, p)) {
				pnextl(p);
				break;
			} else {
				pnextl(p);
				pnextl(p);
				break;
			} */
		}
		pset(last, p);
	}
	prm(last);
	return p;
}

/* Move pointer to end of paragraph.  Pointer must already be on first
 * line of paragraph for this to work correctly.
 *
 * This function moves forwards until it sees:
 *  0) The end of the file.
 *  1) A blank line
 *  2) A line with indentation different from the second line of the paragraph
 *  3) A line with prefix column different from first line
 */

P *peop(BW *bw, P *p)
{
	long indent;
	long prelen;

	if (!pnextl(p) || pisnpara(bw, p) || (within && markk && p->byte >= markk->byte))
		return p;
	indent = nindent(bw, p, 0);
	prelen = prefix(bw, p, 0);
	while (pnextl(p) && (!within || !markk || p->byte < markk->byte)) {
		long ind = nindent(bw, p, 0);
		long len = prefix(bw, p, 0);

		if (ind != indent || len != prelen || pisnpara(bw, p))
			break;
	}
	return p;
}

/* Motion commands */

int ubop(BW *bw)
{
	P *q = pdup(bw->cursor, USTR "ubop");

      up:
	while (pisnpara(bw, q) && !pisbof(q) && (!within || !markb || q->byte > markb->byte))
		pprevl(q);
	pbop(bw, q);
	if (q->byte != bw->cursor->byte) {
		pset(bw->cursor, q);
		prm(q);
		return 0;
	} else if (!pisbof(q)) {
		prgetc(q);
		goto up;
	} else {
		prm(q);
		return -1;
	}
}

int ueop(BW *bw)
{
	P *q = pdup(bw->cursor, USTR "ueop");

      up:
	while (pisnpara(bw, q) && !piseof(q))
		pnextl(q);
	pbop(bw, q);
	peop(bw, q);
	if (q->byte != bw->cursor->byte) {
		pset(bw->cursor, q);
		prm(q);
		return 0;
	} else if (!piseof(q)) {
		pnextl(q);
		goto up;
	} else {
		prm(q);
		return -1;
	}
}

/* Wrap word.  If 'french' is set, only one space will be placed
 * after . ? or !
 */

void wrapword(BW *bw, P *p, long int indent, int french, int no_over, unsigned char *indents)
{
	P *q;
	P *r;
	P *s;
	int rmf = 0;
	int c;
	long to = p->byte;
	int my_indents = 0;
	
	/* autoindent when called by utype */
	if (!indents) {
		/* Get indentation prefix from beginning of line */
		s = pdup(p, USTR "wrapword");
		p_goto_bol(s);
		pbop(bw, s);
		/* Record indentation of second line of paragraph, of first
		 * line if there is only one line */
		q = pdup(s, USTR "wrapword");
		pnextl(q);
		if (q->line < p->line) {
			/* Second line */
			P *r = pdup(q, USTR "wrapword");

			indent = nindent(bw, q, 0);
			pcol(r, indent);
			indents = brs(q, r->byte - q->byte);
			prm(r);
		} else {
			/* First line */
			P *r = pdup(s, USTR "uformat");
			int x, y;

			indent = nindent(bw, s, 1);
			pcol(r, indent);
			indents = brs(s, r->byte - s->byte);
			prm(r);
			if (!bw->o.autoindent) {
				/* Don't indent second line of single-line paragraphs if autoindent is off */
				int x = zlen(indents);
				int orgx = x;
				while (x && (indents[x - 1] == ' ' || indents[x - 1] == '\t'))
					indents[--x] = 0;
				if (x && orgx != x) {
					indents[x++] = ' ';
					indents[x] = 0;
				}
				indent = txtwidth1(bw->o.charmap, bw->o.tab, indents, x);
			}
			for (x = 0; indents[x] && (indents[x] == ' ' || indents[x] == '\t'); ++x);
			y = zlen(indents);
			while (y && (indents[y - 1] == ' ' || indents[y - 1] == '\t'))
				--y;
			/* Don't duplicate bullet */
			if (y && ((indents[y - 1] == '*' && !cpara(bw, '*')) || (indents[y - 1] == '-' && !cpara(bw, '-'))) &&
			    (y == 1 || indents[y - 2] == ' ' || indents[y - 2] == '\t'))
			    	indents[y - 1] = ' ';
			/* Fix C comment */
			if (indents[x] == '/' && indents[x + 1] == '*')
				indents[x] = ' ';
		}
		if (bw->o.lmargin > indent) {
			int x;
			for (x = 0; indents[x] == ' ' || indents[x] == '\t'; ++x);
			if (!indents[x]) {
				joe_free(indents);
				indent = bw->o.lmargin;
				indents = joe_malloc(indent+1);
				for (x = 0; x != indent; ++x)
					indents[x] = ' ';
				indents[x] = 0;
			}
		}
		my_indents = 1;
		prm(q);
		prm(s);
	}


/*
	if(!indents) {
		int f = 0;
		P *r = pdup(p);

		p_goto_bol(r);
		q = pdup(r);
		while(cpara(c = brc(q))) {
			if(!joe_isblank(c))
				f = 1;
			pgetc(q);
		}
		if(f) {
			indents = brs(r, q->byte-r->byte);
			rmf = 1;
			if(indents[0] == '/' && indents[1] == '*')
				indents[0] = ' ';
		}
		prm(r);
		prm(q);
	}
*/

	/* Get to beginning of word */
	while (!pisbol(p) && piscol(p) > indent && !joe_isblank(p->b->o.charmap, prgetc(p)))
		/* do nothing */;

	/* If we found the beginning of a word... */
	if (!pisbol(p) && piscol(p) > indent) {
		/* Move q to two (or one if 'french' is set) spaces after end of previous
		   word */
		q = pdup(p, USTR "wrapword");
		while (!pisbol(q))
			if (!joe_isblank(p->b->o.charmap, (c = prgetc(q)))) {
				pgetc(q);
				if ((c == '.' || c == '?' || c == '!')
				    && q->byte != p->byte && !french)
					pgetc(q);
				break;
			}
		pgetc(p);

		/* Delete space between start of word and end of previous word */
		to -= p->byte - q->byte;
		bdel(q, p);
		prm(q);

		if (bw->o.flowed) {
			binsc(p, ' ');
			pgetc(p);
			++to;
		}

		/* Move word to beginning of next line */
		binsc(p, '\n');
		
		/* When overtype is on, do not insert lines */
		if (!no_over && p->b->o.overtype){
			/* delete the next line break which is unnecessary */
			r = pdup(p, USTR "wrapword");
			/* p_goto_eol(r); */
			pgetc(r);
			p_goto_eol(r);
			s = pdup(r, USTR "wrapword");
			pgetc(r);
			bdel(s,r);
			binsc(r, ' ');
			
			/* Now we got to take care that all subsequent lines are not longer than the right margin */
			/* Move cursor to right margin */
			pfwrd(r, r->b->o.rmargin - r->col);
			
			/* Make a copy of the cursor and move the copied cursor to the end of the line */
			prm(s);
			s = pdup(r, USTR "wrapword");
			p_goto_eol(s);
			
			/* If s is located behind r then the line goes beyond the right margin and we need to call wordwrap() for that line. */
/*
			if (r->byte < s->byte){
				wrapword(bw, r, indent, french, 1, indents);
			}
*/			
			prm(r);
			prm(s);
		}
		
		++to;
		if (p->b->o.crlf)
			++to;
		pgetc(p);

		/* Indent to left margin */
		if (indents) {
			binss(p, indents);
			to += zlen(indents);
		} else
			while (indent--) {
				binsc(p, ' ');
				++to;
			}

		if (rmf)
			joe_free(indents);
	}

	/* Move cursor back to original position */
	pfwrd(p, to - p->byte);
	if (my_indents)
		joe_free(indents);
}

/* Reformat paragraph */

int uformat(BW *bw)
{
	long indent;
	unsigned char *indents;
	B *buf;
	P *b;
	long curoff;
	int c;
	P *p, *q;

	p = pdup(bw->cursor, USTR "uformat");
	p_goto_bol(p);

	/* Do nothing if we're not on a paragraph line */
	if (pisnpara(bw, p)) {
		prm(p);
		return 0;
	}

	/* Move p to beginning of paragraph, bw->cursor to end of paragraph and
	 * set curoff to original cursor offset within the paragraph */
	pbop(bw, p);
	curoff = bw->cursor->byte - p->byte;
	pset(bw->cursor, p);
	peop(bw, bw->cursor);

	/* Ensure that paragraph ends on a beginning of a line */
	if (!pisbol(bw->cursor))
		binsc(bw->cursor, '\n'), pgetc(bw->cursor);

	/* Record indentation of second line of paragraph, of first line if there
	 * is only one line */
	q = pdup(p, USTR "uformat");
	pnextl(q);
	if (q->line != bw->cursor->line) {
		P *r = pdup(q, USTR "uformat");

		indent = nindent(bw, q, 0);
		pcol(r, indent);
		indents = brs(q, r->byte - q->byte);
		prm(r);
	} else {
		P *r = pdup(p, USTR "uformat");
		int x, y;
		indent = nindent(bw, p, 1); /* allowing * and - here */
		pcol(r, indent);
		indents = brs(p, r->byte - p->byte);
		prm(r);
		if (!bw->o.autoindent) {
			/* Don't indent second line of single-line paragraphs if autoindent is off */
			int x = zlen(indents);
			while (x && (indents[x - 1] == ' ' || indents[x - 1] == '\t'))
				indents[--x] = 0;
			if (x) {
				indents[x++] = ' ';
				indents[x] = 0;
			}
			indent = txtwidth1(bw->o.charmap, bw->o.tab, indents, x);
		}
		for (x = 0; indents[x] && (indents[x] == ' ' || indents[x] == '\t'); ++x);
		y = zlen(indents);
		while (y && (indents[y - 1] == ' ' || indents[y - 1] == '\t'))
			--y;
		/* Don't duplicate if it looks like a bullet */
		if (y && ((indents[y - 1] == '*' && !cpara(bw, '*')) || (indents[y - 1] == '-' && !cpara(bw, '-'))) &&
		    (y == 1 || indents[y - 2] == ' ' || indents[y - 2] == '\t'))
			indents[y - 1] = ' ';
		/* Fix C comment */
		if (indents[x] == '/' && indents[x + 1] == '*')
			indents[x] = ' ';
	}
	prm(q);

	/* But if the left margin is greater, we use that instead */
	if (bw->o.lmargin > indent) {
		int x;
		for (x = 0; indents[x] == ' ' || indents[x] == '\t'; ++x);
		if (!indents[x]) {
			joe_free(indents);
			indent = bw->o.lmargin;
			indents = joe_malloc(indent+1);
			for (x = 0; x != indent; ++x)
				indents[x] = ' ';
			indents[x] = 0;
		}
	}

	/* Cut paragraph into new buffer */
	
	/* New buffer needs to inherit UTF-8 and CR-LF options */
	buf = bcpy(p, bw->cursor);
	buf->o.crlf = p->b->o.crlf;
	buf->o.charmap = p->b->o.charmap;
	bdel(p, bw->cursor);

	/* text is in buffer.  insert it at cursor */

	b = pdup(buf->bof, USTR "uformat");

	/* First line: preserve whitespace within this line
	   (but still apply french spacing after periods) */
	while (!piseof(b)) {
		c = brch(b);
		if (joe_isblank(b->b->o.charmap,c) || c == '\n') {
			int f = 0;

			/* Stop if we're at end of line */
			if (c == '\n' || piseolblank(b))
				break;

			/* Set f if previous character was '.', '?' or '!' */
			if (!pisbof(b)) {
				P *d = pdup(b, USTR "uformat");
				int g = prgetc(d);
				if (g=='.' || g=='?' || g=='!') {
					f = 1;
				}
				prm(d);
			}
			
			if (f) {
				/* Skip past the whitespace. */
				while (joe_isblank(b->b->o.charmap, brch(b))) {
					if(b->byte == curoff) 
						pset(bw->cursor, p);
					pgetc(b);
				}

				/* Insert proper amount of whitespace */
				if (!piseof(b)) {
					if (!bw->o.french)
						binsc(p, ' '), pgetc(p);
					binsc(p, ' ');
					pgetc(p);
				}
			} else {
				/* Insert whitespace character, advance pointer */
				binsc(p, pgetc(b));
				pgetc(p);
			}
		} else {
			/* Insert characters of word and wrap if necessary */
			if (b->byte == curoff)
				pset(bw->cursor, p);

			binsc(p, pgetc(b));
			pgetc(p);
			if (piscol(p) > bw->o.rmargin)
				wrapword(bw, p, indent, bw->o.french, 1, indents);
		}
	}

	/* Remaining lines: collapse whitespace */

	while (!piseof(b)) {
		c = brch(b);
		if (joe_isblank(b->b->o.charmap,c) || c == '\n') {
			int f = 0;
			P *d;
			int g;

			/* Detect end of sentence */
			d=pdup(b, USTR "uformat");
			g=prgetc(d);
			if (g=='.' || g=='?' || g=='!') {
				f = 1;
			}
			prm(d);
			
			/* Skip past the whitespace.  Skip over indentations */
		      loop:
			
			c = brch(b);
			if (c == '\n') {
				if (b->byte == curoff)
					pset(bw->cursor, p);

				pgetc(b);
				while (cpara(bw, (c=brch(b)))) {
					if (b->byte == curoff)
						pset(bw->cursor, p);
					pgetc(b);
				}
			}

			if (joe_isblank(b->b->o.charmap,c)) {
				if(b->byte == curoff)
					pset(bw->cursor, p);
				pgetc(b);
				goto loop;
			}

			/* Insert proper amount of whitespace */
			if (!piseof(b)) {
				if (f && !bw->o.french)
					binsc(p, ' '), pgetc(p);
				binsc(p, ' ');
				pgetc(p);
			}
		} else {
			/* Insert characters of word and wrap if necessary */
			if (b->byte == curoff)
				pset(bw->cursor, p);

			binsc(p, pgetc(b));
			pgetc(p);
			if (piscol(p) > bw->o.rmargin)
				wrapword(bw, p, indent, bw->o.french, 1, indents);
		}
	}

	binsc(p, '\n');
	prm(p);
	brm(buf);
	joe_free(indents);
	return 0;
}

/* Format entire block */

int ufmtblk(BW *bw)
{
	if (markv(1) && bw->cursor->byte >= markb->byte && bw->cursor->byte <= markk->byte) {
		markk->end = 1;
		utomarkk(bw);
		within = 1;
		do {
			ubop(bw), uformat(bw);
		} while (bw->cursor->byte > markb->byte);
		within = 0;
		markk->end = 0;
		if (lightoff)
			unmark(bw);
		return 0;
	} else
		return uformat(bw);
}
