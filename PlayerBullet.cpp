#include "main.h"
#include "Manager.h"
#include "Scene.h"
#include "PlayerBullet.h"
#include "Explosion.h"
#include "SphereCollider.h"
#include "Player.h"
#include "TankHealth.h"
#include "EnemyAI.h"

void PlayerBullet::Init()
{
	Bullet::Init();

	m_Damage = 35.0f;
}

void PlayerBullet::Uninit()
{
	Bullet::Uninit();
}

void PlayerBullet::Update()
{
	Bullet::Update();
}

void PlayerBullet::Draw()
{
	Bullet::Draw();
}

void PlayerBullet::OnCollisionEnter(Collider* otherCollider)
{
	Bullet::OnCollisionEnter(otherCollider);

	if (otherCollider->GetTag() == CollisionTag::Enemy)
	{
		auto* other = otherCollider->GetOwner();
		if (!other) return;

		auto* health = other->GetComponent<TankHealth>();

		if (health)
		{
			health->TakeDamage(m_Damage);
		}

		auto* player = Manager::GetScene()->GetGameObject<Player>();
		if (!player) return;

		auto* enemyAI = other->GetComponent<EnemyAI>();

		if (enemyAI)
		{
			float radius = (other->GetPosition() - player->GetPosition()).length() + 5.0f;
			enemyAI->SetScaleRadius(radius);
		}


	}

	return;
}
