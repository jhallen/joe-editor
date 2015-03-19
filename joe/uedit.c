/*
 *	Basic user edit functions
 *	Copyright
 * 		(C) 1992 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */
#include "types.h"

/***************/
/* Global options */
int pgamnt = -1;		/* No. of PgUp/PgDn lines to keep */

/* 
 * Move cursor to beginning of line
 */
int u_goto_bol(BW *bw)
{
	if (bw->o.hex) {
		pbkwd(bw->cursor,bw->cursor->byte%16);
	} else {
		p_goto_bol(bw->cursor);
	}
	return 0;
}

/*
 * Move cursor to first non-whitespace character, unless it is
 * already there, in which case move it to beginning of line
 */
int uhome(BW *bw)
{
	P *p;

	if (bw->o.hex) {
		return u_goto_bol(bw);
	}

	p = pdup(bw->cursor, USTR "uhome");

	if (bw->o.indentfirst) {
		if ((bw->o.smarthome) && (piscol(p) > pisindent(p))) { 
			p_goto_bol(p);
			while (joe_isblank(p->b->o.charmap,brc(p)))
				pgetc(p);
		} else
			p_goto_bol(p);
	} else {
		if (bw->o.smarthome && piscol(p)==0 && pisindent(p)) {
			while (joe_isblank(p->b->o.charmap,brc(p)))
				pgetc(p);
		} else
			p_goto_bol(p);
	}

	pset(bw->cursor, p);
	prm(p);
	return 0;
}

/*
 * Move cursor to end of line
 */
int u_goto_eol(BW *bw)
{
	if (bw->o.hex) {
		if (bw->cursor->byte + 15 - bw->cursor->byte%16 > bw->b->eof->byte)
			pset(bw->cursor,bw->b->eof);
		else
			pfwrd(bw->cursor, 15 - bw->cursor->byte%16);
	} else
		p_goto_eol(bw->cursor);
	return 0;
}

/*
 * Move cursor to beginning of file
 */
int u_goto_bof(BW *bw)
{
	p_goto_bof(bw->cursor);
	return 0;
}

/*
 * Move cursor to end of file
 */
int u_goto_eof(BW *bw)
{
	if (bw->b->vt && bw->b->pid) {
		pset(bw->cursor, bw->b->vt->vtcur);
	} else {
		p_goto_eof(bw->cursor);
	}
	return 0;
}

/*
 * Move cursor left
 */
int u_goto_left(BW *bw)
{
	if (bw->o.hex) {
		if (prgetb(bw->cursor) != NO_MORE_DATA) {
			return 0;
		} else {
			return -1;
		}
	}
	if (bw->o.picture) {
		if (bw->cursor->xcol) {
			--bw->cursor->xcol;
			pcol(bw->cursor,bw->cursor->xcol);
			return 0;
		} else
			return -1;
	} else {
		/* Have to do ECHKXCOL here because of picture mode */
		if (bw->cursor->xcol != piscol(bw->cursor)) {
			bw->cursor->xcol = piscol(bw->cursor);
			return 0;
		} else if (prgetc(bw->cursor) != NO_MORE_DATA) {
			bw->cursor->xcol = piscol(bw->cursor);
			return 0;
		} else {
			return -1;
		}
	}
}

/*
 * Move cursor right
 */
int u_goto_right(BW *bw)
{
	if (bw->o.hex) {
		if (pgetb(bw->cursor) != NO_MORE_DATA) {
			return 0;
		} else {
			return -1;
		}
	}
	if (bw->o.picture) {
		++bw->cursor->xcol;
		pcol(bw->cursor,bw->cursor->xcol);
		return 0;
	} else {
		int rtn;
		if (pgetc(bw->cursor) != NO_MORE_DATA) {
			bw->cursor->xcol = piscol(bw->cursor);
			rtn = 0;
		} else {
			rtn = -1;
		}
		/* Have to do EFIXXCOL here because of picture mode */
		if (bw->cursor->xcol != piscol(bw->cursor))
			bw->cursor->xcol = piscol(bw->cursor);
		return rtn;
	}
}

/*
 * Move cursor to beginning of previous word or if there isn't 
 * previous word then go to beginning of the file
 *
 * WORD is a sequence non-white-space characters
 */
int p_goto_prev(P *ptr)
{
	P *p = pdup(ptr, USTR "p_goto_prev");
	struct charmap *map=ptr->b->o.charmap;
	int c = prgetc(p);

	if (joe_isalnum_(map,c)) {
		while (joe_isalnum_(map,(c=prgetc(p))))
			/* Do nothing */;
		if (c != NO_MORE_DATA)
			pgetc(p);
	} else if (joe_isspace(map,c) || joe_ispunct(map,c)) {
		while ((c=prgetc(p)), (joe_isspace(map,c) || joe_ispunct(map,c)))
			/* Do nothing */;
		while(joe_isalnum_(map,(c=prgetc(p))))
			/* Do nothing */;
		if (c != NO_MORE_DATA)
			pgetc(p);
	}
	pset(ptr, p);
	prm(p);
	return 0;
}

int u_goto_prev(BW *bw)
{
	return p_goto_prev(bw->cursor);
}

/*
 * Move cursor to end of next word or if there isn't 
 * next word then go to end of the file
 *
 * WORD is a sequence non-white-space characters
 */
int p_goto_next(P *ptr)
{
	P *p = pdup(ptr, USTR "p_goto_next");
	struct charmap *map=ptr->b->o.charmap;
	int c = brch(p);
	int rtn = -1;

	if (joe_isalnum_(map,c)) {
		rtn = 0;
		while (joe_isalnum_(map,(c = brch(p))))
			pgetc(p);
	} else if (joe_isspace(map,c) || joe_ispunct(map,c)) {
		while (joe_isspace(map, (c = brch(p))) || joe_ispunct(map,c))
			pgetc(p);
		while (joe_isalnum_(map,(c = brch(p)))) {
			rtn = 0;
			pgetc(p);
		}
	} else
		pgetc(p);
	pset(ptr, p);
	prm(p);
	return rtn;
}

int u_goto_next(BW *bw)
{
	return p_goto_next(bw->cursor);
}

static P *pboi(P *p)
{
	p_goto_bol(p);
	while (joe_isblank(p->b->o.charmap,brch(p)))
		pgetc(p);
	return p;
}

static int pisedge(P *p)
{
	P *q;
	int c;

	if (pisbol(p))
		return -1;
	if (piseol(p))
		return 1;
	q = pdup(p, USTR "pisedge");
	pboi(q);
	if (q->byte == p->byte)
		goto left;
	if (joe_isblank(p->b->o.charmap,(c = brch(p)))) {
		pset(q, p);
		if (joe_isblank(p->b->o.charmap,prgetc(q)))
			goto no;
		if (c == '\t')
			goto right;
		pset(q, p);
		pgetc(q);
		if (pgetc(q) == ' ')
			goto right;
		goto no;
	} else {
		pset(q, p);
		c = prgetc(q);
		if (c == '\t')
			goto left;
		if (c != ' ')
			goto no;
		if (prgetc(q) == ' ')
			goto left;
		goto no;
	}

      right:prm(q);
	return 1;
      left:prm(q);
	return -1;
      no:prm(q);
	return 0;
}

int upedge(BW *bw)
{
	if (prgetc(bw->cursor) == NO_MORE_DATA)
		return -1;
	while (pisedge(bw->cursor) != -1)
		prgetc(bw->cursor);
	return 0;
}

int unedge(BW *bw)
{
	if (pgetc(bw->cursor) == NO_MORE_DATA)
		return -1;
	while (pisedge(bw->cursor) != 1)
		pgetc(bw->cursor);
	return 0;
}

/* Move cursor to matching delimiter */
/*
 * begin end
 *
 * module endmodule
 *
 * function endfunction
 * 
 * <word </word
 *
 * if elif else fi
 *
 * do done
 *
 * case esac, endcase
 *
 * #if #ifdef #ifndef #elseif #else #endif
 *
 * `ifdef  `ifndef  `else `endif
 *
 */

/* A delimiter set list is a : separated list of delimiter sets.
   A delimiter set is two or more groups of matching delimiters.
   A group is a list of equivalent delimiters separated with |.

   For example, here is a delimiter set list, with three sets:
   	"case|casex|casez=endcase:begin=end:if=elif=else=fi:

   In the first delimiter set: "case," "casex" and "casez" all match with
   "endcase." In the third set: "if" matches with "elif," which matches with
   "else," which finally matches with "fi".

   The search goes forward if the delimiter matches any words of any group
   but the last of the set.  If the delimiter matches a word in the last
   group, the search goes backward to the first delimiter.
*/

/* Return pointer to first matching set in delimiter set list.  Returns NULL
   if no matches were found. */

unsigned char *next_set(unsigned char *set)
{
	while (*set && *set!=':')
		++set;
	if (*set==':')
		++set;
	return set;
}

unsigned char *next_group(unsigned char *group)
{
	while (*group && *group!='=' && *group!=':')
		++group;
	if (*group=='=')
		++group;
	return group;
}

