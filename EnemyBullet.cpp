#include "main.h"
#include "Manager.h"
#include "Scene.h"
#include "EnemyBullet.h"
#include "Explosion.h"
#include "SphereCollider.h"
#include "TankHealth.h"

void EnemyBullet::Init()
{
	Bullet::Init();

	m_Damage = 10.0f;
}

void EnemyBullet::Uninit()
{
	Bullet::Uninit();
}

void EnemyBullet::Update()
{
	Bullet::Update();
}

void EnemyBullet::Draw()
{
	Bullet::Draw();
}

void EnemyBullet::OnCollisionEnter( Collider* otherCollider)
{
	Bullet::OnCollisionEnter(otherCollider);

	if (otherCollider->GetTag() == CollisionTag::Player)
	{
		auto* other = otherCollider->GetOwner();
		if (!other) return;

		auto* health = other->GetComponent<TankHealth>();

		if (health)
		{
			health->TakeDamage(m_Damage);
		}
	}
}

