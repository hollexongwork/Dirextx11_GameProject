#include "main.h"
#include "Manager.h"
#include "Scene.h"
#include "Time.h"
#include "TankMovement.h"
#include "Tank.h"
#include "TankEvent.h"
#include "BoxCollider.h"

void TankMovement::Start()
{
	m_Owner = GetOwnerAs<Tank>();
	if (!m_Owner)
		return;

	m_Event = m_Owner->GetComponent<TankEvent>();

	m_InputProvider = m_Owner->GetComponent<InputProvider>();

	m_Collider = m_Owner->GetComponent<Collider>();
}

void TankMovement::Uninit()
{
	if (m_Collider) { m_Collider = nullptr; }
	if (m_InputProvider) { m_InputProvider = nullptr; }
	if (m_Event) { m_Event = nullptr; }
	if (m_Owner) { m_Owner = nullptr; }
}

void TankMovement::Update()
{
	if (!m_InputProvider)
		return;

	m_DeltaTime = Time::GetDeltaTime();

	if (m_Event->IsDestory())
	{
		InertialDampening();
	}
	else
	{
		m_Input = m_InputProvider->GetInputState();

		Movement();
		Spring();
		Recoil();
	}
}

void TankMovement::Movement()
{
	const float throttle = m_Input.Throttle;
	const bool  braking = m_Input.Braking;
	const float steer = m_Input.Steer;

	// --- Force ---
	float engineForce = throttle * MAX_FORCE;
	float rollForce = ROLL_RESISTANCE * MASS * G;
	float dragForce = 0.5f * AIR_DENSITY * DRAG_COEF * FRONTAL_AREA * m_Velocity * std::fabsf(m_Velocity);
	float brakeForce = braking && std::fabsf(m_Velocity) > THRESHOLD ?
					   MAX_BRAKE_FORCE * (m_Velocity > 0.0f ? 1.0f : -1.0f) :
					   0.0f;

	// --- Accel & Speed ---
	float netForce = engineForce - (m_Velocity > THRESHOLD ? 1.0f : (m_Velocity < -THRESHOLD ? -1.0f : 0.0f)) * rollForce - dragForce - brakeForce;
	m_Acceleration = netForce / MASS;

	m_Velocity += m_Acceleration * m_DeltaTime;
	m_Velocity = Clamp(m_Velocity, MAX_REVERSE_SPEED, MAX_SPEED);

	if (std::fabsf(m_Velocity) < THRESHOLD && std::fabsf(throttle) < THRESHOLD)
	{
		m_Velocity = 0.0f;
	}

	// --- Turn ---
	float targetAngularVelocity = 0.0f;

	if (std::fabsf(m_Velocity) > THRESHOLD)
	{
		float speedFactor = std::fabsf(m_Velocity) / MAX_SPEED;
		targetAngularVelocity = steer * TURN_COEF * speedFactor;
	}
	else if (std::fabsf(steer) > THRESHOLD)
	{
		targetAngularVelocity = steer * STATIONARY_TURN_RATE;
	}

	// --- Angular Acceleration ---
	float delta = targetAngularVelocity - m_AngularVelocity;
	float maxDelta = STATIONARY_TURN_ACCEL * m_DeltaTime;

	delta = Clamp(delta, -maxDelta, maxDelta);
	m_AngularVelocity += delta;
}

void TankMovement::Spring()
{
	float localAccelForward = m_Acceleration;
	float localAccelLateral = m_Velocity * m_AngularVelocity;

	Vector3 targetLocalRot;
	targetLocalRot.x = localAccelForward * 0.1f;
	targetLocalRot.y = 0.0f;
	targetLocalRot.z = localAccelLateral * 0.1f;

	// --- Spring physics --- 
	Vector3 stretch = m_CogRot - targetLocalRot;
	Vector3 torque = stretch * (-SPRING_COEF) - m_CogRotVel * SPRING_DAMPING;

	m_CogRotVel += torque * m_DeltaTime;
	m_CogRot += m_CogRotVel * m_DeltaTime;
}

void TankMovement::Recoil()
{
	if (!m_Event)
		return;

	if (m_Event->IsGunRecoiling())
	{
		m_GunRecoilTime += m_DeltaTime;

		if (m_GunRecoilTime < RECOIL_DURATION)
		{
			// ---------- Back ----------
			float t = m_GunRecoilTime / RECOIL_DURATION; 
			m_RecoilOffset = RECOIL_DISTANCE * t;
		}
		else if (m_GunRecoilTime < RECOIL_TOTAL_TIME)
		{
			// ---------- Return ----------
			float t = (m_GunRecoilTime - RECOIL_DURATION) / RECOIL_BACK_DURATION; 
			m_RecoilOffset = RECOIL_DISTANCE * (1.0f - t);
		}
		else
		{
			// ---------- Stop ----------
			m_RecoilOffset = 0.0f;
			m_Event->SetGunRecoiling(false);
		}
	}
}

void TankMovement::InertialDampening()
{
	float drag = DAMPING_COEF * m_Velocity;

	m_Velocity -= drag * m_DeltaTime;

	float torquedrag = ANGULAR_DAMPING_COEF * m_AngularVelocity;
	m_AngularVelocity -= torquedrag * m_DeltaTime;
}


