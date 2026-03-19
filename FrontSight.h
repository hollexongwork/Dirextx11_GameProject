#pragma once
#include "Component.h"

class FrontSight : public Component
{
private:
	class TankPosture* m_Posture;

	class Sight* m_Sight;

	float m_Distance = 100.0f;

public:
	void Init()override;
	void Start()override;
	void Uninit()override;
	void Update()override;

	void SetDistance(float distance) { m_Distance = distance; }

};

