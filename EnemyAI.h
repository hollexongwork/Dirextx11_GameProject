#pragma once
#include "Component.h"
#include "InputProvider.h"
#include "InputState.h"

class EnemyAI : public Component, public InputProvider
{
private:
	class Enemy* m_Owner;
	class TankEvent* m_Event;
	class TankPosture* m_Posture;
	class TankHealth* m_Health;
	class Player* m_Player;
	class TankEvent* m_PlayerEvent;

	InputState m_InputState;

	float m_ScaleRadius = 125.0f;

	Vector3 m_TargetDirection;

public:
	void Start() override;
	void Uninit() override;
	void Update() override;

	const InputState& GetInputState() const override { return m_InputState; }

	void ScaleTarget();
	void Fire();

	void SetScaleRadius(const float& radius) { m_ScaleRadius = radius; }
};

