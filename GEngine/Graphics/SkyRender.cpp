#include "SkyRender.h"
#include "Geometry.h"
#include "DXOthers/d3dUtil.h"

#pragma warning(disable: 26812)

using namespace DirectX;
using namespace Microsoft::WRL;

HRESULT SkyRender::InitResource(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const std::wstring& cubemapFilename, float skySphereRadius, bool generateMips)
{
	// ��ֹ�ظ���ʼ������ڴ�й©
	m_pIndexBuffer.Reset();
	m_pVertexBuffer.Reset();
	m_pTextureCubeSRV.Reset();

	HRESULT hr;
	// ��պ��������
	if (cubemapFilename.substr(cubemapFilename.size() - 3) == L"dds")
	{
		hr = CreateDDSTextureFromFile(device,
			generateMips ? deviceContext : nullptr,
			cubemapFilename.c_str(),
			nullptr,
			m_pTextureCubeSRV.GetAddressOf());
	}
	else
	{
		hr = CreateWICTexture2DCubeFromFile(device,
			deviceContext,
			cubemapFilename,
			nullptr,
			m_pTextureCubeSRV.GetAddressOf(),
			generateMips);
	}

	if (FAILED(hr))
		return hr;

	return InitResource(device, skySphereRadius);
}

HRESULT SkyRender::InitResource(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const std::vector<std::wstring>& cubemapFilenames, float skySphereRadius, bool generateMips)
{
	// ��ֹ�ظ���ʼ������ڴ�й©
	m_pIndexBuffer.Reset();
	m_pVertexBuffer.Reset();
	m_pTextureCubeSRV.Reset();

	HRESULT hr;
	// ��պ��������
	hr = CreateWICTexture2DCubeFromFile(device,
		deviceContext,
		cubemapFilenames,
		nullptr,
		m_pTextureCubeSRV.GetAddressOf(),
		generateMips);
	if (FAILED(hr))
		return hr;

	return InitResource(device, skySphereRadius);
}

ID3D11ShaderResourceView* SkyRender::GetTextureCube()
{
	return m_pTextureCubeSRV.Get();
}

void SkyRender::Draw(ID3D11DeviceContext* deviceContext, SkyEffect& skyEffect, const GRiCamera& GRiCamera)
{
	auto cor = GRiCamera.GetCorner();
	auto view = GRiCamera.GetViewXM();
	auto corMat = XMLoadFloat4x4(&cor);
	auto inView = XMMatrixInverse(nullptr, view);
	auto corDirMat = corMat * inView;
	XMFLOAT4X4 corDir;
	XMStoreFloat4x4(&corDir, corDirMat);

	UINT strides[1] = { sizeof(VertexPosTex) };
	UINT offsets[1] = { 0 };
	deviceContext->IASetVertexBuffers(0, 1, m_pVertexBuffer.GetAddressOf(), strides, offsets);
	deviceContext->IASetIndexBuffer(m_pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	skyEffect.SetCorners(corDir);
	skyEffect.SetTextureCube(m_pTextureCubeSRV.Get());
	skyEffect.Apply(deviceContext);
	deviceContext->DrawIndexed(m_IndexCount, 0, 0);
}

void SkyRender::SetDebugObjectName(const std::string& name)
{
#if (defined(DEBUG) || defined(_DEBUG)) && (GRAPHICS_DEBUGGER_OBJECT_NAME)
	// ����տ��ܴ��ڵ�����
	D3D11SetDebugObjectName(m_pTextureCubeSRV.Get(), nullptr);

	D3D11SetDebugObjectName(m_pTextureCubeSRV.Get(), name + ".CubeMapSRV");
	D3D11SetDebugObjectName(m_pVertexBuffer.Get(), name + ".VertexBuffer");
	D3D11SetDebugObjectName(m_pIndexBuffer.Get(), name + ".IndexBuffer");
#else
	UNREFERENCED_PARAMETER(name);
#endif
}

HRESULT SkyRender::InitResource(ID3D11Device* device, float skySphereRadius)
{

	HRESULT hr;
	auto mesh = Geometry::Create2DShow<VertexPosTex>();

	// ���㻺��������
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(VertexPosTex) * (UINT)mesh.vertexVec.size();
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = mesh.vertexVec.data();

	hr = device->CreateBuffer(&vbd, &InitData, &m_pVertexBuffer);
	if (FAILED(hr))
		return hr;

	// ��������������
	m_IndexCount = (UINT)mesh.indexVec.size();

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(DWORD) * m_IndexCount;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.StructureByteStride = 0;
	ibd.MiscFlags = 0;

	InitData.pSysMem = mesh.indexVec.data();

	return device->CreateBuffer(&ibd, &InitData, &m_pIndexBuffer);
}