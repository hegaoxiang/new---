#include "GDxRenderer.h"
#include "GDxRendererFactory.h"
#include "Effect/Effects.h"
#include <GUI/GUI.h>
#include "Graphics/Geometry.h"
#include <DXOthers\GameObject.h>
#include "GUI/Editor.h"
#include "GDxMesh.h"
#include "GDxTexture.h"
using namespace DirectX;

GDxRenderer& GDxRenderer::GetRenderer()
{
	static GDxRenderer* instance = new GDxRenderer();
	return *instance;
}


void GDxRenderer::Initialize()
{
	if (!GUI::Get().Init(mhMainWnd, m_pd3dDevice.Get(), m_pd3dImmediateContext.Get()))
		return ;

	InitEffect();

	InitResource();
}

void GDxRenderer::PreInitialize(HWND OutputWindow, double width, double height)
{
	mhMainWnd = OutputWindow;
	mClientWidth = width;
	mClientHeight = height;

	if (!InitDirect3D())
		return;

	CreateRendererFactory();
}

void GDxRenderer::CreateRendererFactory()
{
	GDxRendererFactory fac(m_pd3dDevice1.Get(),m_pd3dImmediateContext.Get());

	mFactory = std::make_unique<GDxRendererFactory>(fac);
}

void GDxRenderer::Update(const float gt)
{
	BasicEffect::Get().SetViewMatrix(pCamera->GetViewXM());


#pragma region SkyPassCB
	auto cor = pCamera->GetCorner();
	auto view = pCamera->GetViewXM();
	auto corMat = XMLoadFloat4x4(&cor);
	auto inView = XMMatrixInverse(nullptr, view);
	auto corDirMat = corMat * inView;
	XMFLOAT4X4 corDir;
	XMStoreFloat4x4(&corDir, corDirMat);

	SkyEffect::Get().SetCorners(corDir);
#pragma endregion SkyPassCB
	
}

void GDxRenderer::Draw(const float gt)
{
	assert(m_pd3dImmediateContext);
	assert(m_pSwapChain);
	static float blue[4] = { 0.0f, 0.0f, 1.0f, 1.0f };	// RGBA = (0,0,255,255)


	m_pd3dImmediateContext->OMSetRenderTargets(1, m_pRenderTargetView.GetAddressOf(), m_pDepthStencilView.Get());
	m_pd3dImmediateContext->ClearRenderTargetView(m_pRenderTargetView.Get(), blue);
	m_pd3dImmediateContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	BasicEffect::Get().SetRenderDefault(m_pd3dImmediateContext.Get());
	DrawSceneObject(RenderLayer::Default, true, true, &BasicEffect::Get());

	SkyEffect::Get().SetRenderDefault(m_pd3dImmediateContext.Get());
	DrawSceneObject(RenderLayer::Sky, false, false, &SkyEffect::Get());

	{
		GUI::Get().BeginGUI();


#ifdef USE_IMGUI
		ImGui::Begin("1", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize);
		auto& io = ImGui::GetIO();
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
		ImGui::End();
#endif

		GUI::Get().EndGUI();

		GUI::Get().Render();
	}

	HR(m_pSwapChain->Present(1, 0));
}

void GDxRenderer::OnResize()
{
	#pragma region D3DResize
    assert(m_pd3dImmediateContext);
	assert(m_pd3dDevice);
	assert(m_pSwapChain);

	if (m_pd3dDevice1 != nullptr)
	{
		assert(m_pd3dImmediateContext1);
		assert(m_pd3dDevice1);
		assert(m_pSwapChain1);
	}

	// 释放渲染管线输出用到的相关资源
	m_pRenderTargetView.Reset();
	m_pDepthStencilView.Reset();
	m_pDepthStencilBuffer.Reset();

	// 重设交换链并且重新创建渲染目标视图
	ComPtr<ID3D11Texture2D> backBuffer;
	HR(m_pSwapChain->ResizeBuffers(1, mClientWidth, mClientHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0));
	HR(m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf())));
	HR(m_pd3dDevice->CreateRenderTargetView(backBuffer.Get(), nullptr, m_pRenderTargetView.GetAddressOf()));

	// 设置调试对象名
	D3D11SetDebugObjectName(backBuffer.Get(), "BackBuffer[0]");

	backBuffer.Reset();


	D3D11_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.Width = mClientWidth;
	depthStencilDesc.Height = mClientHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	// 要使用 4X MSAA? --需要给交换链设置MASS参数

	{
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
	}



	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	// 创建深度缓冲区以及深度模板视图
	HR(m_pd3dDevice->CreateTexture2D(&depthStencilDesc, nullptr, m_pDepthStencilBuffer.GetAddressOf()));
	HR(m_pd3dDevice->CreateDepthStencilView(m_pDepthStencilBuffer.Get(), nullptr, m_pDepthStencilView.GetAddressOf()));


	// 将渲染目标视图和深度/模板缓冲区结合到管线
	m_pd3dImmediateContext->OMSetRenderTargets(1, m_pRenderTargetView.GetAddressOf(), m_pDepthStencilView.Get());

	// 设置视口变换
	m_ScreenViewport.TopLeftX = 0;
	m_ScreenViewport.TopLeftY = 0;
	m_ScreenViewport.Width = static_cast<float>(mClientWidth);
	m_ScreenViewport.Height = static_cast<float>(mClientHeight);
	m_ScreenViewport.MinDepth = 0.0f;
	m_ScreenViewport.MaxDepth = 1.0f;

	m_pd3dImmediateContext->RSSetViewports(1, &m_ScreenViewport);

	// 设置调试对象名
	D3D11SetDebugObjectName(m_pDepthStencilBuffer.Get(), "DepthStencilBuffer");
	D3D11SetDebugObjectName(m_pDepthStencilView.Get(), "DepthStencilView");
	D3D11SetDebugObjectName(m_pRenderTargetView.Get(), "BackBufferRTV[0]");
