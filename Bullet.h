#pragma once
#include"GameObject.h"

class Bullet : public GameObject
{
private:
	static constexpr float LifeTime = 5.0f;
	static constexpr float Speed = 700.0f;

	class MODEL* m_Model;

	ID3D11InputLayout* m_VertexLayout;
	ID3D11VertexShader* m_VertexShader;
	ID3D11PixelShader* m_PixelShader;

	ID3D11ShaderResourceView* m_Texture;

	Vector3 m_Velocity;
	float m_LifeTimeCount;
	bool m_IsHit = false;

protected:
	float m_Damage;

public:
	void Init()override;
	void Uninit()override;
	void Update()override;
	void Draw()override;

	Vector3 GetVelocity() const { return m_Velocity; }
	void SetVelocity(const Vector3& velocity) { m_Velocity = velocity; }

	void OnCollisionEnter(Collider* otherCollider) override;

	void Shot(const Vector3& muzzlePos, const Vector3& gunDirection, const XMVECTOR& X, const XMVECTOR& Y, const XMVECTOR& Z);
};