unsigned char *next_word(unsigned char *word)
{
	while (*word && *word!='|' && *word!='=' && *word!=':')
		++word;
	if (*word=='|')
		++word;
	return word;
}

int match_word(unsigned char *word,unsigned char *s)
{
	while (*word==*s && *s && *word) {
		++word;
		++s;
	}
	if (!*s && (!*word || *word=='|' || *word=='=' || *word==':'))
		return 1;
	else
		return 0;
}

int is_in_group(unsigned char *group,unsigned char *s)
{
	while (*group && *group!='=' && *group!=':') {
		if (match_word(group, s))
			return 1;
		else
			group = next_word(group);
	}
	return 0;
}

int is_in_any_group(unsigned char *group,unsigned char *s)
{
	while (*group && *group!=':') {
		if (match_word(group, s))
			return 1;
		else {
			group = next_word(group);
			if (*group == '=')
				++group;
		}
	}
	return 0;
}

unsigned char *find_last_group(unsigned char *group)
{
	unsigned char *s;
	for (s = group; *s && *s!=':'; s=next_group(s))
		group = s;
	return group;
}

#define MAX_WORD_SIZE 255

/* Search for matching delimiter: ignore things in comments or strings */

int tomatch_char_or_word(BW *bw,int word_delimiter,int c,int f,unsigned char *set,unsigned char *group,int backward)
{
	P *p = pdup(bw->cursor, USTR "tomatch_char_or_word");
	P *q = pdup(p, USTR "tomatch_char_or_word");
	unsigned char *last_of_set = USTR "";
	unsigned char buf[MAX_WORD_SIZE+1];
	int len;
	int query_highlighter = bw->o.highlighter_context && bw->o.syntax && bw->db;
	int initial_context = 0;
	int col = 0;
	int cnt = 0;	/* No. levels of delimiters we're in */
	int d;

	if (word_delimiter) {
		if (backward) {
			last_of_set = find_last_group(set);
			p_goto_next(p);
			p_goto_prev(p);
		} else {
			last_of_set = find_last_group(group);
			p_goto_next(p);
		}
		pset(q, p);
	}

	if (query_highlighter) {
		col = -1;
		do {
			d = prgetc(q);
			++col;
		} while (d != NO_MORE_DATA && d != '\n');
		if (d != NO_MORE_DATA)
			pgetc(q);
		parse(bw->o.syntax, q, lattr_get(bw->db, bw->o.syntax, q, q->line));
		initial_context = attr_buf[col] & CONTEXT_MASK;
	}

	if (backward) {
		/* Backward search */
		while ((d = prgetc(p)) != NO_MORE_DATA) {
			int peek;
			int peek1;

			if (query_highlighter && d == '\n'){
				pset(q, p);
				col = -1;
				do {
					d = prgetc(q);
					++col;
				} while (d != NO_MORE_DATA && d != '\n');
				if (d != NO_MORE_DATA)
					pgetc(q);
				parse(bw->o.syntax, q, lattr_get(bw->db, bw->o.syntax, q, q->line));
				continue;
			}

			peek = prgetc(p);
			peek1 = 0;
			if(peek != NO_MORE_DATA) {
				peek1 = prgetc(p);
				if (peek1 != NO_MORE_DATA)
					pgetc(p);
				pgetc(p);
			}
			--col;

			if (query_highlighter
			    && (attr_buf[col] & (CONTEXT_COMMENT | CONTEXT_STRING))
			    && (attr_buf[col] & CONTEXT_MASK) != initial_context) {
				/* Ignore */
			} else if (!query_highlighter
			           && (bw->o.cpp_comment || bw->o.pound_comment ||
			               bw->o.semi_comment || bw->o.tex_comment || bw->o.vhdl_comment) && d == '\n') {
				int cc;
				pset(q, p);
				p_goto_bol(q);
				while((cc = pgetc(q)) != '\n') {
					if (cc == '\\') {
						if (pgetc(q) == '\n')
							break;
					} else if (bw->o.pound_comment && cc == '$' && brch(q)=='#') {
						pgetc(q);
					} else if(!bw->o.no_double_quoted && cc=='"') {
						while ((cc = pgetc(q)) != '\n')
							if (cc == '"') break;
							else if (cc == '\\') if ((cc = pgetc(q)) == '\n') break;
						if (cc == '\n')
							break;
					} else if (bw->o.single_quoted && cc == '\'') {
						while((cc = pgetc(q)) != '\n')
							if (cc == '\'') break;
							else if (cc == '\\') if ((cc = pgetc(q)) == '\n') break;
						if (cc == '\n')
							break;
					} else if (bw->o.cpp_comment && cc == '/') {
						if (brch(q)=='/') {
							prgetc(q);
							pset(p,q);
							break;
						}
					} else if (bw->o.vhdl_comment && cc == '-') {
						if (brch(q)=='-') {
							prgetc(q);
							pset(p,q);
							break;
						}
					} else if (bw->o.pound_comment && cc == '#') {
						pset(p,q);
						break;
					} else if (bw->o.semi_comment && cc == ';') {
						pset(p,q);
						break;
					} else if (bw->o.tex_comment && cc == '%') {
						pset(p,q);
						break;
					}
				}
			} else if (peek == '\\' && peek1!='\\') {
				/* Ignore */
			} else if (!query_highlighter && !bw->o.no_double_quoted && d == '"') {
				while((d = prgetc(p)) != NO_MORE_DATA) {
					if (d == '"') {
						d = prgetc(p);
						if (d != '\\') {
							if (d != NO_MORE_DATA)
								pgetc(p);
							break;
						}
					}
				}
			} else if (!query_highlighter && bw->o.single_quoted && d == '\'' && c != '\'' && c != '`') {
				while((d = prgetc(p)) != NO_MORE_DATA)
					if (d == '\'') {
						d = prgetc(p);
						if (d != '\\') {
							if (d != NO_MORE_DATA)
								pgetc(p);
							break;
						}
					}
			} else if (!query_highlighter && bw->o.c_comment && d == '/') {
				d = prgetc(p);
				if (d == '*') {
					d = prgetc(p);
					do {
						do {
							if (d == '*') break;
						} while ((d = prgetc(p)) != NO_MORE_DATA);
						d = prgetc(p);
					} while (d != NO_MORE_DATA && d != '/');
				} else if (d != NO_MORE_DATA)
					pgetc(p);
			} else if (word_delimiter) {
				if ((d >= 'a' && d <= 'z') || (d>='A' && d<='Z') || (d>='0' && d<='9') || d=='_') {
					int x;
					int flg=0;
					P *r;
					len=0;
					while ((d >= 'a' && d <= 'z') || (d>='A' && d<='Z') || d=='_' || (d>='0' && d<='9')) {
						if(len!=MAX_WORD_SIZE)
							buf[len++]=d;
						d=prgetc(p);
						--col;
					}
					/* ifdef hack */
					r = pdup(p, USTR "tomatch_char_or_word");
					while (d ==' ' || d=='\t')
						d=prgetc(r);
					/* VHDL hack */
					if ((d=='d' || d=='D') && bw->o.vhdl_comment) {
						d=prgetc(r);
						if(d=='n' || d=='N') {
							d=prgetc(r);
							if(d=='e' || d=='E') {
								d=prgetc(r);
								if(d==' ' || d=='\t' || d=='\n' || d==NO_MORE_DATA)
									flg=1;
							}
						}
					}
					prm(r);
					if (d == set[0])
						buf[len++] = d;
					if(d!=NO_MORE_DATA)
						pgetc(p);
					++col;
					buf[len]=0;
					for(x=0;x!=len/2;++x) {
						int e = buf[x];
						buf[x] = buf[len-x-1];
						buf[len-x-1] = e;
					}
					if (is_in_group(last_of_set,buf)) {
						++cnt;
					} else if(is_in_group(set,buf) && !flg && !cnt--) {
						pset(bw->cursor,p);
						prm(q);
						prm(p);
						return 0;
					}
				}
			} else if (d == c) {
				++cnt;
			} else if (d == f && !cnt--) {
				pset(bw->cursor, p);
				prm(q);
				prm(p);
				return 0;
			}
		}
	} else {
		/* Forward search */
		while ((d = pgetc(p)) != NO_MORE_DATA) {
			if (query_highlighter && d == '\n') {
				parse(bw->o.syntax, q, lattr_get(bw->db, bw->o.syntax, q, q->line));
				col = 0;
				continue;
			}

			if (query_highlighter
			    && (attr_buf[col] & (CONTEXT_COMMENT | CONTEXT_STRING))
			    && (attr_buf[col] & CONTEXT_MASK) != initial_context) {
				/* Ignore */
			} else if (d == '\\') {
				if (!(query_highlighter && brch(p) == '\n')) {
					pgetc(p);
					++col;
				}
			} else if (!query_highlighter && !bw->o.no_double_quoted && d == '"') {
				while ((d = pgetc(p)) != NO_MORE_DATA)
					if (d == '"') break;
					else if (d == '\\') pgetc(p);
			} else if (!query_highlighter && bw->o.single_quoted && d == '\'' && c != '\'' && c != '`') {
				while((d = pgetc(p)) != NO_MORE_DATA)
					if (d == '\'') break;
					else if (d == '\\') pgetc(p);
			} else if (!query_highlighter && d == '$' && brch(p)=='#' && bw->o.pound_comment) {
				pgetc(p);
			} else if (!query_highlighter
			           && ((bw->o.pound_comment && d == '#') ||
				       (bw->o.semi_comment && d == ';') ||
				       (bw->o.tex_comment && d == '%') ||
				       (bw->o.vhdl_comment && d == '-' && brch(p) == '-') ||
				       (bw->o.cpp_comment && d == '/' && brch(p) == '/'))) {
				while ((d = pgetc(p)) != NO_MORE_DATA)
					if (d == '\n')
						break;
			} else if (!query_highlighter && bw->o.c_comment && d == '/' && brch(p) == '*') {
				pgetc(p);
				d = pgetc(p);
				do {
					do {
						if (d == '*') break;
					} while ((d = pgetc(p)) != NO_MORE_DATA);
					d = pgetc(p);
				} while (d != NO_MORE_DATA && d != '/');
			} else if (word_delimiter) {
				if (d == set[0]) {
					/* ifdef hack */
					while ((d = pgetc(p))!=NO_MORE_DATA) {
						++col;
						if (d!=' ' && d!='\t')
							break;
					}
					buf[0]=set[0];
					len=1;
					if (d >= 'a' && d <= 'z')
						goto doit;
					if (d!=NO_MORE_DATA) {
						prgetc(p);
						--col;
					}
				} else if ((d >= 'a' && d <= 'z') || (d>='A' && d<='Z') || d=='_') {
					len=0;
					doit:
					while ((d >= 'a' && d <= 'z') || (d>='A' && d<='Z') || d=='_' || (d>='0' && d<='9')) {
						if(len!=MAX_WORD_SIZE)
							buf[len++]=d;
						d=pgetc(p);
						++col;
					}
					if (d!=NO_MORE_DATA)
						prgetc(p);
					--col;
					buf[len]=0;
					if (is_in_group(set,buf)) {
						++cnt;
					} else if (cnt==0) {
						if (is_in_any_group(group,buf)) {
							if (!((buf[0]>='a' && buf[0]<='z') || (buf[0]>='A' && buf[0]<='Z')))
								pgoto(p,p->byte-len+1);
							else
								pgoto(p,p->byte-len);
							pset(bw->cursor,p);
							prm(q);
							prm(p);
							return 0;
						}
					} else if(is_in_group(last_of_set,buf)) {
						/* VHDL hack */
						if (bw->o.vhdl_comment && (!zcmp(buf,USTR "end") || !zcmp(buf,USTR "END")))
							while((d=pgetc(p))!=NO_MORE_DATA) {
								++col;
								if (d==';' || d=='\n') {
									prgetc(p);
									--col;
									break;
								}
							}
						--cnt;
					}
				}
			} else if (d == c) {
				++cnt;
			} else if (d == f && !--cnt) {
				prgetc(p);
				pset(bw->cursor, p);
				prm(q);
				prm(p);
				return 0;
			}
			++col;
		}
	}
	prm(q);
	prm(p);
	return -1;
}

