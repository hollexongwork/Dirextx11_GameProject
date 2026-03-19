#pragma once

#include "GameObject.h"

class FireEffect : public GameObject
{
private:

	class MuzzleFlash* m_muzzleFlash;
	class ShootSmoke* m_ShootSmoke;

public:
	void Init()override;
	void Uninit()override;

	void PlayEffect(Vector3 position, Vector3 direction);
};

