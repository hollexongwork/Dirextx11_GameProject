#include "main.h"
#include "Renderer.h"
#include "Texture.h"
#include "Time.h"
#include "Fade.h"

FADE_MODE  Fade::m_FadeMode = FADE_MODE_NONE;
FADE_STATE Fade::m_FadeState = FADE_STATE_STANDBY;
float Fade::m_PlayTime = 0.0f;

void Fade::Init()
{
	VERTEX_3D vertex[4];

	vertex[0].Position = XMFLOAT3(-0.5f, -0.5f, 0.0f);
	vertex[0].Normal = XMFLOAT3(0.0f, 0.0f, 0.0f);
	vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[0].TexCoord = XMFLOAT2(0.0f, 0.0f);

	vertex[1].Position = XMFLOAT3(0.5f, -0.5f, 0.0f);
	vertex[1].Normal = XMFLOAT3(0.0f, 0.0f, 0.0f);
	vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[1].TexCoord = XMFLOAT2(1.0f, 0.0f);

	vertex[2].Position = XMFLOAT3(-0.5f, 0.5f, 0.0f);
	vertex[2].Normal = XMFLOAT3(0.0f, 0.0f, 0.0f);
	vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[2].TexCoord = XMFLOAT2(0.0f, 1.0f);

	vertex[3].Position = XMFLOAT3(0.5f, 0.5f, 0.0f);
	vertex[3].Normal = XMFLOAT3(0.0f, 0.0f, 0.0f);
	vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[3].TexCoord = XMFLOAT2(1.0f, 1.0f);

	D3D11_BUFFER_DESC bd{};

	//bd.Usage = D3D11_USAGE_DEFAULT;
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4 /*NUM_VERTEX*/; //頂点バッファの量
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	//bd.CPUAccessFlags = 0;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA sd{};
	sd.pSysMem = vertex;

	Renderer::GetDevice()->CreateBuffer(&bd, &sd, &m_VertexBuffer);

	//Shader Set
	Renderer::CreateVertexShader(&m_VertexShader, &m_VertexLayout, "shader/cso/unlitTextureVS.cso");
	Renderer::CreatePixelShader(&m_PixelShader, "shader/cso/unlitTexturePS.cso");

	//Texture Load
	m_Texture = Texture::Load("asset/texture/Fade.png");

	Position = Vector3(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f, 0.0f);
	Rotation = Vector3(0.0f, 0.0f, 0.0f);
	Scale = Vector3(SCREEN_WIDTH, SCREEN_HEIGHT, 1.0f);

	Fade.Diffuse = XMFLOAT4( 0.0f,0.0f,0.0f,0.0f );
	Fade.TextureEnable = true;

	//m_FadeMode = FADE_MODE_NONE;
	//m_FadeState = FADE_STATE_STANDBY;

	//m_PlayTime = 0.0f;
	m_TimeCount = 0.0f;
}

void Fade::Uninit()
{
	if (m_VertexBuffer) { m_VertexBuffer->Release(); }
	if (m_VertexLayout) { m_VertexLayout->Release(); }
	if (m_VertexShader) { m_VertexShader->Release(); }
	if (m_PixelShader) { m_PixelShader->Release(); }
}

void Fade::Update()
{
	if (m_FadeState == FADE_STATE_STANDBY)
		return;

	if (m_FadeState == FADE_STATE_STANDBY || m_FadeState == FADE_STATE_CHANGE)
	{
		return;
	}

	if (m_FadeState == FADE_STATE_IN)
	{
		Fade.Diffuse.w = 1.0f - (1.0f * (m_TimeCount / m_PlayTime));
	}
	else if (m_FadeState == FADE_STATE_OUT)
	{
		Fade.Diffuse.w = 1.0f * (m_TimeCount / m_PlayTime);
	}

	if (m_TimeCount >= m_PlayTime)
	{
		Fade.Diffuse.w = m_FadeState == FADE_STATE_IN ? 0.0f : 1.0f;
		m_FadeState = m_FadeState == FADE_STATE_IN ? FADE_STATE_STANDBY : FADE_STATE_CHANGE;
	}
	else
	{
		m_TimeCount += Time::GetDeltaTime();
	}
	
}

void Fade::Draw()
{
	if (m_FadeState == FADE_STATE_STANDBY)
		return;

	Renderer::GetDeviceContext()->IASetInputLayout(m_VertexLayout);

	//shader setting
	Renderer::GetDeviceContext()->VSSetShader(m_VertexShader, NULL, 0);
	Renderer::GetDeviceContext()->PSSetShader(m_PixelShader, NULL, 0);

	//matrix setting
	Renderer::SetWorldViewProjection2D();
	XMMATRIX PivotMatrix = XMMatrixTranslation(Pivot.x, Pivot.y, Pivot.z);
	XMMATRIX RotationMatrix = XMMatrixRotationRollPitchYaw(XMConvertToRadians(Rotation.x), XMConvertToRadians(Rotation.y), XMConvertToRadians(Rotation.z));
	XMMATRIX ScaleMatrix = XMMatrixScaling(Scale.x, Scale.y, Scale.z);
	XMMATRIX PositionMatrix = XMMatrixTranslation(Position.x, Position.y, Position.z);

	XMMATRIX world = ScaleMatrix * RotationMatrix * PositionMatrix;

	Renderer::SetWorldMatrix(world);

	//Material Setting
	Renderer::SetMaterial(Fade);

	//Vertex Buffer Setting
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	Renderer::GetDeviceContext()->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);

	//Texture Setting
	Renderer::GetDeviceContext()->PSSetShaderResources(0, 1, &m_Texture);

	//Primitive Topology Setting
	Renderer::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// ★★★★★★★★★★★★★ポリゴン描画★★★★★★★★★★★★★
	Renderer::GetDeviceContext()->Draw(4, 0);//頂点NUM_VERTEX個描画する
}

void Fade::PlayFade(FADE_MODE mode, float playTime)
{
	m_FadeMode = mode;
	m_PlayTime = playTime;

	if (mode == FADE_MODE_IN)
	{
		Fade.Diffuse.w = 1.0f;
		m_FadeState = FADE_STATE_IN;
	}
	else
	{
		Fade.Diffuse.w = 0.0f;
		m_FadeState = FADE_STATE_OUT;
	}
	m_TimeCount = 0.0f;
}

