/*
 *	Command execution
 *	Copyright
 *		(C) 1992 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */
#include "types.h"

int joe_beep = 0;

/* Command table */

int ubeep(BW *bw, int k)
{
	ttputc(7);
	return 0;
}

CMD cmds[] = {
	{"abort", TYPETW + TYPEPW + TYPEMENU + TYPEQW, uabort, NULL, 0, NULL},
	{"abortbuf", TYPETW, uabortbuf, NULL, 0, NULL},
	{"arg", TYPETW + TYPEPW + TYPEMENU + TYPEQW, uarg, NULL, 0, NULL},
	{"ask", TYPETW + TYPEPW, uask, NULL, 0, NULL},
	{"uarg", TYPETW + TYPEPW + TYPEMENU + TYPEQW, uuarg, NULL, 0, NULL},
	{"backs", TYPETW + TYPEPW + ECHKXCOL + EFIXXCOL + EMINOR + EKILL + EMOD, ubacks, NULL, 1, "delch"},
	{"backsmenu", TYPEMENU, umbacks, NULL, 1, NULL},
	{"backw", TYPETW + TYPEPW + ECHKXCOL + EFIXXCOL + EKILL + EMOD, ubackw, NULL, 1, "delw"},
	{"beep", TYPETW + TYPEPW + TYPEMENU + TYPEQW, ubeep, NULL, 0, NULL},
	{"begin_marking", TYPETW + TYPEPW, ubegin_marking, NULL, 0, NULL},
	{"bknd", TYPETW, ubknd, NULL, 0, NULL},
	{"bkwdc", TYPETW + TYPEPW, ubkwdc, NULL, 1, "fwrdc"},
	{"blkcpy", TYPETW + TYPEPW + EFIXXCOL + EMOD + EBLOCK, ublkcpy, NULL, 1, NULL},
	{"blkdel", TYPETW + TYPEPW + EFIXXCOL + EKILL + EMOD + EBLOCK, ublkdel, NULL, 0, NULL},
	{"blkmove", TYPETW + TYPEPW + EFIXXCOL + EMOD + EBLOCK, ublkmove, NULL, 0, NULL},
	{"blksave", TYPETW + TYPEPW + EBLOCK, ublksave, NULL, 0, NULL},
	{"bof", TYPETW + TYPEPW + EMOVE + EFIXXCOL, u_goto_bof, NULL, 0, NULL},
	{"bofmenu", TYPEMENU, umbof, NULL, 0, NULL},
	{"bol", TYPETW + TYPEPW + EFIXXCOL, u_goto_bol, NULL, 0, NULL},
	{"bolmenu", TYPEMENU, umbol, NULL, 0, NULL},
	{"bop", TYPETW + TYPEPW + EFIXXCOL, ubop, NULL, 1, "eop"},
	{"bos", TYPETW + TYPEPW + EMOVE, ubos, NULL, 0, NULL},
	{"brpaste", TYPETW + TYPEPW + EMOD + EFIXXCOL, ubrpaste, NULL, 0, NULL},
	{"bufed", TYPETW, ubufed, NULL, 0, NULL},
	{"build", TYPETW + TYPEPW, ubuild, NULL, 0, NULL},
	{"byte", TYPETW + TYPEPW, ubyte, NULL, 0, NULL},
	{"cancel", TYPETW + TYPEPW + TYPEMENU + TYPEQW, ucancel, NULL, 0, NULL},
	{"cd", TYPETW, usetcd, NULL, 0, NULL},
	{"center", TYPETW + TYPEPW + EFIXXCOL + EMOD, ucenter, NULL, 1, NULL},
	{"charset", TYPETW + TYPEPW, ucharset, NULL, 0, NULL}, 
	{"ctrl", TYPETW + TYPEPW + EMOD, uctrl, NULL, 0, NULL},
	{"col", TYPETW + TYPEPW, ucol, NULL, 0, NULL},
	{"complete", TYPEPW + EMINOR + EMOD, ucmplt, NULL, 0, NULL},
	{"copy", TYPETW + TYPEPW, ucopy, NULL, 0, NULL},
	{"crawll", TYPETW + TYPEPW, ucrawll, NULL, 1, "crawlr"},
	{"crawlr", TYPETW + TYPEPW, ucrawlr, NULL, 1, "crawll"},
	{"defmdown", TYPETW+TYPEPW+TYPEQW+TYPEMENU, udefmdown, 0, 0, 0 },
	{"defmup", TYPETW+TYPEPW, udefmup, 0, 0, 0 },
	{"defmdrag", TYPETW+TYPEPW, udefmdrag, 0, 0, 0 },
	{"defm2down", TYPETW+TYPEPW+TYPEMENU, udefm2down, 0, 0, 0 },
	{"defm2up", TYPETW+TYPEPW, udefm2up, 0, 0, 0 },
	{"defm2drag", TYPETW+TYPEPW, udefm2drag, 0, 0, 0 },
	{"defm3down", TYPETW+TYPEPW, udefm3down, 0, 0, 0 },
	{"defm3up", TYPETW+TYPEPW, udefm3up, 0, 0, 0 },
	{"defm3drag", TYPETW+TYPEPW, udefm3drag, 0, 0, 0 },
	{"delbol", TYPETW + TYPEPW + EFIXXCOL + EKILL + EMOD, udelbl, NULL, 1, "deleol"},
	{"delch", TYPETW + TYPEPW + ECHKXCOL + EFIXXCOL + EMINOR + EKILL + EMOD, udelch, NULL, 1, "backs"},
	{"deleol", TYPETW + TYPEPW + EKILL + EMOD, udelel, NULL, 1, "delbol"}, 
	{"dellin", TYPETW + TYPEPW + EFIXXCOL + EKILL + EMOD, udelln, NULL, 1, NULL}, 
	{"delw", TYPETW + TYPEPW + EFIXXCOL + ECHKXCOL + EKILL + EMOD, u_word_delete, NULL, 1, "backw"},
	{"dnarw", TYPETW + TYPEPW + EMOVE, udnarw, NULL, 1, "uparw"},
	{"dnarwmenu", TYPEMENU, umdnarw, NULL, 1, "uparwmenu"}, 
	{"dnslide", TYPETW + TYPEPW + TYPEMENU + TYPEQW + EMOVE, udnslide, NULL, 1, "upslide"},
	{"dnslidemenu", TYPEMENU, umscrdn, NULL, 1, "upslidemenu"},
	{"drop", TYPETW + TYPEPW, udrop, NULL, 0, NULL},
	{"dupw", TYPETW, uduptw, NULL, 0, NULL},
	{"edit", TYPETW, uedit, NULL, 0, NULL},
	{"else", TYPETW+TYPEPW+TYPEMENU+TYPEQW+EMETA, uelse, 0, 0, 0 },
	{"elsif", TYPETW+TYPEPW+TYPEMENU+TYPEQW+EMETA, uelsif, 0, 0, 0 },
	{"endif", TYPETW+TYPEPW+TYPEMENU+TYPEQW+EMETA, uendif, 0, 0, 0 },
	{"eof", TYPETW + TYPEPW + EFIXXCOL + EMOVE, u_goto_eof, NULL, 0, NULL},
	{"eofmenu", TYPEMENU, umeof, NULL, 0, NULL},
	{"eol", TYPETW + TYPEPW + EFIXXCOL, u_goto_eol, NULL, 0, NULL},
	{"eolmenu", TYPEMENU, umeol, NULL, 0, NULL},
	{"eop", TYPETW + TYPEPW + EFIXXCOL, ueop, NULL, 1, "bop"},
	{"execmd", TYPETW + TYPEPW, uexecmd, NULL, 0, NULL},
	{"explode", TYPETW + TYPEPW + TYPEMENU + TYPEQW, uexpld, NULL, 0, NULL},
	{"exsave", TYPETW + TYPEPW, uexsve, NULL, 0, NULL},
	{"extmouse", TYPETW+TYPEPW+TYPEMENU+TYPEQW, uextmouse, 0, 0, 0 },
	{"ffirst", TYPETW + TYPEPW, pffirst, NULL, 0, NULL},
	{"filt", TYPETW + TYPEPW + EMOD + EBLOCK, ufilt, NULL, 0, NULL},
	{"finish", TYPETW + TYPEPW + EMOD, ufinish, NULL, 1, NULL},
	{"fnext", TYPETW + TYPEPW, pfnext, NULL, 1, NULL},
	{"format", TYPETW + TYPEPW + EFIXXCOL + EMOD, uformat, NULL, 1, NULL},
	{"fmtblk", TYPETW + EMOD + EFIXXCOL + EBLOCK, ufmtblk, NULL, 1, NULL},
	{"fwrdc", TYPETW + TYPEPW, ufwrdc, NULL, 1, "bkwdc"},
	{"gomark", TYPETW + TYPEPW + EMOVE, ugomark, NULL, 0, NULL},
	{"gparse", TYPETW, ugparse, NULL, 0, NULL},
	{"grep", TYPETW, ugrep, NULL, 0, NULL},
	{"groww", TYPETW, ugroww, NULL, 1, "shrinkw"},
	{"if", TYPETW+TYPEPW+TYPEMENU+TYPEQW+EMETA, uif, 0, 0, 0 },
	{"isrch", TYPETW + TYPEPW, uisrch, NULL, 0, NULL},
	{"jump", TYPETW, ujump, NULL, 0, NULL },
	{"killjoe", TYPETW + TYPEPW + TYPEMENU + TYPEQW, ukilljoe, NULL, 0, NULL},
	{"killproc", TYPETW + TYPEPW, ukillpid, NULL, 0, NULL},
	{"help", TYPETW + TYPEPW + TYPEQW, u_help, NULL, 0, NULL},
	{"home", TYPETW + TYPEPW + EFIXXCOL, uhome, NULL, 0, NULL},
	{"hnext", TYPETW + TYPEPW + TYPEQW, u_help_next, NULL, 0, NULL},
	{"hprev", TYPETW + TYPEPW + TYPEQW, u_help_prev, NULL, 0, NULL},
	{"insc", TYPETW + TYPEPW + EFIXXCOL + EMOD, uinsc, NULL, 1, "delch"},
	{"keymap", TYPETW, ukeymap, 0, 0, 0 },    /* JM */
	{"insf", TYPETW + TYPEPW + EMOD, uinsf, NULL, 0, NULL}, 
	{"language", TYPETW + TYPEPW, ulanguage, NULL, 0, NULL}, 
	{"lindent", TYPETW + TYPEPW + EFIXXCOL + EMOD + EBLOCK, ulindent, NULL, 1, "rindent"},
	{"line", TYPETW + TYPEPW, uline, NULL, 0, NULL},
	{"lose", TYPETW + TYPEPW, ulose, NULL, 0, NULL}, 
	{"lower", TYPETW + TYPEPW + EMOD + EBLOCK, ulower, NULL, 0, NULL},
	{"ltarw", TYPETW + TYPEPW /* + EFIXXCOL + ECHKXCOL */, u_goto_left, NULL, 1, "rtarw"},
	{"ltarwmenu", TYPEMENU, umltarw, NULL, 1, "rtarwmenu"},
	{"macros", TYPETW + EFIXXCOL, umacros, NULL, 0, NULL},
	{"debug_joe", TYPETW + EFIXXCOL, udebug_joe, NULL, 0, NULL},
	{"markb", TYPETW + TYPEPW, umarkb, NULL, 0, NULL},
	{"markk", TYPETW + TYPEPW, umarkk, NULL, 0, NULL},
	{"markl", TYPETW + TYPEPW, umarkl, NULL, 0, NULL},
	{"math", TYPETW + TYPEPW, umath, NULL, 0, NULL},
	{"maths", TYPETW + TYPEPW, usmath, NULL, 0, NULL},
	{"menu", TYPETW + TYPEPW + TYPEQW, umenu, NULL, 0, NULL},
	{"mode", TYPETW + TYPEPW + TYPEQW, umode, NULL, 0, NULL},
	{"msg", TYPETW + TYPEPW + TYPEQW + TYPEMENU, umsg, NULL, 0, NULL},
	{"mfit", TYPETW, umfit, NULL, 0, NULL},
	{"mwind", TYPETW, umwind, NULL, 0, NULL},
	{"name", TYPETW + TYPEPW, uname_joe, NULL, 0, NULL}, 
	{"nbuf", TYPETW + EFIXXCOL, unbuf, NULL, 1, "pbuf"},
	{"nedge", TYPETW + TYPEPW + EFIXXCOL, unedge, NULL, 1, "pedge"}, 
	{"nextpos", TYPETW + TYPEPW + EFIXXCOL + EMID + EPOS, unextpos, NULL, 1, "prevpos"}, 
	{"nextw", TYPETW + TYPEPW + TYPEMENU + TYPEQW, unextw, NULL, 1, "prevw"},
	{"nextword", TYPETW + TYPEPW + EFIXXCOL, u_goto_next, NULL, 1, "prevword"},
	{"nmark", TYPETW + TYPEPW, unmark, NULL, 0, NULL},
	{"notmod", TYPETW, unotmod, NULL, 0, NULL},
	{"nxterr", TYPETW, unxterr, NULL, 1, "prverr"},
	{"open", TYPETW + TYPEPW + EFIXXCOL + EMOD, uopen, NULL, 1, "deleol"},
	{"parserr", TYPETW, uparserr, NULL, 0, NULL},
	{"paste", TYPETW + TYPEPW + EMOD, upaste, NULL, 0, NULL },
	{"pbuf", TYPETW + EFIXXCOL, upbuf, NULL, 1, "nbuf"},
	{"pedge", TYPETW + TYPEPW + EFIXXCOL, upedge, NULL, 1, "nedge"}, 
	{"pgdn", TYPETW + TYPEPW + TYPEMENU + TYPEQW + EMOVE, upgdn, NULL, 1, "pgup"},
	{"pgdnmenu", TYPEMENU, umpgdn, NULL, 1, "pgupmenu"}, 
	{"pgup", TYPETW + TYPEPW + TYPEMENU + TYPEQW + EMOVE, upgup, NULL, 1, "pgdn"},
	{"pgupmenu", TYPEMENU, umpgup, NULL, 1, "pgdnmenu"}, 
	{"picokill", TYPETW + TYPEPW + EFIXXCOL + EKILL + EMOD, upicokill, NULL, 1, NULL},
	{"play", TYPETW + TYPEPW + TYPEMENU + TYPEQW, uplay, NULL, 1, NULL},	/* EFIXX? */ 
	{"prevpos", TYPETW + TYPEPW + EPOS + EMID + EFIXXCOL, uprevpos, NULL, 1, "nextpos"}, 
	{"prevw", TYPETW + TYPEPW + TYPEMENU + TYPEQW, uprevw, NULL, 1, "nextw"}, 
	{"prevword", TYPETW + TYPEPW + EFIXXCOL + ECHKXCOL, u_goto_prev, NULL, 1, "nextword"},
	{"prverr", TYPETW, uprverr, NULL, 1, "nxterr"},
	{"psh", TYPETW + TYPEPW + TYPEMENU + TYPEQW, upsh, NULL, 0, NULL},
	{"pop", TYPETW + TYPEPW + TYPEMENU + TYPEQW, upop, NULL, 0, NULL},
	{"popabort", TYPETW + TYPEPW + TYPEMENU + TYPEQW, upopabort, NULL, 0, NULL},
	{"qrepl", TYPETW + TYPEPW + EMOD, pqrepl, NULL, 0, NULL},
	{"query", TYPETW + TYPEPW + TYPEMENU + TYPEQW, uquery, NULL, 0, NULL},
	{"querysave", TYPETW, uquerysave, NULL, 0, NULL},
	{"quote", TYPETW + TYPEPW + EMOD, uquote, NULL, 0, NULL},
	{"quote8", TYPETW + TYPEPW + EMOD, uquote8, NULL, 0, NULL},
	{"record", TYPETW + TYPEPW + TYPEMENU + TYPEQW, urecord, NULL, 0, NULL},
	{"redo", TYPETW + TYPEPW + EFIXXCOL, uredo, NULL, 1, "undo"},
	{"release", TYPETW, urelease, NULL, 0, NULL},
	{"reload", TYPETW, ureload, NULL, 0, NULL },
	{"reloadall", TYPETW, ureload_all, NULL, 0, NULL },
	{"retype", TYPETW + TYPEPW + TYPEMENU + TYPEQW, uretyp, NULL, 0, NULL},
	{"rfirst", TYPETW + TYPEPW, prfirst, NULL, 0, NULL}, 
	{"rindent", TYPETW + TYPEPW + EFIXXCOL + EMOD + EBLOCK, urindent, NULL, 1, "lindent"},
	{"run", TYPETW + TYPEPW, urun, NULL, 0, NULL},
	{"rsrch", TYPETW + TYPEPW, ursrch, NULL, 0, NULL},
	{"rtarw", TYPETW + TYPEPW /* + EFIXXCOL */, u_goto_right, NULL, 1, "ltarw"}, /* EFIX removed for picture mode */
	{"rtarwmenu", TYPEMENU, umrtarw, NULL, 1, "ltarwmenu"},
	{"rtn", TYPETW + TYPEPW + TYPEMENU + TYPEQW + EMOD, urtn, NULL, 1, NULL},
	{"save", TYPETW + TYPEPW, usave, NULL, 0, NULL},
	{"savenow", TYPETW + TYPEPW, usavenow, NULL, 0, NULL},
	{"scratch", TYPETW + TYPEPW, uscratch, NULL, 0, NULL},
	{"scratch_push", TYPETW + TYPEPW, uscratch_push, NULL, 0, NULL},
	{"secure_type", TYPEPW + TYPEQW + TYPEMENU + EMINOR + EMOD, utype, NULL, 1, "backs"},
	{"select", TYPETW + TYPEPW, uselect, NULL, 0, NULL},
	{"setmark", TYPETW + TYPEPW, usetmark, NULL, 0, NULL},
	{"shell", TYPETW + TYPEPW + TYPEMENU + TYPEQW, ushell, NULL, 0, NULL},
	{"showerr", TYPETW + TYPEPW, ucurrent_msg, NULL, 0, NULL},
	{"showlog", TYPETW, ushowlog, NULL, 0, NULL},
	{"shrinkw", TYPETW, ushrnk, NULL, 1, "groww"},
	{"splitw", TYPETW, usplitw, NULL, 0, NULL},
	{"stat", TYPETW + TYPEPW, ustat, NULL, 0, NULL},
	{"stop", TYPETW + TYPEPW + TYPEMENU + TYPEQW, ustop, NULL, 0, NULL},
	{"swap", TYPETW + TYPEPW + EFIXXCOL, uswap, NULL, 0, NULL},
	{"switch", TYPETW + TYPEPW, uswitch, NULL, 0, NULL},
	{"sys", TYPETW + TYPEPW, usys, NULL, 0, NULL },
	{"tabmenu", TYPEMENU, umtab, NULL, 1, "ltarwmenu"},
	{"tag", TYPETW + TYPEPW, utag, NULL, 0, NULL},
	{"tagjump", TYPETW + TYPEPW, utagjump, NULL, 0, NULL},
	{"toggle_marking", TYPETW + TYPEPW, utoggle_marking, NULL, 0, NULL},
	{"then", TYPEPW+EMOD, urtn, 0, 0, 0 },
	{"timer", TYPETW + TYPEPW + TYPEMENU + TYPEQW, utimer, NULL, 1, NULL},
	{"tomarkb", TYPETW + TYPEPW + EFIXXCOL + EBLOCK, utomarkb, NULL, 0, NULL},
	{"tomarkbk", TYPETW + TYPEPW + EFIXXCOL + EBLOCK, utomarkbk, NULL, 0, NULL},
	{"tomarkk", TYPETW + TYPEPW + EFIXXCOL + EBLOCK, utomarkk, NULL, 0, NULL},
	{"tomatch", TYPETW + TYPEPW + EFIXXCOL, utomatch, NULL, 0, NULL},
	{"tomouse", TYPETW+TYPEPW+TYPEQW+TYPEMENU, utomouse, 0, 0, 0 },
	{"tos", TYPETW + TYPEPW + EMOVE, utos, NULL, 0, NULL},
	{"tw0", TYPETW + TYPEPW + TYPEQW + TYPEMENU, utw0, NULL, 0, NULL},
	{"tw1", TYPETW + TYPEPW + TYPEQW + TYPEMENU, utw1, NULL, 0, NULL},
	{"txt", TYPETW + TYPEPW, utxt, NULL, 0, NULL}, 
	{"type", TYPETW + TYPEPW + TYPEQW + TYPEMENU + EMINOR + EMOD, utype, NULL, 1, "backs"},
	{"undo", TYPETW + TYPEPW + EFIXXCOL, uundo, NULL, 1, "redo"},
	{"uparw", TYPETW + TYPEPW + EMOVE, uuparw, NULL, 1, "dnarw"},
	{"uparwmenu", TYPEMENU, umuparw, NULL, 1, "dnarwmenu"}, 
	{"upper", TYPETW + TYPEPW + EMOD + EBLOCK, uupper, NULL, 0, NULL},
	{"upslide", TYPETW + TYPEPW + TYPEMENU + TYPEQW + EMOVE, uupslide, NULL, 1, "dnslide"},
	{"upslidemenu", TYPEMENU, umscrup, NULL, 1, "dnslidemenu"},
	{"vtbknd", TYPETW, uvtbknd, NULL, 0, NULL},
	{"xtmouse", TYPETW+TYPEPW+TYPEMENU+TYPEQW, uxtmouse, 0, 0, 0 },
	{"yank", TYPETW + TYPEPW + EFIXXCOL + EMOD, uyank, NULL, 1, NULL},
	{"yapp", TYPETW + TYPEPW + EKILL, uyapp, NULL, 0, NULL},
	{"yankpop", TYPETW + TYPEPW + EFIXXCOL + EMOD, uyankpop, NULL, 1, NULL}
};

