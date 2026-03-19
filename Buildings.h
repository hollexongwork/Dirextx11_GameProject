#pragma once
#include"GameObject.h"
#include <random>

class Buildings
{
private:
	static constexpr int GRID_SIZE = 6;
	static constexpr float SPACING = 50.0f;

	std::mt19937 gen;

	std::uniform_real_distribution<float> m_Dist;
	std::uniform_int_distribution<int> m_ModelDist;

	Vector3 m_StartPos = Vector3(-120.0f, 0.0f, -120.0f);

public:
	void Init();
	void Uninit();
};

