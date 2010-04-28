#ifndef _Ii18n
#define _Ii18n 1

int joe_iswupper PARAMS((struct charmap *,int c));
int joe_iswlower PARAMS((struct charmap *,int c));

int joe_iswalpha PARAMS((struct charmap *,int c));	/* or _ */
int joe_iswalpha_ PARAMS((struct charmap *,int c));

int joe_iswalnum PARAMS((struct charmap *,int c));
int joe_iswalnum_ PARAMS((struct charmap *,int c));

int joe_iswdigit PARAMS((struct charmap *,int c));
int joe_iswspace PARAMS((struct charmap *,int c));
int joe_iswctrl PARAMS((struct charmap *,int c));
int joe_iswpunct PARAMS((struct charmap *,int c));
int joe_iswgraph PARAMS((struct charmap *,int c));
int joe_iswprint PARAMS((struct charmap *,int c));
int joe_iswxdigit PARAMS((struct charmap *,int c));
int joe_iswblank PARAMS((struct charmap *,int c));

int joe_wcwidth PARAMS((int wide,int c));
/* Looking for wswidth? Take a look at scrn.c/txtwidth() */

int joe_towupper PARAMS((struct charmap *,int c));
int joe_towlower PARAMS((struct charmap *,int c));

int unictrl PARAMS((int c));

#endif
