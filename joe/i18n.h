#ifndef _Ii18n
#define _Ii18n 1

int joe_iswupper(struct charmap *,int c);
int joe_iswlower(struct charmap *,int c);

int joe_iswalpha(struct charmap *,int c);	/* or _ */
int joe_iswalpha_(struct charmap *,int c);

int joe_iswalnum(struct charmap *,int c);
int joe_iswalnum_(struct charmap *,int c);

int joe_iswdigit(struct charmap *,int c);
int joe_iswspace(struct charmap *,int c);
int joe_iswctrl(struct charmap *,int c);
int joe_iswpunct(struct charmap *,int c);
int joe_iswgraph(struct charmap *,int c);
int joe_iswprint(struct charmap *,int c);
int joe_iswxdigit(struct charmap *,int c);
int joe_iswblank(struct charmap *,int c);

int joe_wcwidth(int wide,int c);
/* Looking for wswidth? Take a look at scrn.c/txtwidth() */

int joe_towupper(struct charmap *,int c);
int joe_towlower(struct charmap *,int c);

int unictrl(int c);

#endif
