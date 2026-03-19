#pragma once
#include "component.h"

class SmokeParticle;

class WheelSmoke : public Component
{
private:
	static constexpr float THRESHOLD = 0.01f;
	static constexpr float EMISSION_COEF = 50.0f;
	static constexpr float ANGULAR_SPEED_COEF = 15.0f;

	class Tank* m_Owner;
	class TankMovement* m_Movement;
	class TankPosture* m_Posture;

	static constexpr size_t NUM_PARTICLES_PER_WHEEL = 7;
	std::vector<SmokeParticle*> m_leftWheelSmoke;
	std::vector<SmokeParticle*> m_rightWheelSmoke;

public:
	void Init()override;
	void Start()override;
	void Uninit()override;
	void Update()override;

};