int tomatch_char(BW *bw,int c,int f,int dir)
{
	return tomatch_char_or_word(bw, 0, c, f, 0, 0, dir == -1);
}

int tomatch_word(BW *bw,unsigned char *set,unsigned char *group)
{
	return tomatch_char_or_word(bw, 1, 0, 0, set, group, !*group || *group==':');
}

/* Return true if <foo /> */

int xml_startend(P *p)
{
	int c, d=0;
	p=pdup(p, USTR "xml_startend");
	while((c=pgetc(p)) != NO_MORE_DATA) {
		if(d=='/' && c=='>') {
			prm(p);
			return 1;
		} else if(c=='>')
			break;
		d=c;
	}
	prm(p);
	return 0;
}

int tomatch_xml(BW *bw,unsigned char *word,int dir)
{
	if (dir== -1) {
		/* Backward search */
		P *p=pdup(bw->cursor, USTR "tomatch_xml");
		int c;
		unsigned char buf[MAX_WORD_SIZE+1];
		int len;
		int cnt = 1;
		p_goto_next(p);
		p_goto_prev(p);
		while ((c=prgetc(p)) != NO_MORE_DATA) {
			if ((c >= 'a' && c <= 'z') || (c>='A' && c<='Z') || (c>='0' && c<='9') || c=='.' || c==':' || c=='-' || c=='_') {
				int x;
				len=0;
				while ((c >= 'a' && c <= 'z') || (c>='A' && c<='Z') || c=='_' || (c>='0' && c<='9') || c=='.' ||
				       c == '-' || c == ':') {
					if(len!=MAX_WORD_SIZE)
						buf[len++]=c;
					c=prgetc(p);
				}
				if(c!=NO_MORE_DATA)
					c = pgetc(p);
				buf[len]=0;
				for(x=0;x!=len/2;++x) {
					int d = buf[x];
					buf[x] = buf[len-x-1];
					buf[len-x-1] = d;
				}
				if (!zcmp(word,buf) && !xml_startend(p)) {
					if (c=='<') {
						if (!--cnt) {
							pset(bw->cursor,p);
							prm(p);
							return 0;
						}
					}
					else if (c=='/') {
						++cnt;
					}
				}
			}
		}
		prm(p);
		return -1;
	} else {
		/* Forward search */
		P *p=pdup(bw->cursor, USTR "tomatch_xml");
		int c;
		unsigned char buf[MAX_WORD_SIZE+1];
		int len;
		int cnt = 1;
		while ((c=pgetc(p)) != NO_MORE_DATA) {
			if (c == '<') {
				int e = 1;
				c = pgetc(p);
				if (c=='/') {
					e = 0;
					c = pgetc(p);
				}
				if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c=='_' || c==':' || c=='-' || c=='.') {
					len=0;
					while ((c >= 'a' && c <= 'z') || (c>='A' && c<='Z') || c=='_' || c==':' || c=='-' || c=='.' ||
					       (c >= '0' && c <= '9')) {
						if(len!=MAX_WORD_SIZE)
							buf[len++]=c;
						c=pgetc(p);
					}
					if (c!=NO_MORE_DATA)
						prgetc(p);
					buf[len]=0;
					if (!zcmp(word,buf) && !xml_startend(p)) {
						if (e) {
							++cnt;
						}
						else if (!--cnt) {
							pgoto(p,p->byte-len);
							pset(bw->cursor,p);
							prm(p);
							return 0;
						}
					}
				} else if (c!=NO_MORE_DATA) {
					prgetc(p);
				}
			}
		}
		prm(p);
		return -1;
	}
}

void get_xml_name(P *p,unsigned char *buf)
{
	int c;
	int len=0;
	p=pdup(p, USTR "get_xml_name");
	c=pgetc(p);
	while ((c >= 'a' && c <= 'z') || (c>='A' && c<='Z') || c=='_' || c==':' || c=='-' || c=='.' ||
	       (c >= '0' && c <= '9')) {
		if(len!=MAX_WORD_SIZE)
			buf[len++]=c;
		c=pgetc(p);
	}
	buf[len]=0;
	prm(p);
}

void get_delim_name(P *q,unsigned char *buf)
{
	int c;
	int len=0;
	P *p=pdup(q, USTR "get_delim_name");
	while ((c=prgetc(p))!=NO_MORE_DATA)
		if (c!=' ' && c!='\t')
			break;
	prm(p);
	/* preprocessor directive hack */
	if (c=='#' || c=='`')
		buf[len++]=c;

	p=pdup(q, USTR "get_delim_name");
	c=pgetc(p);
	while ((c >= 'a' && c <= 'z') || (c>='A' && c<='Z') || c=='_' || (c >= '0' && c <= '9')) {
		if(len!=MAX_WORD_SIZE)
			buf[len++]=c;
		c=pgetc(p);
	}
	buf[len]=0;
	prm(p);
}

