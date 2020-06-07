#include "GScene.h"

#include <fstream>
#include "DXOthers/TextureManage.h"
#include "GRendererInfra/GRiRenderer.h"
#include <GDX11Renderer\GDxTexture.h>

using namespace std;


namespace boost {
	namespace serialization {

		template<class Archive>
		void serialize(Archive& ar, GScene& s, const unsigned int version)
		{
			// serialize base class information
			ar& s.masks;
			ar& s.modelType;
			ar& s.names;
			ar& s.worldMats;
			ar& s.materials;
		}

		template<class Archive>
		void serialize(Archive& ar, GScene::Material& m, const unsigned int version)
		{
			// serialize base class information
			ar& m.diffuse;
		}

	} // namespace serialization
} // namespace boost

void GScene::Draw(GRiRenderer* test, ID3D11DeviceContext* deviceContext, BasicEffect& effect)const
{
	int i = 0;
	for (auto& item : masks)
	{
		if (item & COMPONENT_MODEL)
		{
			DrawItem(test,i, deviceContext, effect);
		}
		i++;
	}
}

void GScene::Serialize(const wchar_t* name)const
{
	std::ofstream fout(name);// 把对象写到file.txt文件中
	boost::archive::text_oarchive oa(fout); // 文本的输出归档类，使用一个ostream来构造

	oa << *this; // 保存obj对象
	fout.close();// 关闭文件

	return;

}

void GScene::AntiSerialize(const wchar_t* name)
{

	std::ifstream fin(name);

	if (!fin.is_open() || fin.eof())
	{
		return;
	}
	boost::archive::text_iarchive ia(fin); // 文本的输入归档类

	ia >> *this; // 恢复到newobj对象

	fin.close();

	
	return;
}

void GScene::DrawItem(GRiRenderer* test, int i, ID3D11DeviceContext* deviceContext, BasicEffect& effect)const
{
	auto& modelPart = modelParts[i];
	auto& worldMat = worldMats[i];
	auto& material = materials[i];

	UINT strides = modelPart.vertexStride;
	UINT offsets = 0;


	// 设置顶点/索引缓冲区
	deviceContext->IASetVertexBuffers(0, 1, modelPart.vertexBuffer.GetBufferAddress(), &strides, &offsets);
	deviceContext->IASetIndexBuffer(modelPart.indexBuffer.GetBuffer(), modelPart.indexFormat, 0);

	// 更新数据并应用
	/*effect.SetWorldMatrix(XMLoadFloat4x4(&worldMat));*/

	auto tex = test->pTextures.find(material.diffuse);
	if (tex!= test->pTextures.end())
	{
		auto t = test->pTextures[material.diffuse];
		GDxTexture* tt =(GDxTexture*) t;
		effect.SetTextureDiffuse(tt->mSRV.Get());
	}
	effect.SetWorldMatrix(worldMat);

	effect.Apply(deviceContext);

	deviceContext->DrawIndexed(modelPart.indexCount, 0, 0);
}