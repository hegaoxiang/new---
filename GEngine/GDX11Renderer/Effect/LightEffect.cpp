#include "Effects.h"
#include "DXOthers/d3dUtil.h"
#include "EffectHelper.h"	
#include "DXOthers/DXTrace.h"
#include "Graphics/Vertex.h"

using namespace DirectX;

class LightEffect::Impl :public  AlignedType<LightEffect::Impl>
{
public:
	struct MVP
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX proj;
	};
public:
	Impl() {};
	~Impl() = default;
	CBufferObject<0, MVP> m_pBuffer;

	ComPtr<ID3D11VertexShader> m_pVS;
	ComPtr<ID3D11PixelShader> m_pPS;

	ComPtr<ID3D11InputLayout> m_pPosInputLayout;

};


LightEffect::LightEffect()
{
	pImpl = std::make_unique<Impl>();
}
LightEffect::~LightEffect()
{

}

LightEffect& LightEffect::Get()
{
	static LightEffect* lightEffect = new LightEffect();
	return *lightEffect;
}

bool LightEffect::InitAll(ID3D11Device* device)
{
	if (device == nullptr)
		return false;

	if (!RenderStates::IsInit())
		RenderStates::InitAll(device);

	ComPtr<ID3DBlob> blob;
	// 创建顶点着色器
	HR(CreateShaderFromFile(L"HLSL\\VertexShader.cso", L"HLSL\\VertexShader.hlsl", "VS", "vs_5_0", blob.ReleaseAndGetAddressOf()));
	HR(device->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, pImpl->m_pVS.GetAddressOf()));
	// 创建顶点布局
	HR(device->CreateInputLayout(VertexPos::inputLayout, ARRAYSIZE(VertexPos::inputLayout),
		blob->GetBufferPointer(), blob->GetBufferSize(), pImpl->m_pPosInputLayout.GetAddressOf()));

	// 创建像素着色器
	HR(CreateShaderFromFile(L"HLSL\\PixelShader.cso", L"HLSL\\PixelShader.hlsl", "PS", "ps_5_0", blob.ReleaseAndGetAddressOf()));
	HR(device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, pImpl->m_pPS.GetAddressOf()));


	HR(pImpl->m_pBuffer.CreateBuffer(device));

	D3D11SetDebugObjectName(pImpl->m_pPosInputLayout.Get(), "VertexPosLayout");
	D3D11SetDebugObjectName(pImpl->m_pBuffer.cBuffer.Get(), "MVP");
	D3D11SetDebugObjectName(pImpl->m_pVS.Get(), "Light_VS");
	D3D11SetDebugObjectName(pImpl->m_pPS.Get(), "Light_PS");

	return true;
}

void LightEffect::SetRenderState(ID3D11DeviceContext* deviceContext)
{
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	deviceContext->IASetInputLayout(pImpl->m_pPosInputLayout.Get());
	deviceContext->VSSetShader(pImpl->m_pVS.Get(), nullptr, 0);
	deviceContext->GSSetShader(nullptr, nullptr, 0);
	deviceContext->RSSetState(nullptr);
	deviceContext->PSSetShader(pImpl->m_pPS.Get(), nullptr, 0);
	deviceContext->OMSetDepthStencilState(nullptr, 0);
	deviceContext->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);
}

void XM_CALLCONV LightEffect::SetWorldMatrix(DirectX::FXMMATRIX W)
{
	auto& cBuffer = pImpl->m_pBuffer;
	cBuffer.data.world = XMMatrixTranspose(W);
	cBuffer.isDirty = true;
}

void XM_CALLCONV LightEffect::SetViewMatrix(FXMMATRIX V)
{
	auto& cBuffer = pImpl->m_pBuffer;
	cBuffer.data.view = XMMatrixTranspose(V);
	cBuffer.isDirty = true;
}

void XM_CALLCONV LightEffect::SetProjMatrix(FXMMATRIX P)
{
	auto& cBuffer = pImpl->m_pBuffer;
	cBuffer.data.proj = XMMatrixTranspose(P);
	cBuffer.isDirty = true;
}

void LightEffect::Apply(ID3D11DeviceContext* deviceContext)
{
	pImpl->m_pBuffer.BindVS(deviceContext);

	pImpl->m_pBuffer.UpdateBuffer(deviceContext);

}
