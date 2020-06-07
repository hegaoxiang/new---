#include "GDxVertexIndexBuffer.h"

GDxVertexIndexBuffer::GDxVertexIndexBuffer(ID3D11Device* device, std::vector<GRiVertex> vertices, std::vector<uint32_t> indices)
{
	// Data about the buffers.
	Create(device, vertices, indices);
}

void GDxVertexIndexBuffer::Create(ID3D11Device* device, std::vector<GRiVertex> vertices, std::vector<uint32_t> indices)
{
	VertexByteStride = sizeof(GRiVertex);
	IndexFormat = DXGI_FORMAT_R32_UINT;
	VertexCount = (UINT)vertices.size();
	IndexCount = (UINT)indices.size();
	VertexBufferByteSize = VertexCount * sizeof(GRiVertex);
	IndexBufferByteSize = IndexCount * sizeof(std::uint32_t);

	VBuffer.Create(device, vertices.data(), VertexByteStride, VertexCount);
	IBuffer.Create(device, indices.data(), IndexCount, IndexFormat);
}
