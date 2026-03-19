#include "main.h"
#include "Renderer.h"
#include "Manager.h"
#include "Scene.h"
#include "Model.h"
#include "Texture.h"
#include "Time.h"
#include "Bullet.h"

#include "Explosion.h"

#include "SphereCollider.h"

void Bullet::Init()
{
	m_Model = ModelLoad("asset/model/Sphere.fbx");

	// Load Shader
	Renderer::CreateVertexShader(&m_VertexShader, &m_VertexLayout, "shader/cso/unlitTextureVS.cso");
	Renderer::CreatePixelShader(&m_PixelShader, "shader/cso/unlitColorPS.cso");

	// Load Texture
	m_Texture = Texture::Load("asset/texture/field.png");

	Scale = { 0.1f,0.1f,0.5f };

	m_LifeTimeCount = 0.0f;

	auto* col = AddComponent<SphereCollider>();
	col->SetRadius(0.2f);
	col->SetTag(CollisionTag::Bullet);
	col->SetTrigger(true);
}

void Bullet::Uninit()
{
	ModelRelease(m_Model);
	m_Model = nullptr;

	if(m_VertexLayout) m_VertexLayout->Release();
	if(m_VertexShader) m_VertexShader->Release();
	if(m_PixelShader) m_PixelShader->Release();
}

void Bullet::Update()
{
	auto* col = GetComponent<SphereCollider>();
	col->Update();

	float dt = Time::GetDeltaTime();

	m_LifeTimeCount += dt;
	if (m_LifeTimeCount >= LifeTime)
	{
		SetDestory();
		return;
	}

	m_Velocity += Vector3(0.0f, -9.8f, 0.0f) * dt;

	Position += m_Velocity * dt;
}

void Bullet::Draw()
{
	Renderer::GetDeviceContext()->IASetInputLayout(m_VertexLayout);

	//shader setting
	Renderer::GetDeviceContext()->VSSetShader(m_VertexShader, NULL, 0);
	Renderer::GetDeviceContext()->PSSetShader(m_PixelShader, NULL, 0);

	//Texture Setting
	Renderer::GetDeviceContext()->PSSetShaderResources(0, 1, &m_Texture);

	//matrix setting
	XMMATRIX ScaleMatrix = XMMatrixScaling(Scale.x, Scale.y, Scale.z);
	XMMATRIX PositionMatrix = XMMatrixTranslation(Position.x, Position.y, Position.z);

	XMMATRIX world = ScaleMatrix * RotationMatrix * PositionMatrix;

	Renderer::SetWorldMatrix(world);

	//Material Setting
	MATERIAL material{};
	material.Diffuse = { 7.5f,6.0f,0.75f,1.0f };
	Renderer::SetMaterial(material);

	ModelDraw(m_Model);
}

void Bullet::Shot(const Vector3& muzzlePos, const Vector3& gunDirection, const XMVECTOR& X, const XMVECTOR& Y, const XMVECTOR& Z)
{
	RotationMatrix.r[0] = XMVectorSelect(XMVectorZero(), X, g_XMSelect1110);
	RotationMatrix.r[1] = XMVectorSelect(XMVectorZero(), Y, g_XMSelect1110);
	RotationMatrix.r[2] = XMVectorSelect(XMVectorZero(), Z, g_XMSelect1110);
	RotationMatrix.r[3] = XMVectorSet(0, 0, 0, 1);

	Position = muzzlePos;
	m_Velocity = gunDirection * Speed;
	m_LifeTimeCount = 0.0f;
}

void Bullet::OnCollisionEnter(Collider* otherCollider)
{
	if (m_IsHit)
		return;

	if (otherCollider)
	{
		m_IsHit = true;

		auto* explosion = Manager::GetScene()->AddGameObject<Explosion>(RENDER_LAYER_PARTICLE);
		explosion->SetPosition(Position);
		explosion->EmissionParticle(Position, 1);

		SetDestory();
	}
}

