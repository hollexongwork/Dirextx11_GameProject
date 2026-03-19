#include "TankHealth.h"
#include "Tank.h"
#include "TankEvent.h"

void TankHealth::Init()
{
	m_MaxHP = 100.0f;
	m_CurrentHP = m_MaxHP;
}

void TankHealth::Start()
{
	m_Owner = m_Owner = GetOwnerAs<Tank>();
	if (!m_Owner)
		return;

	m_Event = m_Owner->GetComponent<TankEvent>();
}

void TankHealth::Uninit()
{
	if (m_Event) { m_Event = nullptr; }
	if (m_Owner) { m_Owner = nullptr; }
}

void TankHealth::Update()
{
	m_HPRatio = m_CurrentHP / m_MaxHP;

	m_Owner->SetDamageMaterialAlpha(m_HPRatio);

	if (OnDestory())
	{
		m_Event->SetIsDestory(true);
		m_Event->SetCanFire(false);
	}
}

void TankHealth::TakeDamage(const float& Damage)
{
	if (m_Event->IsDestory())
		return;

	m_CurrentHP = fmaxf(0.0f, m_CurrentHP - Damage);
}

bool TankHealth::OnDestory()
{
	return m_CurrentHP <= 0.0f && !m_Event->IsDestory();
}