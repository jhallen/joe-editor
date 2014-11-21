#ifndef _JOEWIN_JWCONFIG_H
#define _JOEWIN_JWCONFIG_H

void jwLoadConfig(Config*);
void jwReloadColors(Config*, wchar_t*);

void jwSaveSettings(Config *cfg);
int jwLoadWindowLoc(POINT *point);
void jwSaveWindowCoords(HWND hwnd, int rows, int cols);

#endif // _JOEWIN_JWCONFIG_H
