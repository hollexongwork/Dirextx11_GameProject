#include "main.h"
#include "Manager.h"
#include "Scene.h"
#include "Time.h"

#include "WheelSmoke.h"
#include "Tank.h"
#include "TankMovement.h"
#include "TankPosture.h"
#include "SmokeParticle.h"

void WheelSmoke::Init()
{
    m_leftWheelSmoke.resize(NUM_PARTICLES_PER_WHEEL);
    m_rightWheelSmoke.resize(NUM_PARTICLES_PER_WHEEL);
    for (size_t i = 0; i < NUM_PARTICLES_PER_WHEEL; ++i) 
    {
        m_leftWheelSmoke[i] = Manager::GetScene()->AddGameObject<SmokeParticle>(RENDER_LAYER_PARTICLE);
        m_rightWheelSmoke[i] = Manager::GetScene()->AddGameObject<SmokeParticle>(RENDER_LAYER_PARTICLE);
    }
}

void WheelSmoke::Start()
{
	m_Owner = GetOwnerAs<Tank>();
	if (!m_Owner)
		return;
	m_Movement = m_Owner->GetComponent<TankMovement>();
	m_Posture = m_Owner->GetComponent<TankPosture>();
}

void WheelSmoke::Uninit()
{
    for (auto* smoke : m_leftWheelSmoke)
    {
        if (smoke)
            smoke->SetDestory();
    }

    for (auto* smoke : m_rightWheelSmoke)
    {
        if (smoke)
            smoke->SetDestory();
    }

    m_leftWheelSmoke.clear();
    m_rightWheelSmoke.clear();

    if (m_Posture) { m_Posture = nullptr; }
    if (m_Movement) { m_Movement = nullptr; }
    if (m_Owner) { m_Owner = nullptr; }
}

void WheelSmoke::Update()
{
    if (!m_Owner) 
        return;

	float speed = std::fabsf(m_Movement->GetVelocity());
    float angularSpeed = std::fabsf(m_Movement->GetAngularVelocity());

    if (speed > THRESHOLD || angularSpeed > THRESHOLD)
    {
        float momentum = speed + angularSpeed * ANGULAR_SPEED_COEF;
        float rate = momentum * EMISSION_COEF * Time::GetDeltaTime();
        const auto& leftTrackPositions = m_Posture->GetLeftWheelTrackPositions();
        const auto& rightTrackPositions = m_Posture->GetRightWheelTrackPositions();

        for (size_t i = 0; i < m_leftWheelSmoke.size() && i < leftTrackPositions.size(); i++)
        {
            m_leftWheelSmoke[i]->SetPosition(leftTrackPositions[i]);
            m_leftWheelSmoke[i]->SetEmissionRate(rate);
        }
        for (size_t i = 0; i < m_rightWheelSmoke.size() && i < rightTrackPositions.size(); ++i)
        {
            m_rightWheelSmoke[i]->SetPosition(rightTrackPositions[i]);
            m_rightWheelSmoke[i]->SetEmissionRate(rate);
        }
    }
}
