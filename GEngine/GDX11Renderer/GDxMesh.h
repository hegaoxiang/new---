#pragma once
#include "GDxPreInclude.h"
#include "GRendererInfra/GRiMesh.h"
#include "GDxVertexIndexBuffer.h"

class GDxMesh : public GRiMesh
{
public:
	GDxMesh() = default;
	GDxMesh(const GDxMesh & rhs) = delete;
	~GDxMesh() = default;

	GDxMesh(ID3D11Device* device, std::vector<GRiMeshData> meshData);

	void Create(ID3D11Device* device, std::vector<GRiMeshData> meshData);

	std::shared_ptr<GDxVertexIndexBuffer> mVIBuffer;
};

