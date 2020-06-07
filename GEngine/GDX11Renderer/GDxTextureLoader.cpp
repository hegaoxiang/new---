#include "GDxTextureLoader.h"
#include "DXOthers/DXTrace.h"
#include "DXOthers/DDSTextureLoader.h"
#include "GDxTexture.h"
#include <filesystem>
#include <string>
#include "GGenericInfra/GGiEngineUtil.h"
using namespace std;
using namespace std::filesystem;

using namespace DirectX;
using namespace Microsoft::WRL;


GRiTexture* GDxTextureLoader::LoadTexture(std::wstring fileName, bool isCubeMap)
{
	GDxTexture* tex = new GDxTexture();

	tex->UniqueFileName = GGiEngineUtil::WStringToString(fileName);

	if (isCubeMap)
	{
		(CreateWICTexture2DCubeFromFile(mpDevice,
			mpDeviceContext,
			fileName,
			nullptr,
			tex->mSRV.ReleaseAndGetAddressOf(),
			false));
		
		return tex;
	}
	

	// only dds has the s in the last location
	if (fileName[fileName.size() - 1] == 's')
	{
		HR(CreateDDSTextureFromFile(mpDevice, fileName.c_str(), nullptr, tex->mSRV.GetAddressOf()));
	}
	else
	{
		HR(CreateWICTextureFromFile(mpDevice, fileName.c_str(), nullptr, tex->mSRV.GetAddressOf()));
	}
	return tex;
}

GRiTexture* GDxTextureLoader::LoadTexture(std::string fileName, bool isCubeMap /*= false*/)
{
	return LoadTexture(GGiEngineUtil::StringToWString(fileName), isCubeMap);
}
