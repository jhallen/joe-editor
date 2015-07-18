/*
 *	Syntax highlighting DFA interpreter
 *	Copyright
 *		(C) 2004 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */

#include "types.h"

/* Parse one line.  Returns new state.
   'syntax' is the loaded syntax definition for this buffer.
   'line' is advanced to start of next line.
   Global array 'attr_buf' end up with coloring for each character of line.
   'state' is initial parser state for the line (0 is initial state).
*/

int *attr_buf = 0;
int attr_size = 0;

int stack_count = 0;
static int state_count = 0; /* Max transitions possible without cycling */

struct high_syntax *ansi_syntax;

/* ANSI highlighter */

#define IDLE 0
#define AFTER_ESC 1
#define AFTER_BRACK 2
#define IN_NUMBER 3

static HIGHLIGHT_STATE ansi_parse(P *line, HIGHLIGHT_STATE h_state)
{
	int *attr = attr_buf;
	int *attr_end = attr_buf + attr_size;
	int c;

	char state = h_state.saved_s[0];
	char accu = h_state.saved_s[1];
	int current_attr = (int)h_state.state;
	// int new_attr = *(int *)(h_state.saved_s + 8);

	int ansi_mode = line->b->o.ansi;

	current_attr = 0; /* Do not let attributes cross lines - simplifies vt.c */

	line->b->o.ansi = 0;

	while ((c = pgetc(line)) != NO_MORE_DATA) {
		if (c < 0)
			c += 256;
		if (c < 0 || c > 255)
			c = 0x1F;
		if (attr == attr_end) {
			if (!attr_buf) {
				attr_size = 1024;
				attr_buf = (int *)joe_malloc(SIZEOF(int) * attr_size);
				attr = attr_buf;
			} else {
				attr_buf = (int *)joe_realloc(attr_buf, SIZEOF(int) * (attr_size * 2));
				attr = attr_buf + attr_size;
				attr_size *= 2;
			}
			attr_end = attr_buf + attr_size;
		}
		*attr++ = current_attr;
		switch (state) {
			case IDLE: {
				if (c == 27) {
					state = AFTER_ESC;
				}
				break;
			} case AFTER_ESC: {
				if (c == '[') {
					state = AFTER_BRACK;
					// new_attr = (current_attr & (FG_MASK | BG_MASK));
				} else {
					state = IDLE;
				}
				break;
			} case AFTER_BRACK: {
				if (c == ';') {
					/* RESET */
					current_attr = 0;
					/* but stay in this state */
				} else if (c >= '0' && c <= '9') {
					accu = (char)(c - '0');
					state = IN_NUMBER;
				} else if (c == 'm') {
					/* APPLY NEW ATTRIBUTES */
					current_attr = 0;
					state = IDLE;
				} else {
					state = IDLE;
				}
				break;
			} case IN_NUMBER: {
				if (c == ';' || c == 'm') {
					if (accu == 0) {
						current_attr = 0;
					} else if (accu == 1) {
						current_attr |= BOLD;
					} else if (accu == 4) {
						current_attr |= UNDERLINE;
					} else if (accu == 5) {
						current_attr |= BLINK;
					} else if (accu == 7) {
						current_attr |= INVERSE;
					} else if (accu >= 30 && accu <= 37) {
						current_attr = ((current_attr & ~FG_MASK) | FG_NOT_DEFAULT | ((accu - 30) << FG_SHIFT));
					} else if (accu >= 40 && accu <= 47) {
						current_attr = ((current_attr & ~BG_MASK) | BG_NOT_DEFAULT | ((accu - 40) << BG_SHIFT));
					}
					if (c == ';') {
						accu = 0;
						state = IN_NUMBER;
					} else if (c == 'm') {
						state = IDLE;
					}
				} else if (c >= '0' && c <= '9') {
					accu = (char)(accu * 10 + c - '0');
				} else {
					state = IDLE;
				}
			}
		}
		if (c == '\n')
			break;
	}
	line->b->o.ansi = ansi_mode;
	h_state.saved_s[0] = state;
	h_state.saved_s[1] = accu;
	h_state.state = current_attr;
//	*(int *)(h_state.saved_s + 8) = new_attr;
	return h_state;
}

