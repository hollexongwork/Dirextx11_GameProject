#include "Enemy.h"

// --- Components --- 
#include "BoxCollider.h"
#include "EnemyAI.h"
#include "TankEvent.h"
#include "TankHealth.h"
#include "TankMovement.h"
#include "TankPosture.h"
#include "TankFire.h"
#include "WheelSmoke.h"
#include "DamageEffect.h"
#include "EnemyManager.h"

void Enemy::Init()
{
	Tank::Init();

	// --- Components --- 
	auto* col = AddComponent<BoxCollider>(Vector3(1.0f, 0.45f, 1.95f));
	col->SetTag(CollisionTag::Enemy);
	col->SetOffset(Vector3(0.0f, 0.45f, 0.0f));

	AddComponent<EnemyAI>();
	AddComponent<TankEvent>();
	AddComponent<TankHealth>();
	AddComponent<TankMovement>();
	AddComponent<TankPosture>();
	auto* fire = AddComponent<TankFire>();
	fire->SetBulletType(BulletType::Enemy);
	AddComponent<WheelSmoke>();
	AddComponent<DamageEffect>();

	EnemyManager::RegisterEnemy();
}

void Enemy::Uninit()
{
	Tank::Uninit();
	EnemyManager::UnregisterEnemy();
}






