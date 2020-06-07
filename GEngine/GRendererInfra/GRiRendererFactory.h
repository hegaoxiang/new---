#pragma once
#include "GRiTextureLoader.h"
#include "GRiMaterial.h"
#include "GRiGeometryGenerator.h"
#include "GRiMesh.h"
#include "GRiSceneObject.h"

class GRiRendererFactory
{
public:
	virtual GRiTextureLoader* CreateTextureLoader() = 0;

	virtual GRiMaterial* CreateMaterial() = 0;

	GRiGeometryGenerator* CreateGeometryGenerator();

	virtual GRiMesh* CreateMesh(std::vector<GRiMeshData> meshData) = 0;

	GRiSceneObject* CreateSceneObjet();
};

