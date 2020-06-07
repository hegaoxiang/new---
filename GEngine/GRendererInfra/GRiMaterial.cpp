#include "GRiMaterial.h"

void GRiMaterial::AddTexture(GRiTexture* tex)
{
	pTextures.push_back(tex->UniqueFileName);
}

void GRiMaterial::AddTexture(std::string tex)
{
	pTextures.push_back(tex);
}
