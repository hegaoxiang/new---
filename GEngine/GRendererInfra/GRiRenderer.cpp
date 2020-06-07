#include "GRiRenderer.h"

float GRiRenderer::AspectRatio() const
{
	return static_cast<float>(mClientWidth) / mClientHeight;
}


HWND GRiRenderer::MainWnd()const
{
	return mhMainWnd;
}


void GRiRenderer::SetClientWidth(int width)
{
	mClientWidth = width;
}

void GRiRenderer::SetClientHeight(int height)
{
	mClientHeight = height;
}
bool GRiRenderer::IsRunning() const
{
	return mIsRunning;
}
void GRiRenderer::SetCamera(GRiCamera* cam)
{
	pCamera = cam;
}
void GRiRenderer::SetScene(GScene* scene)
{
	pScene = scene;
}
GRiRendererFactory* GRiRenderer::GetFactory()
{
	return mFactory.get();
}
void GRiRenderer::SyncTextures(std::unordered_map<std::string, std::unique_ptr<GRiTexture>>& mTextures)
{
	pTextures.clear();
	std::unordered_map<std::string, std::unique_ptr<GRiTexture>>::iterator i;
	for (i = mTextures.begin(); i != mTextures.end(); i++)
	{
		pTextures[i->first] = i->second.get();
	}
}

void GRiRenderer::SyncMaterials(std::unordered_map<std::string, std::unique_ptr<GRiMaterial>>& mMaterials)
{
	pMaterials.clear();
	std::unordered_map<std::string, std::unique_ptr<GRiMaterial>>::iterator i;
	for (i = mMaterials.begin(); i != mMaterials.end(); i++)
	{
		pMaterials[i->first] = i->second.get();
	}
}

void GRiRenderer::SyncMeshes(std::unordered_map<std::string, std::unique_ptr<GRiMesh>>& mMeshes)
{
	pMeshes.clear();
	std::unordered_map<std::string, std::unique_ptr<GRiMesh>>::iterator i;
	for (i = mMeshes.begin(); i != mMeshes.end(); i++)
	{
		pMeshes[i->first] = i->second.get();
	}
}

void GRiRenderer::SyncSceneObjects(std::unordered_map<std::string, std::unique_ptr<GRiSceneObject>>& mSceneObjects, std::vector<GRiSceneObject*>* mSceneObjectLayer)
{
	pSceneObjects.clear();
	std::unordered_map<std::string, std::unique_ptr<GRiSceneObject>>::iterator i;
	for (i = mSceneObjects.begin(); i != mSceneObjects.end(); i++)
	{
		pSceneObjects[i->first] = i->second.get();
	}
	for (size_t layer = 0; layer != (size_t)(RenderLayer::Count); layer++)
	{
		pSceneObjectLayer[layer].clear();
		for (auto pSObj : mSceneObjectLayer[layer])
		{
			pSceneObjectLayer[layer].push_back(pSObj);
 		}
	}
}
int GRiRenderer::GetClientWidth()const
{
	return mClientWidth;
}

int GRiRenderer::GetClientHeight()const
{
	return mClientHeight;
}
