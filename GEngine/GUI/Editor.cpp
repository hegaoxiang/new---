#include "Editor.h"
#include <IMGUI/imgui_stdlib.h>
#include <IMGUI/ImGuizmo.h>
#include "GRendererInfra/GRiCamera.h"
#include "DXOthers/TextureManage.h"

#define TRACE(...) GUI::Get().AddLog(__VA_ARGS__ + "\n")


Editor& Editor::Get()
{
	static Editor* impl = new Editor();
	return *impl;
}

void Editor::OnGUI(ID3D11Device* device, ID3D11ShaderResourceView* gameContent, const GRiCamera& GRiCamera)
{
	// need to choose a GScene first 
	if (!m_pScene)
		return;
	
	ShowGame(gameContent,GRiCamera);

	if (m_ShowHierarchy)
		ShowHierarchy();

	if (m_ShowInspector)
		ShowInspector();

	if (HasComponent(COMPONENT_TRANSFORM))
		ShowTransForm();

	if (HasComponent(COMPONENT_MODEL))
		ShowModel();

}

bool Editor::IsGameWindowResize()
{
	auto ans = m_GWSizeChange;

	m_GWSizeChange = false;
	return  ans;
}

std::tuple<float, float> Editor::GetWindowSize()
{
	return { m_GameSize._Myfirst._Val,m_GameSize._Get_rest()._Myfirst._Val };
}

void Editor::RayCheck(const GRiCamera& GRiCamera)
{
	auto& io = ImGui::GetIO();

	// cause start pos is the left_right corner of the game window
	auto location = { io.MousePos.x - get<0>(m_StartPos),io.MousePos.x - get<1>(m_StartPos) };

	GRiCamera.GetCorner();
// 	for ()
// 	{
// 	}
}

void Editor::ShowInspector()
{
	ImGui::Begin("Inspector");

	if (SelectedID == -1)
	{
		ImGui::End();
		return;
	}

	auto& name = m_pScene->names[SelectedID];
	auto& mask = m_pScene->masks[SelectedID];

	bool toggles[] = { mask & COMPONENT_TRANSFORM,mask&COMPONENT_MODEL, mask & COMPONENT_ROTATE };

	if (ImGui::Button("Components"))
	{
		ImGui::OpenPopup("_Comp");
	}
	if (ImGui::BeginPopup("_Comp"))
	{
		UINT totalComp = NameMap.size();

		UINT k = 1;
		for (UINT i = 0,j = k; i < totalComp; i++)
		{
			j = k << i;
			ImGui::MenuItem(NameMap[(Component)j].c_str(), "", &toggles[i]);
		}

		k = 1;
		for (UINT i = 0, j = k; i < totalComp; i++)
		{
			j =k << i;
			// 如果本身不包含组件，才添加
			// 或者本身包含组件，才移除
			if (toggles[i] && !(mask & j))
				AddComponent((Component)j);
			else if(!toggles[i] && mask & j)
				RemoveComponent((Component)j);
		}
		ImGui::EndPopup();
	}
	ImGui::InputText("Name", &name);

	ImGui::End();
}

void Editor::ShowHierarchy()
{
	auto& names = m_pScene->names;

	ImGui::Begin("Hierarchy", nullptr, ImGuiWindowFlags_MenuBar);
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Add"))
		{
			if (ImGui::MenuItem("Sphere"))
			{
				CreateOBJ(Shape::SPHER);
			}
			if (ImGui::MenuItem("Box"))
			{
				CreateOBJ(Shape::BOX);
			}
			if (ImGui::MenuItem("Cylinder"))
			{
				CreateOBJ(Shape::CYLINDER);
			}
			if (ImGui::MenuItem("Plane"))
			{
				CreateOBJ(Shape::PLANE);
			}
			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}
	UINT i = 0;
	for (auto& item : names)
	{
		if (ImGui::Selectable(item.c_str(), SelectedID == i))
			SelectedID = i;
		i++;
	}

	ImGui::End();
}