/* Steal Lock dialog */

int nolocks;

#define LOCKMSG2 _("Could not create lock. (I) edit anyway, (Q) cancel edit? ")
#define LOCKMSG1 _("Locked by %s. (S)teal lock, (I) edit anyway, (Q) cancel edit? ")

char *steallock_key= _("|steal the lock|sS");
char *canceledit_key= _("|cancel edit due to lock|qQ");
char *ignorelock_key=  _("|ignore lock, continue with edit|iI");

int steal_lock(BW *bw,int c,B *b,int *notify)
{
	if (yncheck(steallock_key, c)) {
		char bf1[256];
		char bf[300];
		unlock_it(b->name);
		if (lock_it(b->name,bf1)) {
			int x;
			for(x=0;bf1[x] && bf1[x]!=':';++x);
			bf1[x]=0;
			if(bf1[0])
				joe_snprintf_1(bf,SIZEOF(bf),joe_gettext(LOCKMSG1),bf1);
			else
				joe_snprintf_0(bf, SIZEOF(bf), joe_gettext(LOCKMSG2));
			if (mkqw(bw->parent, sz(bf), steal_lock, NULL, b, notify)) {
				return 0;
			} else {
				if (notify)
					*notify = -1;
				return -1;
			}
		} else {
			b->locked=1;
			if (notify)
				*notify = 1;
			return 0;
		}
	} else if (yncheck(ignorelock_key, c)) {
		b->locked=1;
		b->ignored_lock=1;
		if (notify)
			*notify = 1;
		return 0;
	} else if (yncheck(canceledit_key, c)) {
		if (notify)
			*notify = 1;
		return 0;
	} else {
		if (mkqw(bw->parent, sz(joe_gettext(LOCKMSG2)), steal_lock, NULL, b, notify)) {
			return 0;
		} else
			return -1;
	}
}

