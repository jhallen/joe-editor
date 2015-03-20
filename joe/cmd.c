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
	{USTR "abort", TYPETW + TYPEPW + TYPEMENU + TYPEQW, uabort, NULL, 0, NULL},
	{USTR "abortbuf", TYPETW, uabortbuf, NULL, 0, NULL},
	{USTR "arg", TYPETW + TYPEPW + TYPEMENU + TYPEQW, uarg, NULL, 0, NULL},
	{USTR "ask", TYPETW + TYPEPW, uask, NULL, 0, NULL},
	{USTR "uarg", TYPETW + TYPEPW + TYPEMENU + TYPEQW, uuarg, NULL, 0, NULL},
	{USTR "backs", TYPETW + TYPEPW + ECHKXCOL + EFIXXCOL + EMINOR + EKILL + EMOD, ubacks, NULL, 1, USTR "delch"},
	{USTR "backsmenu", TYPEMENU, umbacks, NULL, 1, NULL},
	{USTR "backw", TYPETW + TYPEPW + ECHKXCOL + EFIXXCOL + EKILL + EMOD, ubackw, NULL, 1, USTR "delw"},
	{USTR "beep", TYPETW + TYPEPW + TYPEMENU + TYPEQW, ubeep, NULL, 0, NULL},
	{USTR "begin_marking", TYPETW + TYPEPW, ubegin_marking, NULL, 0, NULL},
	{USTR "bknd", TYPETW, ubknd, NULL, 0, NULL},
	{USTR "bkwdc", TYPETW + TYPEPW, ubkwdc, NULL, 1, USTR "fwrdc"},
	{USTR "blkcpy", TYPETW + TYPEPW + EFIXXCOL + EMOD + EBLOCK, ublkcpy, NULL, 1, NULL},
	{USTR "blkdel", TYPETW + TYPEPW + EFIXXCOL + EKILL + EMOD + EBLOCK, ublkdel, NULL, 0, NULL},
	{USTR "blkmove", TYPETW + TYPEPW + EFIXXCOL + EMOD + EBLOCK, ublkmove, NULL, 0, NULL},
	{USTR "blksave", TYPETW + TYPEPW + EBLOCK, ublksave, NULL, 0, NULL},
	{USTR "bof", TYPETW + TYPEPW + EMOVE + EFIXXCOL, u_goto_bof, NULL, 0, NULL},
	{USTR "bofmenu", TYPEMENU, umbof, NULL, 0, NULL},
	{USTR "bol", TYPETW + TYPEPW + EFIXXCOL, u_goto_bol, NULL, 0, NULL},
	{USTR "bolmenu", TYPEMENU, umbol, NULL, 0, NULL},
	{USTR "bop", TYPETW + TYPEPW + EFIXXCOL, ubop, NULL, 1, USTR "eop"},
	{USTR "bos", TYPETW + TYPEPW + EMOVE, ubos, NULL, 0, NULL},
	{USTR "brpaste", TYPETW + TYPEPW + EMOD + EFIXXCOL, ubrpaste, NULL, 0, NULL},
	{USTR "bufed", TYPETW, ubufed, NULL, 0, NULL},
	{USTR "build", TYPETW + TYPEPW, ubuild, NULL, 0, NULL},
	{USTR "byte", TYPETW + TYPEPW, ubyte, NULL, 0, NULL},
	{USTR "cancel", TYPETW + TYPEPW + TYPEMENU + TYPEQW, ucancel, NULL, 0, NULL},
	{USTR "cd", TYPETW, usetcd, NULL, 0, NULL},
	{USTR "center", TYPETW + TYPEPW + EFIXXCOL + EMOD, ucenter, NULL, 1, NULL},
	{USTR "charset", TYPETW + TYPEPW, ucharset, NULL, 0, NULL}, 
	{USTR "ctrl", TYPETW + TYPEPW + EMOD, uctrl, NULL, 0, NULL},
	{USTR "col", TYPETW + TYPEPW, ucol, NULL, 0, NULL},
	{USTR "complete", TYPEPW + EMINOR + EMOD, ucmplt, NULL, 0, NULL},
	{USTR "copy", TYPETW + TYPEPW, ucopy, NULL, 0, NULL},
	{USTR "crawll", TYPETW + TYPEPW, ucrawll, NULL, 1, USTR "crawlr"},
	{USTR "crawlr", TYPETW + TYPEPW, ucrawlr, NULL, 1, USTR "crawll"},
	{USTR "defmdown", TYPETW+TYPEPW+TYPEQW+TYPEMENU, udefmdown, 0, 0, 0 },
	{USTR "defmup", TYPETW+TYPEPW, udefmup, 0, 0, 0 },
	{USTR "defmdrag", TYPETW+TYPEPW, udefmdrag, 0, 0, 0 },
	{USTR "defm2down", TYPETW+TYPEPW+TYPEMENU, udefm2down, 0, 0, 0 },
	{USTR "defm2up", TYPETW+TYPEPW, udefm2up, 0, 0, 0 },
	{USTR "defm2drag", TYPETW+TYPEPW, udefm2drag, 0, 0, 0 },
	{USTR "defm3down", TYPETW+TYPEPW, udefm3down, 0, 0, 0 },
	{USTR "defm3up", TYPETW+TYPEPW, udefm3up, 0, 0, 0 },
	{USTR "defm3drag", TYPETW+TYPEPW, udefm3drag, 0, 0, 0 },
	{USTR "delbol", TYPETW + TYPEPW + EFIXXCOL + EKILL + EMOD, udelbl, NULL, 1, USTR "deleol"},
	{USTR "delch", TYPETW + TYPEPW + ECHKXCOL + EFIXXCOL + EMINOR + EKILL + EMOD, udelch, NULL, 1, USTR "backs"},
	{USTR "deleol", TYPETW + TYPEPW + EKILL + EMOD, udelel, NULL, 1, USTR "delbol"}, 
	{USTR "dellin", TYPETW + TYPEPW + EFIXXCOL + EKILL + EMOD, udelln, NULL, 1, NULL}, 
	{USTR "delw", TYPETW + TYPEPW + EFIXXCOL + ECHKXCOL + EKILL + EMOD, u_word_delete, NULL, 1, USTR "backw"},
	{USTR "dnarw", TYPETW + TYPEPW + EMOVE, udnarw, NULL, 1, USTR "uparw"},
	{USTR "dnarwmenu", TYPEMENU, umdnarw, NULL, 1, USTR "uparwmenu"}, 
	{USTR "dnslide", TYPETW + TYPEPW + TYPEMENU + TYPEQW + EMOVE, udnslide, NULL, 1, USTR "upslide"},
	{USTR "dnslidemenu", TYPEMENU, umscrdn, NULL, 1, USTR "upslidemenu"},
	{USTR "drop", TYPETW + TYPEPW, udrop, NULL, 0, NULL},
	{USTR "dupw", TYPETW, uduptw, NULL, 0, NULL},
	{USTR "edit", TYPETW, uedit, NULL, 0, NULL},
	{USTR "else", TYPETW+TYPEPW+TYPEMENU+TYPEQW+EMETA, uelse, 0, 0, 0 },
	{USTR "elsif", TYPETW+TYPEPW+TYPEMENU+TYPEQW+EMETA, uelsif, 0, 0, 0 },
	{USTR "endif", TYPETW+TYPEPW+TYPEMENU+TYPEQW+EMETA, uendif, 0, 0, 0 },
	{USTR "eof", TYPETW + TYPEPW + EFIXXCOL + EMOVE, u_goto_eof, NULL, 0, NULL},
	{USTR "eofmenu", TYPEMENU, umeof, NULL, 0, NULL},
	{USTR "eol", TYPETW + TYPEPW + EFIXXCOL, u_goto_eol, NULL, 0, NULL},
	{USTR "eolmenu", TYPEMENU, umeol, NULL, 0, NULL},
	{USTR "eop", TYPETW + TYPEPW + EFIXXCOL, ueop, NULL, 1, USTR "bop"},
	{USTR "execmd", TYPETW + TYPEPW, uexecmd, NULL, 0, NULL},
	{USTR "explode", TYPETW + TYPEPW + TYPEMENU + TYPEQW, uexpld, NULL, 0, NULL},
	{USTR "exsave", TYPETW + TYPEPW, uexsve, NULL, 0, NULL},
	{USTR "extmouse", TYPETW+TYPEPW+TYPEMENU+TYPEQW, uextmouse, 0, 0, 0 },
	{USTR "ffirst", TYPETW + TYPEPW, pffirst, NULL, 0, NULL},
	{USTR "filt", TYPETW + TYPEPW + EMOD + EBLOCK, ufilt, NULL, 0, NULL},
	{USTR "finish", TYPETW + TYPEPW + EMOD, ufinish, NULL, 1, NULL},
	{USTR "fnext", TYPETW + TYPEPW, pfnext, NULL, 1, NULL},
	{USTR "format", TYPETW + TYPEPW + EFIXXCOL + EMOD, uformat, NULL, 1, NULL},
	{USTR "fmtblk", TYPETW + EMOD + EFIXXCOL + EBLOCK, ufmtblk, NULL, 1, NULL},
	{USTR "fwrdc", TYPETW + TYPEPW, ufwrdc, NULL, 1, USTR "bkwdc"},
	{USTR "gomark", TYPETW + TYPEPW + EMOVE, ugomark, NULL, 0, NULL},
	{USTR "gparse", TYPETW, ugparse, NULL, 0, NULL},
	{USTR "grep", TYPETW, ugrep, NULL, 0, NULL},
	{USTR "groww", TYPETW, ugroww, NULL, 1, USTR "shrinkw"},
	{USTR "if", TYPETW+TYPEPW+TYPEMENU+TYPEQW+EMETA, uif, 0, 0, 0 },
	{USTR "isrch", TYPETW + TYPEPW, uisrch, NULL, 0, NULL},
	{USTR "jump", TYPETW, ujump, NULL, 0, NULL },
	{USTR "killjoe", TYPETW + TYPEPW + TYPEMENU + TYPEQW, ukilljoe, NULL, 0, NULL},
	{USTR "killproc", TYPETW + TYPEPW, ukillpid, NULL, 0, NULL},
	{USTR "help", TYPETW + TYPEPW + TYPEQW, u_help, NULL, 0, NULL},
	{USTR "home", TYPETW + TYPEPW + EFIXXCOL, uhome, NULL, 0, NULL},
	{USTR "hnext", TYPETW + TYPEPW + TYPEQW, u_help_next, NULL, 0, NULL},
	{USTR "hprev", TYPETW + TYPEPW + TYPEQW, u_help_prev, NULL, 0, NULL},
	{USTR "insc", TYPETW + TYPEPW + EFIXXCOL + EMOD, uinsc, NULL, 1, USTR "delch"},
	{USTR "keymap", TYPETW, ukeymap, 0, 0, 0 },    /* JM */
	{USTR "insf", TYPETW + TYPEPW + EMOD, uinsf, NULL, 0, NULL}, 
	{USTR "language", TYPETW + TYPEPW, ulanguage, NULL, 0, NULL}, 
	{USTR "lindent", TYPETW + TYPEPW + EFIXXCOL + EMOD + EBLOCK, ulindent, NULL, 1, USTR "rindent"},
	{USTR "line", TYPETW + TYPEPW, uline, NULL, 0, NULL},
	{USTR "lose", TYPETW + TYPEPW, ulose, NULL, 0, NULL}, 
	{USTR "lower", TYPETW + TYPEPW + EMOD + EBLOCK, ulower, NULL, 0, NULL},
	{USTR "ltarw", TYPETW + TYPEPW /* + EFIXXCOL + ECHKXCOL */, u_goto_left, NULL, 1, USTR "rtarw"},
	{USTR "ltarwmenu", TYPEMENU, umltarw, NULL, 1, USTR "rtarwmenu"},
	{USTR "macros", TYPETW + EFIXXCOL, umacros, NULL, 0, NULL},
	{USTR "debug_joe", TYPETW + EFIXXCOL, udebug_joe, NULL, 0, NULL},
	{USTR "markb", TYPETW + TYPEPW, umarkb, NULL, 0, NULL},
	{USTR "markk", TYPETW + TYPEPW, umarkk, NULL, 0, NULL},
	{USTR "markl", TYPETW + TYPEPW, umarkl, NULL, 0, NULL},
	{USTR "math", TYPETW + TYPEPW, umath, NULL, 0, NULL},
	{USTR "maths", TYPETW + TYPEPW, usmath, NULL, 0, NULL},
	{USTR "menu", TYPETW + TYPEPW + TYPEQW, umenu, NULL, 0, NULL},
	{USTR "mode", TYPETW + TYPEPW + TYPEQW, umode, NULL, 0, NULL},
	{USTR "msg", TYPETW + TYPEPW + TYPEQW + TYPEMENU, umsg, NULL, 0, NULL},
	{USTR "mfit", TYPETW, umfit, NULL, 0, NULL},
	{USTR "mwind", TYPETW, umwind, NULL, 0, NULL},
	{USTR "name", TYPETW + TYPEPW, uname_joe, NULL, 0, NULL}, 
	{USTR "nbuf", TYPETW + EFIXXCOL, unbuf, NULL, 1, USTR "pbuf"},
	{USTR "nedge", TYPETW + TYPEPW + EFIXXCOL, unedge, NULL, 1, USTR "pedge"}, 
	{USTR "nextpos", TYPETW + TYPEPW + EFIXXCOL + EMID + EPOS, unextpos, NULL, 1, USTR "prevpos"}, 
	{USTR "nextw", TYPETW + TYPEPW + TYPEMENU + TYPEQW, unextw, NULL, 1, USTR "prevw"},
	{USTR "nextword", TYPETW + TYPEPW + EFIXXCOL, u_goto_next, NULL, 1, USTR "prevword"},
	{USTR "nmark", TYPETW + TYPEPW, unmark, NULL, 0, NULL},
	{USTR "notmod", TYPETW, unotmod, NULL, 0, NULL},
	{USTR "nxterr", TYPETW, unxterr, NULL, 1, USTR "prverr"},
	{USTR "open", TYPETW + TYPEPW + EFIXXCOL + EMOD, uopen, NULL, 1, USTR "deleol"},
	{USTR "parserr", TYPETW, uparserr, NULL, 0, NULL},
	{USTR "paste", TYPETW + TYPEPW + EMOD, upaste, NULL, 0, NULL },
	{USTR "pbuf", TYPETW + EFIXXCOL, upbuf, NULL, 1, USTR "nbuf"},
	{USTR "pedge", TYPETW + TYPEPW + EFIXXCOL, upedge, NULL, 1, USTR "nedge"}, 
	{USTR "pgdn", TYPETW + TYPEPW + TYPEMENU + TYPEQW + EMOVE, upgdn, NULL, 1, USTR "pgup"},
	{USTR "pgdnmenu", TYPEMENU, umpgdn, NULL, 1, USTR "pgupmenu"}, 
	{USTR "pgup", TYPETW + TYPEPW + TYPEMENU + TYPEQW + EMOVE, upgup, NULL, 1, USTR "pgdn"},
	{USTR "pgupmenu", TYPEMENU, umpgup, NULL, 1, USTR "pgdnmenu"}, 
	{USTR "picokill", TYPETW + TYPEPW + EFIXXCOL + EKILL + EMOD, upicokill, NULL, 1, NULL},
	{USTR "play", TYPETW + TYPEPW + TYPEMENU + TYPEQW, uplay, NULL, 1, NULL},	/* EFIXX? */ 
	{USTR "prevpos", TYPETW + TYPEPW + EPOS + EMID + EFIXXCOL, uprevpos, NULL, 1, USTR "nextpos"}, 
	{USTR "prevw", TYPETW + TYPEPW + TYPEMENU + TYPEQW, uprevw, NULL, 1, USTR "nextw"}, 
	{USTR "prevword", TYPETW + TYPEPW + EFIXXCOL + ECHKXCOL, u_goto_prev, NULL, 1, USTR "nextword"},
	{USTR "prverr", TYPETW, uprverr, NULL, 1, USTR "nxterr"},
	{USTR "psh", TYPETW + TYPEPW + TYPEMENU + TYPEQW, upsh, NULL, 0, NULL},
	{USTR "pop", TYPETW + TYPEPW + TYPEMENU + TYPEQW, upop, NULL, 0, NULL},
	{USTR "popabort", TYPETW + TYPEPW + TYPEMENU + TYPEQW, upopabort, NULL, 0, NULL},
	{USTR "qrepl", TYPETW + TYPEPW + EMOD, pqrepl, NULL, 0, NULL},
	{USTR "query", TYPETW + TYPEPW + TYPEMENU + TYPEQW, uquery, NULL, 0, NULL},
	{USTR "querysave", TYPETW, uquerysave, NULL, 0, NULL},
	{USTR "quote", TYPETW + TYPEPW + EMOD, uquote, NULL, 0, NULL},
	{USTR "quote8", TYPETW + TYPEPW + EMOD, uquote8, NULL, 0, NULL},
	{USTR "record", TYPETW + TYPEPW + TYPEMENU + TYPEQW, urecord, NULL, 0, NULL},
	{USTR "redo", TYPETW + TYPEPW + EFIXXCOL, uredo, NULL, 1, USTR "undo"},
	{USTR "release", TYPETW, urelease, NULL, 0, NULL},
	{USTR "reload", TYPETW, ureload, NULL, 0, NULL },
	{USTR "reloadall", TYPETW, ureload_all, NULL, 0, NULL },
	{USTR "retype", TYPETW + TYPEPW + TYPEMENU + TYPEQW, uretyp, NULL, 0, NULL},
	{USTR "rfirst", TYPETW + TYPEPW, prfirst, NULL, 0, NULL}, 
	{USTR "rindent", TYPETW + TYPEPW + EFIXXCOL + EMOD + EBLOCK, urindent, NULL, 1, USTR "lindent"},
	{USTR "run", TYPETW + TYPEPW, urun, NULL, 0, NULL},
	{USTR "rsrch", TYPETW + TYPEPW, ursrch, NULL, 0, NULL},
	{USTR "rtarw", TYPETW + TYPEPW /* + EFIXXCOL */, u_goto_right, NULL, 1, USTR "ltarw"}, /* EFIX removed for picture mode */
	{USTR "rtarwmenu", TYPEMENU, umrtarw, NULL, 1, USTR "ltarwmenu"},
	{USTR "rtn", TYPETW + TYPEPW + TYPEMENU + TYPEQW + EMOD, urtn, NULL, 1, NULL},
	{USTR "save", TYPETW + TYPEPW, usave, NULL, 0, NULL},
	{USTR "savenow", TYPETW + TYPEPW, usavenow, NULL, 0, NULL},
	{USTR "scratch", TYPETW + TYPEPW, uscratch, NULL, 0, NULL},
	{USTR "secure_type", TYPEPW + TYPEQW + TYPEMENU + EMINOR + EMOD, utype, NULL, 1, USTR "backs"},
	{USTR "select", TYPETW + TYPEPW, uselect, NULL, 0, NULL},
	{USTR "setmark", TYPETW + TYPEPW, usetmark, NULL, 0, NULL},
	{USTR "shell", TYPETW + TYPEPW + TYPEMENU + TYPEQW, ushell, NULL, 0, NULL},
	{USTR "showerr", TYPETW + TYPEPW, ucurrent_msg, NULL, 0, NULL},
	{USTR "showlog", TYPETW, ushowlog, NULL, 0, NULL},
	{USTR "shrinkw", TYPETW, ushrnk, NULL, 1, USTR "groww"},
	{USTR "splitw", TYPETW, usplitw, NULL, 0, NULL},
	{USTR "stat", TYPETW + TYPEPW, ustat, NULL, 0, NULL},
	{USTR "stop", TYPETW + TYPEPW + TYPEMENU + TYPEQW, ustop, NULL, 0, NULL},
	{USTR "swap", TYPETW + TYPEPW + EFIXXCOL, uswap, NULL, 0, NULL},
	{USTR "switch", TYPETW + TYPEPW, uswitch, NULL, 0, NULL},
	{USTR "sys", TYPETW + TYPEPW, usys, NULL, 0, NULL },
	{USTR "tabmenu", TYPEMENU, umtab, NULL, 1, USTR "ltarwmenu"},
	{USTR "tag", TYPETW + TYPEPW, utag, NULL, 0, NULL},
	{USTR "tagjump", TYPETW + TYPEPW, utagjump, NULL, 0, NULL},
	{USTR "toggle_marking", TYPETW + TYPEPW, utoggle_marking, NULL, 0, NULL},
	{USTR "then", TYPEPW+EMOD, urtn, 0, 0, 0 },
	{USTR "timer", TYPETW + TYPEPW + TYPEMENU + TYPEQW, utimer, NULL, 1, NULL},
	{USTR "tomarkb", TYPETW + TYPEPW + EFIXXCOL + EBLOCK, utomarkb, NULL, 0, NULL},
	{USTR "tomarkbk", TYPETW + TYPEPW + EFIXXCOL + EBLOCK, utomarkbk, NULL, 0, NULL},
	{USTR "tomarkk", TYPETW + TYPEPW + EFIXXCOL + EBLOCK, utomarkk, NULL, 0, NULL},
	{USTR "tomatch", TYPETW + TYPEPW + EFIXXCOL, utomatch, NULL, 0, NULL},
	{USTR "tomouse", TYPETW+TYPEPW+TYPEQW+TYPEMENU, utomouse, 0, 0, 0 },
	{USTR "tos", TYPETW + TYPEPW + EMOVE, utos, NULL, 0, NULL},
	{USTR "tw0", TYPETW + TYPEPW + TYPEQW + TYPEMENU, utw0, NULL, 0, NULL},
	{USTR "tw1", TYPETW + TYPEPW + TYPEQW + TYPEMENU, utw1, NULL, 0, NULL},
	{USTR "txt", TYPETW + TYPEPW, utxt, NULL, 0, NULL}, 
	{USTR "type", TYPETW + TYPEPW + TYPEQW + TYPEMENU + EMINOR + EMOD, utype, NULL, 1, USTR "backs"},
	{USTR "undo", TYPETW + TYPEPW + EFIXXCOL, uundo, NULL, 1, USTR "redo"},
	{USTR "uparw", TYPETW + TYPEPW + EMOVE, uuparw, NULL, 1, USTR "dnarw"},
	{USTR "uparwmenu", TYPEMENU, umuparw, NULL, 1, USTR "dnarwmenu"}, 
	{USTR "upper", TYPETW + TYPEPW + EMOD + EBLOCK, uupper, NULL, 0, NULL},
	{USTR "upslide", TYPETW + TYPEPW + TYPEMENU + TYPEQW + EMOVE, uupslide, NULL, 1, USTR "dnslide"},
	{USTR "upslidemenu", TYPEMENU, umscrup, NULL, 1, USTR "dnslidemenu"},
	{USTR "vtbknd", TYPETW, uvtbknd, NULL, 0, NULL},
	{USTR "xtmouse", TYPETW+TYPEPW+TYPEMENU+TYPEQW, uxtmouse, 0, 0, 0 },
	{USTR "yank", TYPETW + TYPEPW + EFIXXCOL + EMOD, uyank, NULL, 1, NULL},
	{USTR "yapp", TYPETW + TYPEPW + EKILL, uyapp, NULL, 0, NULL},
	{USTR "yankpop", TYPETW + TYPEPW + EFIXXCOL + EMOD, uyankpop, NULL, 1, NULL}
};