void Editor::ShowTransForm()
{
	auto& worldMats = m_pScene->worldMats;

	ImGui::Begin("TransForm");

	ImGui::Text("transform");
	ImGui::NewLine();

	static float translation[3];
	static float rotation[3];
	static float scale[3];

	float mat[16];
	ImGuizmo::DecomposeMatrixToComponents(worldMats[SelectedID].data(), translation, rotation, scale);
	ImGui::InputFloat3("Tr", translation, 3);
	ImGui::InputFloat3("Rt", rotation, 3);
	ImGui::InputFloat3("Sc", scale, 3);
	ImGuizmo::RecomposeMatrixFromComponents(translation, rotation, scale, mat);

	for (size_t i = 0; i < 16; i++)
	{
		worldMats[SelectedID][i] = mat[i];
	}

	ImGui::End();
}

void Editor::ShowModel()
{
	ImGui::Begin("Model");

	ImGui::Text("Model");
	
	auto& material = m_pScene->materials[SelectedID];

	int i = m_pScene->modelType[SelectedID];
	ImGui::Combo("Shape", &i, "SPHER\0BOX\0CYLINDER\0PLANE\0OTHER\0");
	if (i != m_pScene->modelType[SelectedID])
	{
		m_pScene->modelType[SelectedID] = UINT(i);

		InitModelData();
	}
	
	ImGui::Text("Diffuse Texture");

	if (ImGui::ImageButton(TextureManage::Get().GetTexture(material.diffuse), ImVec2(ImGui::GetContentRegionAvail().x / 2, ImGui::GetContentRegionAvail().y / 2)))
	{
		ImGui::OpenPopup("ChooseTexture");
	}

	static int columns_count = 4;
	ImGui::SetNextItemWidth(ImGui::GetFontSize() * 8);

	if (ImGui::BeginPopupModal("ChooseTexture", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::DragInt("##columns_count", &columns_count, 0.1f, 2, 10, "%d columns");
		ImGui::Columns(columns_count);
		
		auto texNum = TextureManage::Get().m_TexNames.size();
		for (int i = 0; i < texNum; i++)
		{
			if (ImGui::ImageButton(TextureManage::Get().m_Textures[i].Get(), ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().x)))
			{
				material.diffuse = TextureManage::Get().m_TexNames[i];

				ImGui::CloseCurrentPopup();
			}
			ImGui::NextColumn();
		}
		ImGui::Columns();
		if (ImGui::Button("OK", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
		ImGui::EndPopup();
	}
	ImGui::End();
}

void EditTransform(const float* cameraView, const float* cameraProjection, float* matrix,ImVec2 startPos, ImVec2 size,ImGuizmo::OPERATION mCurrentGizmoOperation)
{
	//static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);
	static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::LOCAL);
	static bool useSnap = false;
	static float snap[3] = { 1.f, 1.f, 1.f };
	static float bounds[] = { -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f };
	static float boundsSnap[] = { 0.1f, 0.1f, 0.1f };
	static bool boundSizing = false;
	static bool boundSizingSnap = false;

	// 	float matrixTranslation[3], matrixRotation[3], matrixScale[3];
	// 	ImGuizmo::DecomposeMatrixToComponents(matrix, matrixTranslation, matrixRotation, matrixScale);
	// 	ImGui::InputFloat3("Tr", matrixTranslation, 3);
	// 	ImGui::InputFloat3("Rt", matrixRotation, 3);
	// 	ImGui::InputFloat3("Sc", matrixScale, 3);
	// 	ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, matrix);

	// 	if (mCurrentGizmoOperation != ImGuizmo::SCALE)
	// 	{
	// 		if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
	// 			mCurrentGizmoMode = ImGuizmo::LOCAL;
	// 		ImGui::SameLine();
	// 		if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
	// 			mCurrentGizmoMode = ImGuizmo::WORLD;
	// 	}
	// 	if (ImGui::IsKeyPressed(83))
	// 		useSnap = !useSnap;
	// 	ImGui::Checkbox("", &useSnap);
	// 	ImGui::SameLine();

	// 	switch (mCurrentGizmoOperation)
	// 	{
	// 	case ImGuizmo::TRANSLATE:
	// 		ImGui::InputFloat3("Snap", &snap[0]);
	// 		break;
	// 	case ImGuizmo::ROTATE:
	// 		ImGui::InputFloat("Angle Snap", &snap[0]);
	// 		break;
	// 	case ImGuizmo::SCALE:
	// 		ImGui::InputFloat("Scale Snap", &snap[0]);
	// 		break;
	// 	}
	// 	ImGui::Checkbox("Bound Sizing", &boundSizing);
	// 	if (boundSizing)
	// 	{
	// 		ImGui::PushID(3);
	// 		ImGui::Checkbox("", &boundSizingSnap);
	// 		ImGui::SameLine();
	// 		ImGui::InputFloat3("Snap", boundsSnap);
	// 		ImGui::PopID();
	// 	}

		// 微调一下显示框
	ImGuizmo::SetRect(startPos.x, startPos.y , size.x , size.y );
	ImGuizmo::Manipulate(cameraView, cameraProjection, mCurrentGizmoOperation, mCurrentGizmoMode, matrix, NULL, useSnap ? &snap[0] : NULL, boundSizing ? bounds : NULL, boundSizingSnap ? boundsSnap : NULL);
}


void Editor::ShowGame(ID3D11ShaderResourceView* gameContent, const GRiCamera& GRiCamera)
{
	ImGui::Begin("Game");

	static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);
	if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
		mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
		mCurrentGizmoOperation = ImGuizmo::ROTATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
		mCurrentGizmoOperation = ImGuizmo::SCALE;

	static auto size = ImGui::GetContentRegionAvail();
	
	if ((size.x != ImGui::GetContentRegionAvail().x) || (size.y != ImGui::GetContentRegionAvail().y))
	{
		m_GWSizeChange = true;
		size = ImGui::GetContentRegionAvail();
		m_GameSize = {size.x, size.y};
		
	}

	auto imageStartPos = ImVec2{ ImGui::GetWindowPos().x + ImGui::GetCursorPos().x,ImGui::GetWindowPos().y + ImGui::GetCursorPos().y };
	ImGui::Image(gameContent,size);

	XMFLOAT4X4 t1;
	XMStoreFloat4x4(&t1, GRiCamera.GetViewXM());
	XMFLOAT4X4 t2;
	XMStoreFloat4x4(&t2, GRiCamera.GetProjXM());

	if (SelectedID == -1)
	{
		return ImGui::End();
	}
	auto t = m_pScene->worldMats[SelectedID].data();
// 
// 	TRACE("winposSize x:" + to_string(ImGui::GetWindowSize().x));
// 	TRACE("winposSize y:" + to_string(ImGui::GetWindowSize().y));
// 	TRACE("winpos x:" + to_string(ImGui::GetWindowPos().x));
// 	TRACE("winpos y:" + to_string(ImGui::GetWindowPos().y));
// 	TRACE("before curpos x:" + to_string(ImGui::GetCursorPos().x));
// 	TRACE("before curpos y:" + to_string(ImGui::GetCursorPos().y));
// 	
// 	TRACE("mousepos x:" + to_string(ImGui::GetMousePos().x));
// 	TRACE("mousepos y:" + to_string(ImGui::GetMousePos().y));
// 	TRACE("size x:" + to_string(ImGui::GetContentRegionAvail().x));
// 	TRACE("size y:" + to_string(ImGui::GetContentRegionAvail().y));

	EditTransform((float*)t1.m, (float*)t2.m, t, imageStartPos, size, mCurrentGizmoOperation);

	m_StartPos = { imageStartPos.x,imageStartPos.y };

	ImGui::End();	
}

