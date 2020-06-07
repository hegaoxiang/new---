#pragma once
#include "GEngine/GCore.h"

#define DLLEXPORT __declspec(dllexport)
extern "C"
{
	DLLEXPORT void __stdcall InitD3D(HWND hWnd, double width, double height);
	
	DLLEXPORT int __stdcall RenderFrame(void);
	
	DLLEXPORT void __stdcall MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
}

