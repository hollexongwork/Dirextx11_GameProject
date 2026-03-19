#include "main.h"
#include "Renderer.h"
#include "Texture.h"
#include "Manager.h"
#include "Scene.h"
#include "Camera.h"
#include "ShootSmoke.h"

void ShootSmoke::Init()
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

	bufferDesc.ByteWidth = sizeof(ShootSmoke::Material);
	Renderer::GetDevice()->CreateBuffer(&bufferDesc, nullptr, &m_MaterialBuffer);

	// Emitter
	Position = Vector3(0.0f, 0.0f, 0.0f);

	// Particle Parameters
	m_LifeDist = std::uniform_real_distribution<float>(1.5f, 2.0f);
	m_RotaDist = std::uniform_real_distribution<float>(0.0f, 360.0f);
	m_RotaSpeedDist = std::uniform_real_distribution<float>(-45.0f, 45.0f);
	m_SizeDist = std::uniform_real_distribution<float>(0.6, 0.75f);
	m_VelocityDistX = std::uniform_real_distribution<float>(-10.0, 10.0f);
	m_VelocityDistY = std::uniform_real_distribution<float>(0.0, 5.0f);
	m_VelocityDistZ = std::uniform_real_distribution<float>(4.5, 30.0f);
}

void ShootSmoke::Uninit()
{
	Particle::Uninit();

	if (m_MaterialBuffer) { m_MaterialBuffer->Release(); m_MaterialBuffer = nullptr; }
}

void ShootSmoke::Update()
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
			m_Material[i].Frame = Clamp(static_cast<int>(life * 144.0f), 0, 143);// 12x12

			//Position
			{
				float t = Clamp(life / 0.1f, 0.0f, 1.0f);// (life / end time)
				Drag[i] = Lerp(0.0f, 50.0f, t);
			}

			m_Particle[i].Position += m_Particle[i].Velocity * deltaTime;
			m_Particle[i].Velocity -= m_Particle[i].Velocity * Drag[i] * deltaTime;

			//Rotation
			m_Particle[i].Rotation.z += m_RotaSpeed[i] * deltaTime;

			//Rota Speed Over Life Time
			m_RotaSpeed[i] = m_InitialRotaSpeed[i] * Lerp(1.0f, 0.1f, life);

			//Size Over Life Time
			{
				float easedT = 1.0f - powf(1.0f - life, 3.0f);
				m_Particle[i].Size = m_Particle[i].InitialSize * Lerp(1.0f, 4.0f, easedT);
			}

			//Color Over Life Time
			{
				// RGB
				float t = Clamp(life / 0.1f, 0.0f, 1.0f); // (life / end time)
				m_Material[i].Color.x = Lerp(m_InitialColor.x, 0.75f, t);
				m_Material[i].Color.y = Lerp(m_InitialColor.y, 0.75f, t);
				m_Material[i].Color.z = Lerp(m_InitialColor.z, 0.75f, t);
			}

			{
				// Alpha
				float t = (life - 0.3f) / 0.7f; // (life - start time) / (1.0f - start time)
				t = Clamp(t, 0.0f, 1.0f);
				m_Material[i].Color.w = 0.5f * powf(1.0f - t, 2.5f);
			}

			if (m_Particle[i].Life <= 0)
			{
				m_Particle[i].Enable = false;
			}
		}		
	}
}

void ShootSmoke::Draw()
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

void ShootSmoke::EmissionParticle(Vector3 position, Vector3 direction,int Count)
{
	int count = 0;
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

			m_Particle[i].Position = position;

			m_Particle[i].Rotation = rota;
			m_RotaSpeed[i] = rotaSpeed;

			m_Particle[i].Size = size;
			m_Particle[i].InitialSize = size;

			Vector3 worldUp = fabsf(direction.y) > 0.99999f ? Vector3(1, 0, 0) : Vector3(0, 1, 0);

			Vector3 right = Vector3::Cross(worldUp, direction);
			right.normalize();

			Vector3 up = Vector3::Cross(direction, right);

			m_Particle[i].Velocity = right * velocity.x + up * velocity.y + direction * velocity.z;

			count++;

			if (count >= Count)
			{
				break;
			}
		}
	}
}