#pragma endregion D3DResize

	if (pCamera != nullptr && BasicEffect::Get().InitAll(m_pd3dDevice.Get()))
	{
		BasicEffect::Get().SetProjMatrix(pCamera->GetProjXM());
	}
}

bool GDxRenderer::InitDirect3D()
{
	HRESULT hr = S_OK;

	// 创建D3D设备 和 D3D设备上下文
	UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)  
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	// 驱动类型数组
	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};
	UINT numDriverTypes = ARRAYSIZE(driverTypes);

	// 特性等级数组
	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	D3D_FEATURE_LEVEL featureLevel;
	D3D_DRIVER_TYPE d3dDriverType;
	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
	{
		d3dDriverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDevice(nullptr, d3dDriverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
			D3D11_SDK_VERSION, m_pd3dDevice.GetAddressOf(), &featureLevel, m_pd3dImmediateContext.GetAddressOf());

		if (hr == E_INVALIDARG)
		{
			// Direct3D 11.0 的API不承认D3D_FEATURE_LEVEL_11_1，所以我们需要尝试特性等级11.0以及以下的版本
			hr = D3D11CreateDevice(nullptr, d3dDriverType, nullptr, createDeviceFlags, &featureLevels[1], numFeatureLevels - 1,
				D3D11_SDK_VERSION, m_pd3dDevice.GetAddressOf(), &featureLevel, m_pd3dImmediateContext.GetAddressOf());
		}

		if (SUCCEEDED(hr))
			break;
	}

	if (FAILED(hr))
	{
		MessageBox(0, L"D3D11CreateDevice Failed.", 0, 0);
		return false;
	}

	// 检测是否支持特性等级11.0或11.1
	if (featureLevel != D3D_FEATURE_LEVEL_11_0 && featureLevel != D3D_FEATURE_LEVEL_11_1)
	{
		MessageBox(0, L"Direct3D Feature Level 11 unsupported.", 0, 0);
		return false;
	}


	ComPtr<IDXGIDevice> dxgiDevice = nullptr;
	ComPtr<IDXGIAdapter> dxgiAdapter = nullptr;
	ComPtr<IDXGIFactory1> dxgiFactory1 = nullptr;	// D3D11.0(包含DXGI1.1)的接口类
	ComPtr<IDXGIFactory2> dxgiFactory2 = nullptr;	// D3D11.1(包含DXGI1.2)特有的接口类

	// 为了正确创建 DXGI交换链，首先我们需要获取创建 D3D设备 的 DXGI工厂，否则会引发报错：
	// "IDXGIFactory::CreateSwapChain: This function is being called with a device from a different IDXGIFactory."
	HR(m_pd3dDevice.As(&dxgiDevice));
	HR(dxgiDevice->GetAdapter(dxgiAdapter.GetAddressOf()));
	HR(dxgiAdapter->GetParent(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(dxgiFactory1.GetAddressOf())));

	// 查看该对象是否包含IDXGIFactory2接口
	hr = dxgiFactory1.As(&dxgiFactory2);
	// 如果包含，则说明支持D3D11.1
	if (dxgiFactory2 != nullptr)
	{
		HR(m_pd3dDevice.As(&m_pd3dDevice1));
		HR(m_pd3dImmediateContext.As(&m_pd3dImmediateContext1));
		// 填充各种结构体用以描述交换链
		DXGI_SWAP_CHAIN_DESC1 sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.Width = mClientWidth;
		sd.Height = mClientHeight;
		sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		// 是否开启4倍多重采样？
		{
			sd.SampleDesc.Count = 1;
			sd.SampleDesc.Quality = 0;
		}
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.BufferCount = 2;
		sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		DXGI_SWAP_CHAIN_FULLSCREEN_DESC fd;
		fd.RefreshRate.Numerator = 60;
		fd.RefreshRate.Denominator = 1;
		fd.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		fd.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		fd.Windowed = TRUE;
		// 为当前窗口创建交换链
		HR(dxgiFactory2->CreateSwapChainForHwnd(m_pd3dDevice.Get(), mhMainWnd, &sd, &fd, nullptr, m_pSwapChain1.GetAddressOf()));
		HR(m_pSwapChain1.As(&m_pSwapChain));
	}
	else
	{
		// 填充DXGI_SWAP_CHAIN_DESC用以描述交换链
		DXGI_SWAP_CHAIN_DESC sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.BufferDesc.Width = mClientWidth;
		sd.BufferDesc.Height = mClientHeight;
		sd.BufferDesc.RefreshRate.Numerator = 60;
		sd.BufferDesc.RefreshRate.Denominator = 1;
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		{
			sd.SampleDesc.Count = 1;
			sd.SampleDesc.Quality = 0;
		}
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.BufferCount = 1;
		sd.OutputWindow = mhMainWnd;
		sd.Windowed = TRUE;
		sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		sd.Flags = 0;
		HR(dxgiFactory1->CreateSwapChain(m_pd3dDevice.Get(), &sd, m_pSwapChain.GetAddressOf()));
	}



	// 可以禁止alt+enter全屏
	dxgiFactory1->MakeWindowAssociation(mhMainWnd, DXGI_MWA_NO_ALT_ENTER | DXGI_MWA_NO_WINDOW_CHANGES);

	// 设置调试对象名
	D3D11SetDebugObjectName(m_pd3dImmediateContext.Get(), "ImmediateContext");
	DXGISetDebugObjectName(m_pSwapChain.Get(), "SwapChain");

	// 每当窗口被重新调整大小的时候，都需要调用这个OnResize函数。现在调用
	// 以避免代码重复
	OnResize();

	return true;
}

