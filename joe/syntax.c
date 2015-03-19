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

struct high_syntax ansi_syntax[1] = { { NULL, USTR "ansi" } };

/* ANSI highlighter */

#define IDLE 0
#define AFTER_ESC 1
#define AFTER_BRACK 2
#define IN_NUMBER 3

HIGHLIGHT_STATE ansi_parse(P *line, HIGHLIGHT_STATE h_state)
{
	int *attr = attr_buf;
	int *attr_end = attr_buf + attr_size;
	int c;

	int state = h_state.saved_s[0];
	int accu = h_state.saved_s[1];
	int current_attr = h_state.state;
	// int new_attr = *(int *)(h_state.saved_s + 8);

	int ansi_mode = line->b->o.ansi;

	current_attr = 0; /* Do not let attributes cross lines - simplifies vt.c */

	line->b->o.ansi = 0;

	while ((c = pgetc(line)) != NO_MORE_DATA) {
		if (c < 0 || c > 255)
			c = 0x1F;
		if (attr == attr_end) {
			if (!attr_buf) {
				attr_size = 1024;
				attr_buf = joe_malloc(sizeof(int) * attr_size);
				attr = attr_buf;
			} else {
				attr_buf = joe_realloc(attr_buf, sizeof(int) * (attr_size * 2));
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
					accu = c - '0';
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
					accu = accu * 10 + c - '0';
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
	unsigned char buf[24];		/* Name buffer (trunc after 23 characters) */
	unsigned char lbuf[24];		/* Lower case version of name buffer */
	unsigned char lsaved_s[24];	/* Lower case version of delimiter match buffer */
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
		int x;

		/* Hack so we can have UTF-8 characters without crashing */
		if (c < 0 || c > 255)
			c = 0x1F;

		/* Create or expand attribute array if necessary */
		if(attr==attr_end) {
			if(!attr_buf) {
				attr_size = 1024;
				attr_buf = joe_malloc(sizeof(int)*attr_size);
				attr = attr_buf;
			} else {
				attr_buf = joe_realloc(attr_buf,sizeof(int)*(attr_size*2));
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
			else
				cmd = h->cmd[c];

			/* Lowerize strings for case-insensitive matching */
			if (cmd->ignore) {
				zlcpy(lbuf, sizeof(lbuf), buf);
				lowerize(lbuf);
				if (cmd->delim) {
					zlcpy(lsaved_s, sizeof(lsaved_s), h_state.saved_s);
					lowerize(lsaved_s);
				}
			}

			/* Check for delimiter or keyword matches */
			recolor_delimiter_or_keyword = 0;
			if (cmd->delim && (cmd->ignore ? !zcmp(lsaved_s,lbuf) : !zcmp(h_state.saved_s,buf))) {
				cmd = cmd->delim;
				recolor_delimiter_or_keyword = 1;
			} else if (cmd->keywords && (cmd->ignore ? (kw_cmd=htfind(cmd->keywords,lbuf)) : (kw_cmd=htfind(cmd->keywords,buf)))) {
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
					struct high_frame *frame = joe_malloc(sizeof(struct high_frame));
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
				zlcpy(h_state.saved_s, sizeof(h_state.saved_s), buf);

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
					h_state.saved_s[0] = c;
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
			buf[buf_idx++]=c;
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

static struct high_state *find_state(struct high_syntax *syntax,unsigned char *name)
{
	struct high_state *state;

	/* Find state */
	state = htfind(syntax->ht_states, name);

	/* It doesn't exist, so create it */
	if(!state) {
		int y;
		state=joe_malloc(sizeof(struct high_state));
		state->name=zdup(name);
		state->no=syntax->nstates;
		state->color=FG_WHITE;
		/* Expand the state table if necessary */
		if(syntax->nstates==syntax->szstates)
			syntax->states=joe_realloc(syntax->states,sizeof(struct high_state *)*(syntax->szstates*=2));
		syntax->states[syntax->nstates++]=state;
		for(y=0; y!=256; ++y)
			state->cmd[y] = &syntax->default_cmd;
		state->delim = 0;
		htadd(syntax->ht_states, state->name, state);
		++state_count;
	}
	return state;
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
	struct high_cmd *cmd = joe_malloc(sizeof(struct high_cmd));
	iz_cmd(cmd);
	return cmd;
}

/* Globally defined colors */

struct high_color *global_colors;

struct high_color *find_color(struct high_color *colors,unsigned char *name,unsigned char *syn)
{
	unsigned char bf[256];
	struct high_color *color;
	joe_snprintf_2(bf, sizeof(bf), "%s.%s", syn, name);
	for (color = colors; color; color = color->next)
		if (!zcmp(color->name,bf)) break;
	if (color)
		return color;
	for (color = colors; color; color = color->next)
		if (!zcmp(color->name,name)) break;
	return color;
}

void parse_color_def(struct high_color **color_list,unsigned char *p,unsigned char *name,int line)
{
	unsigned char bf[256];
	if(!parse_tows(&p, bf)) {
		struct high_color *color, *gcolor;

		/* Find color */
		color=find_color(*color_list,bf,name);

		/* If it doesn't exist, create it */
		if(!color) {
			color = joe_malloc(sizeof(struct high_color));
			color->name = zdup(bf);
			color->color = 0;
			color->next = *color_list;
			*color_list = color;
		} else {
			logerror_2((char *)joe_gettext(_("%s %d: Class already defined\n")),name,line);
		}

		/* Find it in global list */
		if (color_list != &global_colors && (gcolor=find_color(global_colors,bf,name))) {
			color->color = gcolor->color;
		} else {
			/* Parse color definition */
			while(parse_ws(&p,'#'), !parse_ident(&p,bf,sizeof(bf))) {
				color->color |= meta_color(bf);
			}
		}
	} else {
		logerror_2((char *)joe_gettext(_("%s %d: Missing class name\n")),name,line);
	}
}

/* Load syntax file */

struct high_syntax *syntax_list;

/* Dump sytnax file */

void dump_syntax(BW *bw)
{
	struct high_syntax *syntax;
	struct high_param *params;
	unsigned char buf[1024];
	joe_snprintf_1(buf, sizeof(buf), "Allocated %d stack frames\n", stack_count);
	binss(bw->cursor, buf);
	pnextl(bw->cursor);
	for (syntax = syntax_list; syntax; syntax = syntax->next) {
		int x;
		joe_snprintf_3(buf, sizeof(buf), "Syntax name=%s, subr=%s, nstates=%d\n",syntax->name,syntax->subr,syntax->nstates);
		binss(bw->cursor, buf);
		pnextl(bw->cursor);
		zlcpy(buf, sizeof(buf), USTR "params=(");
		for(params = syntax->params; params; params = params->next) {
			zlcat(buf, sizeof(buf), USTR " ");
			zlcat(buf, sizeof(buf), params->name);
		}
		zlcat(buf, sizeof(buf), USTR " )\n");
		binss(bw->cursor, buf);
		pnextl(bw->cursor);
		for(x=0;x!=syntax->nstates;++x) {
			int y;
			int f = -1;
			struct high_state *s = syntax->states[x];
			joe_snprintf_2(buf, sizeof(buf), "   state %s %x\n",s->name,s->color);
			binss(bw->cursor, buf);
			pnextl(bw->cursor);
			for (y = 0; y != 256; ++y) {
				if (f == -1)
					f = y;
				else if (s->cmd[f]->new_state != s->cmd[y]->new_state) {
					joe_snprintf_4(buf, sizeof(buf), "     [%d-%d] -> %s %d\n",f,y-1,(s->cmd[f]->new_state ? s->cmd[f]->new_state->name : USTR "ERROR! Unknown state!"),s->cmd[f]->recolor);
					binss(bw->cursor, buf);
					pnextl(bw->cursor);
					f = y;
				}
			}
			joe_snprintf_4(buf, sizeof(buf), "     [%d-%d] -> %s %d\n",f,y-1,(s->cmd[f]->new_state ? s->cmd[f]->new_state->name : USTR "ERROR! Unknown state!"),s->cmd[f]->recolor);
			binss(bw->cursor, buf);
			pnextl(bw->cursor);
		}
	}
}

struct high_param *parse_params(struct high_param *current_params,unsigned char **ptr,unsigned char *name,int line)
{
	unsigned char *p = *ptr;
	unsigned char bf[256];
	struct high_param *params;
	struct high_param **param_ptr;

	/* Propagate currently defined parameters */
	param_ptr = &params;
	while (current_params) {
		*param_ptr = joe_malloc(sizeof(struct high_param));
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
				if (!parse_ident(&p,bf,sizeof(bf))) {
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
					logerror_2((char *)joe_gettext(_("%s %d: Missing parameter name\n")),name,line);
				}
			} else if (!parse_ident(&p,bf,sizeof(bf))) {
				int cmp = 0;
				param_ptr = &params;
				/* Keep parameters sorted */
				while (*param_ptr && (cmp = zcmp(bf,(*param_ptr)->name)) > 0)
					param_ptr = &(*param_ptr)->next;
				/* Discard duplicates */
				if (!*param_ptr || cmp) {
					struct high_param *param = joe_malloc(sizeof(struct high_param));
					param->name = zdup(bf);
					param->next = *param_ptr;
					*param_ptr = param;
				}
			} else {
				logerror_2((char *)joe_gettext(_("%s %d: Missing )\n")),name,line);
				break;
			}
		}
	}

	*ptr = p;
	return params;
}


struct high_syntax *load_syntax_subr(unsigned char *name,unsigned char *subr,struct high_param *params);

/* Parse options */

int parse_options(struct high_syntax *syntax,struct high_cmd *cmd,JFILE *f,unsigned char *p,int parsing_strings,unsigned char *name,int line)
{
	unsigned char buf[1024];
	unsigned char bf[256];
	unsigned char bf1[256];

	while (parse_ws(&p,'#'), !parse_ident(&p,bf,sizeof(bf)))
		if(!zcmp(bf,USTR "buffer")) {
			cmd->start_buffering = 1;
		} else if(!zcmp(bf,USTR "hold")) {
			cmd->stop_buffering = 1;
		} else if(!zcmp(bf,USTR "save_c")) {
			cmd->save_c = 1;
		} else if(!zcmp(bf,USTR "save_s")) {
			cmd->save_s = 1;
		} else if(!zcmp(bf,USTR "recolor")) {
			parse_ws(&p,'#');
			if(!parse_char(&p,'=')) {
				parse_ws(&p,'#');
				if(parse_int(&p,&cmd->recolor))
					logerror_2((char *)joe_gettext(_("%s %d: Missing value for option\n")),name,line);
			} else
				logerror_2((char *)joe_gettext(_("%s %d: Missing value for option\n")),name,line);
		} else if(!zcmp(bf,USTR "call")) {
			parse_ws(&p,'#');
			if(!parse_char(&p,'=')) {
				parse_ws(&p,'#');
				if (!parse_char(&p,'.')) {
					zlcpy(bf, sizeof(bf), syntax->name);
					goto subr;
				} else if (parse_ident(&p,bf,sizeof(bf)))
					logerror_2((char *)joe_gettext(_("%s %d: Missing value for option\n")),name,line);
				else {
					if (!parse_char(&p,'.')) {
						subr:
						if (parse_ident(&p,bf1,sizeof(bf1)))
							logerror_2((char *)joe_gettext(_("%s %d: Missing subroutine name\n")),name,line);
						cmd->call = load_syntax_subr(bf,bf1,parse_params(syntax->params,&p,name,line));
					} else
						cmd->call = load_syntax_subr(bf,0,parse_params(syntax->params,&p,name,line));
				}
			} else
				logerror_2((char *)joe_gettext(_("%s %d: Missing value for option\n")),name,line);
		} else if(!zcmp(bf,USTR "return")) {
			cmd->rtn = 1;
		} else if(!zcmp(bf,USTR "reset")) {
			cmd->reset = 1;
		} else if(!parsing_strings && (!zcmp(bf,USTR "strings") || !zcmp(bf,USTR "istrings"))) {
			if (bf[0]=='i')
				cmd->ignore = 1;
			while(jfgets(buf,1023,f)) {
				++line;
				p = buf;
				parse_ws(&p,'#');
				if (*p) {
					if(!parse_field(&p,USTR "done"))
						break;
					if(parse_string(&p,bf,sizeof(bf)) >= 0) {
						parse_ws(&p,'#');
						if (cmd->ignore)
							lowerize(bf);
						if(!parse_ident(&p,bf1,sizeof(bf1))) {
							struct high_cmd *kw_cmd=mkcmd();
							kw_cmd->noeat=1;
							kw_cmd->new_state = find_state(syntax,bf1);
							if (!zcmp(bf, USTR "&")) {
								cmd->delim = kw_cmd;
							} else {
								if(!cmd->keywords)
									cmd->keywords = htmk(64);
								htadd(cmd->keywords,zdup(bf),kw_cmd);
							}
							line = parse_options(syntax,kw_cmd,f,p,1,name,line);
						} else
							logerror_2((char *)joe_gettext(_("%s %d: Missing state name\n")),name,line);
					} else
						logerror_2((char *)joe_gettext(_("%s %d: Missing string\n")),name,line);
				}
			}
		} else if(!zcmp(bf,USTR "noeat")) {
			cmd->noeat = 1;
		} else if(!zcmp(bf,USTR "mark")) {
			cmd->start_mark = 1;
		} else if(!zcmp(bf,USTR "markend")) {
			cmd->stop_mark = 1;
		} else if(!zcmp(bf,USTR "recolormark")) {
			cmd->recolor_mark = 1;
		} else
			logerror_2((char *)joe_gettext(_("%s %d: Unknown option\n")),name,line);
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

struct high_state *load_dfa(struct high_syntax *syntax)
{
	unsigned char name[1024];
	unsigned char buf[1024];
	unsigned char bf[256];
	int clist[256];
	unsigned char *p;
	int c;
	JFILE *f = 0;
	struct ifstack *stack=0;
	struct high_state *state=0;	/* Current state */
	struct high_state *first=0;	/* First state */
	int line = 0;
	int this_one = 0;
	int inside_subr = 0;

	/* Load it */
	p = (unsigned char *)getenv("HOME");
	if (p) {
		joe_snprintf_2(name,sizeof(name),"%s/.joe/syntax/%s.jsf",p,syntax->name);
		f = jfopen(name,"r");
	}

	if (!f) {
		joe_snprintf_2(name,sizeof(name),"%ssyntax/%s.jsf",JOEDATA,syntax->name);
		f = jfopen(name,"r");
	}
	if (!f) {
		joe_snprintf_1(name,sizeof(name),"*%s.jsf",syntax->name);
		f = jfopen(name,"r");
	}
	if (!f) {
		return 0;
	}

	/* Parse file */
	while(jfgets(buf,1023,f)) {
		++line;
		p = buf;
		c = parse_ws(&p,'#');
		if (!parse_char(&p, '.')) {
			if (!parse_ident(&p, bf, sizeof(bf))) {
				if (!zcmp(bf, USTR "ifdef")) {
					struct ifstack *st = joe_malloc(sizeof(struct ifstack));
					st->next = stack;
					st->else_part = 0;
					st->ignore = 1;
					st->skip = 1;
					st->line = line;
					if (!stack || !stack->ignore) {
						parse_ws(&p,'#');
						if (!parse_ident(&p, bf, sizeof(bf))) {
							struct high_param *param;
							for (param = syntax->params; param; param = param->next)
								if (!zcmp(param->name, bf)) {
									st->ignore = 0;
									break;
								}
							st->skip = 0;
						} else {
							logerror_2((char *)joe_gettext(_("%s %d: missing parameter for ifdef\n")),name,line);
						}
					}
					stack = st;
				} else if (!zcmp(bf, USTR "else")) {
					if (stack && !stack->else_part) {
						stack->else_part = 1;
						if (!stack->skip)
							stack->ignore = !stack->ignore;
					} else
						logerror_2((char *)joe_gettext(_("%s %d: else with no matching if\n")),name,line);
				} else if (!zcmp(bf, USTR "endif")) {
					if (stack) {
						struct ifstack *st = stack;
						stack = st->next;
						joe_free(st);
					} else
						logerror_2((char *)joe_gettext(_("%s %d: endif with no matching if\n")),name,line);
				} else if (!zcmp(bf, USTR "subr")) {
					parse_ws(&p, '#');
					if (parse_ident(&p, bf, sizeof(bf))) {
						logerror_2((char *)joe_gettext(_("%s %d: Missing subroutine name\n")),name,line);
					} else {
						if (!stack || !stack->ignore) {
							inside_subr = 1;
							this_one = 0;
							if (syntax->subr && !zcmp(bf, syntax->subr))
								this_one = 1;
						}
					}
				} else if (!zcmp(bf, USTR "end")) {
					if (!stack || !stack->ignore) {
						this_one = 0;
						inside_subr = 0;
					}
				} else {
					logerror_2((char *)joe_gettext(_("%s %d: Unknown control statement\n")),name,line);
				}
			} else {
				logerror_2((char *)joe_gettext(_("%s %d: Missing control statement name\n")),name,line);
			}
		} else if (stack && stack->ignore) {
			/* Ignore this line because of ifdef */
		} else if(!parse_char(&p, '=')) {
			/* Parse color */
			parse_color_def(&syntax->color,p,name,line);
		} else if ((syntax->subr && !this_one) || (!syntax->subr && inside_subr)) {
			/* Ignore this line because it's not the code we want */
		} else if(!parse_char(&p, ':')) {
			if(!parse_ident(&p, bf, sizeof(bf))) {

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
						logerror_2((char *)joe_gettext(_("%s %d: Unknown class\n")),name,line);
					}
					while(parse_ws(&p, '#'), !parse_ident(&p, bf, sizeof(bf))) {
						if(!zcmp(bf, USTR "comment")) {
							state->color |= CONTEXT_COMMENT;
						} else if(!zcmp(bf, USTR "string")) {
							state->color |= CONTEXT_STRING;
						} else {
							logerror_2((char *)joe_gettext(_("%s %d: Unknown context\n")),name,line);
						}
					}
				} else
					logerror_2((char *)joe_gettext(_("%s %d: Missing color for state definition\n")),name,line);
			} else
				logerror_2((char *)joe_gettext(_("%s %d: Missing state name\n")),name,line);
		} else if(!parse_char(&p, '-')) {
			/* No. sync lines ignored */
		} else {
			c = parse_ws(&p,'#');

			if (!c) {
			} else if (c=='"' || c=='*' || c=='&') {
				if (state) {
					struct high_cmd *cmd;
					int delim = 0;
					if(!parse_field(&p, USTR "*")) {
						int z;
						for(z=0;z!=256;++z)
							clist[z] = 1;
					} else if(!parse_field(&p, USTR "&")) {
						delim = 1;
					} else {
						c = parse_string(&p, bf, sizeof(bf));
						if(c < 0)
							logerror_2((char *)joe_gettext(_("%s %d: Bad string\n")),name,line);
						else {
							int z;
							int first, second;
							unsigned char *t = bf;
							for(z=0;z!=256;++z)
								clist[z] = 0;
							while(!parse_range(&t, &first, &second)) {
								if(first>second)
									second = first;
								while(first<=second)
									clist[first++] = 1;
							}
						}
					}
					/* Create command */
					cmd = mkcmd();
					parse_ws(&p,'#');
					if(!parse_ident(&p,bf,sizeof(bf))) {
						int z;
						cmd->new_state = find_state(syntax,bf);
						line = parse_options(syntax,cmd,f,p,0,name,line);

						/* Install command */
						if (delim)
							state->delim = cmd;
						else for(z=0;z!=256;++z)
							if(clist[z])
								state->cmd[z]=cmd;
					} else
						logerror_2((char *)joe_gettext(_("%s %d: Missing jump\n")),name,line);
				} else
					logerror_2((char *)joe_gettext(_("%s %d: No state\n")),name,line);
			} else
				logerror_2((char *)joe_gettext(_("%s %d: Unknown character\n")),name,line);
		}
	}

	while (stack) {
		struct ifstack *st = stack;
		stack = st->next;
		logerror_2((char *)joe_gettext(_("%s %d: ifdef with no matching endif\n")),name,st->line);
		joe_free(st);
	}

	jfclose(f);

	return first;
}

int syntax_match(struct high_syntax *syntax,unsigned char *name,unsigned char *subr,struct high_param *params)
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

struct high_syntax *load_syntax_subr(unsigned char *name,unsigned char *subr,struct high_param *params)
{
	struct high_syntax *syntax;	/* New syntax table */

	/* Find syntax table */

	/* Already loaded? */
	for(syntax=syntax_list;syntax;syntax=syntax->next)
		if(syntax_match(syntax,name,subr,params))
			return syntax;

	/* Create new one */
	syntax = joe_malloc(sizeof(struct high_syntax));
	syntax->name = zdup(name);
	syntax->subr = subr ? zdup(subr) : 0;
	syntax->params = params;
	syntax->next = syntax_list;
	syntax->nstates = 0;
	syntax->color = 0;
	syntax->states = joe_malloc(sizeof(struct high_state *)*(syntax->szstates = 64));
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

struct high_syntax *load_syntax(unsigned char *name)
{
	if (!name)
		return 0;

	if (!zcmp(name, USTR "ansi"))
		return ansi_syntax;

	return load_syntax_subr(name,0,0);
}
