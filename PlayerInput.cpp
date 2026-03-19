#include "main.h"
#include "Manager.h"
#include "Scene.h"
#include "Input.h"
#include "Mouse.h"
#include "PlayerInput.h"
#include "Player.h"
#include "CrossSight.h"
#include "TankPosture.h"
#include "TankEvent.h"

void PlayerInput::Start()
{
    auto* owner = GetOwnerAs<Player>();

    m_Event = owner->GetComponent<TankEvent>();

    m_Posture = owner->GetComponent<TankPosture>();

	m_CrossSight = Manager::GetScene()->GetGameObject<CrossSight>();
}

void PlayerInput::Uninit()
{
    if (m_CrossSight) { m_CrossSight = nullptr; }
    if (m_Posture) { m_Posture = nullptr; }
    if (m_Event) { m_Event = nullptr; }
}

void PlayerInput::Update()
{
    if (m_Event->IsDestory())
        return;

    InputState state;

    if (Input::GetKeyPress('W')) state.Throttle += 1.0f;
    if (Input::GetKeyPress('S')) state.Throttle -= 1.0f;
    if (Input::GetKeyPress('A')) state.Steer -= 1.0f;
    if (Input::GetKeyPress('D')) state.Steer += 1.0f;

    state.Braking = Input::GetKeyPress(VK_SPACE);

    state.Fire = IsLeftClickTrigger();

    m_InputState = state;

	Vector3 targetPoint;
	targetPoint = m_CrossSight->GetRayEndPoint();

	m_Posture->SetTargetPoint(targetPoint);
}