void Editor::OnAddComponent(Component c)
{
	HR(SelectedID != -1);
	switch (c)
	{
	case COMPONENT_NONE:
		break;
	case COMPONENT_TRANSFORM:
		{
			array<float, 16>m16 = {
				1,0,0,0,
				0,1,0,0,
				0,0,1,0,
				0,0,0,1
			};
			m_pScene->worldMats[SelectedID] = m16;
		}
		break;
	case COMPONENT_MODEL:
	{
		m_pScene->modelType[SelectedID] = 0;
		InitModelData();
	}
		break;
	case COMPONENT_ROTATE:
		break;
	default:
		break;
	}
}

void Editor::InitModelData()
{
	Shape p = (Shape)m_pScene->modelType[SelectedID];
	switch (p)
	{
	case Shape::SPHER:
		SetMesh(m_pd3dDevice.Get(), Geometry::CreateSphere());
		break;
	case Shape::BOX:
		SetMesh(m_pd3dDevice.Get(), Geometry::CreateBox());
		break;
	case Shape::CYLINDER:
		SetMesh(m_pd3dDevice.Get(), Geometry::CreateCylinder());
		break;
	case Shape::PLANE:
		SetMesh(m_pd3dDevice.Get(), Geometry::CreatePlane());
		break;
	case Shape::OTHER:
		// todo
		RemoveComponent(COMPONENT_MODEL);
		break;
	default:
		break;
	}

}

