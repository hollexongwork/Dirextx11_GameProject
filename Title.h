#pragma once
#include "Scene.h"

class Title : public Scene
{
private:
	class Camera* m_Camera;
	class Enemy* m_Tank;
	class TankPosture* m_Posture;
	class EnterIcon* m_EnterIcon;

	float m_TimeCount;
	float m_Duration = 5.0f;

public:
	void Init() override;
	void Start() override;
	void Uninit() override;
	void Update() override;
};

