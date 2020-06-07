#pragma once
#include "GRendererInfra/GRiTimer.h"
#include "GDX11Renderer/GDxRenderer.h"
#include <memory>
#include "GRendererInfra/GRiCamera.h"
#include "GRendererInfra/GRiTexture.h"
#include <unordered_map>
#include "GScene.h"
#include <GRendererInfra\GRiMesh.h>

class GCore
{
public:
	GCore(const GCore& rhs) = delete;
	GCore& operator=(const GCore& rhs) = delete;
	~GCore();

	static GCore& GetCore();

	void Run();

	void RenderFrame();

	void Initialize(HWND OutputWindow, double width, double height);

	void MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
private:

	bool      mAppPaused = false;  // is the application paused?
	bool      mMinimized = false;  // is the application minimized?
	bool      mMaximized = false;  // is the application maximized?
	bool      mResizing = false;   // are the resize bars being dragged?
	bool      mFullscreenState = false;// full screen enabled

	GRiRenderer* mRenderer;

	GRiRendererFactory* mFactory;

	std::unique_ptr<GRiCamera>			m_pCamera;
	CameraMode m_CameraMode;			// ÉãÏñ»úÄ£Ê½

	std::unique_ptr<GRiTimer> mTimer;

	// Texture
	std::unordered_map<std::string, std::unique_ptr<GRiTexture>> mTextures;
	std::unordered_map<std::string, std::unique_ptr<GRiMaterial>> mMaterials;
	std::unordered_map<std::string, std::unique_ptr<GRiMesh>> mMeshes;
	std::unordered_map<std::string, std::unique_ptr<GRiSceneObject>> mSceneObjects;
	std::vector<GRiSceneObject*>mSceneObjectLayer[(int)RenderLayer::Count];
// 
// 	std::vector<std::string> m_TexNames;
// 	std::unordered_map<std::string, int> m_NameMap;
// 	std::vector<std::unique_ptr<GRiTexture>> m_Textures;

	std::unique_ptr<GScene> mScene;

protected:
#pragma region Initialize Func

	void LoadScene();

	bool LoadAllTexture();

	void LoadMaterials();

	void LoadMeshes();

	void LoadSceneObjects();

	void LoadCamera();


#pragma endregion Initialize
	
	std::string WorkDir;
	std::string mSkyCubeFileName;
	std::vector<std::string> FindFileInFolder(const std::string& relPath,const std::vector<std::string>& formats,bool bCheckFormat = false);
protected:

	void OnResize();

	void Update();

	void Draw();



	//void SaveScene();
private:
	GCore();
};

