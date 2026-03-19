#pragma once
#include "GameObject.h"
#include "Particle.h"

class MuzzleFlash : public Particle
{
private:
	static const int PARTICLE_MAX = 3;
	PARTICLE m_Particle[PARTICLE_MAX];

	struct Material
	{
		XMFLOAT4 Color = { 1.0f, 0.321739f, 0.079966f, 1.0f };
		float Brightness = 7.0f;
		int Frame = 0;
		int WidthDivision = 2;
		int HeightDivision = 2;
		float DepthFadeDistance = 0.1f;
		int Padding[3];
	};

	ID3D11Buffer* m_MaterialBuffer;
	Material m_Material[PARTICLE_MAX] = {};

	std::uniform_real_distribution<float> m_LifeDist;
	std::uniform_real_distribution<float> m_SizeDist;
	std::uniform_real_distribution<float> m_RotaDist;

public:
	void Init()override;
	void Uninit()override;
	void Update()override;
	void Draw()override;

	void EmissionParticle(Vector3 position, int Count = 1);
};

