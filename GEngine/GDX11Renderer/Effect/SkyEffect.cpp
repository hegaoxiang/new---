#include "Effects.h"
#include "DXOthers/d3dUtil.h"
#include "EffectHelper.h"	// ��������Effects.h��d3dUtil.h����
#include "DXOthers/DXTrace.h"
#include "Graphics/Vertex.h"
using namespace DirectX;


//
// SkyEffect::Impl ��Ҫ����SkyEffect�Ķ���
//

class SkyEffect::Impl : public AlignedType<SkyEffect::Impl>
{
public:
	//
	// ��Щ�ṹ���ӦHLSL�Ľṹ�塣��Ҫ��16�ֽڶ���
	//
	struct CBChangesEveryFrame
	{
		DirectX::XMMATRIX Corners;
	};

public:
	// ������ʽָ��
	Impl() : m_IsDirty() {}
	~Impl() = default;

public:
	CBufferObject<1, CBChangesEveryFrame>	m_CBFrame;	        // ÿ֡���Ƶĳ���������

	BOOL m_IsDirty;										        // �Ƿ���ֵ���
	std::vector<CBufferBase*> m_pCBuffers;				        // ͳһ�����������еĳ���������

	ComPtr<ID3D11VertexShader> m_pSkyVS;
	ComPtr<ID3D11PixelShader> m_pSkyPS;

	ComPtr<ID3D11InputLayout> m_pVertexPosLayout;

	ComPtr<ID3D11ShaderResourceView> m_pTextureCube;			// ��պ�����
};

//
// SkyEffect
//

namespace
{
	// SkyEffect����
	static SkyEffect* g_pInstance = nullptr;
}

SkyEffect::SkyEffect()
{
	if (g_pInstance)
		throw std::exception("SkyEffect is a singleton!");
	g_pInstance = this;
	pImpl = std::make_unique<SkyEffect::Impl>();
}

SkyEffect::~SkyEffect()
{
}

SkyEffect::SkyEffect(SkyEffect&& moveFrom) noexcept
{
	pImpl.swap(moveFrom.pImpl);
}

SkyEffect& SkyEffect::operator=(SkyEffect&& moveFrom) noexcept
{
	pImpl.swap(moveFrom.pImpl);
	return *this;
}

SkyEffect& SkyEffect::Get()
{
	if (!g_pInstance)
	{
		new SkyEffect();
	}
	return *g_pInstance;
}

bool SkyEffect::InitAll(ID3D11Device* device)
{
	if (!device)
		return false;

	if (!pImpl->m_pCBuffers.empty())
		return true;

	if (!RenderStates::IsInit())
	{
		RenderStates::InitAll(device);
	}

	ComPtr<ID3DBlob> blob;

	// ******************
	// ����������ɫ��
	//

	HR(CreateShaderFromFile(L"HLSL\\Sky_VS.cso", L"HLSL\\Sky_VS.hlsl", "VS", "vs_5_0", blob.ReleaseAndGetAddressOf()));
	HR(device->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, pImpl->m_pSkyVS.GetAddressOf()));
	// �������㲼��
	HR(device->CreateInputLayout(VertexPosNormalTangentTex::inputLayout, ARRAYSIZE(VertexPosNormalTangentTex::inputLayout),
		blob->GetBufferPointer(), blob->GetBufferSize(), pImpl->m_pVertexPosLayout.GetAddressOf()));

	// ******************
	// ����������ɫ��
	//

	HR(CreateShaderFromFile(L"HLSL\\Sky_PS.cso", L"HLSL\\Sky_PS.hlsl", "PS", "ps_5_0", blob.ReleaseAndGetAddressOf()));
	HR(device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, pImpl->m_pSkyPS.GetAddressOf()));


	pImpl->m_pCBuffers.assign({
		&pImpl->m_CBFrame,
		});

	// ��������������
	for (auto& pBuffer : pImpl->m_pCBuffers)
	{
		HR(pBuffer->CreateBuffer(device));
	}

	// ���õ��Զ�����
	D3D11SetDebugObjectName(pImpl->m_pVertexPosLayout.Get(), "SkyEffect.VertexPosTexLayout");
	D3D11SetDebugObjectName(pImpl->m_pCBuffers[0]->cBuffer.Get(), "SkyEffect.CBFrame");
	D3D11SetDebugObjectName(pImpl->m_pSkyVS.Get(), "SkyEffect.Sky_VS");
	D3D11SetDebugObjectName(pImpl->m_pSkyPS.Get(), "SkyEffect.Sky_PS");

	return true;
}

void SkyEffect::SetRenderDefault(ID3D11DeviceContext* deviceContext)
{
	deviceContext->IASetInputLayout(pImpl->m_pVertexPosLayout.Get());
	deviceContext->VSSetShader(pImpl->m_pSkyVS.Get(), nullptr, 0);
	deviceContext->PSSetShader(pImpl->m_pSkyPS.Get(), nullptr, 0);

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	deviceContext->RSSetState(nullptr);

	deviceContext->PSSetSamplers(0, 1, RenderStates::SSLinearWrap.GetAddressOf());
	deviceContext->OMSetDepthStencilState(RenderStates::DSSLessEqual.Get(), 0);
	deviceContext->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);
}

void SkyEffect::SetCorners(DirectX::XMFLOAT4X4 corners)
{
	auto& cBuffer = pImpl->m_CBFrame;
	cBuffer.data.Corners = XMLoadFloat4x4(&corners);
	pImpl->m_IsDirty = cBuffer.isDirty = true;
}

void SkyEffect::SetTextureCube(ID3D11ShaderResourceView* m_pTextureCube)
{
	pImpl->m_pTextureCube = m_pTextureCube;
}

void SkyEffect::Apply(ID3D11DeviceContext* deviceContext)
{
	auto& pCBuffers = pImpl->m_pCBuffers;
	// ���������󶨵���Ⱦ������
	pCBuffers[0]->BindVS(deviceContext);
		// ����SRV
	deviceContext->PSSetShaderResources(0, 1, pImpl->m_pTextureCube.GetAddressOf());

	if (pImpl->m_IsDirty)
	{
		pImpl->m_IsDirty = false;
		for (auto& pCBuffer : pCBuffers)
		{
			pCBuffer->UpdateBuffer(deviceContext);
		}
	}
}

