#pragma once
#include "Scene.h"

class Game : public Scene
{
private:
	class Player* m_Player;
	class TankEvent* m_PlayerEvent;
	class EnemyManager* m_EnemyManager;


	float m_Delay = 5.0f;
	float m_Timer = 0.0f;

	int m_Count;

public:
	void Init() override;
	void Start() override;
	void Uninit() override; 
	void Update() override;
};

