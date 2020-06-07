#pragma once
#include "GDxPreInclude.h"
#include "GDXBuffer.h"

class GDxVertexIndexBuffer
{
public:
	GDxVertexIndexBuffer() = delete;
	virtual ~GDxVertexIndexBuffer() {}

	GDxVertexIndexBuffer(ID3D11Device* device, std::vector<GRiVertex> vertices, std::vector<uint32_t> indices);

	void Create(ID3D11Device* device, std::vector<GRiVertex> vertices, std::vector<uint32_t> indices) ;

	// Data about the buffers.
	UINT VertexByteStride = 0;
	UINT VertexBufferByteSize = 0;
	UINT VertexCount = 0;
	DXGI_FORMAT IndexFormat = DXGI_FORMAT_R32_UINT;
	UINT IndexBufferByteSize = 0;
	UINT IndexCount = 0;

	VertexBuffer VBuffer;

	IndexBuffer IBuffer;
};


