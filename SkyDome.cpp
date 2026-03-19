#include "main.h"
#include "Renderer.h"
#include "Model.h"
#include "Texture.h"
#include "Manager.h"
#include "Scene.h"
#include "SkyDome.h"
#include "Camera.h"

MODEL* SkyDome::m_Model;

ID3D11InputLayout* SkyDome::m_VertexLayout;
ID3D11VertexShader* SkyDome::m_VertexShader;
ID3D11PixelShader* SkyDome::m_PixelShader;

ID3D11ShaderResourceView* SkyDome::m_Texture;

void SkyDome::Load()
{
	//Load Model 
	m_Model = ModelLoad("asset/model/Dome.fbx");

	//Load Texture 
	m_Texture = Texture::Load("asset/texture/evening_road_01_puresky.jpg");

	//Load Shader
	Renderer::CreateVertexShader(&m_VertexShader, &m_VertexLayout, "shader/cso/unlitTextureVS.cso");
	Renderer::CreatePixelShader(&m_PixelShader, "shader/cso/unlitTexturePS.cso");
}

void SkyDome::Init()
{
	Rotation = Vector3(-90.0f, 0.0f, 0.0f);
	Scale = Vector3(10.0f, 10.0f, 10.0f);
}

void SkyDome::Uninit()
{
	if (m_VertexLayout) { m_VertexLayout->Release(); m_VertexLayout = nullptr; }
	if (m_VertexShader) { m_VertexShader->Release(); m_VertexShader = nullptr; }
	if (m_PixelShader) { m_PixelShader->Release(); m_PixelShader = nullptr; }

	ModelRelease(m_Model);
}

void SkyDome::Update()
{
	Camera* camera = Manager::GetScene()->GetGameObject<Camera>();
	Position = camera->GetPosition();
}

void SkyDome::Draw()
{
	Renderer::GetDeviceContext()->IASetInputLayout(m_VertexLayout);

	//Texture Setting
	Renderer::GetDeviceContext()->PSSetShaderResources(0, 1, &m_Texture);

	//shader setting
	Renderer::GetDeviceContext()->VSSetShader(m_VertexShader, NULL, 0);
	Renderer::GetDeviceContext()->PSSetShader(m_PixelShader, NULL, 0);

	//matrix setting
	XMMATRIX RotationMatrix = XMMatrixRotationRollPitchYaw(XMConvertToRadians(Rotation.x), XMConvertToRadians(Rotation.y), XMConvertToRadians(Rotation.z));
	XMMATRIX ScaleMatrix = XMMatrixScaling(Scale.x, Scale.y, Scale.z);
	XMMATRIX PositionMatrix = XMMatrixTranslation(Position.x, Position.y, Position.z);

	XMMATRIX world = ScaleMatrix * RotationMatrix * PositionMatrix;

	Renderer::SetWorldMatrix(world);

	//Material Setting
	MATERIAL material{};
	material.Diffuse = { 1.0f,1.0f,1.0f,1.0f };
	material.TextureEnable = true;
	Renderer::SetMaterial(material);

	//Draw
	ModelDraw(m_Model);
}