int file_changed(BW *bw,int c,B *b,int *notify)
{
	if (mkqw(bw->parent, sz(joe_gettext(_("Notice: File on disk changed! (hit ^C to continue)  "))), file_changed, NULL, b, notify)) {
		b->gave_notice = 1;
		return 0;
	} else
		return -1;
}

/* Try to lock: start dialog if we can't.  Returns 0 if we couldn't lock */

int try_lock(BW *bw,B *b)
{
	/* First time we modify the file */
	/* If we're a plain file, acquire lock */
	if (!nolocks && plain_file(b)) {
		char bf1[256];
		char bf[300];
		int x;
		/* It's a plain file- try to lock it */
		if (lock_it(b->name,bf1)) {
			for(x=0;bf1[x] && bf1[x]!=':';++x);
			bf1[x]=0;
			if(bf1[0])
				joe_snprintf_1(bf,SIZEOF(bf),joe_gettext(LOCKMSG1),bf1);
			else
				joe_snprintf_0(bf, SIZEOF(bf), joe_gettext(LOCKMSG2));
			if (mkqw(bw->parent, sz(bf), steal_lock, NULL, b, NULL)) {
				uquery(bw);
				if (!b->locked)
					return 0;
			} else
				return 0;
		} else {
			/* Remember to unlock it */
			b->locked = 1;
		}
	}
	return 1;
}

