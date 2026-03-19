#include "main.h"
#include "Manager.h"
#include "Scene.h"
#include "TankFire.h"
#include "Time.h"
#include "Tank.h"
#include "InputProvider.h"
#include "TankPosture.h"
#include "TankEvent.h"
#include "PlayerBullet.h"
#include "EnemyBullet.h"
#include "FireEffect.h"

void TankFire::Init()
{
    m_FireEffect = Manager::GetScene()->AddGameObject<FireEffect>(RENDER_LAYER_PARTICLE);
}

void TankFire::Start()
{
    m_Owner = GetOwnerAs<Tank>();

    if (!m_Owner)
        return;

    m_InputProvider = m_Owner->GetComponent<InputProvider>();

    m_Event = m_Owner->GetComponent<TankEvent>();

	m_Posture = m_Owner->GetComponent<TankPosture>();
}

void TankFire::Uninit()
{
    if (m_FireEffect) { m_FireEffect->Uninit(); m_FireEffect = nullptr;}

    if (m_Posture) { m_Posture = nullptr; }
    if (m_Event) { m_Event = nullptr; }
    if (m_InputProvider) {  m_InputProvider = nullptr; }
    if (m_Owner) {  m_Owner = nullptr; }
}

void TankFire::Update()
{
    if (!m_Owner || m_Event->IsDestory())
        return;

    if (!m_Event->CanFire())
    {
        m_ReloadTimer -= Time::GetDeltaTime();
        if (m_ReloadTimer <= 0.0f)
        {
            m_Event->SetCanFire(true);
            m_ReloadTimer = 0.0f;
        }
    }

	if (!m_InputProvider)
		return;

    m_Input = m_InputProvider->GetInputState();

    if (m_Input.Fire)
    {
		Fire();
    }

}

void TankFire::Fire()
{
    if (!m_Event->CanFire())
        return;

    aiMatrix4x4 boneMatrix = m_Posture->GetBoneMatrix("gun_jnt");
    Vector3 gunAxisX = { boneMatrix.a1, boneMatrix.b1, boneMatrix.c1, };
    Vector3 gunAxisZ = { boneMatrix.a3, boneMatrix.b3, boneMatrix.c3, };

    Vector3 muzzlePos = m_Posture->GetBoneWorldPosition("gun_jnt") + gunAxisZ * 2.0f - gunAxisX * 0.033f;

    FXMVECTOR up = { boneMatrix.a2, boneMatrix.b2, boneMatrix.c2 };
    FXMVECTOR forward = { boneMatrix.a3, boneMatrix.b3, boneMatrix.c3 };

    XMVECTOR RotaVectorZ = forward;
    XMVECTOR RotaVectorX = XMVector3Cross(up, RotaVectorZ);
    XMVECTOR RotaVectorY = XMVector3Cross(RotaVectorZ, RotaVectorX);

    switch (m_Type)
    {
        case BulletType::Player:
        {
            PlayerBullet* bullet = Manager::GetScene()->AddGameObject<PlayerBullet>(RENDER_LAYER_OPAQUE);
            bullet->Shot(muzzlePos, gunAxisZ, RotaVectorX, RotaVectorY, RotaVectorZ);
            break;
        }
    
        case BulletType::Enemy:
        {
            EnemyBullet* bullet = Manager::GetScene()->AddGameObject<EnemyBullet>(RENDER_LAYER_OPAQUE);
            bullet->Shot(muzzlePos, gunAxisZ, RotaVectorX, RotaVectorY, RotaVectorZ);
            break;
        }
    }

    m_Event->SetCanFire(false);
    m_ReloadTimer = ReloadTime;

	m_Event->StartRecoil(gunAxisZ);

    m_FireEffect->PlayEffect(muzzlePos, gunAxisZ);
}

void TankFire::SetBulletType(const BulletType& type)
{
    m_Type = type;
}
