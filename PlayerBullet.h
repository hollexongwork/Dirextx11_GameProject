#pragma once
#include"Bullet.h"

class PlayerBullet : public Bullet
{
private:
	class Explosion* m_Explosion;
protected:

public:
	void Init()override;
	void Uninit()override;
	void Update()override;
	void Draw()override;

	void OnCollisionEnter(Collider* otherCollider) override;
};

