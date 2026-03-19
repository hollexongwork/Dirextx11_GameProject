#pragma once
#include "Polygon.h"
#include "RayCast.h"

class CrossSight : public Polygon2D
{
private:
	Ray m_Ray;
	RayHit m_Hit;

	float m_MaxDistance = 100.0f;

protected:

public:
	void Init()override;
	void Uninit()override;
	void Update()override;
	void Draw()override;

	Vector3 GetRayEndPoint() const { return m_Ray.end; };
	Vector3 GetRayHitPoint() const { return m_Hit.point; };
};