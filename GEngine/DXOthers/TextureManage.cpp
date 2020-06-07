#include "TextureManage.h"
#include "DXOthers/DXTrace.h"
#include "DXOthers/DDSTextureLoader.h"
#include <filesystem>
#include <string>

using namespace std;
using namespace std::filesystem;

using namespace DirectX;
using namespace Microsoft::WRL;

const size_t MAXTEX = 100;
TextureManage& TextureManage::Get()
{
	static TextureManage* g_TexManager = new TextureManage();

	g_TexManager->m_Textures.resize(MAXTEX);
	return *g_TexManager;
}

bool TextureManage::LoadAllTexture(ID3D11Device* device)
{
	HR(device != nullptr);

	path str("Texture");
	if (!exists(str))		//必须先检测目录是否存在才能使用文件入口.
		return 1;
	directory_entry entry(str);		//文件入口
	if (entry.status().type() == file_type::directory)	//这里用了C++11的强枚举类型
		;// cout << "该路径是一个目录" << endl;
	directory_iterator list(str);	        //文件入口容器

	int i = 0;
	for (auto& it : list)
	{
		wstring file(L"Texture\\");
		file+= it.path().filename().generic_wstring();

		m_TexNames.emplace_back(it.path().stem().generic_string());
		m_NameMap.insert(std::pair<string, int>(m_TexNames[i], i));

		// only dds has the s in the last location
		if (file[file.size() - 1] == 's')
		{
			HR(CreateDDSTextureFromFile(device, file.c_str(), nullptr, m_Textures[i++].GetAddressOf()));
		}
		else
		{
			HR(CreateWICTextureFromFile(device, file.c_str(), nullptr, m_Textures[i++].GetAddressOf()));
		}
	}
	

	return true;
}

ID3D11ShaderResourceView* TextureManage::GetTexture(const std::string& texName)
{
	auto iter = m_NameMap.find(texName);

	if (iter == m_NameMap.end())
	{
		return nullptr;
	}

	return m_Textures[iter->second].Get();
}
