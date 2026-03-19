#include "main.h"
#include "Manager.h"
#include "Scene.h"
#include "EnemyAI.h"
#include "Enemy.h"
#include "TankEvent.h"
#include "TankPosture.h"
#include "TankHealth.h"
#include "Player.h"
#include "EnemyManager.h"

void EnemyAI::Start()
{
	m_Owner = GetOwnerAs<Enemy>();
	if (!m_Owner)
		return;

	m_Event = m_Owner->GetComponent<TankEvent>();

	m_Posture = m_Owner->GetComponent<TankPosture>();

	m_Health = m_Owner->GetComponent<TankHealth>();

	m_Player = Manager::GetScene()->GetGameObject<Player>();
	if (!m_Player)
		return;
	m_PlayerEvent = m_Player->GetComponent<TankEvent>();
}

void EnemyAI::Uninit()
{
	if (m_PlayerEvent) { m_PlayerEvent = nullptr; }
	if (m_Player) { m_Player = nullptr; }
	if (m_Posture) { m_Posture = nullptr; }
	if (m_Event) { m_Event = nullptr; }
	if (m_Owner) { m_Owner = nullptr; }
}

void EnemyAI::Update()
{
	if (m_Event->IsDestory())
		return;

	if (m_Health->OnDestory())
	{
		EnemyManager::UnregisterEnemy();
		return;
	}

	ScaleTarget();
	Fire();
}

void EnemyAI::ScaleTarget()
{
	if (!m_Player)
		return;

	Vector3 targetPosition = m_Player->GetPosition();
	targetPosition.y += 0.25f;

	Vector3 targetDirection = targetPosition - m_Owner->GetPosition();

	float playerDistanceSq = targetDirection.lengthSq();

	if (playerDistanceSq <= (m_ScaleRadius * m_ScaleRadius) && !m_PlayerEvent->IsDestory())
	{
		m_Event->SetHasTarget(true);
		m_Posture->SetTargetPoint(targetPosition);
		m_TargetDirection = targetDirection;
	}
	else
	{
		m_Event->SetHasTarget(false);
	}
}

void EnemyAI::Fire()
{
	if (!m_Event)
		return;

	if (!m_Event->HasTarget())
	{
		m_InputState.Fire = false;
		return;
	}

	if (m_Event->CanFire())
	{
		aiMatrix4x4 boneMatrix = m_Posture->GetBoneMatrix("gun_jnt");
		Vector3 gunDirection = { boneMatrix.a3, boneMatrix.b3, boneMatrix.c3, };

		float angle = AngleBetweenVector(m_TargetDirection, gunDirection);

		if (fabsf(angle) < 3.5f)
		{
			m_InputState.Fire = true;
		}
	}
	else
	{
		m_InputState.Fire = false;
	}
}