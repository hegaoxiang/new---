#include "LogicSystem.h"


LogicSystem& LogicSystem::Get()
{
	static LogicSystem* imp = new LogicSystem();

	return *imp;
}

void LogicSystem::Update(float dt)
{
	RotateComp(dt);
}

void LogicSystem::RotateComp(float dt)
{
	int i = 0;
	for (auto& item:m_pScene->masks)
	{
		if (item & COMPONENT_ROTATE)
		{
			static float phi = 0.0f, theta = 0.0f;
			phi += 0.0001f, theta += 0.00015f;
			//XMStoreFloat4x4(&m_pScene->worldMats[i], (XMMatrixRotationX(phi) * XMMatrixRotationY(theta) * XMMatrixTranslation(1, 1, 1))); 
		}
		i++;
	}
}