HIGHLIGHT_STATE parse(struct high_syntax *syntax,P *line,HIGHLIGHT_STATE h_state)
{
	struct high_frame *stack;
	struct high_state *h;
			/* Current state */
	char buf[24];		/* Name buffer (trunc after 23 characters) */
	char lbuf[24];		/* Lower case version of name buffer */
	char lsaved_s[24];	/* Lower case version of delimiter match buffer */
	int buf_idx;	/* Index into buffer */
	int c;		/* Current character */
	int *attr;
	int *attr_end;
	int buf_en;	/* Set for name buffering */
	int ofst;	/* record offset after we've stopped buffering */
	int mark1;	/* offset to mark start from current pos */
	int mark2;	/* offset to mark end from current pos */
	int mark_en;	/* set if marking */
	int recolor_delimiter_or_keyword;
	
	/* Nothing should reference 'h' above here. */
	if (h_state.state < 0) {
		/* Indicates a previous error -- highlighting disabled */
		return h_state;
	}

	if (syntax == ansi_syntax)
		return ansi_parse(line, h_state);

	stack = h_state.stack;
	h = (stack ? stack->syntax : syntax)->states[h_state.state];
	buf_idx = 0;
	attr = attr_buf;
	attr_end = attr_buf + attr_size;
	buf_en = 0;
	ofst = 0;
	mark1 = 0;
	mark2 = 0;
	mark_en = 0;

	buf[0]=0;	/* Forgot this originally... took 5 months to fix! */

	/* Get next character */
	while((c=pgetc(line))!=NO_MORE_DATA) {
		struct high_cmd *cmd, *kw_cmd;
		int iters = -8; /* +8 extra iterations before cycle detect. */
		ptrdiff_t x;

		/* Fix signed char promoted to int */
		if (c < 0)
			c += 256;

		/* Hack so we can have UTF-8 characters without crashing */
		if (c < 0 || c > 255)
			c = 0x1F;

		/* Create or expand attribute array if necessary */
		if(attr==attr_end) {
			if(!attr_buf) {
				attr_size = 1024;
				attr_buf = (int *)joe_malloc(SIZEOF(int)*attr_size);
				attr = attr_buf;
			} else {
				attr_buf = (int *)joe_realloc(attr_buf,SIZEOF(int)*(attr_size*2));
				attr = attr_buf + attr_size;
				attr_size *= 2;
			}
			attr_end = attr_buf + attr_size;
		}

		/* Advance to next attribute position (note attr[-1] below) */
		attr++;

		/* Loop while noeat */
		do {
			/* Guard against infinite loops from buggy syntaxes */
			if (iters++ > state_count) {
				invalidate_state(&h_state);
				return h_state;
			}
			
			/* Color with current state */
			attr[-1] = h->color;

			/* Get command for this character */
			if (h->delim && c == h_state.saved_s[0] && h_state.saved_s[1] == 0)
				cmd = h->delim;
			else {
				struct bind b = cmap_lookup(&h->cmap, c);
				cmd = (struct high_cmd *)b.thing;
			}

			/* Lowerize strings for case-insensitive matching */
			if (cmd->ignore) {
				zlcpy(lbuf, SIZEOF(lbuf), buf);
				lowerize(lbuf);
				if (cmd->delim) {
					zlcpy(lsaved_s, SIZEOF(lsaved_s), h_state.saved_s);
					lowerize(lsaved_s);
				}
			}

			/* Check for delimiter or keyword matches */
			recolor_delimiter_or_keyword = 0;
			if (cmd->delim && (cmd->ignore ? !zcmp(lsaved_s,lbuf) : !zcmp(h_state.saved_s,buf))) {
				cmd = cmd->delim;
				recolor_delimiter_or_keyword = 1;
			} else if (cmd->keywords && (cmd->ignore ? (kw_cmd=(struct high_cmd *)htfind(cmd->keywords,lbuf)) : (kw_cmd=(struct high_cmd *)htfind(cmd->keywords,buf)))) {
				cmd = kw_cmd;
				recolor_delimiter_or_keyword = 1;
			}

			/* Determine new state */
			if (cmd->call) {
				/* Call */
				struct high_frame **frame_ptr = stack ? &stack->child : &syntax->stack_base;
				/* Search for an existing stack frame for this call */
				while (*frame_ptr && !((*frame_ptr)->syntax == cmd->call && (*frame_ptr)->return_state == cmd->new_state))
					frame_ptr = &(*frame_ptr)->sibling;
				if (*frame_ptr)
					stack = *frame_ptr;
				else {
					struct high_frame *frame = (struct high_frame *)joe_malloc(SIZEOF(struct high_frame));
					frame->parent = stack;
					frame->child = 0;
					frame->sibling = 0;
					frame->syntax = cmd->call;
					frame->return_state = cmd->new_state;
					*frame_ptr = frame;
					stack = frame;
					++stack_count;
				}
				h = stack->syntax->states[0];
			} else if (cmd->rtn) {
				/* Return */
				if (stack) {
					h = stack->return_state;
					stack = stack->parent;
				} else
					/* Not in a subroutine, so ignore the return */
					h = cmd->new_state;
			} else if (cmd->reset) {
				/* Reset the state and call stack */
				h = syntax->states[0];
				stack = syntax->stack_base;
			} else {
				/* Normal edge */
				h = cmd->new_state;
			}

			/* Recolor if necessary */
			if (recolor_delimiter_or_keyword)
				for(x= -(buf_idx+1);x<-1;++x)
					attr[x-ofst] = h->color;
			for(x=cmd->recolor;x<0;++x)
				if (attr + x >= attr_buf)
					attr[x] = h->color;

			/* Mark recoloring */
			if (cmd->recolor_mark)
				for(x= -mark1;x<-mark2;++x)
					attr[x] = h->color;

			/* Save string? */
			if (cmd->save_s)
				zlcpy(h_state.saved_s, SIZEOF(h_state.saved_s), buf);

			/* Save character? */
			if (cmd->save_c) {
				h_state.saved_s[1] = 0;
				if (c=='<')
					h_state.saved_s[0] = '>';
				else if (c=='(')
					h_state.saved_s[0] = ')';
				else if (c=='[')
					h_state.saved_s[0] = ']';
				else if (c=='{')
					h_state.saved_s[0] = '}';
				else if (c=='`')
					h_state.saved_s[0] = '\'';
				else
					h_state.saved_s[0] = TO_CHAR_OK(c);
			}

			/* Start buffering? */
			if (cmd->start_buffering) {
				buf_idx = 0;
				buf_en = 1;
				ofst = 0;
			}

			/* Stop buffering? */
			if (cmd->stop_buffering)
				buf_en = 0;

			/* Set mark begin? */
			if (cmd->start_mark)
			{
				mark2 = 1;
				mark1 = 1;
				mark_en = 1;
			}

			/* Set mark end? */
			if(cmd->stop_mark)
			{
				mark_en = 0;
				mark2 = 1;
			}
		} while(cmd->noeat);

		/* Save character in buffer */
		if (buf_idx<23 && buf_en)
			buf[buf_idx++] = TO_CHAR_OK(c);
		if (!buf_en)
			++ofst;
		buf[buf_idx] = 0;

		/* Update mark pointers */
		++mark1;
		if(!mark_en)
			++mark2;

		if(c=='\n')
			break;
	}
	/* Return new state */
	h_state.stack = stack;
	h_state.state = h->no;
	return h_state;
}

