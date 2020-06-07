#pragma once
#include "GDxPreInclude.h"

class Buffer
{
public:
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	Buffer() = default;

	Buffer(Buffer const&) = delete;
	Buffer& operator= (Buffer const&) = delete;


	virtual ~Buffer() { m_Buffer.Reset(); }

	// Looks up the underlying D3D buffer.
	ID3D11Buffer* GetBuffer() const noexcept
	{
		return m_Buffer.Get();
	}
	auto GetBufferAddress() const noexcept
	{
		return m_Buffer.GetAddressOf();
	}
	virtual void SetBufferName(const std::string& name)const
	{
		D3D11SetDebugObjectName(m_Buffer.Get(), name);
	}
protected:
	ComPtr<ID3D11Buffer> m_Buffer;
};

class VertexBuffer :public Buffer
{
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	void Create(ID3D11Device* device, const void* vertices, UINT vertexByteStride, UINT vertexCount)
	{
		m_Buffer.Reset();

		D3D11_BUFFER_DESC vbd;
		ZeroMemory(&vbd, sizeof(vbd));
		vbd.Usage = D3D11_USAGE_IMMUTABLE;
		vbd.ByteWidth = vertexByteStride * vertexCount;
		vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbd.CPUAccessFlags = 0;
		// 新建顶点缓冲区
		D3D11_SUBRESOURCE_DATA InitData;
		ZeroMemory(&InitData, sizeof(InitData));
		InitData.pSysMem = vertices;
		HR(device->CreateBuffer(&vbd, &InitData, m_Buffer.GetAddressOf()));
	}
	void SetBufferName(const std::string& name)const override
	{
		SetBufferName(name + "VertexBuffer");
	}
};

class IndexBuffer :public Buffer
{
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	void Create(ID3D11Device* device, const void* indices, UINT indexCount, DXGI_FORMAT indexFormat)
	{
		m_Buffer.Reset();

		D3D11_BUFFER_DESC ibd;
		ZeroMemory(&ibd, sizeof(ibd));
		ibd.Usage = D3D11_USAGE_IMMUTABLE;
		if (indexFormat == DXGI_FORMAT_R16_UINT)
		{
			ibd.ByteWidth = indexCount * (UINT)sizeof(WORD);
		}
		else
		{
			ibd.ByteWidth = indexCount * (UINT)sizeof(DWORD);
		}
		ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		ibd.CPUAccessFlags = 0;
		// 新建索引缓冲区
		D3D11_SUBRESOURCE_DATA InitData;
		ZeroMemory(&InitData, sizeof(InitData));
		InitData.pSysMem = indices;
		HR(device->CreateBuffer(&ibd, &InitData, m_Buffer.GetAddressOf()));

		m_EleCount = indexCount;
	}

	void SetBufferName(const std::string& name)const override
	{
		SetBufferName(name + "IndexBuffer");
	}
	uint32_t GetCount()const
	{
		return m_EleCount;
	}

private:
	uint32_t m_EleCount;
};
template<typename T = WORD>
class ConstantBuffer :public Buffer
{
public:
	void Create(ID3D11Device* device)
	{
		D3D11_BUFFER_DESC desc = {};


		desc.ByteWidth = sizeof(T);
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		HR(
			device->CreateBuffer(&desc, nullptr, m_Buffer.ReleaseAndGetAddressOf())
		);
	}

	// Writes new data into the buffer.
	void SetData(ID3D11DeviceContext* deviceContext, T const& value)
	{
		assert(m_Buffer);

		D3D11_MAPPED_SUBRESOURCE mappedResource;

		HR(
			deviceContext->Map(m_Buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)
		);

		*static_cast<T*>(mappedResource.pData) = value;

		deviceContext->Unmap(m_Buffer.Get(), 0);
	}
};


