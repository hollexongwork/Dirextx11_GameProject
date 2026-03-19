#pragma once
#include "GameObject.h"
#include <random>
#include <mutex>
#include "Time.h"

class Particle : public GameObject
{
private:

protected:
	ID3D11Buffer* m_VertexBuffer;

	ID3D11InputLayout* m_VertexLayout;
	ID3D11VertexShader* m_VertexShader;
	ID3D11PixelShader* m_PixelShader;

	ID3D11ShaderResourceView* m_Texture;

	struct PARTICLE
	{
		bool Enable = false;
		float Life = 0.0f;
		float InitialLife = 0.0f;
		Vector3 Position = {};
		Vector3 Rotation = {};
		Vector3 Size = {};
		Vector3 InitialSize = {};
		Vector3 Velocity = {};
	};

	static std::random_device rd;
	static std::mt19937 engine;
	static std::mutex engine_mutex;

public:
	void Init()override;
	void Uninit()override;
	void Update()override;
	void Draw()override;
};

