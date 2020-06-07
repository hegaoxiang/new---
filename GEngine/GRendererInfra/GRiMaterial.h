#pragma once
#include "GRiPreInclude.h"
#include "GRiTexture.h"
class GRiMaterial
{
public:
	//size_t GetTextureNum();

	void AddTexture(GRiTexture* tex);

	void AddTexture(std::string tex);
public:
	//std::vector<GRiTexture*> pTexture;
	std::vector<std::string> pTextures;

	std::string Name;
};
