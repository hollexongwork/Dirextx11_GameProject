#pragma once
#include "GameObject.h"
#include "Particle.h"

class Explosion : public Particle
{
private:
	static constexpr int PARTICLE_MAX = 10;
	PARTICLE m_Particle[PARTICLE_MAX];

	struct Material
	{
		XMFLOAT4 Color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		int Frame = 0;
		int WidthDivision = 8;
		int HeightDivision = 8;
		float DepthFadeDistance = 0.25f;
	};

	ID3D11Buffer* m_MaterialBuffer;
	Material m_Material[PARTICLE_MAX] = {};

	static constexpr float m_Life = 1.5f;

	std::uniform_real_distribution<float> m_SizeDist;

public:
	void Init()override;
	void Uninit()override;
	void Update()override;
	void Draw()override;

	void EmissionParticle(Vector3 position, int Count = 1);
};

