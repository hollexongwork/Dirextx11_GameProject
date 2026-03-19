#include "Vector3.h"
#include "TankEvent.h"
#include "Tank.h"
#include "TankMovement.h"
#include "Collision.h"
#include "BoxCollider.h"

void TankEvent::Start()
{
	m_Owner = GetOwnerAs<Tank>();
	if (!m_Owner)
		return;

	m_Movement = m_Owner->GetComponent<TankMovement>();
}

void TankEvent::Uninit()
{
	if (m_Owner) { m_Owner = nullptr; }
	if (m_Movement) { m_Movement = nullptr; }
}

void TankEvent::StartRecoil(const Vector3& gunForward)
{
	if (!m_Owner)
		return;

	m_IsGunRecoiling = true;
	m_Movement->ReetRecoilTime();

	// Add Impulse Forse to Spring
	Vector3 recoilVel = m_Movement->GetCogRotVel();
	float forwardRecoil = Vector3::Dot(gunForward, m_Owner->GetAxisZ());
	float lateralRecoil = Vector3::Dot(gunForward, m_Owner->GetAxisX());

	recoilVel.x += forwardRecoil * RECOIL_SPRING_COEF;
	recoilVel.z -= lateralRecoil * RECOIL_SPRING_COEF * 0.75f;

	m_Movement->SetCogRotVel(recoilVel);
}