void Editor::InitAdditionData()
{
	auto lastSelectedID = SelectedID;
	for (size_t i = 0; i < m_pScene->Num; i++)
	{
		if (m_pScene->masks[i] & COMPONENT_MODEL)
		{
			SelectedID = i;
			InitModelData();
		}
	}

	 SelectedID = lastSelectedID;
}

void Editor::SetMesh( ID3D11Device* device, const void* vertices, UINT vertexSize, UINT vertexCount, const void* indices, UINT indexCount, DXGI_FORMAT indexFormat)
{
	HR(SelectedID != -1);
	auto& m_Model = m_pScene->modelParts[SelectedID];
	m_Model.vertexStride = vertexSize;

	m_Model.vertexCount = vertexCount;
	m_Model.indexCount = indexCount;
	m_Model.indexFormat = indexFormat;

	m_Model.vertexBuffer.Create(device, vertices, vertexSize, vertexCount);

	m_Model.indexBuffer.Create(device, indices, indexCount, indexFormat);
}

void Editor::CreateEnity()
{
	for (size_t i = 0; i < m_pScene->Num; i++)
	{
		if (m_pScene->masks[i] == COMPONENT_NONE)
		{
			SelectedID = i;
			return;
		}
	}
}

void Editor::CreateOBJ(Shape type)
{
	CreateEnity();

	// Name
	m_pScene->names[SelectedID] = "Obj" + to_string(SelectedID);
	// Transform
	AddComponent(COMPONENT_TRANSFORM);

	m_pScene->worldMats[SelectedID][0] = 1;
	m_pScene->worldMats[SelectedID][5] = 1;
	m_pScene->worldMats[SelectedID][10] = 1;
	m_pScene->worldMats[SelectedID][15] = 1;
	// Model
	AddComponent(COMPONENT_MODEL);
	m_pScene->modelType[SelectedID] = (UINT)type;

	switch (type)
	{
	case Shape::OTHER:
		// TODO load model
		// temporary solution
		RemoveComponent(COMPONENT_MODEL);
		break;
	case Shape::SPHER:
		SetMesh(m_pd3dDevice.Get(), Geometry::CreateSphere());
		break;
	case Shape::BOX:
		SetMesh(m_pd3dDevice.Get(), Geometry::CreateBox());
		break;
	case Shape::CYLINDER:
		SetMesh(m_pd3dDevice.Get(), Geometry::CreateCylinder());
		break;
	case Shape::PLANE:
		SetMesh(m_pd3dDevice.Get(), Geometry::CreatePlane());
		break;
	default:
		break;
	}
	
}
