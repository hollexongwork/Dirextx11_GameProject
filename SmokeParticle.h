#pragma once

#include "Particle.h"

class SmokeParticle : public Particle
{
private:
	static const int PARTICLE_MAX = 200;
	PARTICLE m_Particle[PARTICLE_MAX];

	struct SnokeMaterial
	{
		XMFLOAT4 Color = XMFLOAT4(0.34375f, 0.29296875f, 0.2265625f, 1.0f);
		int Frame = 0;
		int WidthDivision = 12;
		int HeightDivision = 12;
		float DepthFadeDistance = 0.25f;
	};

	ID3D11Buffer* m_MaterialBuffer;
	SnokeMaterial m_Material[PARTICLE_MAX] = {};

	ID3D11ShaderResourceView* m_Normal;

	std::uniform_real_distribution<float> m_LifeDist;
	std::uniform_real_distribution<float> m_RotaDist;
	std::uniform_real_distribution<float> m_RotaSpeedDist;
	std::uniform_real_distribution<float> m_SizeDist;
	std::uniform_real_distribution<float> m_VelocityDistX;
	std::uniform_real_distribution<float> m_VelocityDistY;
	std::uniform_real_distribution<float> m_VelocityDistZ;

	float m_Drag;

	float m_EmissionRate;
	float m_EmissionAccumulator;

	float m_RotaSpeed[PARTICLE_MAX];

	void EmissionParticle();

public:
	void Init()override;
	void Uninit()override;
	void Update()override;
	void Draw()override;

	void SetEmissionRate(const float rate) { m_EmissionRate = rate; }
};

