#pragma once
#include "Component.h"

class TankEvent : public Component
{
protected:
	class Tank* m_Owner;
	class TankMovement* m_Movement;

	bool m_IsDestory = false;
	bool m_IsYawInitialized = false;
	bool m_IsGunRecoiling = false;
	bool m_HasTarget = false;
	bool m_CanFire = false;

public:
	void Start() override;
	void Uninit() override;

	bool IsDestory() const { return m_IsDestory; }
	void SetIsDestory(const bool& destoryed) { m_IsDestory = destoryed; }

	bool IsYawInitialized() const { return m_IsYawInitialized; }
	void SetYawInitialized(const bool& initialized) { m_IsYawInitialized = initialized; }

	bool IsGunRecoiling() const { return m_IsGunRecoiling; }
	void SetGunRecoiling(const bool& recoiling) { m_IsGunRecoiling = recoiling; }
	void StartRecoil(const Vector3& gunForward);

	bool HasTarget() const { return m_HasTarget; }
	void SetHasTarget(const bool& hasTarget) { m_HasTarget = hasTarget; }

	bool CanFire() const { return m_CanFire; }
	void SetCanFire(const bool& canFire) { m_CanFire = canFire; }

};

