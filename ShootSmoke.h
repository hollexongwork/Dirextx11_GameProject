#pragma once
#include "GameObject.h"
#include "Particle.h"

class ShootSmoke : public Particle
{
private:
	static const int PARTICLE_MAX = 12;
	PARTICLE m_Particle[PARTICLE_MAX];

	struct Material
	{
		XMFLOAT4 Color = XMFLOAT4(0.546875f, 0.332325f, 0.193685f, 0.5f);
		int Frame = 0;
		int WidthDivision = 12;
		int HeightDivision = 12;
		float DepthFadeDistance = 0.5f;
	};

	ID3D11Buffer* m_MaterialBuffer;
	Material m_Material[PARTICLE_MAX] = {};

	ID3D11ShaderResourceView* m_Normal;

	std::uniform_real_distribution<float> m_LifeDist;
	std::uniform_real_distribution<float> m_RotaDist;
	std::uniform_real_distribution<float> m_RotaSpeedDist;
	std::uniform_real_distribution<float> m_SizeDist;
	std::uniform_real_distribution<float> m_VelocityDistX;
	std::uniform_real_distribution<float> m_VelocityDistY;
	std::uniform_real_distribution<float> m_VelocityDistZ;

	float Drag[PARTICLE_MAX];

	float m_RotaSpeed[PARTICLE_MAX];
	float m_InitialRotaSpeed[PARTICLE_MAX];

	XMFLOAT3 m_InitialColor = XMFLOAT3(0.546875f, 0.332325f, 0.193685f );

public:
	void Init()override;
	void Uninit()override;
	void Update()override;
	void Draw()override;

	void EmissionParticle(Vector3 position, Vector3 direction, int Count = 3);
};

