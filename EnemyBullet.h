#pragma once
#include"Bullet.h"

class EnemyBullet : public Bullet
{
private:

protected:

public:
	void Init()override;
	void Uninit()override;
	void Update()override;
	void Draw()override;

	void OnCollisionEnter(Collider* otherCollider) override;
};

