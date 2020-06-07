#include "GDxRendererFactory.h"
#include "GDxTextureLoader.h"
#include "GDxMesh.h"



GDxRendererFactory::GDxRendererFactory(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	mpDevice = device;
	mpDeviceContext = deviceContext;
}

GRiTextureLoader* GDxRendererFactory::CreateTextureLoader()
{
	GRiTextureLoader* texloader = new GDxTextureLoader(mpDevice,mpDeviceContext);
	return texloader;
}

GRiMaterial* GDxRendererFactory::CreateMaterial()
{
	GRiMaterial* material = new GRiMaterial();
	return material;
}

GRiMesh* GDxRendererFactory::CreateMesh(std::vector<GRiMeshData> meshData)
{
	GDxMesh* mesh = new GDxMesh(mpDevice, meshData);
	return mesh;
}