/* Subroutines for load_dfa() */

static struct high_state *find_state(struct high_syntax *syntax,char *name)
{
	struct high_state *state;

	/* Find state */
	state = (struct high_state *)htfind(syntax->ht_states, name);

	/* It doesn't exist, so create it */
	if(!state) {
		state=(struct high_state *)joe_malloc(SIZEOF(struct high_state));
		state->name=zdup(name);
		state->no=syntax->nstates;
		state->color=FG_WHITE;
		/* Expand the state table if necessary */
		if(syntax->nstates==syntax->szstates)
			syntax->states=(struct high_state **)joe_realloc(syntax->states,SIZEOF(struct high_state *)*(syntax->szstates*=2));
		syntax->states[syntax->nstates++]=state;
		state->src = 0;
		state->dflt = &syntax->default_cmd;
		state->delim = 0;
		htadd(syntax->ht_states, state->name, state);
		++state_count;
	}
	return state;
}

/* Build cmaps */

static void build_cmaps(struct high_syntax *syntax)
{
	int x;
	for (x = 0; x != syntax->ht_states->len; ++x) {
		HENTRY *p;
		for (p = syntax->ht_states->tab[x]; p; p = p->next) {
			struct high_state *st = (struct high_state *)p->val;
			cmap_build(&st->cmap, st->src, mkbinding(st->dflt, 0));
		}
	}
}