/* Steal Lock dialog */

int nolocks;

#define LOCKMSG2 _("Could not create lock. (I) edit anyway, (Q) cancel edit? ")
#define LOCKMSG1 _("Locked by %s. (S)teal lock, (I) edit anyway, (Q) cancel edit? ")

unsigned char *steallock_key= (unsigned char *) _("|steal the lock|sS");
unsigned char *canceledit_key= (unsigned char *) _("|cancel edit due to lock|qQ");
unsigned char *ignorelock_key= (unsigned char *) _("|ignore lock, continue with edit|iI");

int steal_lock(BW *bw,int c,B *b,int *notify)
{
	if (yncheck(steallock_key, c)) {
		unsigned char bf1[256];
		unsigned char bf[300];
		unlock_it(b->name);
		if (lock_it(b->name,bf1)) {
			int x;
			for(x=0;bf1[x] && bf1[x]!=':';++x);
			bf1[x]=0;
			if(bf1[0])
				joe_snprintf_1(bf,sizeof(bf),joe_gettext(LOCKMSG1),bf1);
			else
				joe_snprintf_0(bf, sizeof(bf), joe_gettext(LOCKMSG2));
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
		unsigned char bf1[256];
		unsigned char bf[300];
		int x;
		/* It's a plain file- try to lock it */
		if (lock_it(b->name,bf1)) {
			for(x=0;bf1[x] && bf1[x]!=':';++x);
			bf1[x]=0;
			if(bf1[0])
				joe_snprintf_1(bf,sizeof(bf),joe_gettext(LOCKMSG1),bf1);
			else
				joe_snprintf_0(bf, sizeof(bf), joe_gettext(LOCKMSG2));
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
		unsigned char c = k;
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
		scrup = findcmd(USTR "upslide");
		scrdn = findcmd(USTR "dnslide");
		drag = findcmd(USTR "defmdrag");
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
	for (x = 0; x != sizeof(cmds) / sizeof(CMD); ++x)
		htadd(cmdhash, cmds[x].name, cmds + x);
}

CMD *findcmd(unsigned char *s)
{
	if (!cmdhash)
		izcmds();
	return (CMD *) htfind(cmdhash, s);
}

void addcmd(unsigned char *s, MACRO *m)
{
	CMD *cmd = (CMD *) joe_malloc(sizeof(CMD));

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

static unsigned char **getcmds(void)
{
	unsigned char **s = vaensure(NULL, sizeof(cmds) / sizeof(CMD));
	int x;
	HENTRY *e;

	for (x = 0; x != cmdhash->len; ++x)
		for (e = cmdhash->tab[x]; e; e = e->next)
			s = vaadd(s, vsncpy(NULL, 0, sz(e->name)));
	vasort(s, aLen(s));
	return s;
}

/* Command line */

unsigned char **scmds = NULL;	/* Array of command names */

static int cmdcmplt(BW *bw)
{
	if (!scmds)
		scmds = getcmds();
	return simple_cmplt(bw,scmds);
}

static int docmd(BW *bw, unsigned char *s, void *object, int *notify)
{
	MACRO *mac;
	int ret = -1;

	mac = mparse(NULL, s, &ret,0);
	if (ret < 0) {
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
	if (wmkpw(bw->parent, joe_gettext(USTR _("Command: ")), &cmdhist, docmd, USTR "cmd", NULL, cmdcmplt, NULL, NULL, locale_map, 0)) {
		return 0;
	} else {
		return -1;
	}
}