/* Called when we are about to modify a buffer */
/* Returns 0 if we're not allowed to modify buffer */

#define CHECK_INTERVAL 15
int nomodcheck;

int modify_logic(BW *bw,B *b)
{
	if (last_time > b->check_time + CHECK_INTERVAL) {
		b->check_time = last_time;
		if (!nomodcheck && !b->gave_notice && check_mod(b)) {
			file_changed(bw,0,b,NULL);
			return 0;
		}
	}

	if (b != bw->b) {
		if (!b->didfirst) {
			/* This happens when we try to block move from a window
			   which is not on the screen */
			if (bw->o.mfirst) {
				msgnw(bw->parent,joe_gettext(_("Modify other window first for macro")));
				return 0;
			}
			b->didfirst = 1;
			if (bw->o.mfirst)
				exmacro(bw->o.mfirst,1);
		}
		if (b->rdonly) {
			msgnw(bw->parent,joe_gettext(_("Other buffer is read only")));
			if (joe_beep)
				ttputc(7);
			return 0;
		} else if (!b->changed && !b->locked) {
			if (!try_lock(bw,b))
				return 0;
		}
	} else {
		if (!b->didfirst) {
			b->didfirst = 1;
			if (bw->o.mfirst)
				exmacro(bw->o.mfirst,1);
		}
		if (b->rdonly) {
			msgnw(bw->parent,joe_gettext(_("Read only")));
			if (joe_beep)
				ttputc(7);
			return 0;
		} else if (!b->changed && !b->locked) {
			if (!try_lock(bw,b))
				return 0;
		}
	}
	return 1;
}

