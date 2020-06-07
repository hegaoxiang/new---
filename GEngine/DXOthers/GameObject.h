 #pragma once
#include "GDX11Renderer\GDXBuffer.h"
#include "Graphics\Geometry.h"
#include <GDX11Renderer\Effect\Effects.h>


class GameObject
{
public:
	struct ModelPart
	{
		VertexBuffer vertexBuffer;
		IndexBuffer indexBuffer;
		UINT vertexCount;
		UINT indexCount;
		DXGI_FORMAT indexFormat;

		UINT vertexStride;
	};
	
	template<class VertexType = VertexPos, class IndexType = WORD>
	void SetMesh(ID3D11Device* device, const Geometry::MeshData<VertexType, IndexType>& meshData);

	template<class VertexType, class IndexType>
	void SetMesh(ID3D11Device* device, const std::vector<VertexType>& vertices, const std::vector<IndexType>& indices);

	void SetMesh(ID3D11Device* device, const void* vertices, UINT vertexSize, UINT vertexCount,
		const void* indices, UINT indexCount, DXGI_FORMAT indexFormat);

	void SetWorldMatrix(const DirectX::XMFLOAT4X4& world);
	void XM_CALLCONV SetWorldMatrix(DirectX::FXMMATRIX world);


	void Draw(ID3D11DeviceContext* deviceContext, LightEffect& effect)const;

	void SetDebugName(const std::string& name);

	std::string Name;
	ModelPart m_Model;
private:
	DirectX::XMFLOAT4X4 m_WorldMatrix;							// 世界矩阵
};

template<class VertexType, class IndexType>
inline void GameObject::SetMesh(ID3D11Device* device, const Geometry::MeshData<VertexType, IndexType>& meshData)
{
	SetMesh(device, meshData.vertexVec, meshData.indexVec);
}

template<class VertexType, class IndexType>
inline void GameObject::SetMesh(ID3D11Device* device, const std::vector<VertexType>& vertices, const std::vector<IndexType>& indices)
{
	static_assert(sizeof(IndexType) == 2 || sizeof(IndexType) == 4, "The size of IndexType must be 2 bytes or 4 bytes!");
	static_assert(std::is_unsigned<IndexType>::value, "IndexType must be unsigned integer!");

	SetMesh(device,
		vertices.data(), sizeof(VertexType), (UINT)vertices.size(),
		indices.data(), (UINT)indices.size(),
		(sizeof(IndexType) == 2) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT);
}
