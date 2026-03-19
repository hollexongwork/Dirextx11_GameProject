#pragma once
#include"Tank.h"

class Player : public Tank
{
private:
	class TankMovement* m_Movement;
public:
	void Init()override;
	void OnCollisionEnter(Collider* otherCollider) override;
};

