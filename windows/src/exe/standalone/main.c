// joewin.cpp : Defines the entry point for the application.
//

#include "jwwin.h"
#include "jwglobals.h"

// From putty
int PuttyWinMain(HINSTANCE, HINSTANCE, LPSTR, int);

int APIENTRY wWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPWSTR     lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	if (!jwInitJoe(__argc, __wargv))
	{
		return PuttyWinMain(hInstance, hPrevInstance, "", nCmdShow);
	}
	else
	{
		return 1;
	}
}
