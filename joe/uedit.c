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
	p_goto_eof(bw->cursor);
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

int tomatch_word(BW *bw,unsigned char *set,unsigned char *group)
{
	if (!*group || *group==':') {
		/* Backward search */
		unsigned char *last_of_set = find_last_group(set);
		P *p=pdup(bw->cursor, USTR "tomatch_word");
		int c;
		unsigned char buf[MAX_WORD_SIZE+1];
		int len;
		int cnt = 1;
		p_goto_next(p);
		p_goto_prev(p);
		while ((c=prgetc(p)) != NO_MORE_DATA) {
			int peek = prgetc(p);
			if(peek!=NO_MORE_DATA)
				pgetc(p);
			if (peek=='\\') {
			} else if (!bw->o.no_double_quoted && c == '"') {
				while((c = prgetc(p)) != NO_MORE_DATA) {
					if (c == '"') {
						c = prgetc(p);
						if (c != '\\') {
							if (c != NO_MORE_DATA)
								pgetc(p);
							break;
						}
					}
				}
			} else if (bw->o.single_quoted && c == '\'') {
				while((c = prgetc(p)) != NO_MORE_DATA)
					if (c == '\'') {
						c = prgetc(p);
						if (c != '\\') {
							if (c != NO_MORE_DATA)
								pgetc(p);
							break;
						}
					}
			} else if (bw->o.c_comment && c == '/') {
				c = prgetc(p);
				if (c == '*') {
					c = prgetc(p);
					do {
						do {
							if (c == '*') break;
						} while ((c = prgetc(p)) != NO_MORE_DATA);
						c = prgetc(p);
					} while (c != NO_MORE_DATA && c != '/');
				} else if (c != NO_MORE_DATA)
					pgetc(p);
			} else if ((bw->o.cpp_comment || bw->o.pound_comment ||
			            bw->o.semi_comment || bw->o.tex_comment || bw->o.vhdl_comment) && c == '\n') {
				P *q = pdup(p, USTR "tomatch_word");
				int cc;
				p_goto_bol(q);
				while((cc = pgetc(q)) != '\n') {
					if (bw->o.pound_comment && cc == '$' && brch(q)=='#') {
						pgetc(q);
					} else if(!bw->o.no_double_quoted && cc=='"') {
						while ((cc = pgetc(q)) != '\n')
							if (cc == '"') break;
							else if (cc == '\\') pgetc(q);
					} else if (bw->o.cpp_comment && cc == '/') {
						if (brch(q)=='/') {
							prgetc(q);
							pset(p,q);
							break;
						}
					} else if (bw->o.single_quoted && cc == '\'') {
						while((cc = pgetc(q)) != '\n')
							if (cc == '\'') break;
							else if (cc == '\\') pgetc(q);
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
				prm(q);
			} else if ((c >= 'a' && c <= 'z') || (c>='A' && c<='Z') || (c>='0' && c<='9') || c=='_') {
				int x;
				int flg=0;
				P *q;
				len=0;
				while ((c >= 'a' && c <= 'z') || (c>='A' && c<='Z') || c=='_' || (c>='0' && c<='9')) {
					if(len!=MAX_WORD_SIZE)
						buf[len++]=c;
					c=prgetc(p);
				}
				/* ifdef hack */
				q=pdup(p, USTR "tomatch_word");
				while (c ==' ' || c=='\t')
					c=prgetc(q);
				prm(q);
				/* VHDL hack */
				if ((c=='d' || c=='D') && bw->o.vhdl_comment) {
					c=prgetc(q);
					if(c=='n' || c=='N') {
						c=prgetc(q);
						if(c=='e' || c=='E') {
							c=prgetc(q);
							if(c==' ' || c=='\t' || c=='\n' || c==NO_MORE_DATA)
								flg=1;
						}
					}
				}
				prm(q);
				if (c == set[0])
					buf[len++] = c;
				if(c!=NO_MORE_DATA)
					pgetc(p);
				buf[len]=0;
				for(x=0;x!=len/2;++x) {
					int d = buf[x];
					buf[x] = buf[len-x-1];
					buf[len-x-1] = d;
				}
				if (is_in_group(last_of_set,buf)) {
					++cnt;
				} else if(is_in_group(set,buf) && !flg && !--cnt) {
					pset(bw->cursor,p);
					prm(p);
					return 0;
				}
			}
		}
		prm(p);
		return -1;
	} else {
		/* Forward search */
		unsigned char *last_of_set = find_last_group(group);
		P *p=pdup(bw->cursor, USTR "tomatch_word");
		int c;
		unsigned char buf[MAX_WORD_SIZE+1];
		int len;
		int cnt = 1;
		p_goto_next(p);
		while ((c=pgetc(p)) != NO_MORE_DATA) {
			if (c == '\\') {
				pgetc(p);
			} else if (!bw->o.no_double_quoted && c == '"') {
				while ((c = pgetc(p)) != NO_MORE_DATA)
					if (c == '"') break;
					else if (c == '\\') pgetc(p);
			} else if (c == '$' && brch(p)=='#' && bw->o.pound_comment) {
				pgetc(p);
			} else if ((bw->o.pound_comment && c == '#') ||
				   (bw->o.semi_comment && c == ';') ||
				   (bw->o.tex_comment && c == '%') ||
				   (bw->o.vhdl_comment && c == '-' && brch(p) == '-')) {
				while ((c = pgetc(p)) != NO_MORE_DATA)
					if (c == '\n')
						break;
			} else if (bw->o.single_quoted && c == '\'') {
				while((c = pgetc(p)) != NO_MORE_DATA)
					if (c == '\'') break;
					else if (c == '\\') pgetc(p);
			} else if ((bw->o.c_comment || bw->o.cpp_comment) && c == '/') {
				c = pgetc(p);
				if (bw->o.c_comment && c == '*') {
					c = pgetc(p);
					do {
						do {
							if (c == '*') break;
						} while ((c = pgetc(p)) != NO_MORE_DATA);
						c = pgetc(p);
					} while (c != NO_MORE_DATA && c != '/');
				} else if (bw->o.cpp_comment && c == '/') {
					while ((c = pgetc(p)) != NO_MORE_DATA)
						if (c == '\n')
							break;
				} else if (c != NO_MORE_DATA)
					prgetc(p);
			} else if (c == set[0]) {
				/* ifdef hack */
				while ((c = pgetc(p))!=NO_MORE_DATA) {
					if (c!=' ' && c!='\t')
						break;
				}
				buf[0]=set[0];
				len=1;
				if (c >= 'a' && c <= 'z')
					goto doit;
			} else if ((c >= 'a' && c <= 'z') || (c>='A' && c<='Z') || c=='_') {
				len=0;
				doit:
				while ((c >= 'a' && c <= 'z') || (c>='A' && c<='Z') || c=='_' || (c>='0' && c<='9')) {
					if(len!=MAX_WORD_SIZE)
						buf[len++]=c;
					c=pgetc(p);
				}
				if (c!=NO_MORE_DATA)
					prgetc(p);
				buf[len]=0;
				if (is_in_group(set,buf)) {
					++cnt;
				} else if (cnt==1) {
					if (is_in_any_group(group,buf)) {
						if (!((buf[0]>='a' && buf[0]<='z') || (buf[0]>='A' && buf[0]<='Z')))
							pgoto(p,p->byte-len+1);
						else
							pgoto(p,p->byte-len);
						pset(bw->cursor,p);
						prm(p);
						return 0;
					}
				} else if(is_in_group(last_of_set,buf)) {
					/* VHDL hack */
					if (bw->o.vhdl_comment && (!zcmp(buf,USTR "end") || !zcmp(buf,USTR "END")))
						while((c=pgetc(p))!=NO_MORE_DATA)
							if (c==';' || c=='\n')
								break;
					--cnt;
				}
			}
		}
		prm(p);
		return -1;
	}
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
	if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_') {
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
	if (f == '/') {
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
				pset(bw->cursor,p);
			}
		}
		prm(p);
		if (d == NO_MORE_DATA)
			return -1;
		else
			return 0;
	}

	/* Search for matching delimiter: ignore things in comments or strings */
	/* This really should be language dependent */
	if (dir == 1) {
		P *p = pdup(bw->cursor, USTR "utomatch");
		int cnt = 0;	/* No. levels of delimiters we're in */

		while ((d = pgetc(p)) != NO_MORE_DATA) {
			if (d == '\\') {
				pgetc(p);
			} else if (d == '$' && brch(p)=='#' && bw->o.pound_comment) {
				pgetc(p);
			} else if (!bw->o.no_double_quoted && d == '"') {
				while ((d = pgetc(p)) != NO_MORE_DATA)
					if (d == '"') break;
					else if (d == '\\') pgetc(p);
			} else if (bw->o.single_quoted && d == '\'' && c!='\'' && c!='`') {
				while((d = pgetc(p)) != NO_MORE_DATA)
					if (d == '\'') break;
					else if (d == '\\') pgetc(p);
			} else if ((bw->o.pound_comment && d == '#') ||
				   (bw->o.semi_comment && d == ';') ||
				   (bw->o.tex_comment && d == '%') ||
				   (bw->o.vhdl_comment && d == '-' && brch(p) == '-')) {
				while ((d = pgetc(p)) != NO_MORE_DATA)
					if (d == '\n')
						break;
			} else if ((bw->o.c_comment || bw->o.cpp_comment) && d == '/') {
				d = pgetc(p);
				if (bw->o.c_comment && d == '*') {
					d = pgetc(p);
					do {
						do {
							if (d == '*') break;
						} while ((d = pgetc(p)) != NO_MORE_DATA);
						d = pgetc(p);
					} while (d != NO_MORE_DATA && d != '/');
				} else if (bw->o.cpp_comment && d == '/') {
					while ((d = pgetc(p)) != NO_MORE_DATA)
						if (d == '\n')
							break;
				} else if (d != NO_MORE_DATA)
					prgetc(p);
			} else if (d == c)
				++cnt;
			else if (d == f && !--cnt) {
				prgetc(p);
				pset(bw->cursor, p);
				break;
			}
		}
		prm(p);
	} else {
		P *p = pdup(bw->cursor, USTR "utomatch");
		int cnt = 0;	/* No. levels of delimiters we're in */

		while ((d = prgetc(p)) != NO_MORE_DATA) {
			int peek = prgetc(p);
			int peek1 = 0;
			if(peek != NO_MORE_DATA) {
				peek1 = prgetc(p);
				if (peek1 != NO_MORE_DATA)
					pgetc(p);
				pgetc(p);
			}
			if (peek == '\\' && peek1!='\\') {
			} else if (!bw->o.no_double_quoted && d == '"') {
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
			} else if (bw->o.c_comment && d =='/') {
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
			} else if ((bw->o.cpp_comment || bw->o.pound_comment ||
			            bw->o.semi_comment || bw->o.tex_comment || bw->o.vhdl_comment) && d == '\n') {
				P *q = pdup(p, USTR "utomatch");
				int cc;
				p_goto_bol(q);
				while((cc = pgetc(q)) != '\n') {
					if (bw->o.pound_comment && cc == '$' && brch(q)=='#') {
						pgetc(q);
					} else if (cc == '\\') {
						if (pgetc(q) == '\n')
							break;
					} else if(!bw->o.no_double_quoted && cc=='"') {
						while ((cc = pgetc(q)) != '\n')
							if (cc == '"') break;
							else if (cc == '\\') pgetc(q);
						if (cc == '\n')
							break;
					} else if (bw->o.cpp_comment && cc == '/') {
						if (brch(q)=='/') {
							prgetc(q);
							pset(p,q);
							break;
						}
					} else if (bw->o.single_quoted && cc == '\'') {
						while((cc = pgetc(q)) != '\n')
							if (cc == '\'') break;
							else if (cc == '\\') pgetc(q);
						if (cc == '\n')
							break;
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
				prm(q);
			} else if (bw->o.single_quoted && d =='\'' && c!='\'' && c!='`') {
				while((d = prgetc(p)) != NO_MORE_DATA)
					if (d == '\'') {
						d = prgetc(p);
						if (d != '\\') {
							if (d != NO_MORE_DATA)
								pgetc(p);
							break;
						}
					}
			} else if (d == c)
				++cnt;
			else if (d == f)
				if (!cnt--) {
					pset(bw->cursor, p);
					break;
				}
		}
		prm(p);
	}
	if (d == NO_MORE_DATA)
		return -1;
	else
		return 0;
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
#ifdef JOEWIN
		/* I don't like the scroll-wheel behavior.  I could add a command *just* for the mouse events, but I'm lazy. */
		return -1;
#else
		/* was return -1; */
		return uuparw(bw);
#endif
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
#ifdef JOEWIN
		/* I don't like the scroll-wheel behavior.  I could add a command *just* for the mouse events, but I'm lazy. */
		return -1;
#else
		/* was return -1; */
		return udnarw(bw);
#endif
}

/* Move cursor to specified line number */

static B *linehist = NULL;	/* History of previously entered line numbers */

int uline(BW *bw)
{
	long num;
	unsigned char *s;

	s = ask(bw->parent, joe_gettext(_("Go to line (^C to abort): ")), &linehist,
	        NULL, NULL, locale_map, 0, 0, NULL);

	if (!s)
		return -1;

	num = calc(bw, s);

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

/* Move cursor to specified column number */

static B *colhist = NULL;	/* History of previously entered column numbers */

int ucol(BW *bw)
{
	long num;
	unsigned char *s;

	s = ask(bw->parent, joe_gettext(_("Go to column (^C to abort): ")), &colhist,
	        NULL, NULL, locale_map, 0, 0, NULL);

	if (!s)
		return -1;

	num = calc(bw, s);

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

/* Move cursor to specified byte number */

static B *bytehist = NULL;	/* History of previously entered byte numbers */

int ubyte(BW *bw)
{
	long num;
	unsigned char *s;

	s = ask(bw->parent, joe_gettext(_("Go to byte (^C to abort): ")), &bytehist,
	        NULL, NULL, locale_map, 0, 0, NULL);
	
	if (!s)
		return -1;

	num = calc(bw, s);

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
	if (bw->b->pid && piseof(bw->cursor)) {
		unsigned char c = k;
		writempx(bw->b->out, &c, 1);
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

int quotestate;
int quoteval;

int uquote(BW *bw)
{
	unsigned char buf[40];
	int c;

	quotestate = 0;
	c = query(bw->parent, sz(joe_gettext(_("Ctrl- (or 0-9 for dec. ascii, x for hex, or o for octal)"))), QW_STAY);
	if (c == -1)
		return -1;

	again:

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
			c = query(bw->parent, sz(buf), QW_STAY);
			if (c == -1)
				return -1;
			else
				goto again;
		} else if (c == 'x' || c == 'X') {
			if (bw->b->o.charmap->type) {
				unsigned char *s = ask(bw->parent, joe_gettext(_("Unicode (ISO-10646) character in hex (^C to abort): ")), &unicodehist, NULL, utypebw, locale_map, 0, 0, NULL);
				if (s) {
					int num;
					sscanf((char *)s,"%x",(unsigned *)&num);
					utypebw_raw(bw, num, 1);
					bw->cursor->xcol = piscol(bw->cursor);
					return 0;
				} else
					return -1;
			} else {
				quotestate = 3;
				c = query(bw->parent, sc("ASCII 0x--"), QW_STAY);
				if (c == -1)
					return -1;
				else
					goto again;
			}
		} else if (c == 'o' || c == 'O') {
			quotestate = 5;
			c = query(bw->parent, sc("ASCII 0---"), QW_STAY);
			if (c == -1)
				return -1;
			else
				goto again;
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
			c = query(bw->parent, sz(buf), QW_STAY);
			if (c == -1)
				return -1;
			else
				goto again;
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
			c = query(bw->parent, sz(buf), QW_STAY);
			if (c == -1)
				return -1;
			else
				goto again;
		} else if (c >= 'a' && c <= 'f') {
			joe_snprintf_1(buf, sizeof(buf), "ASCII 0x%c-", c + 'A' - 'a');
			quoteval = c - 'a' + 10;
			quotestate = 4;
			c = query(bw->parent, sz(buf), QW_STAY);
			if (c == -1)
				return -1;
			else
				goto again;
		} else if (c >= 'A' && c <= 'F') {
			joe_snprintf_1(buf, sizeof(buf), "ASCII 0x%c-", c);
			quoteval = c - 'A' + 10;
			quotestate = 4;
			c = query(bw->parent, sz(buf), QW_STAY);
			if (c == -1)
				return -1;
			else
				goto again;
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
			c = query(bw->parent, sz(buf), QW_STAY);
			if (c == -1)
				return -1;
			else
				goto again;
		}
		break;
	case 6:
		if (c >= '0' && c <= '7') {
			joe_snprintf_2(buf, sizeof(buf), "ASCII 0%c%c-", quoteval + '0', c);
			quoteval = quoteval * 8 + c - '0';
			quotestate = 7;
			c = query(bw->parent, sz(buf), QW_STAY);
			if (c == -1)
				return -1;
			else
				goto again;
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
	return 0;
}

int uquote8(BW *bw)
{
	int c;
	c = query(bw->parent, sc("Meta-"), QW_STAY);
	if (c == -1)
		return -1;
	if (c == '`') {
		c = query(bw->parent, sc("Meta-Ctrl-"), QW_STAY);
		if (c == -1)
			return -1;
		if ((c >= 0x40 && c <= 0x5F) || (c >= 'a' && c <= 'z'))
			c &= 0x1F;
		if (c == '?')
			c = 127;
	}
	c |= 128;
	utypebw_raw(bw, c, 1);
	bw->cursor->xcol = piscol(bw->cursor);
	return 0;

}

int uctrl(BW *bw)
{
	int c;
	int org;
	c = query(bw->parent, sz(joe_gettext(_("Quote"))), QW_STAY);
	if (c == -1)
		return -1;
	org = bw->o.overtype;
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

int usetmark(BW *bw, int c)
{
	if (c < '0' || c > ':') {
		c = query(bw->parent, sz(joe_gettext(_("Set mark (0-9):"))), QW_STAY);
		if (c == -1)
			return -1;
		if (c < '0' || c > ':') {
			nungetc(c);
			return -1;
		}
	}
	pdupown(bw->cursor, bw->b->marks + c - '0', USTR "dosetmark");
	poffline(bw->b->marks[c - '0']);
	if (c!=':') {
		msgnw(bw->parent, vsfmt(NULL, 0, joe_gettext(_("Mark %d set")), c - '0'));
	}
	return 0;
}

/* Goto book-mark */

int ugomark(BW *bw, int c)
{
	if (c < '0' || c > ':') {
		c = query(bw->parent, sz(joe_gettext(_("Set mark (0-9):"))), QW_STAY);
		if (c == -1)
			return -1;
		if (c < '0' || c > ':') {
			nungetc(c);
			return -1;
		}
	}
	if (bw->b->marks[c - '0']) {
		pset(bw->cursor, bw->b->marks[c - '0']);
		bw->cursor->xcol = piscol(bw->cursor);
		return 0;
	} else {
		msgnw(bw->parent, vsfmt(NULL, 0, joe_gettext(_("Mark %d not set")), c - '0'));
		return -1;
	}
}

/* Goto next instance of character */

int ufwrdc(BW *bw, int k)
{
	int c;
	P *q;
	if (k < 0 || k >= 256) {
		k = query(bw->parent, sz(joe_gettext(_("Forward to char: "))), QW_STAY);
		if (k < 0 || k >= 256) {
			nungetc(k);
			return -1;
		}
	}
	q = pdup(bw->cursor, USTR "dofwrdc");
	while ((c = pgetc(q)) != NO_MORE_DATA)
		if (c == k)
			break;
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

int ubkwdc(BW *bw, int k)
{
	int c;
	P *q;
	if (k < 0 || k >= 256) {
		k = query(bw->parent, sz(joe_gettext(_("Backward to char: "))), QW_STAY);
		if (k < 0 || k >= 256) {
			nungetc(k);
			return -1;
		}
	}
	q = pdup(bw->cursor, USTR "dofwrdc");
	while ((c = prgetc(q)) != NO_MORE_DATA)
		if (c == k)
			break;
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

/* Display a message */

int umsg(BASE *b)
{
	unsigned char *s;
	s = ask(b->parent, joe_gettext(_("Message (^C to abort): ")), NULL,
	        NULL, NULL, locale_map, 0, 0, NULL);

	if (!s)
		return -1;

	msgnw(b->parent, s);
	return 0;
}

/* Insert text */

int utxt(BW *bw)
{
	int x, fill;
	unsigned char *s;

	s = ask(bw->parent, joe_gettext(_("Insert (^C to abort): ")),
	        NULL, NULL, utypebw, bw->b->o.charmap, 0, 0, NULL);

	if (!s) {
		return -1;
	} else if (s[0] == '`') {
		unsigned char *str = vsmk(1024);
		fill = ' ';
		str = stagen(str, bw, &s[1], fill);
		if (str) {
			for (x = 0; x != vslen(str); ++x)
				utypebw(bw, str[x]);
		}
	} else {
		for (x = 0; x != vslen(s); ++x)
			utypebw(bw, s[x]);
	}

	return 0;
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
	const unsigned char *terminator = "\033[201~";
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
