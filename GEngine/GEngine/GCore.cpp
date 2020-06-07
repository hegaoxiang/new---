#include "GCore.h"
#include "GCameraController.h"
#include <filesystem>
#include <string>
#include <Vendor\IMGUI\imgui_impl_win32.h>

using namespace std;
using namespace std::filesystem;
GCore::~GCore()
{
	mScene->Serialize(L"file.data");
}

GCore& GCore::GetCore()
{
	static GCore* instance = new GCore();

	return *instance;
}

void GCore::Run()
{
	MSG msg = { 0 };

	mTimer->Reset();

	while (msg.message != WM_QUIT)
	{
		// If there are Window messages then process them.
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		// Otherwise, do animation/game stuff.
		else
		{
			mTimer->Tick();

			if (!mAppPaused)
			{
				
				Update();
				Draw();
			}
			else
			{
				Sleep(100);
			}
		}
	}
}

void GCore::RenderFrame()
{
	Update();

	Draw();
}

void GCore::Initialize(HWND OutputWindow, double width, double height)
{
	mRenderer->PreInitialize(OutputWindow, width, height);


	mFactory = mRenderer->GetFactory();

	LoadScene();
	mRenderer->SetScene(mScene.get());

	if (!LoadAllTexture())
		return;
	mRenderer->SyncTextures(mTextures);

	LoadMaterials();
	mRenderer->SyncMaterials(mMaterials);

	LoadMeshes();
	mRenderer->SyncMeshes(mMeshes);

	LoadSceneObjects();
	mRenderer->SyncSceneObjects(mSceneObjects,mSceneObjectLayer);

	LoadCamera();
	mRenderer->SetCamera(m_pCamera.get());


	mRenderer->Initialize();
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

void GCore::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
#ifdef USE_IMGUI
	ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam);
#endif 
	switch (msg)
	{
		// WM_ACTIVATE is sent when the window is activated or deactivated.
		// We pause the game when the window is deactivated and unpause it
		// when it becomes active.
	case WM_ACTIVATE:
		if (LOWORD(wParam) == WA_INACTIVE)
		{
			mAppPaused = true;
			mTimer->Stop();
		}
		else
		{
			mAppPaused = false;
			mTimer->Start();
		}
		return ;

		// WM_SIZE is sent when the user resizes the window.
	case WM_SIZE:
		// Save the new client area dimensions.
		mRenderer->SetClientWidth(LOWORD(lParam));
		mRenderer->SetClientHeight(HIWORD(lParam));
		if (mRenderer->IsRunning())
		{
			if (wParam == SIZE_MINIMIZED)
			{
				mAppPaused = true;
				mMinimized = true;
				mMaximized = false;
			}
			else if (wParam == SIZE_MAXIMIZED)
			{
				mAppPaused = false;
				mMinimized = false;
				mMaximized = true;
				OnResize();
			}
			else if (wParam == SIZE_RESTORED)
			{

				// Restoring from minimized state?
				if (mMinimized)
				{
					mAppPaused = false;
					mMinimized = false;
					OnResize();
				}

				// Restoring from maximized state?
				else if (mMaximized)
				{
					mAppPaused = false;
					mMaximized = false;
					OnResize();
				}
				else if (mResizing)
				{
					// If user is dragging the resize bars, we do not resize
					// the buffers here because as the user continuously
					// drags the resize bars, a stream of WM_SIZE messages are
					// sent to the window, and it would be pointless (and slow)
					// to resize for each WM_SIZE message received from dragging
					// the resize bars.  So instead, we reset after the user is
					// done resizing the window and releases the resize bars, which
					// sends a WM_EXITSIZEMOVE message.
				}
				else // API call such as SetWindowPos or mSwapChain->SetFullscreenState.
				{
					OnResize();
				}
			}
		}
		return; 0;

		// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
	case WM_ENTERSIZEMOVE:
		mAppPaused = true;
		mResizing = true;
		mTimer->Stop();
		return; 0;

		// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
		// Here we reset everything based on the new window dimensions.
	case WM_EXITSIZEMOVE:
		mAppPaused = false;
		mResizing = false;
		mTimer->Start();
		OnResize();
		return; 0;

		// WM_DESTROY is sent when the window is being destroyed.
	case WM_DESTROY:
		PostQuitMessage(0);
		return; 0;

		// The WM_MENUCHAR message is sent when a menu is active and the user presses
		// a key that does not correspond to any mnemonic or accelerator key.
	case WM_MENUCHAR:
		// Don't beep when we alt-enter.
		return; MAKELRESULT(0, MNC_CLOSE);

		// Catch this message so to prevent the window from becoming too small.
	case WM_GETMINMAXINFO:
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;
		return; 0;

	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
	case WM_KEYUP:
		if (wParam == VK_ESCAPE)
		{
			;//PostQuitMessage(0);
		}

		return; 0;
	}
}

