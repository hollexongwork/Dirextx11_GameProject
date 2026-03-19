#include "main.h"
#include "Manager.h"
#include "Scene.h"
#include "Time.h"

#include "DamageEffect.h"
#include "DamageSmoke.h"
#include "Burn.h"
#include "Tank.h"
#include "TankMovement.h"
#include "TankHealth.h"


void DamageEffect::Init()
{
	m_DamageSmoke = Manager::GetScene()->AddGameObject<DamageSmoke>(RENDER_LAYER_PARTICLE);
	m_Burn = Manager::GetScene()->AddGameObject<Burn>(RENDER_LAYER_PARTICLE);
}

void DamageEffect::Start()
{
	m_Owner = GetOwnerAs<Tank>();
	if (!m_Owner)
		return;
	m_Movement = m_Owner->GetComponent<TankMovement>();

	m_Health = m_Owner->GetComponent<TankHealth>();
}

void DamageEffect::Uninit()
{
	if (m_DamageSmoke)
	{
		m_DamageSmoke->SetDestory();
		m_DamageSmoke = nullptr;
	}

	if (m_Burn)
	{
		m_Burn->SetDestory();
		m_Burn = nullptr;
	}

	if (m_Health) { m_Health = nullptr; }
	if (m_Movement) { m_Movement = nullptr; }
	if (m_Owner) { m_Owner = nullptr; }
}

void DamageEffect::Update()
{
    if (!m_Owner) 
        return;

    float dt = Time::GetDeltaTime();

	Vector3 axisX = m_Owner->GetAxisX();
	Vector3 axisY = m_Owner->GetAxisY();
	Vector3 axisZ = m_Owner->GetAxisZ();

	Vector3 position = m_Owner->GetPosition();
	position -= axisX * 0.2f;
	position.y += 0.5f;

	if (!m_Health || !m_Movement)
		return;

	float speed = m_Movement->GetVelocity();
	float damageRatio = 1.0f - m_Health->GetHPRatio();
	float ratio = damageRatio < 0.5f ? 0.0f : damageRatio;
	float smokeEmissionRate = (SMOKE_EMISSION_RATE_BASE * ratio * SMOKE_EMISSION_RATE_SPEED_MULTIPLIER) * (1.0f + (std::fabsf(speed) * dt * SMOKE_EMISSION_RATE_DAMAGE_MULTIPLIER));
	
	if (m_DamageSmoke)
	{
		m_DamageSmoke->SetAxis(axisX, axisY, axisZ);
		m_DamageSmoke->SetPosition(position);
		m_DamageSmoke->SetEmissionRate(smokeEmissionRate);
	}

	if (m_Burn)
	{
		m_Burn->SetAxis(axisX, axisY, axisZ);
		m_Burn->SetPosition(position);
	}

	if (m_Health->GetHPRatio() <= 0.0f)
	{
		if (m_BurnEmissionRate < 30.0f)
		{
			m_BurnEmissionRate += 10.0f * dt;
		}
		m_Burn->SetEmissionRate(m_BurnEmissionRate);
	}


}