/* Create empty command */

static void iz_cmd(struct high_cmd *cmd)
{
	cmd->noeat = 0;
	cmd->recolor = 0;
	cmd->start_buffering = 0;
	cmd->stop_buffering = 0;
	cmd->save_c = 0;
	cmd->save_s = 0;
	cmd->new_state = 0;
	cmd->keywords = 0;
	cmd->delim = 0;
	cmd->ignore = 0;
	cmd->start_mark = 0;
	cmd->stop_mark = 0;
	cmd->recolor_mark = 0;
	cmd->rtn = 0;
	cmd->reset = 0;
	cmd->call = 0;
}

static struct high_cmd *mkcmd()
{
	struct high_cmd *cmd = (struct high_cmd *)joe_malloc(SIZEOF(struct high_cmd));
	iz_cmd(cmd);
	return cmd;
}

/* Globally defined colors */

struct high_color *global_colors;

static struct high_color *find_color(struct high_color *colors,char *name,char *syn)
{
	char bf[256];
	struct high_color *color;
	joe_snprintf_2(bf, SIZEOF(bf), "%s.%s", syn, name);
	for (color = colors; color; color = color->next)
		if (!zcmp(color->name,bf)) break;
	if (color)
		return color;
	for (color = colors; color; color = color->next)
		if (!zcmp(color->name,name)) break;
	return color;
}

void parse_color_def(struct high_color **color_list,const char *p,char *name,int line)
{
	char bf[256];
	if(!parse_tows(&p, bf)) {
		struct high_color *color, *gcolor;

		/* Find color */
		color=find_color(*color_list,bf,name);

		/* If it doesn't exist, create it */
		if(!color) {
			color = (struct high_color *)joe_malloc(SIZEOF(struct high_color));
			color->name = zdup(bf);
			color->color = 0;
			color->next = *color_list;
			*color_list = color;
		} else {
			logerror_2(joe_gettext(_("%s %d: Class already defined\n")),name,line);
		}

		/* Find it in global list */
		if (color_list != &global_colors && (gcolor=find_color(global_colors,bf,name))) {
			color->color = gcolor->color;
		} else {
			/* Parse color definition */
			while(parse_ws(&p,'#'), !parse_ident(&p,bf,SIZEOF(bf))) {
				color->color |= meta_color(bf);
			}
		}
	} else {
		logerror_2(joe_gettext(_("%s %d: Missing class name\n")),name,line);
	}
}

/* Load syntax file */

struct high_syntax *syntax_list;

/* Dump sytnax file */

void dump_syntax(BW *bw)
{
	struct high_syntax *syntax;
	struct high_param *params;
	char buf[1024];
	joe_snprintf_1(buf, SIZEOF(buf), "Allocated %d stack frames\n", stack_count);
	binss(bw->cursor, buf);
	pnextl(bw->cursor);
	for (syntax = syntax_list; syntax; syntax = syntax->next) {
		int x;
		joe_snprintf_3(buf, SIZEOF(buf), "Syntax name=%s, subr=%s, nstates=%d\n",syntax->name,syntax->subr,(int)syntax->nstates);
		binss(bw->cursor, buf);
		pnextl(bw->cursor);
		zlcpy(buf, SIZEOF(buf), "params=(");
		for(params = syntax->params; params; params = params->next) {
			zlcat(buf, SIZEOF(buf), " ");
			zlcat(buf, SIZEOF(buf), params->name);
		}
		zlcat(buf, SIZEOF(buf), " )\n");
		binss(bw->cursor, buf);
		pnextl(bw->cursor);
		for(x=0;x!=syntax->nstates;++x) {
			struct high_state *s = syntax->states[x];
			struct interval_list *l;
			joe_snprintf_2(buf, SIZEOF(buf), "   state %s %x\n",s->name,s->color);
			binss(bw->cursor, buf);
			pnextl(bw->cursor);
			for (l = s->src; l; l = l->next) {
				struct high_cmd *h = (struct high_cmd *)l->map.thing;
				joe_snprintf_4(buf, SIZEOF(buf), "     [%d-%d] -> %s %d\n",l->interval.first,l->interval.last,(h->new_state ? h->new_state->name : "ERROR! Unknown state!"),(int)h->recolor);
				binss(bw->cursor, buf);
				pnextl(bw->cursor);
			}
			joe_snprintf_2(buf, SIZEOF(buf), "     default -> %s %d\n",(s->dflt->new_state ? s->dflt->new_state->name : "ERROR! Unknown state!"),(int)s->dflt->recolor);
			binss(bw->cursor, buf);
			pnextl(bw->cursor);
		}
	}
}

