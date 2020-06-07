#pragma once
#include "DXOthers/d3dUtil.h"
#include <IMGUI/imgui.h>
#include <IMGUI/imfilebrowser.h>

/*#define TRACE (...) GUI::Get().AddLog(__VA_ARGS__)*/
class IDisplay
{
public:
	virtual void OnGUI() = 0;
};
class GUI
{
public:
	static GUI& Get();

	bool Init(HWND hwnd, ID3D11Device* device, ID3D11DeviceContext* deviceContext);

	void BeginGUI();
	void EndGUI();
	void Render();
	

private:
	GUI();
	bool isInit = false;

};