/* Execute a command n with key k */

int execmd(CMD *cmd, int k)
{
	BW *bw = (BW *) maint->curwin->object;
	int ret = -1;

	/* Warning: bw is a BW * only if maint->curwin->watom->what &
	    (TYPETW|TYPEPW) */

#if junk
	/* Send data to shell window: this is broken ^K ^H (help) sends its ^H to shell */
	if ((maint->curwin->watom->what & TYPETW) && bw->b->pid && !bw->b->vt && piseof(bw->cursor) &&
	(k==3 || k==9 || k==13 || k==8 || k==127 || k==4 || (cmd->func==utype /* && k>=32 && k<256 */))) {
		char c = k;
		joe_write(bw->b->out, &c, 1);
		return 0;
	}
#endif

	if (cmd->m)
		return exmacro(cmd->m, 0);

	/* We don't execute if we have to fix the column position first
	 * (i.e., left arrow when cursor is in middle of nowhere) */
	if (cmd->flag & ECHKXCOL) {
		if (bw->o.hex)
			bw->cursor->xcol = piscol(bw->cursor);
		else if (bw->cursor->xcol != piscol(bw->cursor))
			goto skip;
	}

	/* Don't execute command if we're in wrong type of window */
	if (!(cmd->flag & maint->curwin->watom->what))
		goto skip;

	/* Complete selection for block commands */
	if ((cmd->flag & EBLOCK) && nowmarking)
		utoggle_marking(maint->curwin->object);

	/* We are about to modify the file */
	if ((maint->curwin->watom->what & TYPETW) && (cmd->flag & EMOD)) {
		if (!modify_logic(bw,bw->b))
			goto skip;
	}

	/* Execute command */
	ret = cmd->func(maint->curwin->object, k);

	if (smode)
		--smode;

	/* Don't update anything if we're going to leave */
	if (leave)
		return 0;

	/* cmd->func could have changed bw on us */
	/* This is bad: maint->curwin might not be the same window */
	/* Safer would be to attach a pointer to curwin- if curwin
	   gets clobbered, so does pointer. */
	bw = (BW *) maint->curwin->object;

	/* Maintain position history */
	/* If command was not a positioning command */
	if (!(cmd->flag & EPOS)
	    && (maint->curwin->watom->what & (TYPETW | TYPEPW)))
		afterpos();

	/* If command was not a movement */
	if (!(cmd->flag & (EMOVE | EPOS)) && (maint->curwin->watom->what & (TYPETW | TYPEPW)))
		aftermove(maint->curwin, bw->cursor);

	if (cmd->flag & EKILL)
		justkilled = 1;
	else
		justkilled = 0;

      skip:

	/* Make dislayed cursor column equal the actual cursor column
	 * for commands which arn't simple vertical movements */
	if ((cmd->flag & EFIXXCOL) && (maint->curwin->watom->what & (TYPETW | TYPEPW)))
		bw->cursor->xcol = piscol(bw->cursor);

	/* Recenter cursor to middle of screen */
	if (cmd->flag & EMID) {
		int omid = mid;

		mid = 1;
		dofollows();
		mid = omid;
	}

	if (joe_beep && ret)
		ttputc(7);
	return ret;
}

