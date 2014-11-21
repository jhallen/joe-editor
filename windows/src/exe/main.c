#include "jwwin.h"
#include <stdlib.h>

#include "joedll.h"

int APIENTRY wWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPWSTR     lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(lpCmdLine);
	return joewinmain(hInstance, hPrevInstance, nCmdShow, __argc, __wargv);
}
