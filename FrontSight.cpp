#include "main.h"
#include "Manager.h"
#include "Scene.h"
#include "FrontSight.h"
#include "Sight.h"
#include "Player.h"
#include "TankPosture.h"

void FrontSight::Init()
{
	m_Sight = Manager::GetScene()->AddGameObject <Sight>(RENDER_LAYER_XRAY);
}

void FrontSight::Start()
{
	auto* owner = GetOwnerAs<Player>();
	if (!owner)
		return;

	m_Posture = owner->GetComponent<TankPosture>();
}

void FrontSight::Uninit()
{
	if (m_Sight)
	{
		m_Sight->SetDestory();
		m_Sight = nullptr;
	}

	if (m_Posture) { m_Posture = nullptr; }
}

void FrontSight::Update()
{
	if (!m_Posture)
		return;

	aiMatrix4x4 boneMatrix = m_Posture->GetBoneMatrix("gun_jnt");
	Vector3 gunAxisX = { boneMatrix.a1, boneMatrix.b1, boneMatrix.c1, };
	Vector3 gunAxisZ = { boneMatrix.a3, boneMatrix.b3, boneMatrix.c3, };

	Vector3 gunJntPosition = m_Posture->GetBoneWorldPosition("gun_jnt");

	Vector3 Ppsition = gunJntPosition + gunAxisZ * (2.0f + m_Distance) - gunAxisX * 0.033f;

	m_Sight->SetPosition(Ppsition);
}
