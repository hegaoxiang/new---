#pragma once
#include "GRendererInfra/GRiRenderer.h"


#include <wrl/client.h>
#include <string>
#include <d3d11_1.h>
#include <DirectXMath.h>
#include "Graphics/Buffer.h"
#include <GEngine\GScene.h>

// 添加所有要引用的库
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "D3DCompiler.lib")
#pragma comment(lib, "winmm.lib")

class IEffect;

class GDxRenderer :public GRiRenderer

{
public:
	static GDxRenderer& GetRenderer();


	virtual void Update(const float gt) override;


	virtual void Draw(const float gt) override;


	virtual void OnResize() override;

	virtual void Initialize() override;


	virtual void PreInitialize(HWND OutputWindow, double width, double height) override;


	virtual void CreateRendererFactory() override;

	
protected:
	bool InitDirect3D();        // Direct3D初始化

	bool InitEffect();

	void InitResource();
	
	void DrawSceneObject(const RenderLayer layout,bool bSetObjCb, bool bSetSubmeshCb,IEffect* effect);

	void DrawSceneObject(GRiSceneObject* sObject, bool bSetObjCb, bool bSetSubmeshCb, IEffect* effect);

protected:
protected:
	// 使用模板别名(C++11)简化类型名
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;
	// Direct3D 11
	ComPtr<ID3D11Device> m_pd3dDevice;                    // D3D11设备
	ComPtr<ID3D11DeviceContext> m_pd3dImmediateContext;   // D3D11设备上下文
	ComPtr<IDXGISwapChain> m_pSwapChain;                  // D3D11交换链
	// Direct3D 11.1
	ComPtr<ID3D11Device1> m_pd3dDevice1;                  // D3D11.1设备
	ComPtr<ID3D11DeviceContext1> m_pd3dImmediateContext1; // D3D11.1设备上下文
	ComPtr<IDXGISwapChain1> m_pSwapChain1;                // D3D11.1交换链

	// 常用资源
	ComPtr<ID3D11Texture2D> m_pDepthStencilBuffer;        // 深度模板缓冲区
	ComPtr<ID3D11RenderTargetView> m_pRenderTargetView;   // 渲染目标视图
	ComPtr<ID3D11DepthStencilView> m_pDepthStencilView;   // 深度模板视图
	D3D11_VIEWPORT m_ScreenViewport;                      // 视口
private:
	GDxRenderer() {};
};

