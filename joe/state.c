/*
 *	JOE state file
 *	Copyright
 *		(C) 1992 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */

#include "types.h"

/* Set to enable use of ~/.joe_state file */
int joe_state;

/* Save a history buffer */

static void save_hist(FILE *f,B *b)
{
	char buf[512];
	ptrdiff_t len = SIZEOF(buf);
	if (b) {
		P *p = pdup(b->bof, "save_hist");
		P *q = pdup(b->bof, "save_hist");
		if (b->eof->line>10)
			pline(p,b->eof->line-10);
		pset(q,p);
		while (!piseof(p)) {
			pnextl(q);
			if (q->byte-p->byte < SIZEOF(buf)) {
				len = TO_DIFF_OK(q->byte - p->byte);
				brmem(p,buf,len);
			} else {
				len = SIZEOF(buf);
				brmem(p,buf,len);
				buf[len - 1] = '\n';
			}
			fprintf(f,"\t");
			emit_string(f,buf,len);
			fprintf(f,"\n");
			pset(p,q);
		}
		prm(p);
		prm(q);
	}
	fprintf(f,"done\n");
}

/* Load a history buffer */

static void load_hist(FILE *f,B **bp)
{
	B *b;
	char buf[1024];
	char bf[1024];
	P *q;

	b = *bp;
	if (!b)
		*bp = b = bmk(NULL);

	q = pdup(b->eof, "load_hist");

	while(fgets(buf,sizeof(buf),f) && zcmp(buf,"done\n")) {
		const char *p = buf;
		ptrdiff_t len;
		parse_ws(&p,'#');
		len = parse_string(&p,bf,SIZEOF(bf));
		if (len>0) {
			if (bf[len - 1] != '\n')
				bf[len - 1] = '\n';
			binsm(q,bf,len);
			pset(q,b->eof);
		}
	}

	prm(q);
}

/* Save state */

#define STATE_ID "# JOE state file v1.0\n"

void save_state()
{
	char *home = getenv("HOME");
	mode_t old_mask;
	FILE *f;
	if (!joe_state)
		return;
	if (!home)
		return;
	joe_snprintf_1(stdbuf,stdsiz,"%s/.joe_state",home);
	old_mask = umask(0066);
	f = fopen(stdbuf,"w");
	umask(old_mask);
	if(!f)
		return;

	/* Write ID */
	fprintf(f,"%s",STATE_ID);

	/* Write state information */
	fprintf(f,"search\n"); save_srch(f);
	fprintf(f,"macros\n"); save_macros(f);
	fprintf(f,"files\n"); save_hist(f,filehist);
	fprintf(f,"find\n"); save_hist(f,findhist);
	fprintf(f,"replace\n"); save_hist(f,replhist);
	fprintf(f,"run\n"); save_hist(f,runhist);
	fprintf(f,"build\n"); save_hist(f,buildhist);
	fprintf(f,"grep\n"); save_hist(f,grephist);
	fprintf(f,"cmd\n"); save_hist(f,cmdhist);
	fprintf(f,"math\n"); save_hist(f,mathhist);
	fprintf(f,"yank\n"); save_yank(f);
	fprintf(f,"file_pos\n"); save_file_pos(f);
	fclose(f);
}

/* Load state */

void load_state()
{
	char *home = getenv("HOME");
	char buf[1024];
	FILE *f;
	if (!joe_state)
		return;
	if (!home)
		return;
	joe_snprintf_1(stdbuf,stdsiz,"%s/.joe_state",home);
	f = fopen(stdbuf,"r");
	if(!f)
		return;

	/* Only read state information if the version is correct */
	if (fgets(buf,sizeof(buf),f) && !zcmp(buf,STATE_ID)) {

		/* Read state information */
		while(fgets(buf,sizeof(buf),f)) {
			if(!zcmp(buf,"search\n"))
				load_srch(f);
			else if(!zcmp(buf,"macros\n"))
				load_macros(f);
			else if(!zcmp(buf,"files\n"))
				load_hist(f,&filehist);
			else if(!zcmp(buf,"find\n"))
				load_hist(f,&findhist);
			else if(!zcmp(buf,"replace\n"))
				load_hist(f,&replhist);
			else if(!zcmp(buf,"run\n"))
				load_hist(f,&runhist);
			else if(!zcmp(buf,"build\n"))
				load_hist(f,&buildhist);
			else if(!zcmp(buf,"grep\n"))
				load_hist(f,&grephist);
			else if(!zcmp(buf,"cmd\n"))
				load_hist(f,&cmdhist);
			else if(!zcmp(buf,"math\n"))
				load_hist(f,&mathhist);
			else if(!zcmp(buf,"yank\n"))
				load_yank(f);
			else if (!zcmp(buf,"file_pos\n"))
				load_file_pos(f);
			else { /* Unknown... skip until next done */
				while(fgets(buf,sizeof(buf),f) && zcmp(buf,"done\n"));
			}
		}
	}

	fclose(f);
}