std::vector<std::string> GCore::FindFileInFolder(const std::string& relPath, const std::vector<std::string>& formats, bool bCheckFormat)
{
	vector<string> files;

	string fullPath = WorkDir + relPath;

	path str(fullPath);
	if (!exists(str))		//必须先检测目录是否存在才能使用文件入口.
		return files;
	directory_entry entry(str);		//文件入口
	assert (entry.status().type() == file_type::directory);

	directory_iterator list(str);	        //文件入口容器
	
	for (auto& it : list)
	{
		if (it.is_directory())
		{
			vector<string> temp = move(FindFileInFolder(relPath + "\\" + it.path().filename().string(), formats, bCheckFormat));
			files.insert(files.end(), temp.begin(), temp.end());
		}
		else
		{
			for (auto& fm:formats)
			{
				if (it.path().extension().string() == fm)
				{
					files.emplace_back(it.path().relative_path().string());
					break;
				}
			}
		}
		
	}

	return files;
}

void GCore::OnResize()
{
	mRenderer->OnResize();
}

void GCore::Update()
{
	auto& io = ImGui::GetIO();
	float dt = io.DeltaTime;

	GCameraController::UpdataCamera(m_pCamera.get(), m_CameraMode, dt);

	mRenderer->Update(dt);
}



void GCore::Draw()
{
	auto& io = ImGui::GetIO();
	float dt = io.DeltaTime;


	mRenderer->Draw(dt);
}

bool GCore::LoadAllTexture()
{
	unique_ptr<GRiTextureLoader> texloader(mFactory->CreateTextureLoader());

	WorkDir = "Project\\";

	vector<string> formats{".dds",".png"};

	decltype(formats) files = move(FindFileInFolder("Content\\Textures", formats, true));
	
	GRiTexture* tex;
	for (auto& fileName : files)
	{
		
		tex = texloader->LoadTexture(fileName);
		tex->UniqueFileName = fileName;

		unique_ptr<GRiTexture> temp(tex);

		mTextures.insert(std::pair<string, unique_ptr<GRiTexture>>(tex->UniqueFileName, move(temp)));

	}

	// SkyCubeMap
	mSkyCubeFileName = "Project\\Content\\Textures\\daylight.jpg";
	
	tex = texloader->LoadTexture(mSkyCubeFileName,true);	
	tex->UniqueFileName = "SkyCube";
	unique_ptr<GRiTexture> temp(tex);

	mTextures.insert(std::pair<string, unique_ptr<GRiTexture>>(tex->UniqueFileName, move(temp)));



	return true;
}


void GCore::LoadMaterials()
{
	{
		auto defaultMat = std::make_unique<GRiMaterial>(*mFactory->CreateMaterial());

		defaultMat->AddTexture(mTextures["Project\\Content\\Textures\\floor.dds"].get());
		defaultMat->Name = "DefaultFloor";

		mMaterials[defaultMat->Name] = move(defaultMat);
	}
	{
		auto defaultMat = std::make_unique<GRiMaterial>(*mFactory->CreateMaterial());

		defaultMat->AddTexture(mTextures["Project\\Content\\Textures\\bricks.dds"].get());
		defaultMat->Name = "DefaultBricks";

		mMaterials[defaultMat->Name] = move(defaultMat);
	}


}