static struct high_param *parse_params(struct high_param *current_params,const char **ptr,char *name,int line)
{
	const char *p = *ptr;
	char bf[256];
	struct high_param *params;
	struct high_param **param_ptr;

	/* Propagate currently defined parameters */
	param_ptr = &params;
	while (current_params) {
		*param_ptr = (struct high_param *)joe_malloc(SIZEOF(struct high_param));
		(*param_ptr)->name = zdup(current_params->name);
		param_ptr = &(*param_ptr)->next;
		current_params = current_params->next;
	}
	*param_ptr = 0;

	parse_ws(&p, '#');
	if (!parse_char(&p, '(')) {
		for (;;) {
			parse_ws(&p, '#');
			if (!parse_char(&p, ')'))
				break;
			else if (!parse_char(&p, '-')) {
				if (!parse_ident(&p,bf,SIZEOF(bf))) {
					int cmp = 0;
					param_ptr = &params;
					/* Parameters are sorted */
					while (*param_ptr && (cmp = zcmp(bf,(*param_ptr)->name)) > 0)
						param_ptr = &(*param_ptr)->next;
					if (*param_ptr && !cmp) {
						/* Remove this parameter */
						struct high_param *param = *param_ptr;
						*param_ptr = param->next;
						joe_free(param);
					}
				} else {
					logerror_2(joe_gettext(_("%s %d: Missing parameter name\n")),name,line);
				}
			} else if (!parse_ident(&p,bf,SIZEOF(bf))) {
				int cmp = 0;
				param_ptr = &params;
				/* Keep parameters sorted */
				while (*param_ptr && (cmp = zcmp(bf,(*param_ptr)->name)) > 0)
					param_ptr = &(*param_ptr)->next;
				/* Discard duplicates */
				if (!*param_ptr || cmp) {
					struct high_param *param = (struct high_param *)joe_malloc(SIZEOF(struct high_param));
					param->name = zdup(bf);
					param->next = *param_ptr;
					*param_ptr = param;
				}
			} else {
				logerror_2(joe_gettext(_("%s %d: Missing )\n")),name,line);
				break;
			}
		}
	}

	*ptr = p;
	return params;
}


struct high_syntax *load_syntax_subr(const char *name,char *subr,struct high_param *params);

/* Parse options */

