#pragma once
#include "GRendererInfra/GRiTexture.h"

#include "DXOthers/d3dUtil.h"
#include <wrl/client.h>

class GDxTexture:public GRiTexture
{
public:

	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	ComPtr<ID3D11ShaderResourceView> mSRV;
};

