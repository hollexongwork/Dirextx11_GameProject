#include "Player.h"

// --- Components --- 
#include "BoxCollider.h"
#include "PlayerInput.h"
#include "TankEvent.h"
#include "TankHealth.h"
#include "TankMovement.h"
#include "TankPosture.h"
#include "TankFire.h"
#include "WheelSmoke.h"
#include "DamageEffect.h"
#include "FrontSight.h"

void Player::Init()
{
	Tank::Init();

	// --- Components --- 
	auto* col = AddComponent<BoxCollider>(Vector3(1.0f, 0.45f, 1.95f));
	col->SetTag(CollisionTag::Player);
	col->SetOffset(Vector3(0.0f, 0.45f, 0.0f));

	AddComponent<PlayerInput>();

	auto* even = AddComponent<TankEvent>();
	even->SetHasTarget(true);

	AddComponent<TankHealth>();
	m_Movement = AddComponent<TankMovement>();
	AddComponent<TankPosture>();
	AddComponent<TankFire>();
	AddComponent<WheelSmoke>();
	AddComponent<DamageEffect>();
	AddComponent<FrontSight>();
}


void Player::OnCollisionEnter(Collider* otherCollider)
{
	if (otherCollider->GetTag() == CollisionTag::Bullet)
		return;

	m_Movement->SetVelocity(0.0f);
	m_Movement->SetAcceleration(0.0f);
}