static int parse_options(struct high_syntax *syntax,struct high_cmd *cmd,JFILE *f,const char *p,int parsing_strings,char *name,int line)
{
	char buf[1024];
	char bf[256];
	char bf1[256];

	while (parse_ws(&p,'#'), !parse_ident(&p,bf,SIZEOF(bf)))
		if(!zcmp(bf,"buffer")) {
			cmd->start_buffering = 1;
		} else if(!zcmp(bf,"hold")) {
			cmd->stop_buffering = 1;
		} else if(!zcmp(bf,"save_c")) {
			cmd->save_c = 1;
		} else if(!zcmp(bf,"save_s")) {
			cmd->save_s = 1;
		} else if(!zcmp(bf,"recolor")) {
			parse_ws(&p,'#');
			if(!parse_char(&p,'=')) {
				parse_ws(&p,'#');
				if(parse_diff(&p,&cmd->recolor))
					logerror_2(joe_gettext(_("%s %d: Missing value for option\n")),name,line);
			} else
				logerror_2(joe_gettext(_("%s %d: Missing value for option\n")),name,line);
		} else if(!zcmp(bf,"call")) {
			parse_ws(&p,'#');
			if(!parse_char(&p,'=')) {
				parse_ws(&p,'#');
				if (!parse_char(&p,'.')) {
					zlcpy(bf, SIZEOF(bf), syntax->name);
					goto subr;
				} else if (parse_ident(&p,bf,SIZEOF(bf)))
					logerror_2(joe_gettext(_("%s %d: Missing value for option\n")),name,line);
				else {
					if (!parse_char(&p,'.')) {
						subr:
						if (parse_ident(&p,bf1,SIZEOF(bf1)))
							logerror_2(joe_gettext(_("%s %d: Missing subroutine name\n")),name,line);
						cmd->call = load_syntax_subr(bf,bf1,parse_params(syntax->params,&p,name,line));
					} else
						cmd->call = load_syntax_subr(bf,0,parse_params(syntax->params,&p,name,line));
				}
			} else
				logerror_2(joe_gettext(_("%s %d: Missing value for option\n")),name,line);
		} else if(!zcmp(bf,"return")) {
			cmd->rtn = 1;
		} else if(!zcmp(bf,"reset")) {
			cmd->reset = 1;
		} else if(!parsing_strings && (!zcmp(bf,"strings") || !zcmp(bf,"istrings"))) {
			if (bf[0]=='i')
				cmd->ignore = 1;
			while(jfgets(buf,sizeof(buf),f)) {
				++line;
				p = buf;
				parse_ws(&p,'#');
				if (*p) {
					if(!parse_field(&p,"done"))
						break;
					if(parse_string(&p,bf,SIZEOF(bf)) >= 0) {
						parse_ws(&p,'#');
						if (cmd->ignore)
							lowerize(bf);
						if(!parse_ident(&p,bf1,SIZEOF(bf1))) {
							struct high_cmd *kw_cmd=mkcmd();
							kw_cmd->noeat=1;
							kw_cmd->new_state = find_state(syntax,bf1);
							if (!zcmp(bf, "&")) {
								cmd->delim = kw_cmd;
							} else {
								if(!cmd->keywords)
									cmd->keywords = htmk(64);
								htadd(cmd->keywords,zdup(bf),kw_cmd);
							}
							line = parse_options(syntax,kw_cmd,f,p,1,name,line);
						} else
							logerror_2(joe_gettext(_("%s %d: Missing state name\n")),name,line);
					} else
						logerror_2(joe_gettext(_("%s %d: Missing string\n")),name,line);
				}
			}
		} else if(!zcmp(bf,"noeat")) {
			cmd->noeat = 1;
		} else if(!zcmp(bf,"mark")) {
			cmd->start_mark = 1;
		} else if(!zcmp(bf,"markend")) {
			cmd->stop_mark = 1;
		} else if(!zcmp(bf,"recolormark")) {
			cmd->recolor_mark = 1;
		} else
			logerror_2(joe_gettext(_("%s %d: Unknown option\n")),name,line);
	return line;
}

struct ifstack {
	struct ifstack *next;
	int ignore;	/* Ignore input lines if set */
	int skip;	/* Set to skip the else part */
	int else_part;	/* Set if we're in the else part */
	int line;
};

/* Load dfa */

static struct high_state *load_dfa(struct high_syntax *syntax)
{
	char name[1024];
	char buf[1024];
	char bf[256];
	const char *p;
	ptrdiff_t c;
	JFILE *f = 0;
	struct ifstack *stack=0;
	struct high_state *state=0;	/* Current state */
	struct high_state *first=0;	/* First state */
	int line = 0;
	int this_one = 0;
	int inside_subr = 0;

	/* Load it */
	p = getenv("HOME");
	if (p) {
		joe_snprintf_2(name,SIZEOF(name),"%s/.joe/syntax/%s.jsf",p,syntax->name);
		f = jfopen(name,"r");
	}

	if (!f) {
		joe_snprintf_2(name,SIZEOF(name),"%ssyntax/%s.jsf",JOEDATA,syntax->name);
		f = jfopen(name,"r");
	}
	if (!f) {
		joe_snprintf_1(name,SIZEOF(name),"*%s.jsf",syntax->name);
		f = jfopen(name,"r");
	}
	if (!f) {
		return 0;
	}

