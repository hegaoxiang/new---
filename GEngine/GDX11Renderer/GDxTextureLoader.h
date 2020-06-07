#pragma once
#include "GRendererInfra/GRiTextureLoader.h"
#include <d3d11.h>

class GDxTextureLoader :
	public GRiTextureLoader
{
public:
	GDxTextureLoader(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
		:mpDevice(device), mpDeviceContext(deviceContext){};

	virtual GRiTexture* LoadTexture(std::wstring fileName, bool isCubeMap = false) override;

	virtual GRiTexture* LoadTexture(std::string fileName, bool isCubeMap = false) override;
private:

	ID3D11Device* mpDevice;
	ID3D11DeviceContext* mpDeviceContext;
};