void do_auto_scroll()
{
	static CMD *scrup = 0;
	static CMD *scrdn = 0;
	static CMD *drag = 0;
	if (!scrup) {
		scrup = findcmd("upslide");
		scrdn = findcmd("dnslide");
		drag = findcmd("defmdrag");
	}
	if (auto_scroll > 0)
		execmd(scrdn,0);
	else if (auto_scroll < 0)
		execmd(scrup,0);

	execmd(drag,0);
		
	reset_trig_time();
}

/* Return command table index for given command name */

HASH *cmdhash = NULL;

static void izcmds(void)
{
	int x;

	cmdhash = htmk(256);
	for (x = 0; x != SIZEOF(cmds) / SIZEOF(CMD); ++x)
		htadd(cmdhash, cmds[x].name, cmds + x);
}

CMD *findcmd(char *s)
{
	if (!cmdhash)
		izcmds();
	return (CMD *) htfind(cmdhash, s);
}

void addcmd(char *s, MACRO *m)
{
	CMD *cmd = (CMD *) joe_malloc(SIZEOF(CMD));

	if (!cmdhash)
		izcmds();
	cmd->name = zdup(s);
	cmd->flag = 0;
	cmd->func = NULL;
	cmd->m = m;
	cmd->arg = 1;
	cmd->negarg = NULL;
	htadd(cmdhash, cmd->name, cmd);
}

