#pragma once
#include "Particle.h"

class Burn : public Particle
{
private:
	static const int PARTICLE_MAX = 500;
	PARTICLE m_Particle[PARTICLE_MAX];

	struct Material
	{
		XMFLOAT4 Color = XMFLOAT4( 0.025f, 0.025f, 0.025f, 1.0f );
		int Frame = 0;
		int WidthDivision = 8;
		int HeightDivision = 8;
		float DepthFadeDistance = 0.5f;
	};

	ID3D11Buffer* m_MaterialBuffer;
	Material m_Material[PARTICLE_MAX] = {};

	ID3D11ShaderResourceView* m_ColorMap;

	std::uniform_real_distribution<float> m_LifeDist;
	std::uniform_real_distribution<float> m_RotaDist;
	std::uniform_real_distribution<float> m_RotaSpeedDist;
	std::uniform_real_distribution<float> m_SizeDist;
	std::uniform_real_distribution<float> m_VelocityDistX;
	std::uniform_real_distribution<float> m_VelocityDistY;
	std::uniform_real_distribution<float> m_VelocityDistZ;
	std::uniform_real_distribution<float> m_EmitPosDistX;
	std::uniform_real_distribution<float> m_EmitPosDistY;
	std::uniform_real_distribution<float> m_EmitPosDistZ;

	Vector3 m_AxisX;
	Vector3 m_AxisY;
	Vector3 m_AxisZ;

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

	void SetAxis(const Vector3& axisX, const Vector3& axisY, const Vector3& axisZ)
	{
		m_AxisX = axisX;
		m_AxisY = axisY;
		m_AxisZ = axisZ;
	}

	void SetEmissionRate(const float rate) { m_EmissionRate = rate; }
};

