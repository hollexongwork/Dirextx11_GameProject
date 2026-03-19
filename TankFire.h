#pragma once
#include "Component.h"
#include "InputProvider.h"

enum class BulletType
{
	Player,
	Enemy,
};

class TankFire : public Component
{
private:
	static constexpr float ReloadTime = 1.5f;
	static constexpr float MuzzleVelocity = 1.5f;

	class Tank* m_Owner;
	class TankPosture* m_Posture;
	class TankEvent* m_Event;

	const InputProvider* m_InputProvider;
	InputState m_Input;

	class FireEffect* m_FireEffect;

	BulletType m_Type = BulletType::Player;

	float m_ReloadTimer = 1.5f;

public:
	void Init()override;
	void Start()override;
	void Uninit()override;
	void Update()override;

	void Fire();

	void SetBulletType(const BulletType& type);
};

