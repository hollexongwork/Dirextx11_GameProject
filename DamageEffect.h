#pragma once
#include "component.h"

class DamageEffect : public Component
{
private:


	class Tank* m_Owner;
	class TankMovement* m_Movement;
	class TankHealth* m_Health;

	static constexpr float SMOKE_EMISSION_RATE_BASE = 3.0f;
	static constexpr float SMOKE_EMISSION_RATE_SPEED_MULTIPLIER = 20.0f;
	static constexpr float SMOKE_EMISSION_RATE_DAMAGE_MULTIPLIER = 12.0f;

	class DamageSmoke* m_DamageSmoke;
	class Burn* m_Burn;

	float m_BurnEmissionRate;

public:
	void Init()override;
	void Start()override;
	void Uninit()override;
	void Update()override;

};

