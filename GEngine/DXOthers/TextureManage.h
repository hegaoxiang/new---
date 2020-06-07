#pragma once
#include "DXOthers/d3dUtil.h"
#include <wrl/client.h>
#include <unordered_map>

class TextureManage
{
public:
	friend class Editor;

	static TextureManage& Get();

	bool LoadAllTexture(ID3D11Device* device);

	ID3D11ShaderResourceView* GetTexture(const std::string& texName);
private:
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	std::vector<std::string> m_TexNames;
	std::unordered_map<std::string, int> m_NameMap;
	std::vector<ComPtr<ID3D11ShaderResourceView>> m_Textures;
private:
	TextureManage() {};
};

