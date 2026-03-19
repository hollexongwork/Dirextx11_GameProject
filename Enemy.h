#pragma once
#include"Tank.h"

class Enemy : public Tank
{
public:
	void Init()override;
	void Uninit()override;
};