int utomatch(BW *bw)
{
	int d;
	int c,			/* Character under cursor */
	 f,			/* Character to find */
	 dir;			/* 1 to search forward, -1 to search backward */

	c = brch(bw->cursor);

	/* Check for word delimiters */
	if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_') {
		P *p;
		unsigned char buf[MAX_WORD_SIZE+1];
		unsigned char buf1[MAX_WORD_SIZE+1];
		unsigned char *list = bw->b->o.text_delimiters;
		unsigned char *set;
		unsigned char *group;
		unsigned char *word;
		int flg=0;
		p=pdup(bw->cursor, USTR "utomatch");
		p_goto_next(p);
		p_goto_prev(p);
		get_delim_name(p,buf);
		get_xml_name(p,buf1);
		c=prgetc(p);
		if (c=='<')
			flg = 1;
		else if (c=='/') {
			c=prgetc(p);
			if (c=='<')
				flg = -1;
		}
		prm(p);

		if (flg) {
			return tomatch_xml(bw, buf1, flg);
		}

		for (set = list; set && *set; set = next_set(set)) {
			for (group = set; *group && *group!='=' && *group!=':'; group=next_group(group)) {
				for (word = group; *word && *word!='|' && *word!='=' && *word!=':'; word=next_word(word)) {
					if (match_word(word, buf)) {
						return tomatch_word(bw, set, next_group(word));
					}
				}
			}
				
		}

		/* We don't know the word, so start a search */
		return dofirst(bw, 0, 0, buf);
	}

	switch (c) {
	case '/':
		dir = 1;
		pgetc(bw->cursor);
		f = brch(bw->cursor);
		prgetc(bw->cursor);
		if(f=='*') f = '/';
		else {
			dir = -1;
			f = prgetc(bw->cursor);
			if (f!=NO_MORE_DATA)
				pgetc(bw->cursor);
			if(f=='*') f = '/';
			else
				return -1;
		}
		break;
	case '*':
		dir = -1;
		pgetc(bw->cursor);
		f = brch(bw->cursor);
		prgetc(bw->cursor);
		if(f=='/') f = '*';
		else {
			dir = 1;
			f = prgetc(bw->cursor);
			if (f!=NO_MORE_DATA)
				pgetc(bw->cursor);
			if(f=='/') f = '*';
			else
				return -1;
		}
		break;
	case '(':
		f = ')';
		dir = 1;
		break;
	case '[':
		f = ']';
		dir = 1;
		break;
	case '{':
		f = '}';
		dir = 1;
		break;
	case '`':
		f = '\'';
		dir = 1;
		break;
	case '<':
		f = '>';
		dir = 1;
		break;
	case ')':
		f = '(';
		dir = -1;
		break;
	case ']':
		f = '[';
		dir = -1;
		break;
	case '}':
		f = '{';
		dir = -1;
		break;
	case '\'':
		f = '`';
		dir = -1;
		break;
	case '>':
		f = '<';
		dir = -1;
		break;
	default:
		return -1;
	}

	/* Search for matching C comment */
	if (f == '/' || f == '*') {
		P *p = pdup(bw->cursor, USTR "utomatch");
		if (dir == 1) {
			d = pgetc(p);
			do {
				do {
					if (d == '*') break;
				} while ((d = pgetc(p)) != NO_MORE_DATA);
				d = pgetc(p);
			} while (d != NO_MORE_DATA && d != '/');
			if (d == '/') {
				if (f == '*') {
					prgetc(p);
				}
				pset(bw->cursor,p);
				prgetc(bw->cursor);
			}
		} else {
			d = prgetc(p);
			do {
				do {
					if (d == '*') break;
				} while ((d = prgetc(p)) != NO_MORE_DATA);
				d = prgetc(p);
			} while (d != NO_MORE_DATA && d != '/');
			if (d == '/') {
				if (f == '*') {
					pgetc(p);
				}
				pset(bw->cursor,p);
			}
		}
		prm(p);
		if (d == NO_MORE_DATA)
			return -1;
		else
			return 0;
	}

	return tomatch_char(bw, c, f, dir);
}

/* Move cursor up */

int uuparw(BW *bw)
{
	if (bw->o.hex) {
		if (bw->cursor->byte<16)
			return -1;
		else {
			pbkwd(bw->cursor, 16);
			return 0;
		}
	}
	if (bw->cursor->line) {
		pprevl(bw->cursor);
		pcol(bw->cursor, bw->cursor->xcol);
		return 0;
	} else
		return -1;
}

/* Move cursor down */

int udnarw(BW *bw)
{
	if (bw->o.hex) {
		if (bw->cursor->byte+16 <= bw->b->eof->byte) {
			pfwrd(bw->cursor, 16);
			return 0;
		} else if (bw->cursor->byte != bw->b->eof->byte) {
			pset(bw->cursor, bw->b->eof);
			return 0;
		} else {
			return -1;
		}
	}
	if (bw->cursor->line != bw->b->eof->line) {
		pnextl(bw->cursor);
		pcol(bw->cursor, bw->cursor->xcol);
		return 0;
	} else if(bw->o.picture) {
		p_goto_eol(bw->cursor);
		binsc(bw->cursor,'\n');
		pgetc(bw->cursor);
		pcol(bw->cursor, bw->cursor->xcol);
		return 0;
	} else
		return -1;
}

/* Move cursor to top of window */

int utos(BW *bw)
{
	long col = bw->cursor->xcol;

	pset(bw->cursor, bw->top);
	pcol(bw->cursor, col);
	bw->cursor->xcol = col;
	return 0;
}

/* Move cursor to bottom of window */

int ubos(BW *bw)
{
	long col = bw->cursor->xcol;

	pline(bw->cursor, bw->top->line + bw->h - 1);
	pcol(bw->cursor, col);
	bw->cursor->xcol = col;
	return 0;
}

/* Scroll buffer window up n lines
 * If beginning of file is close, scrolls as much as it can
 * If beginning of file is on-screen, cursor jumps to beginning of file
 *
 * If flg is set: cursor stays fixed relative to screen edge
 * If flg is clr: cursor stays fixed on the buffer line
 */

void scrup(BW *bw, int n, int flg)
{
	int scrollamnt = 0;
	int cursoramnt = 0;
	int x;

	/* Decide number of lines we're really going to scroll */

	if (bw->o.hex) {
		if (bw->top->byte/16 >= n)
			scrollamnt = cursoramnt = n;
		else if (bw->top->byte/16)
			scrollamnt = cursoramnt = bw->top->byte/16;
		else if (flg)
			cursoramnt = bw->cursor->byte/16;
		else if (bw->cursor->byte/16 >= n)
			cursoramnt = n;
	} else {
		if (bw->top->line >= n)
			scrollamnt = cursoramnt = n;
		else if (bw->top->line)
			scrollamnt = cursoramnt = bw->top->line;
		else if (flg)
			cursoramnt = bw->cursor->line;
		else if (bw->cursor->line >= n)
			cursoramnt = n;
	}

	if (bw->o.hex) {
		/* Move top-of-window pointer */
		pbkwd(bw->top,scrollamnt*16);
		/* Move cursor */
		pbkwd(bw->cursor,cursoramnt*16);
		/* If window is on the screen, give (buffered) scrolling command */
		if (bw->parent->y != -1)
			nscrldn(bw->parent->t->t, bw->y, bw->y + bw->h, scrollamnt);
	} else {
		/* Move top-of-window pointer */
		for (x = 0; x != scrollamnt; ++x)
			pprevl(bw->top);
		p_goto_bol(bw->top);

		/* Move cursor */
		for (x = 0; x != cursoramnt; ++x)
			pprevl(bw->cursor);
		p_goto_bol(bw->cursor);
		pcol(bw->cursor, bw->cursor->xcol);

		/* If window is on the screen, give (buffered) scrolling command */
		if (bw->parent->y != -1)
			nscrldn(bw->parent->t->t, bw->y, bw->y + bw->h, scrollamnt);
	}
}

/* Scroll buffer window down n lines
 * If end of file is close, scrolls as much as possible
 * If end of file is on-screen, cursor jumps to end of file
 *
 * If flg is set: cursor stays fixed relative to screen edge
 * If flg is clr: cursor stays fixed on the buffer line
 */

void scrdn(BW *bw, int n, int flg)
{
	int scrollamnt = 0;
	int cursoramnt = 0;
	int x;

	/* How much we're really going to scroll... */
	if (bw->o.hex) {
		if (bw->top->b->eof->byte/16 < bw->top->byte/16 + bw->h) {
			cursoramnt = bw->top->b->eof->byte/16 - bw->cursor->byte/16;
			if (!flg && cursoramnt > n)
				cursoramnt = n;
		} else if (bw->top->b->eof->byte/16 - (bw->top->byte/16 + bw->h) >= n)
			cursoramnt = scrollamnt = n;
		else
			cursoramnt = scrollamnt = bw->top->b->eof->byte/16 - (bw->top->byte/16 + bw->h) + 1;
	} else {
		if (bw->top->b->eof->line < bw->top->line + bw->h) {
			cursoramnt = bw->top->b->eof->line - bw->cursor->line;
			if (!flg && cursoramnt > n)
				cursoramnt = n;
		} else if (bw->top->b->eof->line - (bw->top->line + bw->h) >= n)
			cursoramnt = scrollamnt = n;
		else
			cursoramnt = scrollamnt = bw->top->b->eof->line - (bw->top->line + bw->h) + 1;
	}

	if (bw->o.hex) {
		/* Move top-of-window pointer */
		pfwrd(bw->top,16*scrollamnt);
		/* Move cursor */
		pfwrd(bw->cursor,16*cursoramnt);
		/* If window is on screen, give (buffered) scrolling command to terminal */
		if (bw->parent->y != -1)
			nscrlup(bw->parent->t->t, bw->y, bw->y + bw->h, scrollamnt);
	} else {
		/* Move top-of-window pointer */
		for (x = 0; x != scrollamnt; ++x)
			pnextl(bw->top);

		/* Move cursor */
		for (x = 0; x != cursoramnt; ++x)
			pnextl(bw->cursor);
		pcol(bw->cursor, bw->cursor->xcol);

		/* If window is on screen, give (buffered) scrolling command to terminal */
		if (bw->parent->y != -1)
			nscrlup(bw->parent->t->t, bw->y, bw->y + bw->h, scrollamnt);
	}
}

