#pragma once
#include "GRiTexture.h"
class GRiTextureLoader
{
public:
	virtual GRiTexture* LoadTexture(std::wstring fileName,bool isCubeMap = false) = 0;
	virtual GRiTexture* LoadTexture(std::string fileName, bool isCubeMap = false) = 0;
};

