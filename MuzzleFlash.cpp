#include "main.h"
#include "Renderer.h"
#include "Texture.h"
#include "Manager.h"
#include "Scene.h"
#include "Camera.h"
#include "MuzzleFlash.h"

void MuzzleFlash::Init()
{
	Particle::Init();

	// Load Shader
	Renderer::CreateVertexShader(&m_VertexShader, &m_VertexLayout, "shader/cso/unlitTextureVS.cso");
	Renderer::CreatePixelShader(&m_PixelShader, "shader/cso/MuzzleFlash.cso");

	// Load Texture
	m_Texture = Texture::Load("asset/texture/T_2X2_MuzzlePoint_01_CH.png");

	// Material buffer
	D3D11_BUFFER_DESC bufferDesc{};
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = sizeof(float);

	bufferDesc.ByteWidth = sizeof(MuzzleFlash::Material);
	Renderer::GetDevice()->CreateBuffer(&bufferDesc, nullptr, &m_MaterialBuffer);

	// Emitter
	Position = Vector3(0.0f, 0.0f, 0.0f);

	//Particle Parameters
	m_LifeDist = std::uniform_real_distribution<float>(0.06f, 0.065f);
	m_SizeDist = std::uniform_real_distribution<float>(0.8, 0.95f);
	m_RotaDist = std::uniform_real_distribution<float>(0.0f, 360.0f);


}

void MuzzleFlash::Uninit()
{
	Particle::Uninit();

	if (m_MaterialBuffer) { m_MaterialBuffer->Release(); m_MaterialBuffer = nullptr; }
}

void MuzzleFlash::Update()
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

			//Size Over Life Time
			float scale = (life < 0.1f)?
						   Lerp(0.4f, 1.0f, life / 0.1f) : 
						   Lerp(1.0f, 0.6f, (life - 0.1f) / 0.9f);

			m_Particle[i].Size = m_Particle[i].InitialSize * scale;

			//Color Over Life Time
			m_Material[i].Color.w = Lerp(1.0f, 0.0f, life);

			float t = (life - 0.3f) / 0.7f; // (life - start time) / (1.0f - start time)
			t = Clamp(t, 0.0f, 1.0f);
			m_Material[i].Brightness = Lerp(15.0f, 3.0f, t);

			if (m_Particle[i].Life <= 0)
			{
				m_Particle[i].Enable = false;
			}
		}		
	}
}

void MuzzleFlash::Draw()
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

			Renderer::SetAddBlendEnable(true);
			Renderer::GetDeviceContext()->Draw(4, 0);
			Renderer::SetAlphaBlendEnable(true);
		}
	}
}

void MuzzleFlash::EmissionParticle(Vector3 position, int Count)
{
	for (int i = 0; Count > 0; i++)
	{
		if (!m_Particle[i].Enable)
		{
			Count--;

			float life;
			Vector3 size;
			Vector3 rota;

			{
				std::lock_guard<std::mutex> lock(engine_mutex);
				life = m_LifeDist(engine);
				size = Vector3(m_SizeDist(engine), m_SizeDist(engine), m_SizeDist(engine));
				rota = Vector3(m_RotaDist(engine), m_RotaDist(engine), m_RotaDist(engine));

				std::uniform_int_distribution<int> frameDist(0, 3);
				m_Material[i].Frame = frameDist(engine);
			}

			m_Particle[i].Enable = true;
			m_Particle[i].InitialLife = life;
			m_Particle[i].Life = life;
			m_Particle[i].Position = position;
			m_Particle[i].Size = size;
			m_Particle[i].InitialSize = size;
			m_Particle[i].Rotation = rota;
		}
	}
}
