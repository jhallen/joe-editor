#ifndef _JOEWIN_JOEDATA_H
#define _JOEWIN_JOEDATA_H

/*
 * These are normally -Ddefines a la the Makefile, but they cannot
 * be constant in a Windows setup.
 */

const char* joedata_plus(const char* input);

#ifdef JOEWIN
#define JOEDATA_PLUS(x) joedata_plus(x)
#define JOERC jw_joerc
#define JOEDATA jw_joedata
#else
#define JOEDATA_PLUS(x) (JOEDATA x)
#endif

extern unsigned char* JOEDATA;
extern unsigned char* JOERC;

#endif //_JOEWIN_JOEDATA_H