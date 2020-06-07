#include "GCameraController.h"

#include <IMGUI/imgui.h>
#include <DXOthers/d3dUtil.h>
#include <memory>

void GCameraController::UpdataCamera(GRiCamera* GRiCamera, CameraMode mode, float dt)
{
	auto cam1st = dynamic_cast<FirstPersonCamera*>(GRiCamera);
	auto cam3rd = dynamic_cast<ThirdPersonCamera*>(GRiCamera);


	auto& io = ImGui::GetIO();
	if (io.MouseDown[1])
	{
		// ��һ�˳�/����������Ĳ���

		// �����ƶ�
		if (ImGui::IsKeyDown(Keys::W))
		{
			if (mode == CameraMode::FirstPerson)
				cam1st->Walk(dt * 3.0f);
			else
				cam1st->MoveForward(dt * 3.0f);
		}
		if (ImGui::IsKeyDown(Keys::S))
		{
			if (mode == CameraMode::FirstPerson)
				cam1st->Walk(dt * -3.0f);
			else
				cam1st->MoveForward(dt * -3.0f);
		}
		if (ImGui::IsKeyDown(Keys::A))
			cam1st->Strafe(dt * -3.0f);
		if (ImGui::IsKeyDown(Keys::D))
			cam1st->Strafe(dt * 3.0f);
		if (ImGui::IsKeyDown(Keys::Q))
			cam1st->Up(dt * 3.0f);
		if (ImGui::IsKeyDown(Keys::E))
			cam1st->Down(dt * 3.0f);
		// ��λ��������[-8.9f, 8.9f]��������
		// ��������

		// ��Ұ��ת����ֹ��ʼ�Ĳ�ֵ�����µ�ͻȻ��ת
		cam1st->Pitch(io.MouseDelta.y * dt * 1.25f);
		cam1st->RotateY(io.MouseDelta.x * dt * 1.25f);
	}
	// 	else if (m_CameraMode == CameraMode::ThirdPerson)
	// 	{
	// 		// �����˳�������Ĳ���
	// 
	// 		cam3rd->SetTarget(m_WoodCrate.GetPosition());
	// 
	// 		// ��������ת
	// 		cam3rd->RotateX(mouseState.y * dt * 1.25f);
	// 		cam3rd->RotateY(mouseState.x * dt * 1.25f);
	// 		cam3rd->Approach(-mouseState.scrollWheelValue / 120 * 1.0f);
	// 	}

		// ���¹۲����
	GRiCamera->UpdateViewMatrix();

}