/* Page up */

int upgup(BW *bw)
{
	if (menu_above) {
		if (bw->parent->link.prev->watom==&watommenu) {
			return umpgup(bw->parent->link.prev->object);
		}
	} else {
		if (bw->parent->link.next->watom==&watommenu) {
			return umpgup(bw->parent->link.next->object);
		}
	}
	bw = (BW *) bw->parent->main->object;

	if (bw->o.hex ? bw->cursor->byte < 16 : !bw->cursor->line)
		return -1;
	if (pgamnt < 0)
		scrup(bw, bw->h / 2 + bw->h % 2, 1);
	else if (pgamnt < bw->h)
		scrup(bw, bw->h - pgamnt, 1);
	else
		scrup(bw, 1, 1);
	return 0;
}

/* Page down */

int upgdn(BW *bw)
{
	if (menu_above) {
		if (bw->parent->link.prev->watom==&watommenu) {
			return umpgdn(bw->parent->link.prev->object);
		}
	} else {
		if (bw->parent->link.next->watom==&watommenu) {
			return umpgdn(bw->parent->link.next->object);
		}
	}
	bw = (BW *) bw->parent->main->object;
	if (bw->o.hex ? bw->cursor->byte/16 == bw->b->eof->byte/16 : bw->cursor->line == bw->b->eof->line)
		return -1;
	if (pgamnt < 0)
		scrdn(bw, bw->h / 2 + bw->h % 2, 1);
	else if (pgamnt < bw->h)
		scrdn(bw, bw->h - pgamnt, 1);
	else
		scrdn(bw, 1, 1);
	return 0;
}

/* Scroll by a single line.  The cursor moves with the scroll */

int uupslide(BW *bw)
{
	bw = (BW *) bw->parent->main->object;
	if (bw->o.hex ? bw->top->byte/16 : bw->top->line) {
		if (bw->o.hex ? bw->top->byte/16 + bw->h -1 != bw->cursor->byte/16 : bw->top->line + bw->h - 1 != bw->cursor->line)
			udnarw(bw);
		scrup(bw, 1, 0);
		return 0;
	} else
		/* was return -1; */
		return uuparw(bw);
}

int udnslide(BW *bw)
{
	bw = (BW *) bw->parent->main->object;
	if (bw->o.hex ? bw->top->line/16 + bw->h <= bw->top->b->eof->byte/16 : bw->top->line + bw->h <= bw->top->b->eof->line) {
		if (bw->o.hex ? bw->top->byte/16 != bw->cursor->byte/16 : bw->top->line != bw->cursor->line)
			uuparw(bw);
		scrdn(bw, 1, 0);
		return 0;
	} else
		/* was return -1; */
		return udnarw(bw);
}

/* Move cursor to specified line number */

static B *linehist = NULL;	/* History of previously entered line numbers */

static int doline(BW *bw, unsigned char *s, void *object, int *notify)
{
	long num = calc(bw, s, 1);

	if (notify)
		*notify = 1;
	vsrm(s);
	if (num >= 1 && !merr) {
		int tmp = mid;

		if (num > bw->b->eof->line)
			num = bw->b->eof->line + 1;
		pline(bw->cursor, num - 1), bw->cursor->xcol = piscol(bw->cursor);
		mid = 1;
		dofollows();
		mid = tmp;
		return 0;
	} else {
		if (merr)
			msgnw(bw->parent, merr);
		else
			msgnw(bw->parent, joe_gettext(_("Invalid line number")));
		return -1;
	}
}

int uline(BW *bw)
{
	if (wmkpw(bw->parent, joe_gettext(_("Go to line (^C to abort): ")), &linehist, doline, NULL, NULL, NULL, NULL, NULL, locale_map, 0))
		return 0;
	else
		return -1;
}

/* Move cursor to specified column number */

static B *colhist = NULL;	/* History of previously entered column numbers */

static int docol(BW *bw, unsigned char *s, void *object, int *notify)
{
	long num = calc(bw, s, 1);

	if (notify)
		*notify = 1;
	vsrm(s);
	if (num >= 1 && !merr) {
		int tmp = mid;

		pcol(bw->cursor, num - 1), bw->cursor->xcol = piscol(bw->cursor);
		mid = 1;
		dofollows();
		mid = tmp;
		return 0;
	} else {
		if (merr)
			msgnw(bw->parent, merr);
		else
			msgnw(bw->parent, joe_gettext(_("Invalid column number")));
		return -1;
	}
}

int ucol(BW *bw)
{
	if (wmkpw(bw->parent, joe_gettext(_("Go to column (^C to abort): ")), &colhist, docol, NULL, NULL, NULL, NULL, NULL, locale_map, 0))
		return 0;
	else
		return -1;
}

/* Move cursor to specified byte number */

static B *bytehist = NULL;	/* History of previously entered byte numbers */

static int dobyte(BW *bw, unsigned char *s, void *object, int *notify)
{
	long num = calc(bw, s, 1);

	if (notify)
		*notify = 1;
	vsrm(s);
	if (num >= 0 && !merr) {
		int tmp = mid;

		pgoto(bw->cursor, num), bw->cursor->xcol = piscol(bw->cursor);
		mid = 1;
		dofollows();
		mid = tmp;
		return 0;
	} else {
		if (merr)
			msgnw(bw->parent, merr);
		else
			msgnw(bw->parent, joe_gettext(_("Invalid byte number")));
		return -1;
	}
}

int ubyte(BW *bw)
{
	if (wmkpw(bw->parent, joe_gettext(_("Go to byte (^C to abort): ")), &bytehist, dobyte, NULL, NULL, NULL, NULL, NULL, locale_map, 0))
		return 0;
	else
		return -1;
}

/* Delete character under cursor
 * or write ^D to process if we're at end of file in a shell window
 */

int udelch(BW *bw)
{
	P *p;

	if (piseof(bw->cursor))
		return -1;
	pgetc(p = pdup(bw->cursor, USTR "udelch"));
	bdel(bw->cursor, p);
	prm(p);
	return 0;
}

/* Backspace */

int ubacks(BW *bw, int k)
{
	/* Don't backspace when at beginning of line in prompt windows */
	if (bw->parent->watom->what == TYPETW || !pisbol(bw->cursor)) {
		int c;
		int indent;
		int col;
		int indwid;
		int wid;

		/* Degenerate into ltarw for overtype mode */
		if (bw->o.overtype) {
			return u_goto_left(bw);
		}

		if (pisbof(bw->cursor))
			return -1;

		/* Indentation point of this line */
		indent = pisindent(bw->cursor);

		/* Column position of cursor */
		col = piscol(bw->cursor);

		/* Indentation step in columns */
		if (bw->o.indentc=='\t')
			wid = bw->o.tab;
		else
			wid = 1;

		indwid = (bw->o.istep*wid);

		/* Smart backspace when: cursor is at indentation point, indentation point
		   is a multiple of indentation width, we're not at beginning of line,
		   'smarthome' option is enabled, and indentation is purely made out of
		   indent characters (or purify indents is enabled). */
		
		/* Ignore purify for backspace */
		if (col == indent && (col%indwid)==0 && col!=0 && bw->o.smartbacks && bw->o.autoindent) {
			P *p;

			/* Delete all indentation */
			p = pdup(bw->cursor, USTR "ubacks");
			p_goto_bol(p);
			bdel(p,bw->cursor);
			prm(p);

			/* Indent to new position */
			pfill(bw->cursor,col-indwid,bw->o.indentc);
		} else if (col<indent && bw->o.smartbacks && !pisbol(bw->cursor)) {
			/* We're before indent point: delete indwid worth of space but do not
			   cross line boundary.  We could probably replace the above with this. */
			int cw=0;
			P *p = pdup(bw->cursor, USTR "ubacks");
			do {
				c = prgetc(bw->cursor);
				if(c=='\t') cw += bw->o.tab;
				else cw += 1;
				bdel(bw->cursor, p);
			} while(!pisbol(bw->cursor) && cw<indwid);
			prm(p);
		} else {
			/* Regular backspace */
			P *p = pdup(bw->cursor, USTR "ubacks");
			if ((c = prgetc(bw->cursor)) != NO_MORE_DATA)
				if (!bw->o.overtype || c == '\t' || pisbol(p) || piseol(p))
					bdel(bw->cursor, p);
			prm(p);
		}
		return 0;
	} else
		return -1;
}

/* 
 * Delete sequence of characters (alphabetic, numeric) or (white-space)
 *	if cursor is on the white-space it will delete all white-spaces
 *		until alphanumeric character
 *      if cursor is on the alphanumeric it will delete all alphanumeric
 *		characters until character that is not alphanumeric
 */
