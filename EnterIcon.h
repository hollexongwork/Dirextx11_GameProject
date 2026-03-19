#pragma once
#include "Polygon.h"

class EnterIcon : public Polygon2D
{
private:

	float m_TimeCount;
	float m_StartTime;

protected:

public:
	void Init()override;
	void Uninit()override;
	void Update()override;
	void Draw()override;

	void PlaySacleAnimation();
};

