#include "main.h"
#include "Manager.h"
#include "Scene.h"
#include "Time.h"

#include "FireEffect.h"
#include "MuzzleFlash.h"
#include "ShootSmoke.h"

void FireEffect::Init()
{
	m_muzzleFlash = Manager::GetScene()->AddGameObject<MuzzleFlash>(RENDER_LAYER_PARTICLE);
	m_ShootSmoke = Manager::GetScene()->AddGameObject<ShootSmoke>(RENDER_LAYER_PARTICLE);
	m_muzzleFlash->Init();
	m_ShootSmoke->Init();
}

void FireEffect::Uninit()
{
	if (m_muzzleFlash)
	{
		m_muzzleFlash->SetDestory();
		m_muzzleFlash = nullptr;
	}

	if (m_ShootSmoke)
	{
		m_ShootSmoke->SetDestory();
		m_ShootSmoke = nullptr;
	}
}

void FireEffect::PlayEffect(Vector3 muzzlePos, Vector3 direction)
{
	m_muzzleFlash->SetPosition(muzzlePos);
	m_muzzleFlash->EmissionParticle(muzzlePos,1);

	m_ShootSmoke->SetPosition(muzzlePos);
	m_ShootSmoke->EmissionParticle(muzzlePos, direction,3);

	Vector3 worldUp = fabsf(direction.y) > 0.99999f ? Vector3(1, 0, 0) : Vector3(0, 1, 0);

	Vector3 right = Vector3::Cross(worldUp, direction);
	right.normalize();

	m_ShootSmoke->EmissionParticle(muzzlePos, right, 3);

	m_ShootSmoke->EmissionParticle(muzzlePos, -right, 3);
}
