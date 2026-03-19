#pragma once
#include "Scene.h"

class Loading : public Scene
{
protected:
	bool m_LoadFinish;

	class Polygon2D* m_Circle;
	float m_Rota;

public:
	virtual void Init() override;
	virtual void Start() override;
	virtual void Uninit() override;
	virtual void Update() override;
};

