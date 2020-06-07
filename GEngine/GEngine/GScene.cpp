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
	std::ofstream fout(name);// �Ѷ���д��file.txt�ļ���
	boost::archive::text_oarchive oa(fout); // �ı�������鵵�࣬ʹ��һ��ostream������

	oa << *this; // ����obj����
	fout.close();// �ر��ļ�

	return;

}

void GScene::AntiSerialize(const wchar_t* name)
{

	std::ifstream fin(name);

	if (!fin.is_open() || fin.eof())
	{
		return;
	}
	boost::archive::text_iarchive ia(fin); // �ı�������鵵��

	ia >> *this; // �ָ���newobj����

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


	// ���ö���/����������
	deviceContext->IASetVertexBuffers(0, 1, modelPart.vertexBuffer.GetBufferAddress(), &strides, &offsets);
	deviceContext->IASetIndexBuffer(modelPart.indexBuffer.GetBuffer(), modelPart.indexFormat, 0);

	// �������ݲ�Ӧ��
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