	/* Parse file */
	while(jfgets(buf,SIZEOF(buf),f)) {
		++line;
		p = buf;
		c = parse_ws(&p,'#');
		if (!parse_char(&p, '.')) {
			if (!parse_ident(&p, bf, SIZEOF(bf))) {
				if (!zcmp(bf, "ifdef")) {
					struct ifstack *st = (struct ifstack *)joe_malloc(SIZEOF(struct ifstack));
					st->next = stack;
					st->else_part = 0;
					st->ignore = 1;
					st->skip = 1;
					st->line = line;
					if (!stack || !stack->ignore) {
						parse_ws(&p,'#');
						if (!parse_ident(&p, bf, SIZEOF(bf))) {
							struct high_param *param;
							for (param = syntax->params; param; param = param->next)
								if (!zcmp(param->name, bf)) {
									st->ignore = 0;
									break;
								}
							st->skip = 0;
						} else {
							logerror_2(joe_gettext(_("%s %d: missing parameter for ifdef\n")),name,line);
						}
					}
					stack = st;
				} else if (!zcmp(bf, "else")) {
					if (stack && !stack->else_part) {
						stack->else_part = 1;
						if (!stack->skip)
							stack->ignore = !stack->ignore;
					} else
						logerror_2(joe_gettext(_("%s %d: else with no matching if\n")),name,line);
				} else if (!zcmp(bf, "endif")) {
					if (stack) {
						struct ifstack *st = stack;
						stack = st->next;
						joe_free(st);
					} else
						logerror_2(joe_gettext(_("%s %d: endif with no matching if\n")),name,line);
				} else if (!zcmp(bf, "subr")) {
					parse_ws(&p, '#');
					if (parse_ident(&p, bf, SIZEOF(bf))) {
						logerror_2(joe_gettext(_("%s %d: Missing subroutine name\n")),name,line);
					} else {
						if (!stack || !stack->ignore) {
							inside_subr = 1;
							this_one = 0;
							if (syntax->subr && !zcmp(bf, syntax->subr))
								this_one = 1;
						}
					}
				} else if (!zcmp(bf, "end")) {
					if (!stack || !stack->ignore) {
						this_one = 0;
						inside_subr = 0;
					}
				} else {
					logerror_2(joe_gettext(_("%s %d: Unknown control statement\n")),name,line);
				}
			} else {
				logerror_2(joe_gettext(_("%s %d: Missing control statement name\n")),name,line);
			}
		} else if (stack && stack->ignore) {
			/* Ignore this line because of ifdef */
		} else if(!parse_char(&p, '=')) {
			/* Parse color */
			parse_color_def(&syntax->color,p,name,line);
		} else if ((syntax->subr && !this_one) || (!syntax->subr && inside_subr)) {
			/* Ignore this line because it's not the code we want */
		} else if(!parse_char(&p, ':')) {
			if(!parse_ident(&p, bf, SIZEOF(bf))) {

				state = find_state(syntax,bf);

				if (!first)
					first = state;

				parse_ws(&p,'#');
				if(!parse_tows(&p,bf)) {
					struct high_color *color;
					for(color=syntax->color;color;color=color->next)
						if(!zcmp(color->name,bf))
							break;
					if(color)
						state->color=color->color;
					else {
						state->color=0;
						logerror_2(joe_gettext(_("%s %d: Unknown class\n")),name,line);
					}
					while(parse_ws(&p, '#'), !parse_ident(&p, bf, SIZEOF(bf))) {
						if(!zcmp(bf, "comment")) {
							state->color |= CONTEXT_COMMENT;
						} else if(!zcmp(bf, "string")) {
							state->color |= CONTEXT_STRING;
						} else {
							logerror_2(joe_gettext(_("%s %d: Unknown context\n")),name,line);
						}
					}
				} else
					logerror_2(joe_gettext(_("%s %d: Missing color for state definition\n")),name,line);
			} else
				logerror_2(joe_gettext(_("%s %d: Missing state name\n")),name,line);
		} else if(!parse_char(&p, '-')) {
			/* No. sync lines ignored */
		} else {
			c = parse_ws(&p,'#');

			if (!c) {
			} else if (c=='"' || c=='*' || c=='&') {
				if (state) {
					struct high_cmd *cmd = mkcmd();
					if(!parse_field(&p, "*")) {
						state->dflt = cmd;
					} else if(!parse_field(&p, "&")) {
						state->delim = cmd;
					} else {
						c = parse_string(&p, bf, SIZEOF(bf));
						if(c < 0)
							logerror_2(joe_gettext(_("%s %d: Bad string\n")),name,line);
						else {
							int tfirst, tsecond;
							const char *t = bf;
							while(!parse_range(&t, &tfirst, &tsecond)) {
								if(tfirst>tsecond)
									tsecond = tfirst;
								state->src = interval_add(state->src, tfirst, tsecond, mkbinding(cmd, 0));
							}
						}
					}
					/* Create command */
					parse_ws(&p,'#');
					if(!parse_ident(&p,bf,SIZEOF(bf))) {
						cmd->new_state = find_state(syntax,bf);
						line = parse_options(syntax,cmd,f,p,0,name,line);
					} else
						logerror_2(joe_gettext(_("%s %d: Missing jump\n")),name,line);
				} else
					logerror_2(joe_gettext(_("%s %d: No state\n")),name,line);
			} else
				logerror_2(joe_gettext(_("%s %d: Unknown character\n")),name,line);
		}
	}

