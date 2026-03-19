#pragma once
#include "Component.h"
#include "InputProvider.h"
#include "InputState.h"

class PlayerInput : public Component, public InputProvider
{
private:
	class TankEvent* m_Event;
	class TankPosture* m_Posture;
	class CrossSight* m_CrossSight;

	InputState m_InputState;

public:
	void Start() override;
	void Uninit() override;
	void Update() override;
	const InputState& GetInputState() const override { return m_InputState; }
};

