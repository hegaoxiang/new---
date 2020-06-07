#pragma once
#include "GEngine/GScene.h"

class LogicSystem
{
public:
	static LogicSystem& Get();

	void Update(float dt);

	void SetScene(shared_ptr<GScene> GScene)
	{
		m_pScene = GScene;
	}
private:
	shared_ptr<GScene> m_pScene;
	void RotateComp(float dt);
	LogicSystem() {};

};

