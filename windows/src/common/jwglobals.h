#ifndef _JOEWIN_GLOBALS_H
#define _JOEWIN_GLOBALS_H

#ifdef _JOEWIN_DO_GLOBALS
#define JOEWIN_GLOBAL
#else
#define JOEWIN_GLOBAL extern
#endif

JOEWIN_GLOBAL int jw_argc;
JOEWIN_GLOBAL unsigned char** jw_argv;
JOEWIN_GLOBAL unsigned char* jw_joedata;
JOEWIN_GLOBAL unsigned char* jw_joerc;
JOEWIN_GLOBAL unsigned char* jw_home;
JOEWIN_GLOBAL wchar_t* jw_personality;

JOEWIN_GLOBAL int jw_initialrows, jw_initialcols;

int jwInitJoe(int, wchar_t**);
HANDLE jwStartJoe(unsigned long*);

void jwexit(int);

#endif