int u_word_delete(BW *bw)
{
	P *p = pdup(bw->cursor, USTR "u_word_delete");
	struct charmap *map=bw->b->o.charmap;
	int c = brch(p);

	if (joe_isalnum_(map,c))
		while (joe_isalnum_(map,(c = brch(p))))
			pgetc(p);
	else if (joe_isspace(map,c))
		while (joe_isspace(map,(c = brch(p))))
			pgetc(p);
	else
		pgetc(p);

	if (p->byte == bw->cursor->byte) {
		prm(p);
		return -1;
	}
	bdel(bw->cursor, p);
	prm(p);
	return 0;
}

/* Delete from cursor to beginning of word it's in or immediately after,
 * to start of whitespace, or a single character
 */

int ubackw(BW *bw)
{
	P *p = pdup(bw->cursor, USTR "ubackw");
	int c = prgetc(bw->cursor);
	struct charmap *map=bw->b->o.charmap;

	if (joe_isalnum_(map,c)) {
		while (joe_isalnum_(map,(c = prgetc(bw->cursor))))
			/* do nothing */;
		if (c != NO_MORE_DATA)
			pgetc(bw->cursor);
	} else if (joe_isspace(map,c)) {
		while (joe_isspace(map,(c = prgetc(bw->cursor))))
			/* do nothing */;
		if (c != NO_MORE_DATA)
			pgetc(bw->cursor);
	}
	if (bw->cursor->byte == p->byte) {
		prm(p);
		return -1;
	}
	bdel(bw->cursor, p);
	prm(p);
	return 0;
}

/* Delete from cursor to end of line, or if there's nothing to delete,
 * delete the line-break
 */

int udelel(BW *bw)
{
	P *p = p_goto_eol(pdup(bw->cursor, USTR "udelel"));

	if (bw->cursor->byte == p->byte) {
		prm(p);
		return udelch(bw);
	} else
		bdel(bw->cursor, p);
	prm(p);
	return 0;
}

/* Delete to beginning of line, or if there's nothing to delete,
 * delete the line-break
 */

int udelbl(BW *bw)
{
	P *p = p_goto_bol(pdup(bw->cursor, USTR "udelbl"));

	if (p->byte == bw->cursor->byte) {
		prm(p);
		return ubacks(bw, 8);	/* The 8 goes to the process if we're at EOF of shell window */
	} else
		bdel(p, bw->cursor);
	prm(p);
	return 0;
}

/* Delete entire line */

int udelln(BW *bw)
{
	P *p = pdup(bw->cursor, USTR "udelln");

	p_goto_bol(bw->cursor);
	pnextl(p);
	if (bw->cursor->byte == p->byte) {
		prm(p);
		return -1;
	}
	bdel(bw->cursor, p);
	prm(p);
	return 0;
}

/* Insert a space */

int uinsc(BW *bw)
{
	binsc(bw->cursor, ' ');
	return 0;
}

/* Move p backwards to first non-blank line and return its indentation */

int find_indent(P *p)
{
	int x;
	for (x=0; x != 10; ++x) {
		if (!pprevl(p)) return -1;
		p_goto_bol(p);
		if (!pisblank(p)) break;
	}
	if (x==10)
		return -1;
	else
		return pisindent(p);
}

/* Type a character into the buffer (deal with left margin, overtype mode and
 * word-wrap), if cursor is at end of shell window buffer, just send character
 * to process.
 */

struct utf8_sm utype_utf8_sm;

int utypebw_raw(BW *bw, int k, int no_decode)
{
	struct charmap *map=bw->b->o.charmap;

	/* Send data to shell window */
	if ((bw->b->pid && !bw->b->vt && piseof(bw->cursor)) ||
	   ( bw->b->pid && bw->b->vt && bw->cursor->byte == bw->b->vt->vtcur->byte)) {
		unsigned char c = k;
		joe_write(bw->b->out, &c, 1);
		return 0;
	}

	/* Hex mode overtype is real simple */
	if (bw->o.hex && bw->o.overtype) {
		P *p;
		unsigned char c = k;
		binsm(bw->cursor, &c, 1);
		pgetb(bw->cursor);
		if (piseof(bw->cursor))
			return 0;
		pgetb(p = pdup(bw->cursor, USTR "utypebw_raw"));
		bdel(bw->cursor, p);
		prm(p);
		return 0;
	}

	if (k == '\t' && bw->o.overtype && !piseol(bw->cursor)) { /* TAB in overtype mode is supposed to be just cursor motion */
		int col = bw->cursor->xcol;		/* Current cursor column */
		col = col + bw->o.tab - (col%bw->o.tab);/* Move to next tab stop */
		pcol(bw->cursor,col);			/* Try to position cursor there */
		if (!bw->o.picture && piseol(bw->cursor) && piscol(bw->cursor)<col) {	/* We moved past end of line, insert a tab (unless in picture mode) */
			if (bw->o.spaces)
				pfill(bw->cursor,col,' ');
			else
				pfill(bw->cursor,col,'\t');
		}
		bw->cursor->xcol = col;			/* Put cursor there even if we can't really go there */
	} else if (k == '\t' && bw->o.smartbacks && bw->o.autoindent && pisindent(bw->cursor)>=piscol(bw->cursor)) {
		P *p = pdup(bw->cursor, USTR "utypebw_raw");
		int n = find_indent(p);
		if (n != -1 && pisindent(bw->cursor)==piscol(bw->cursor) && n > pisindent(bw->cursor)) {
			if (!pisbol(bw->cursor))
				udelbl(bw);
			while (joe_isspace(map,(k = pgetc(p))) && k != '\n') {
				binsc(bw->cursor, k);
				pgetc(bw->cursor);
			}
		} else {
			int x;
			for (x=0;x<bw->o.istep;++x) {
				binsc(bw->cursor,bw->o.indentc);
				pgetc(bw->cursor);
			}
		}
		bw->cursor->xcol = piscol(bw->cursor);
		prm (p);
	} else if (k == '\t' && bw->o.spaces) {
		long n;

		if (bw->o.picture)
			n = bw->cursor->xcol;
		else
			n = piscol(bw->cursor);

		utype_utf8_sm.state = 0;
		utype_utf8_sm.ptr = 0;

		n = bw->o.tab - n % bw->o.tab;
		while (n--)
			utypebw(bw, ' ');
	} else {
		int upd;
		int simple;
		int x;

		/* Picture mode */
		if (bw->o.picture && bw->cursor->xcol!=piscol(bw->cursor))
			pfill(bw->cursor,bw->cursor->xcol,' '); /* Why no tabs? */

		/* UTF8 decoder */
		if(locale_map->type && !no_decode) {
			int utf8_char = utf8_decode(&utype_utf8_sm,k);

			if(utf8_char >= 0)
				k = utf8_char;
			else
				return 0;
		}

		upd = bw->parent->t->t->updtab[bw->y + bw->cursor->line - bw->top->line];
		simple = 1;

		if (pisblank(bw->cursor))
			while (piscol(bw->cursor) < bw->o.lmargin) {
				binsc(bw->cursor, ' ');
				pgetc(bw->cursor);
			}

		if (!no_decode) {
			if(locale_map->type && !bw->b->o.charmap->type) {
				unsigned char buf[10];
				utf8_encode(buf,k);
				k = from_utf8(bw->b->o.charmap,buf);
			} else if(!locale_map->type && bw->b->o.charmap->type) {
				unsigned char buf[10];
				to_utf8(locale_map,buf,k);
				k = utf8_decode_string(buf);
			}
		}
		
		binsc(bw->cursor, k);

		/* We need x position before we move cursor */
		x = piscol(bw->cursor) - bw->offset;
		pgetc(bw->cursor);

		/* Tabs are weird here... */
		if (bw->o.overtype && !piseol(bw->cursor) && k != '\t')
			udelch(bw);

		/* Not sure if we're in right position for wordwrap when we're in overtype mode */
		if (bw->o.wordwrap && piscol(bw->cursor) > bw->o.rmargin && !joe_isblank(map,k)) {
			wrapword(bw, bw->cursor, (long) bw->o.lmargin, bw->o.french, 0, NULL);
			simple = 0;
		}

		bw->cursor->xcol = piscol(bw->cursor);
#ifndef __MSDOS__
		if (x < 0 || x >= bw->w)
			simple = 0;
		if (bw->cursor->line < bw->top->line || bw->cursor->line >= bw->top->line + bw->h)
			simple = 0;
		if (simple && bw->parent->t->t->sary[bw->y + bw->cursor->line - bw->top->line])
			simple = 0;
		if (simple && k != '\t' && k != '\n' && !curmacro) {
			int atr;
			SCRN *t = bw->parent->t->t;
			int y = bw->y + bw->cursor->line - bw->top->line;
			int *screen = t->scrn + y * t->co;
			int *attr = t->attr + y * t->co;
			x += bw->x;

			atr = BG_COLOR(bg_text);

			if (!upd && piseol(bw->cursor) && !bw->o.highlight)
				t->updtab[y] = 0;
			if (markb &&
			    markk &&
			    markb->b == bw->b &&
			    markk->b == bw->b &&
			   ((!square && bw->cursor->byte >= markb->byte && bw->cursor->byte < markk->byte) ||
			    ( square && bw->cursor->line >= markb->line && bw->cursor->line <= markk->line && piscol(bw->cursor) >= markb->xcol && piscol(bw->cursor) < markk->xcol)))
				atr |= INVERSE;
			outatr(bw->b->o.charmap, t, screen + x, attr + x, x, y, k, atr);
		}
#endif
	}
	return 0;
}