static char **getcmds(void)
{
	char **s = vaensure(NULL, SIZEOF(cmds) / SIZEOF(CMD));
	int x;
	HENTRY *e;

	for (x = 0; x != cmdhash->len; ++x)
		for (e = cmdhash->tab[x]; e; e = e->next)
			s = vaadd(s, vsncpy(NULL, 0, sz(e->name)));
	vasort(s, aLen(s));
	return s;
}

/* Command line */

char **scmds = NULL;	/* Array of command names */

static int cmdcmplt(BW *bw)
{
	if (!scmds)
		scmds = getcmds();
	return simple_cmplt(bw,scmds);
}

static int docmd(BW *bw, char *s, void *object, int *notify)
{
	MACRO *mac;
	int ret = -1;
	ptrdiff_t sta = -1;
	

	mac = mparse(NULL, s, &sta,0);
	if (sta < 0) {
		msgnw(bw->parent,joe_gettext(_("No such command")));
	} else {
		ret = exmacro(mac, 1);
		rmmacro(mac);
	}

#ifdef junk
	CMD *cmd = findcmd(s);
	vsrm(s);	/* allocated in pw.c::rtnpw() */
	if (!cmd)
		msgnw(bw->parent,joe_gettext(_("No such command")));
	else {
		mac = mkmacro(-1, 0, 0, cmd);
		ret = exmacro(mac, 1);
		rmmacro(mac);
	}
#endif

	if (notify)
		*notify = 1;
	return ret;
}

B *cmdhist = NULL;

int uexecmd(BW *bw)
{
	if (wmkpw(bw->parent, joe_gettext(_("Command: ")), &cmdhist, docmd, "cmd", NULL, cmdcmplt, NULL, NULL, locale_map, 0)) {
		return 0;
	} else {
		return -1;
	}
}
