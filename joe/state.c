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

void save_hist(FILE *f,B *b)
{
	unsigned char *buf = vsmk(128);
	if (b) {
		P *p = pdup(b->bof, USTR "save_hist");
		if (b->eof->line>10)
			pline(p,b->eof->line-10);
		while (!piseof(p)) {
			buf = brlinevs(buf, p);
			buf = vsadd(buf, '\n');
			pnextl(p);
			fprintf(f,"\t");
			emit_string(f,sv(buf));
			fprintf(f,"\n");
		}
		prm(p);
	}
	fprintf(f,"done\n");
}

/* Load a history buffer */

void load_hist(FILE *f,B **bp)
{
	B *b;
	unsigned char *buf = 0;
	unsigned char *bf = 0;
	P *q;

	b = *bp;
	if (!b)
		*bp = b = bmk(NULL);

	q = pdup(b->eof, USTR "load_hist");

	while(vsgets(&buf,f) && zcmp(buf,USTR "done")) {
		unsigned char *p = buf;
		int len;
		parse_ws(&p,'#');
		len = parse_string(&p,&bf);
		if (len>0) {
			binsm(q,bf,len);
			pset(q,b->eof);
		}
	}

	prm(q);
}

/* Save state */

#define STATE_ID (unsigned char *)"# JOE state file v1.0\n"

void save_state()
{
	unsigned char *path = (unsigned char *)getenv("HOME");
	int old_mask;
	FILE *f;
	if (!joe_state)
		return;
	if (!path)
		return;
	path = vsfmt(NULL,0,USTR "%s/.joe_state",path);
	old_mask = umask(0066);
	f = fopen((char *)path,"w");
	umask(old_mask);
	if(!f)
		return;

	/* Write ID */
	fprintf(f,"%s\n",(char *)STATE_ID);

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
	unsigned char *path = (unsigned char *)getenv("HOME");
	unsigned char *buf = vsmk(128);
	FILE *f;
	if (!joe_state)
		return;
	if (!path)
		return;
	path = vsfmt(NULL,0,USTR "%s/.joe_state",path);
	f = fopen((char *)path,"r");
	if(!f)
		return;

	/* Only read state information if the version is correct */
	if (vsgets(&buf, f) && !zcmp(buf,STATE_ID)) {

		/* Read state information */
		while(vsgets(&buf,f)) {
			if(!zcmp(buf,USTR "search"))
				load_srch(f);
			else if(!zcmp(buf,USTR "macros"))
				load_macros(f);
			else if(!zcmp(buf,USTR "files"))
				load_hist(f,&filehist);
			else if(!zcmp(buf,USTR "find"))
				load_hist(f,&findhist);
			else if(!zcmp(buf,USTR "replace"))
				load_hist(f,&replhist);
			else if(!zcmp(buf,USTR "run"))
				load_hist(f,&runhist);
			else if(!zcmp(buf,USTR "build"))
				load_hist(f,&buildhist);
			else if(!zcmp(buf,USTR "grep"))
				load_hist(f,&grephist);
			else if(!zcmp(buf,USTR "cmd"))
				load_hist(f,&cmdhist);
			else if(!zcmp(buf,USTR "math"))
				load_hist(f,&mathhist);
			else if(!zcmp(buf,USTR "yank"))
				load_yank(f);
			else if (!zcmp(buf,USTR "file_pos"))
				load_file_pos(f);
			else { /* Unknown... skip until next done */
				while(vsgets(&buf,f) && zcmp(buf,USTR "done"));
			}
		}
	}

	fclose(f);
}