int utypebw(BW *bw, int k)
{
	return utypebw_raw(bw, k, 0);
}

/* Quoting */

static B *unicodehist = NULL;	/* History of previously entered unicode characters */

static int dounicode(BW *bw, unsigned char *s, void *object, int *notify)
{
	int num;
	sscanf((char *)s,"%x",(unsigned  *)&num);
	if (notify)
		*notify = 1;
	vsrm(s);
	utypebw_raw(bw, num, 1);
	bw->cursor->xcol = piscol(bw->cursor);
	return 0;
}

int quotestate;
int quoteval;

static int doquote(BW *bw, int c, void *object, int *notify)
{
	unsigned char buf[40];

	if (c < 0 || c >= 256) {
		nungetc(c);
		return -1;
	}
	switch (quotestate) {
	case 0:
		if (c >= '0' && c <= '9') {
			quoteval = c - '0';
			quotestate = 1;
			joe_snprintf_1(buf, sizeof(buf), "ASCII %c--", c);
			if (!mkqwna(bw->parent, sz(buf), doquote, NULL, NULL, notify))
				return -1;
			else
				return 0;
		} else if (c == 'x' || c == 'X') {
			if (bw->b->o.charmap->type) {
				if (!wmkpw(bw->parent, joe_gettext(_("Unicode (ISO-10646) character in hex (^C to abort): ")), &unicodehist, dounicode,
				           NULL, NULL, NULL, NULL, NULL, locale_map, 0))
					return 0;
				else
					return -1;
			} else {
				quotestate = 3;
				if (!mkqwna(bw->parent, sc("ASCII 0x--"), doquote, NULL, NULL, notify))
					return -1;
				else
					return 0;
			}
		} else if (c == 'o' || c == 'O') {
			quotestate = 5;
			if (!mkqwna(bw->parent, sc("ASCII 0---"), doquote, NULL, NULL, notify))
				return -1;
			else
				return 0;
		} else {
			if ((c >= 0x40 && c <= 0x5F) || (c >= 'a' && c <= 'z'))
				c &= 0x1F;
			if (c == '?')
				c = 127;
			utypebw_raw(bw, c, 1);
			bw->cursor->xcol = piscol(bw->cursor);
		}
		break;
	case 1:
		if (c >= '0' && c <= '9') {
			joe_snprintf_2(buf, sizeof(buf), "ASCII %c%c-", quoteval + '0', c);
			quoteval = quoteval * 10 + c - '0';
			quotestate = 2;
			if (!mkqwna(bw->parent, sz(buf), doquote, NULL, NULL, notify))
				return -1;
			else
				return 0;
		}
		break;
	case 2:
		if (c >= '0' && c <= '9') {
			quoteval = quoteval * 10 + c - '0';
			utypebw_raw(bw, quoteval, 1);
			bw->cursor->xcol = piscol(bw->cursor);
		}
		break;
	case 3:
		if (c >= '0' && c <= '9') {
			joe_snprintf_1(buf, sizeof(buf), "ASCII 0x%c-", c);
			quoteval = c - '0';
			quotestate = 4;
			if (!mkqwna(bw->parent, sz(buf), doquote, NULL, NULL, notify))
				return -1;
			else
				return 0;
		} else if (c >= 'a' && c <= 'f') {
			joe_snprintf_1(buf, sizeof(buf), "ASCII 0x%c-", c + 'A' - 'a');
			quoteval = c - 'a' + 10;
			quotestate = 4;
			if (!mkqwna(bw->parent, sz(buf), doquote, NULL, NULL, notify))
				return -1;
			else
				return 0;
		} else if (c >= 'A' && c <= 'F') {
			joe_snprintf_1(buf, sizeof(buf), "ASCII 0x%c-", c);
			quoteval = c - 'A' + 10;
			quotestate = 4;
			if (!mkqwna(bw->parent, sz(buf), doquote, NULL, NULL, notify))
				return -1;
			else
				return 0;
		}
		break;
	case 4:
		if (c >= '0' && c <= '9') {
			quoteval = quoteval * 16 + c - '0';
			utypebw_raw(bw, quoteval, 1);
			bw->cursor->xcol = piscol(bw->cursor);
		} else if (c >= 'a' && c <= 'f') {
			quoteval = quoteval * 16 + c - 'a' + 10;
			utypebw_raw(bw, quoteval, 1);
			bw->cursor->xcol = piscol(bw->cursor);
		} else if (c >= 'A' && c <= 'F') {
			quoteval = quoteval * 16 + c - 'A' + 10;
			utypebw_raw(bw, quoteval, 1);
			bw->cursor->xcol = piscol(bw->cursor);
		}
		break;
	case 5:
		if (c >= '0' && c <= '7') {
			joe_snprintf_1(buf, sizeof(buf), "ASCII 0%c--", c);
			quoteval = c - '0';
			quotestate = 6;
			if (!mkqwna(bw->parent, sz(buf), doquote, NULL, NULL, notify))
				return -1;
			else
				return 0;
		}
		break;
	case 6:
		if (c >= '0' && c <= '7') {
			joe_snprintf_2(buf, sizeof(buf), "ASCII 0%c%c-", quoteval + '0', c);
			quoteval = quoteval * 8 + c - '0';
			quotestate = 7;
			if (!mkqwna(bw->parent, sz(buf), doquote, NULL, NULL, notify))
				return -1;
			else
				return 0;
		}
		break;
	case 7:
		if (c >= '0' && c <= '7') {
			quoteval = quoteval * 8 + c - '0';
			utypebw_raw(bw, quoteval, 1);
			bw->cursor->xcol = piscol(bw->cursor);
		}
		break;
	}
	if (notify)
		*notify = 1;
	return 0;
}

int uquote(BW *bw)
{
	quotestate = 0;
	if (mkqwna(bw->parent, sz(joe_gettext(_("Ctrl- (or 0-9 for dec. ascii, x for hex, or o for octal)"))), doquote, NULL, NULL, NULL))
		return 0;
	else
		return -1;
}

static int doquote9(BW *bw, int c, void *object, int *notify)
{
	if (notify)
		*notify = 1;
	if ((c >= 0x40 && c <= 0x5F) || (c >= 'a' && c <= 'z'))
		c &= 0x1F;
	if (c == '?')
		c = 127;
	c |= 128;
	utypebw_raw(bw, c, 1);
	bw->cursor->xcol = piscol(bw->cursor);
	return 0;
}

static int doquote8(BW *bw, int c, void *object, int *notify)
{
	if (c == '`') {
		if (mkqwna(bw->parent, sc("Meta-Ctrl-"), doquote9, NULL, NULL, notify))
			return 0;
		else
			return -1;
	}
	if (notify)
		*notify = 1;
	c |= 128;
	utypebw_raw(bw, c, 1);
	bw->cursor->xcol = piscol(bw->cursor);
	return 0;
}

int uquote8(BW *bw)
{
	if (mkqwna(bw->parent, sc("Meta-"), doquote8, NULL, NULL, NULL))
		return 0;
	else
		return -1;
}

static int doctrl(BW *bw, int c, void *object, int *notify)
{
	int org = bw->o.overtype;

	if (notify)
		*notify = 1;
	bw->o.overtype = 0;
	if (bw->parent->huh == srchstr && c == '\n') {
		utypebw(bw, '\\');
		utypebw(bw, 'n');
	} else
		utypebw_raw(bw, c, 1);
	bw->o.overtype = org;
	bw->cursor->xcol = piscol(bw->cursor);
	return 0;
}

int uctrl(BW *bw)
{
	if (mkqwna(bw->parent, sz(joe_gettext(_("Quote"))), doctrl, NULL, NULL, NULL))
		return 0;
	else
		return -1;
}

/* User hit Return.  Deal with autoindent.
 */

int rtntw(BW *bw)
{
	if (bw->o.overtype) {
		p_goto_eol(bw->cursor);
		if (piseof(bw->cursor))
			binsc(bw->cursor, '\n');
		pgetc(bw->cursor);
		bw->cursor->xcol = piscol(bw->cursor);
	} else {
		P *p = pdup(bw->cursor, USTR "rtntw");
		unsigned char c;

		binsc(bw->cursor, '\n'), pgetc(bw->cursor);
		/* Suppress autoindent if we're on a space or tab... */
		if (bw->o.autoindent /* && (brch(bw->cursor)!=' ' && brch(bw->cursor)!='\t')*/) {
			p_goto_bol(p);
			while (joe_isspace(bw->b->o.charmap,(c = pgetc(p))) && c != '\n') {
				binsc(bw->cursor, c);
				pgetc(bw->cursor);
			}
		}
		prm(p);
		bw->cursor->xcol = piscol(bw->cursor);
	}
	return 0;
}