void GCore::LoadMeshes()
{
	unique_ptr<GRiGeometryGenerator> geo(mFactory->CreateGeometryGenerator());

	vector<GRiMeshData> meshData;

	#pragma region SysShape
{
		{
			GRiMeshData cMeshData = geo->CreateBox(2, 2, 2, 0);
			meshData.push_back(cMeshData);
			auto mesh = mFactory->CreateMesh(meshData);
			mesh->UniqueName = "Box";
			mesh->Submeshes[cMeshData.MeshDataName].SetMaterial(mMaterials["DefaultFloor"].get());
			unique_ptr<GRiMesh> temp1(mesh);
			mMeshes[mesh->UniqueName] = move(temp1);
		}
		meshData.clear();
		{
			GRiMeshData cMeshData = geo->CreateSphere(20,20,20);
			meshData.push_back(cMeshData);
			auto mesh = mFactory->CreateMesh(meshData);
			mesh->UniqueName = "Sphere";
			mesh->Submeshes[cMeshData.MeshDataName].SetMaterial(mMaterials["DefaultBricks"].get());
			unique_ptr<GRiMesh> temp1(mesh);
			mMeshes[mesh->UniqueName] = move(temp1);
		}
	}
#pragma endregion SysShape

	meshData.clear();
	{
		GRiMeshData cMeshData = geo->CreateQuad(-1,-1,2,2);
		meshData.push_back(cMeshData);
		auto mesh = mFactory->CreateMesh(meshData);
		mesh->UniqueName = "Quad";
		mesh->Submeshes[cMeshData.MeshDataName].SetMaterial(mMaterials["DefaultBricks"].get());
		unique_ptr<GRiMesh> temp1(mesh);
		mMeshes[mesh->UniqueName] = move(temp1);
	}

}

void GCore::LoadSceneObjects()
{
	//auto so = make_unique<GRiSceneObject>(*mFactory->CreateSceneObjet());

	
	unique_ptr<GRiSceneObject> so(mFactory->CreateSceneObjet());
	so->UniqueName = "TestBox";
	so->Mesh = mMeshes["Box"].get();
	XMStoreFloat4x4(&so->World,XMMatrixIdentity());
	mSceneObjectLayer[(int)RenderLayer::Default].push_back(so.get());
	mSceneObjects[so->UniqueName] = move(so);
	

	{
		unique_ptr<GRiSceneObject> so(mFactory->CreateSceneObjet());
		so->UniqueName = "TestShpere";
		so->Mesh = mMeshes["Sphere"].get();
		XMStoreFloat4x4(&so->World, XMMatrixTranslation(2, 2, 2));
		mSceneObjectLayer[(int)RenderLayer::Default].push_back(so.get());
		mSceneObjects[so->UniqueName] = move(so);
	}


	unique_ptr<GRiSceneObject> skySo(mFactory->CreateSceneObjet());
	skySo->UniqueName = "Sky";
	skySo->Mesh = mMeshes["Quad"].get();
	mSceneObjectLayer[(int)RenderLayer::Sky].push_back(skySo.get());
	mSceneObjects[skySo->UniqueName] = move(skySo);


}

void GCore::LoadCamera()
{
	FirstPersonCamera cam;
	//camera->SetViewPort(0.0f, 0.0f, (float)width, (float)height);

	cam.LookAt(XMVectorSet(0.0f, 0.0f, -5.0f, 0.0f), XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
		XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));

	m_pCamera = std::make_unique<FirstPersonCamera>(cam);


	// 初始化仅在窗口大小变动时修改的值
	m_pCamera->SetFrustum(XM_PI / 3, mRenderer->AspectRatio(), 0.5f, 1000.0f);

}

void GCore::LoadScene()
{
	mScene->AntiSerialize(L"file.data");
}

GCore::GCore()
{
	mTimer = std::make_unique<GRiTimer>();

	mRenderer = &GDxRenderer::GetRenderer();

	mScene = std::make_unique<GScene>();
}
