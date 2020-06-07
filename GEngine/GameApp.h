#ifndef GAMEAPP_H
#define GAMEAPP_H
#include "d3dApp.h"
#include "Graphics/TextureRender.h"
#include <memory>
#include "GRendererInfra/GRiCamera.h"
#include "GEngine\GScene.h"
/*#include "Graphics\SkyRender.h"*/

namespace RenderContext
{
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;
	// Direct3D 11
	static ComPtr<ID3D11Device> pd3dDevice;                    // D3D11设备
	static ComPtr<ID3D11DeviceContext> pd3dImmediateContext;   // D3D11设备上下文
	static ComPtr<IDXGISwapChain> pSwapChain;                  // D3D11交换链
}
class GameApp : public D3DApp
{
public:

	GameApp(HINSTANCE hInstance);
	~GameApp();

	bool Init();
	void OnResize();
	void UpdateScene(float dt);
	void DrawScene();

private:
	bool InitEffect();
	bool InitResource();

private:
	std::unique_ptr<TextureRender>	m_pGameContent;
	std::unique_ptr<TextureRender>	m_pRayTracingContent;
	std::shared_ptr<GScene>			m_pScene;
	
	

	std::shared_ptr<GRiCamera>			m_pCamera;
	CameraMode m_CameraMode;			// 摄像机模式

	//std::unique_ptr<SkyRender> m_SkyRender;


	// test

	ComPtr<ID3D11VertexShader> m_pVS;
	ComPtr<ID3D11PixelShader> m_pPS;

	ComPtr<ID3D11InputLayout> m_pPosInputLayout;
};


#endif