/* Open a line */

int uopen(BW *bw)
{
	binsc(bw->cursor,'\n');
	if (bw->o.autoindent && (brch(bw->cursor)!=' ' && brch(bw->cursor)!='\t')) {
		P *p = pdup(bw->cursor, USTR "uopen");
		P *q = pdup(p, USTR "uopen");
		int c;
		pgetc(q);
		p_goto_bol(p);
		while (joe_isspace(bw->b->o.charmap,(c = pgetc(p))) && c != '\n') {
			binsc(q, c);
			pgetc(q);
		}
		prm(p); prm(q);
	}
	
	return 0;
}

/* Set book-mark */

static int dosetmark(BW *bw, int c, void *object, int *notify)
{
	if (notify)
		*notify = 1;
	if (c >= '0' && c <= ':') {
		pdupown(bw->cursor, bw->b->marks + c - '0', USTR "dosetmark");
		poffline(bw->b->marks[c - '0']);
		if (c!=':') {
			joe_snprintf_1(msgbuf, JOE_MSGBUFSIZE, joe_gettext(_("Mark %d set")), c - '0');
			msgnw(bw->parent, msgbuf);
		}
		return 0;
	} else {
		nungetc(c);
		return -1;
	}
}

int usetmark(BW *bw, int c)
{
	if (c >= '0' && c <= ':')
		return dosetmark(bw, c, NULL, NULL);
	else if (mkqwna(bw->parent, sz(joe_gettext(_("Set mark (0-9):"))), dosetmark, NULL, NULL, NULL))
		return 0;
	else
		return -1;
}

/* Goto book-mark */

static int dogomark(BW *bw, int c, void *object, int *notify)
{
	if (notify)
		*notify = 1;
	if (c >= '0' && c <= ':')
		if (bw->b->marks[c - '0']) {
			pset(bw->cursor, bw->b->marks[c - '0']);
			bw->cursor->xcol = piscol(bw->cursor);
			return 0;
		} else {
			joe_snprintf_1(msgbuf, JOE_MSGBUFSIZE, joe_gettext(_("Mark %d not set")), c - '0');
			msgnw(bw->parent, msgbuf);
			return -1;
	} else {
		nungetc(c);
		return -1;
	}
}

int ugomark(BW *bw, int c)
{
	if (c >= '0' && c <= '9')
		return dogomark(bw, c, NULL, NULL);
	else if (mkqwna(bw->parent, sz(joe_gettext(_("Goto bookmark (0-9):"))), dogomark, NULL, NULL, NULL))
		return 0;
	else
		return -1;
}

/* Goto next instance of character */

static int dobkwdc;

static int dofwrdc(BW *bw, int k, void *object, int *notify)
{
	int c;
	P *q;

	if (notify)
		*notify = 1;
	if (k < 0 || k >= 256) {
		nungetc(k);
		return -1;
	}
	q = pdup(bw->cursor, USTR "dofwrdc");
	if (dobkwdc) {
		while ((c = prgetc(q)) != NO_MORE_DATA)
			if (c == k)
				break;
	} else {
		while ((c = pgetc(q)) != NO_MORE_DATA)
			if (c == k)
				break;
	}
	if (c == NO_MORE_DATA) {
		msgnw(bw->parent, joe_gettext(_("Not found")));
		prm(q);
		return -1;
	} else {
		pset(bw->cursor, q);
		bw->cursor->xcol = piscol(bw->cursor);
		prm(q);
		return 0;
	}
}

int ufwrdc(BW *bw, int k)
{
	dobkwdc = 0;
	if (k >= 0 && k < 256)
		return dofwrdc(bw, k, NULL, NULL);
	else if (mkqw(bw->parent, sz(joe_gettext(_("Forward to char: "))), dofwrdc, NULL, NULL, NULL))
		return 0;
	else
		return -1;
}

int ubkwdc(BW *bw, int k)
{
	dobkwdc = 1;
	if (k >= 0 && k < 256)
		return dofwrdc(bw, k, NULL, NULL);
	else if (mkqw(bw->parent, sz(joe_gettext(_("Backward to char: "))), dofwrdc, NULL, NULL, NULL))
		return 0;
	else
		return -1;
}

/* Display a message */

static int domsg(BASE *b, unsigned char *s, void *object, int *notify)
{
	if (notify)
		*notify = 1;
	zlcpy(msgbuf, sizeof(msgbuf), s);
	vsrm(s);
	msgnw(b->parent, msgbuf);
	return 0;
}

int umsg(BASE *b)
{
	if (wmkpw(b->parent, joe_gettext(_("Message (^C to abort): ")), NULL, domsg, NULL, NULL, NULL, NULL, NULL, locale_map, 0))
		return 0;
	else
		return -1;
}

/* Insert text */

static int dotxt(BW *bw, unsigned char *s, void *object, int *notify)
{
	int x,fill;
	unsigned char *str;

	if (notify)
		*notify = 1;
	if (s[0]=='`') {   
	   str=vsmk(1024);
	   fill=' ';
	   str=stagen(str,bw,&s[1],fill);
	   if (str) {
	     for(x=0;x!=sLEN(str);++x) utypebw(bw,str[x]);
	     vsrm(str);
	     }
	} else
	for (x = 0; x != sLEN(s); ++x)
		utypebw(bw, s[x]);
	vsrm(s);
	return 0;
}

int utxt(BW *bw)
{
	if (wmkpw(bw->parent, joe_gettext(_("Insert (^C to abort): ")), NULL, dotxt, NULL, NULL, utypebw, NULL, NULL, bw->b->o.charmap, 0))
		return 0;
	else
		return -1;
}

/* Insert current file name */

int uname_joe(BW *bw)
{
	unsigned char *s;
	W *w=bw->parent->main;
	s=((BW *)w->object)->b->name;
	if (!s || !*s)
		return -1;
	while (*s)
		if (utypebw(bw,*s++))
			return -1;
	return 0;
}

/* Insert until non-base64 character received */

int upaste(BW  *bw, int k)
{
	int c;
	int accu = 0;
	int count;
	int tmp_ww = bw->o.wordwrap;
	int tmp_ai = bw->o.autoindent;

	bw->o.wordwrap = 0;
	bw->o.autoindent = 0;
	count = 0;

	/* We have to wait for the second ';' */
	while ((c = ttgetc()) != -1)
		if (c == ';')
			break;
	if (c == -1)
		goto bye;

	while ((c = ttgetc()) != -1) {
		if (c >= 'A' && c <= 'Z')
			c = c - 'A';
		else if (c >= 'a' && c <= 'z')
			c = c - 'a' + 26;
		else if (c >= '0' && c <= '9')
			c = c - '0' + 52;
		else if (c == '+')
			c = 62;
		else if (c == '/')
			c = 63;
		else if (c == '=')
			continue;
		else
			break;

		switch (count) {
			case 0:
				accu = c;
				count = 6;
				break;
			case 2:
				accu = (accu << 6) + c;
				if (accu == 13)
					rtntw(bw);
				else
					utypebw(bw, accu);
				count = 0;
				break;
			case 4:
				accu = (accu << 4) + (c >> 2);
				if (accu == 13)
					rtntw(bw);
				else
					utypebw(bw, accu);
				accu = (c & 0x3);
				count = 2;
				break;
			case 6:
				accu = (accu << 2) + (c >> 4);
				if (accu == 13)
					rtntw(bw);
				else
					utypebw(bw, accu);
				accu = (c & 0xF);
				count = 4;
				break;
		}
	}
	/* Terminator is ESC \ */
	if (c == 033) {
		ttgetc();
	}

	bye:

	bw->o.wordwrap = tmp_ww;
	bw->o.autoindent = tmp_ai;

	return 0;
}

int ubrpaste(BW *bw, int k)
{
	const unsigned char *terminator = USTR "\033[201~";
	int tidx = 0;
	int c;
	int saved_ww = bw->o.wordwrap;
	int saved_ai = bw->o.autoindent;
	int saved_sp = bw->o.spaces;
	
	bw->o.wordwrap = bw->o.autoindent = bw->o.spaces = 0;
	
	while (terminator[tidx] && -1 != (c = ttgetc())) {
		if (c == terminator[tidx]) {
			tidx++;
		} else {
			int i;
			for (i = 0; i < tidx; i++)
				utypebw(bw, terminator[i]);
			tidx = 0;
			
			if (c == 13)
				rtntw(bw);
			else
				utypebw(bw, c);
		}
	}
	
	bw->o.wordwrap = saved_ww;
	bw->o.autoindent = saved_ai;
	bw->o.spaces = saved_sp;
	
	return 0;
}
