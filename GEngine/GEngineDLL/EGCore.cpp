#include "EGCore.h"
#include "GEngine/GCore.h"

DLLEXPORT void __stdcall InitD3D(HWND hWnd, double width, double height)
{
	GCore::GetCore().Initialize(hWnd, width, height);
}

DLLEXPORT int __stdcall RenderFrame(void)
{
	GCore::GetCore().Run();
	return 0;
}

DLLEXPORT void __stdcall MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	
	 GCore::GetCore().MsgProc(hwnd,msg,wParam,lParam);
}
