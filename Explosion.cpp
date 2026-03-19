#include "main.h"
#include "Renderer.h"
#include "Texture.h"
#include "Manager.h"
#include "Scene.h"
#include "Camera.h"
#include "Explosion.h"

void Explosion::Init()
{
	Particle::Init();

	// Load Shader
	Renderer::CreateVertexShader(&m_VertexShader, &m_VertexLayout, "shader/cso/unlitTextureVS.cso");
	Renderer::CreatePixelShader(&m_PixelShader, "shader/cso/Particle.cso");

	// Load Texture
	m_Texture = Texture::Load("asset/texture/T_Sub_EXP_Large_001_01.png");

	// Material buffer
	D3D11_BUFFER_DESC bufferDesc{};
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = sizeof(float);

	bufferDesc.ByteWidth = sizeof(Explosion::Material);
	Renderer::GetDevice()->CreateBuffer(&bufferDesc, nullptr, &m_MaterialBuffer);

	// Emitter
	Position = Vector3(0.0f, 0.0f, 0.0f);

	// Particle Parameters
	m_SizeDist = std::uniform_real_distribution<float>(5.0, 6.25f);

	for (int i = 0; i < PARTICLE_MAX; i++)
	{
		m_Particle[i].InitialLife = m_Life;
	}
}

void Explosion::Uninit()
{
	Particle::Uninit();

	if (m_MaterialBuffer) { m_MaterialBuffer->Release(); m_MaterialBuffer = nullptr; }
}

void Explosion::Update()
{
	float deltaTime = Time::GetDeltaTime();

	for (int i = 0; i < PARTICLE_MAX; i++)
	{
		if (m_Particle[i].Enable == true)
		{
			//Life Time
			m_Particle[i].Life -= deltaTime;

			float lifeRatio = (m_Particle[i].InitialLife > 0.0f) ?
							   m_Particle[i].Life / m_Particle[i].InitialLife :
							   0.0f;

			float life = Clamp(1.0f - lifeRatio, 0.0f, 1.0f);

			//SubUV
			m_Material[i].Frame = Clamp(static_cast<int>(life * 64.0f), 0, 63);// 8x8

			// Alpha
			float t = (life - 0.578f) / (1.0f - 0.578f); // (life - start time) / (1.0f - start time)
			t = Clamp(t, 0.0f, 1.0f);
			m_Material[i].Color.w = powf(1.0f - t, 2.5f);

			if (m_Particle[i].Life <= 0)
			{
				m_Particle[i].Enable = false;
				SetDestory();
			}
		}		
	}
}

void Explosion::Draw()
{
	//shader setting
	Renderer::GetDeviceContext()->IASetInputLayout(m_VertexLayout);
	Renderer::GetDeviceContext()->VSSetShader(m_VertexShader, NULL, 0);
	Renderer::GetDeviceContext()->PSSetShader(m_PixelShader, NULL, 0);

	//Billboard Matrix Setting
	Camera* camera = Manager::GetScene()->GetGameObject<Camera>();
	if (!camera) return;

	XMMATRIX view;
	view = camera->GetCmeraView();

	XMMATRIX inView;
	inView = XMMatrixInverse(nullptr, view);
	inView.r[3].m128_f32[0] = 0.0f;
	inView.r[3].m128_f32[1] = 0.0f;
	inView.r[3].m128_f32[2] = 0.0f;

	//Vertex Buffer Setting
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	Renderer::GetDeviceContext()->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);

	//Texture Setting
	Renderer::GetDeviceContext()->PSSetShaderResources(0, 1, &m_Texture);
	ID3D11ShaderResourceView* depthSRV = Renderer::GetDepthSRV();
	Renderer::GetDeviceContext()->PSSetShaderResources(1, 1, &depthSRV);

	//Primitive Topology Setting
	Renderer::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	for (int i = 0; i < PARTICLE_MAX; i++)
	{
		if (m_Particle[i].Enable == true)
		{
			//Material Setting
			Renderer::GetDeviceContext()->UpdateSubresource(m_MaterialBuffer, 0, nullptr, &m_Material[i], 0, 0);
			Renderer::GetDeviceContext()->PSSetConstantBuffers(7, 1, &m_MaterialBuffer);
			//matrix setting
			XMMATRIX ScaleMatrix = XMMatrixScaling(m_Particle[i].Size.x, m_Particle[i].Size.y, m_Particle[i].Size.z);
			XMMATRIX RotationMatrix = XMMatrixRotationZ(XMConvertToRadians(m_Particle[i].Rotation.z));
			XMMATRIX PositionMatrix = XMMatrixTranslation(m_Particle[i].Position.x, m_Particle[i].Position.y, m_Particle[i].Position.z);

			XMMATRIX world = ScaleMatrix * RotationMatrix * inView * PositionMatrix;

			Renderer::SetWorldMatrix(world);

			Renderer::GetDeviceContext()->Draw(4, 0);
		}
	}
}

void Explosion::EmissionParticle(Vector3 position, int Count)
{
	for (int i = 0; i < PARTICLE_MAX && Count > 0; i++)
	{
		if (!m_Particle[i].Enable)
		{
			Count--;

			Vector3 size;
			size = Vector3(m_SizeDist(engine), m_SizeDist(engine), m_SizeDist(engine));

			m_Particle[i].Enable = true;

			m_Particle[i].Life = m_Life;

			m_Particle[i].Size = size;

			position.y += size.y * 0.484375f;

			m_Particle[i].Position = position;
		}
	}
}
