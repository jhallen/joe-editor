#include "jwwin.h"
#include "joedll.h"

JOEDLL_API int joewinmain(HINSTANCE hInstance, HINSTANCE hPrevInstance, int nCmdShow, int argc, wchar_t** argv)
{
	int PuttyWinMain(HINSTANCE, HINSTANCE, LPSTR, int);
	int jwInitJoe(int, wchar_t**);

	if (!jwInitJoe(argc, argv))
	{
		return PuttyWinMain(hInstance, hPrevInstance, "", nCmdShow);
	}
	else
	{
		return 1;
	}
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

