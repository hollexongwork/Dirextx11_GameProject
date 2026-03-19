#include "main.h"
#include "Renderer.h"
#include "Texture.h"
#include "Manager.h"
#include "Scene.h"
#include "Camera.h"
#include "Functions.h"
#include "SmokeParticle.h"

void SmokeParticle::Init()
{
	Particle::Init();

	// Load Shader
	Renderer::CreateVertexShader(&m_VertexShader, &m_VertexLayout, "shader/cso/PBRVS.cso");
	Renderer::CreatePixelShader(&m_PixelShader, "shader/cso/LitParticle.cso");

	// Load Texture
	m_Texture = Texture::Load("asset/texture/T_SoftSmokeBall_VL144_a.png");
	m_Normal = Texture::Load("asset/texture/T_BlurNormalSphere_N.png");

	// Material buffer
	D3D11_BUFFER_DESC bufferDesc{};
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = sizeof(float);

	bufferDesc.ByteWidth = sizeof(SmokeParticle::SnokeMaterial);
	Renderer::GetDevice()->CreateBuffer(&bufferDesc, nullptr, &m_MaterialBuffer);

	// Emitter
	Position = Vector3(0.0f, 0.0f, 0.0f);

	// Particle Parameters
	m_LifeDist = std::uniform_real_distribution<float>(1.0f, 1.5f);
	m_SizeDist = std::uniform_real_distribution<float>(0.78f, 0.945f);
	m_RotaDist = std::uniform_real_distribution<float>(0.0f, 360.0f);
	m_RotaSpeedDist = std::uniform_real_distribution<float>(-45.0f, 45.0f);
	m_VelocityDistX = std::uniform_real_distribution<float>(0.0f, 0.0f);
	m_VelocityDistY = std::uniform_real_distribution<float>(0.0f, 0.25f);
	m_VelocityDistZ = std::uniform_real_distribution<float>(0.0f, 0.0f);

	m_Drag = 0.5f;
}

void SmokeParticle::Uninit()
{
	Particle::Uninit();
	if (m_MaterialBuffer) { m_MaterialBuffer->Release(); m_MaterialBuffer = nullptr; }
}

void SmokeParticle::Update()
{
	float deltaTime = Time::GetDeltaTime();

	m_EmissionAccumulator += m_EmissionRate * deltaTime;

	int emitCount = static_cast<int>(m_EmissionAccumulator);
	if (emitCount > 0)
	{
		for (int i = 0; i < emitCount; i++)
		{
			EmissionParticle();
		}
		m_EmissionAccumulator -= emitCount;
	}

	for (int i = 0; i < PARTICLE_MAX; i++)
	{
		if (m_Particle[i].Enable == true)
		{
			//Life Time
			m_Particle[i].Life -= deltaTime;
			float lifeRatio = m_Particle[i].Life / m_Particle[i].InitialLife;
			float life = Clamp(1.0f - lifeRatio, 0.0f, 1.0f);

			//SubUV
			m_Material[i].Frame = static_cast<int>(life * 144.0f); // 12x12

			//Position
			m_Particle[i].Velocity -= m_Particle[i].Velocity * m_Drag * deltaTime;
			m_Particle[i].Position += m_Particle[i].Velocity * deltaTime;

			//Rotation
			m_Particle[i].Rotation.z += m_RotaSpeed[i];

			//Size Over Life Time
			m_Particle[i].Size = m_Particle[i].InitialSize * Lerp(0.25f, 1.0f, life);

			//Color Over Life Time
			float t = (life - 0.5f) / 0.5f;
			t = Clamp(t, 0.0f, 1.0f);
			m_Material[i].Color.w = Lerp(1.0f, 0.0f, t);

			//RotaSpeed Over Life Time
			m_RotaSpeed[i] *= Lerp(0.0f, 1.0f, t);

			if (m_Particle[i].Life <= 0)
			{
				m_Particle[i].Enable = false;
			}
		}
	}
}

void SmokeParticle::Draw()
{
	//------------------------------------------------------------------------------------------
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
	Renderer::GetDeviceContext()->PSSetShaderResources(1, 1, &m_Normal);
	ID3D11ShaderResourceView* depthSRV = Renderer::GetDepthSRV();
	Renderer::GetDeviceContext()->PSSetShaderResources(2, 1, &depthSRV);

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

void SmokeParticle::EmissionParticle()
{
	for (int i = 0; i < PARTICLE_MAX; i++)
	{
		if (!m_Particle[i].Enable)
		{
			float life;

			Vector3 rota;
			float rotaSpeed;

			Vector3 size;

			Vector3 velocity;

			{
				std::lock_guard<std::mutex> lock(engine_mutex);
				life = m_LifeDist(engine);

				rota = Vector3(m_RotaDist(engine), m_RotaDist(engine), m_RotaDist(engine));
				rotaSpeed = m_RotaSpeedDist(engine);

				size = Vector3(m_SizeDist(engine), m_SizeDist(engine), m_SizeDist(engine));

				velocity = Vector3(m_VelocityDistX(engine), m_VelocityDistY(engine), m_VelocityDistZ(engine));
			}

			m_Particle[i].Enable = true;

			m_Particle[i].Life = life;
			m_Particle[i].InitialLife = life;

			m_Particle[i].Position = Position;

			m_Particle[i].Rotation = rota;
			m_RotaSpeed[i] = rotaSpeed;

			m_Particle[i].Size = size;
			m_Particle[i].InitialSize = size;

			m_Particle[i].Velocity = velocity;

			break;
		}
	}
}