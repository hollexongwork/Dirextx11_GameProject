#pragma once
#include "Component.h"

class TankHealth : public Component
{
private:
	class Tank* m_Owner;
	class TankEvent* m_Event;

	float m_MaxHP;
	float m_CurrentHP;
	float m_HPRatio;

public:
	void Init()override;
	void Start()override;
	void Uninit()override;
	void Update()override;

	bool OnDestory();

	void TakeDamage(const float& Damage);

	float GetMaxHP() const { return m_MaxHP; }
	float GetCurrentHP() const { return m_CurrentHP; }
	float GetHPRatio() const { return m_HPRatio; }


};

