#pragma once
#include <windows.h>
#include <memory>
#include "GRiTimer.h"
#include "GRiRendererFactory.h"
#include "GRiCamera.h"
#include <GEngine/GScene.h>
#define USE_IMGUI

class GRiRenderer
{
public:
	virtual void Initialize() = 0;

	virtual void PreInitialize(HWND OutputWindow, double width, double height) = 0;

	virtual void Update(const float gt) = 0;

	virtual void Draw(const float gt) = 0;

	virtual void OnResize() = 0;


	virtual float AspectRatio() const;
	HWND MainWnd() const;

	int GetClientWidth()const;
	int GetClientHeight()const;
	void SetClientWidth(int width);
	void SetClientHeight(int height);

	bool IsRunning()const;

	void SetCamera(GRiCamera* cam);
	void SetScene(GScene* scene);
	virtual void CreateRendererFactory() = 0;
	GRiRendererFactory* GetFactory();

	GRiCamera* pCamera = nullptr;

	GScene* pScene = nullptr;

	
protected:
	HWND    mhMainWnd = nullptr;

	int mClientWidth = 800;
	int mClientHeight = 600;

	bool mIsRunning = false;

	std::unique_ptr<GRiRendererFactory> mFactory;

public:

	virtual void SyncTextures(std::unordered_map<std::string, std::unique_ptr<GRiTexture>>& mTextures);
	virtual void SyncMaterials(std::unordered_map<std::string, std::unique_ptr<GRiMaterial>>& mMaterials);
	virtual void SyncMeshes(std::unordered_map<std::string, std::unique_ptr<GRiMesh>>& mMeshes);
	virtual void SyncSceneObjects(std::unordered_map<std::string, std::unique_ptr<GRiSceneObject>>& mSceneObjects, std::vector<GRiSceneObject*>* mSceneObjectLayer);



	std::unordered_map<std::string, GRiTexture* > pTextures;
	std::unordered_map<std::string, GRiMaterial*> pMaterials;
	std::unordered_map<std::string, GRiMesh*> pMeshes;
	std::unordered_map<std::string, GRiSceneObject*> pSceneObjects;
	std::vector<GRiSceneObject*> pSceneObjectLayer[(int)RenderLayer::Count];


	//GRiTimer* mTimer;
};

