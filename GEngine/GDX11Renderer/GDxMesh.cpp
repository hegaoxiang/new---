#include "GDxMesh.h"

GDxMesh::GDxMesh(ID3D11Device* device, std::vector<GRiMeshData> meshData)
{
	Create(device, meshData);
}

void GDxMesh::Create(ID3D11Device* device, std::vector<GRiMeshData> meshData)
{
	UINT vertexOffset = 0;
	UINT indexOffset = 0;
	std::vector<GRiVertex> vertices;
	std::vector<std::uint32_t> indices;
	for (auto mdata : meshData)
	{
		GRiSubmesh submesh;
		submesh.IndexCount = (UINT)mdata.Indices.size();
		submesh.StartIndexLocation = indexOffset;
		submesh.BaseVertexLocation = vertexOffset;

		vertices.insert(vertices.end(), std::begin(mdata.Vertices), std::end(mdata.Vertices));
		indices.insert(indices.end(), std::begin(mdata.Indices), std::end(mdata.Indices));

		Submeshes[mdata.MeshDataName] = submesh;
		Submeshes[mdata.MeshDataName].Name = mdata.MeshDataName;

		vertexOffset += (UINT)mdata.Vertices.size();
		indexOffset += (UINT)mdata.Indices.size();
	}

	mVIBuffer = std::make_shared<GDxVertexIndexBuffer>(device, vertices, indices);

	HR(mVIBuffer != nullptr);
}
