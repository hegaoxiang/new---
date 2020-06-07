#include "GUI.h"
#include <IMGUI/imgui.h>
#include <IMGUI/imgui_impl_win32.h>
#include <IMGUI/imgui_impl_dx11.h>
#include <IMGUI/ImGuizmo.h>



GUI& GUI::Get()
{
	static GUI* impl = new GUI();
	return *impl;
}

bool GUI::Init(HWND hwnd, ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	isInit = true;
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
	//io.ConfigViewportsNoAutoMerge = true;
	//io.ConfigViewportsNoTaskBarIcon = true;
	//io.ConfigViewportsNoDefaultParent = true;
	//io.ConfigDockingAlwaysTabBar = true;
	//io.ConfigDockingTransparentPayload = true;
#if 0
	io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleFonts;     // FIXME-DPI: THIS CURRENTLY DOESN'T WORK AS EXPECTED. DON'T USE IN USER APP!
	io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleViewports; // FIXME-DPI
#endif
	//io.WantCaptureMouse = true;

	 // Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	// Setup Platform/Renderer bindings
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(device, deviceContext);

// 	fileDialog = new ImGui::FileBrowser();
// 
// 	// (optional) set browser properties
// 	fileDialog->SetTitle("title");
// 	fileDialog->SetTypeFilters({ ".dds" });

	return true;
}

void GUI::BeginGUI()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

#ifdef EDITOR
	ImGuizmo::BeginFrame();


#endif

// 	if (ImGui::Begin("dummy window"))
// 	{
// 		// open file dialog when user clicks this button
// 		if (ImGui::Button("open file dialog"))
// 			fileDialog->Open();
// 	}
// 	ImGui::End();
// 
// 	fileDialog->Display();
// 
// 	if (fileDialog->HasSelected())
// 	{
// 		AddLog("Selected filename" + fileDialog->GetSelected().string());
// 		fileDialog->ClearSelected();
// 	}
}

void GUI::EndGUI()
{

	ImGui::Render();
}

void GUI::Render()
{
	
	ImGuiIO& io = ImGui::GetIO();

	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	
	// Update and Render additional Platform Windows
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
}

GUI::GUI()
{

}

// void GUI::Dock()
// {
// 	
// 
// 	static bool opt_fullscreen_persistant = true;
// 	bool opt_fullscreen = opt_fullscreen_persistant;
// 	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
// 
// 	// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
// 	// because it would be confusing to have two docking targets within each others.
// 	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
// 	if (opt_fullscreen)
// 	{
// 		ImGuiViewport* viewport = ImGui::GetMainViewport();
// 		ImGui::SetNextWindowPos(viewport->GetWorkPos());
// 		ImGui::SetNextWindowSize(viewport->GetWorkSize());
// 		ImGui::SetNextWindowViewport(viewport->ID);
// 		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
// 		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
// 		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
// 		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
// 	}
// 
// 	// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background 
// 	// and handle the pass-thru hole, so we ask Begin() to not render a background.
// 	if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
// 		window_flags |= ImGuiWindowFlags_NoBackground;
// 
// 	// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
// 	// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
// 	// all active windows docked into it will lose their parent and become undocked.
// 	// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
// 	// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
// 	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
// 	ImGui::Begin("DockSpace Demo",nullptr, window_flags);
// 	ImGui::PopStyleVar();
// 
// 	if (opt_fullscreen)
// 		ImGui::PopStyleVar(2);
// 
// 	// DockSpace
// 	ImGuiIO& io = ImGui::GetIO();
// 	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
// 	{
// 		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
// 		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
// 	}
// 	else
// 	{
// 		//ShowDockingDisabledMessage();
// 		ImGuiIO& io = ImGui::GetIO();
// 		ImGui::Text("ERROR: Docking is not enabled! See Demo > Configuration.");
// 		ImGui::Text("Set io.ConfigFlags |= ImGuiConfigFlags_DockingEnable in your code, or ");
// 		ImGui::SameLine(0.0f, 0.0f);
// 		if (ImGui::SmallButton("click here"))
// 			io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
// 	}
// 
// 	if (ImGui::BeginMenuBar())
// 	{
// 		if (ImGui::BeginMenu("Docking"))
// 		{
// 			// Disabling fullscreen would allow the window to be moved to the front of other windows,
// 			// which we can't undo at the moment without finer window depth/z control.
// 			//ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);
// 
// 			if (ImGui::MenuItem("Flag: NoSplit", "", (dockspace_flags & ImGuiDockNodeFlags_NoSplit) != 0))                 dockspace_flags ^= ImGuiDockNodeFlags_NoSplit;
// 			if (ImGui::MenuItem("Flag: NoResize", "", (dockspace_flags & ImGuiDockNodeFlags_NoResize) != 0))                dockspace_flags ^= ImGuiDockNodeFlags_NoResize;
// 			if (ImGui::MenuItem("Flag: NoDockingInCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_NoDockingInCentralNode) != 0))  dockspace_flags ^= ImGuiDockNodeFlags_NoDockingInCentralNode;
// 			if (ImGui::MenuItem("Flag: PassthruCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) != 0))     dockspace_flags ^= ImGuiDockNodeFlags_PassthruCentralNode;
// 			if (ImGui::MenuItem("Flag: AutoHideTabBar", "", (dockspace_flags & ImGuiDockNodeFlags_AutoHideTabBar) != 0))          dockspace_flags ^= ImGuiDockNodeFlags_AutoHideTabBar;
// 			//ImGui::Separator();
// 
// 			ImGui::EndMenu();
// 		}
// 		// 			HelpMarker(
// 		// 				"When docking is enabled, you can ALWAYS dock MOST window into another! Try it now!" "\n\n"
// 		// 				" > if io.ConfigDockingWithShift==false (default):" "\n"
// 		// 				"   drag windows from title bar to dock" "\n"
// 		// 				" > if io.ConfigDockingWithShift==true:" "\n"
// 		// 				"   drag windows from anywhere and hold Shift to dock" "\n\n"
// 		// 				"This demo app has nothing to do with it!" "\n\n"
// 		// 				"This demo app only demonstrate the use of ImGui::DockSpace() which allows you to manually create a docking node _within_ another window. This is useful so you can decorate your main application window (e.g. with a menu bar)." "\n\n"
// 		// 				"ImGui::DockSpace() comes with one hard constraint: it needs to be submitted _before_ any window which may be docked into it. Therefore, if you use a dock spot as the central point of your application, you'll probably want it to be part of the very first window you are submitting to imgui every frame." "\n\n"
// 		// 				"(NB: because of this constraint, the implicit \"Debug\" window can not be docked into an explicit DockSpace() node, because that window is submitted as part of the NewFrame() call. An easy workaround is that you can create your own implicit \"Debug##2\" window after calling DockSpace() and leave it in the window stack for anyone to use.)"
// 		// 			);
// 		ImGui::EndMenuBar();
// 	}
// 
// 	
// 	ImGui::End();
// 
// 	
// }
