#pragma once
#include "GRiPreInclude.h"
#include "GRiMesh.h"
#include <DirectXMath.h>

class GRiSceneObject
{
public:
	GRiSceneObject() = default;
	GRiSceneObject(const GRiSceneObject & rhs) = delete;

	// Give it a name so we can look it up by name.
	std::string UniqueName;

	float Location[3] = { 0.0f, 0.0f, 0.0f };
	float Rotation[3] = { 0.0f, 0.0f, 0.0f };
	float Scale[3] = { 1.0f, 1.0f, 1.0f };

	// Index into GPU constant buffer corresponding to the ObjectCB for this render item.
	UINT ObjIndex = -1;

	//GRiMaterial* Mat = nullptr;

	//std::unordered_map<std::string, GRiMaterial*> pOverrideMaterial;

	GRiMesh* Mesh;

	DirectX::XMFLOAT4X4 World;
};

