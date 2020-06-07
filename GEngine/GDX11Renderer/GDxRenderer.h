#pragma once
#include "GRendererInfra/GRiRenderer.h"


#include <wrl/client.h>
#include <string>
#include <d3d11_1.h>
#include <DirectXMath.h>
#include "Graphics/Buffer.h"
#include <GEngine\GScene.h>

// �������Ҫ���õĿ�
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
	bool InitDirect3D();        // Direct3D��ʼ��

	bool InitEffect();

	void InitResource();
	
	void DrawSceneObject(const RenderLayer layout,bool bSetObjCb, bool bSetSubmeshCb,IEffect* effect);

	void DrawSceneObject(GRiSceneObject* sObject, bool bSetObjCb, bool bSetSubmeshCb, IEffect* effect);

protected:
protected:
	// ʹ��ģ�����(C++11)��������
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;
	// Direct3D 11
	ComPtr<ID3D11Device> m_pd3dDevice;                    // D3D11�豸
	ComPtr<ID3D11DeviceContext> m_pd3dImmediateContext;   // D3D11�豸������
	ComPtr<IDXGISwapChain> m_pSwapChain;                  // D3D11������
	// Direct3D 11.1
	ComPtr<ID3D11Device1> m_pd3dDevice1;                  // D3D11.1�豸
	ComPtr<ID3D11DeviceContext1> m_pd3dImmediateContext1; // D3D11.1�豸������
	ComPtr<IDXGISwapChain1> m_pSwapChain1;                // D3D11.1������

	// ������Դ
	ComPtr<ID3D11Texture2D> m_pDepthStencilBuffer;        // ���ģ�建����
	ComPtr<ID3D11RenderTargetView> m_pRenderTargetView;   // ��ȾĿ����ͼ
	ComPtr<ID3D11DepthStencilView> m_pDepthStencilView;   // ���ģ����ͼ
	D3D11_VIEWPORT m_ScreenViewport;                      // �ӿ�
private:
	GDxRenderer() {};
};