	while (stack) {
		struct ifstack *st = stack;
		stack = st->next;
		logerror_2(joe_gettext(_("%s %d: ifdef with no matching endif\n")),name,st->line);
		joe_free(st);
	}

	jfclose(f);

	/* Compile cmaps */
	build_cmaps(syntax);

	return first;
}

static int syntax_match(struct high_syntax *syntax,const char *name,const char *subr,struct high_param *params)
{
	struct high_param *syntax_params;
	if (zcmp(syntax->name,name))
		return 0;
	if (!syntax->subr ^ !subr)
		return 0;
	if (subr && zcmp(syntax->subr,subr))
		return 0;
	syntax_params = syntax->params;
	while (syntax_params && params) {
		if (zcmp(syntax_params->name,params->name))
			return 0;
		syntax_params = syntax_params->next;
		params = params->next;
	}
	return syntax_params == params;
}

struct high_syntax *load_syntax_subr(const char *name,char *subr,struct high_param *params)
{
	struct high_syntax *syntax;	/* New syntax table */

	/* Find syntax table */

	/* Already loaded? */
	for(syntax=syntax_list;syntax;syntax=syntax->next)
		if(syntax_match(syntax,name,subr,params))
			return syntax;

	/* Create new one */
	syntax = (struct high_syntax *)joe_malloc(SIZEOF(struct high_syntax));
	syntax->name = zdup(name);
	syntax->subr = subr ? zdup(subr) : 0;
	syntax->params = params;
	syntax->next = syntax_list;
	syntax->nstates = 0;
	syntax->color = 0;
	syntax->states = (struct high_state **)joe_malloc(SIZEOF(struct high_state *)*(syntax->szstates = 64));
	syntax->ht_states = htmk(syntax->szstates);
	iz_cmd(&syntax->default_cmd);
	syntax->default_cmd.reset = 1;
	syntax->stack_base = 0;
	syntax_list = syntax;

	if (load_dfa(syntax)) {
		/* dump_syntax(syntax); */
		return syntax;
	} else {
		if(syntax_list == syntax)
			syntax_list = syntax_list->next;
		else {
			struct high_syntax *syn;
			for(syn=syntax_list;syn->next!=syntax;syn=syn->next);
			syn->next = syntax->next;
		}
		htrm(syntax->ht_states);
		joe_free(syntax->name);
		joe_free(syntax->states);
		joe_free(syntax);
		return 0;
	}
}

struct high_syntax *load_syntax(const char *name)
{
	if (!ansi_syntax) {
		ansi_syntax = (struct high_syntax *)joe_calloc(1, SIZEOF(struct high_syntax));
		ansi_syntax->name = zdup("ansi");
	}

	if (!name)
		return 0;

	if (!zcmp(name, "ansi"))
		return ansi_syntax;

	return load_syntax_subr(name,0,0);
}