bool GDxRenderer::InitEffect()
{
	BasicEffect::Get().InitAll(m_pd3dDevice.Get());

	SkyEffect::Get().InitAll(m_pd3dDevice.Get());

	LightEffect::Get().InitAll(m_pd3dDevice.Get());

	return true;
}

void GDxRenderer::InitResource()
{

	BasicEffect::Get().SetProjMatrix(pCamera->GetProjXM());

	Editor::Get().SetScene(pScene, m_pd3dDevice.Get());

	auto tex = pTextures["SkyCube"];
	GDxTexture* t = dynamic_cast<GDxTexture*>(tex);

	SkyEffect::Get().SetTextureCube(t->mSRV.Get());

	mIsRunning = true;
}

void GDxRenderer::DrawSceneObject(const RenderLayer layout, bool bSetObjCb, bool bSetSubmeshCb, IEffect* effect)
{
	for (auto& sobj : pSceneObjectLayer[(int)layout])
	{
		DrawSceneObject(sobj, bSetObjCb, bSetSubmeshCb,effect);
	}
}

void GDxRenderer::DrawSceneObject(GRiSceneObject* so, bool bSetObjCb, bool bSetSubmeshCb, IEffect* effect)
{

	GDxMesh* dxMesh = dynamic_cast<GDxMesh*>(so->Mesh);

	HR(dxMesh != nullptr);
	UINT strides = dxMesh->mVIBuffer->VertexByteStride;
	UINT offsets = 0;

	if (bSetObjCb)
	{
		ISetObjEffect* e = dynamic_cast<ISetObjEffect*>(effect);
		// 更新数据并应用
		e->SetWorldMatrix(XMLoadFloat4x4(&so->World));
	}

	// 设置顶点/索引缓冲区
	m_pd3dImmediateContext->IASetVertexBuffers(0, 1, dxMesh->mVIBuffer->VBuffer.GetBufferAddress(), &strides, &offsets);
	m_pd3dImmediateContext->IASetIndexBuffer(dxMesh->mVIBuffer->IBuffer.GetBuffer(), dxMesh->mVIBuffer->IndexFormat, 0);

	for (auto& part : dxMesh->Submeshes)
	{
		auto& submesh = part.second;
		if (bSetSubmeshCb)
		{
			auto texName = submesh.GetMaterial()->pTextures[0];
			auto tex = pTextures[texName];

			GDxTexture* texSrv = dynamic_cast<GDxTexture*>(tex);
			ISetSubMeshEffect* e = dynamic_cast<ISetSubMeshEffect*>(effect);
			e->SetTextureDiffuse(texSrv->mSRV.Get());
		}
	

		effect->Apply(m_pd3dImmediateContext.Get());

		m_pd3dImmediateContext->DrawIndexed(submesh.IndexCount, submesh.StartIndexLocation, submesh.BaseVertexLocation);
	}
}

