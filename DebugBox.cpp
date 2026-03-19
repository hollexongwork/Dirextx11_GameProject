#include "main.h"
#include "Renderer.h"
#include "Model.h"
#include "Texture.h"
#include "Manager.h"
#include "Scene.h"
#include "Time.h"
#include "BoxCollider.h"
#include "Player.h"
#include "DebugBox.h"


void DebugBox::Init()
{
	m_Model = ModelLoad("asset/model/cube.fbx");

	// Load Shader
	Renderer::CreateVertexShader(&m_VertexShader, &m_VertexLayout, "shader/cso/unlitTextureVS.cso");
	Renderer::CreatePixelShader(&m_PixelShader, "shader/cso/unlitTexturePS.cso");
	
	// Load Texture 
	m_Texture = Texture::Load("asset/texture/field.png");


	Scale = { 1.0f,0.45f,1.95f };

	m_Offset = { 0.0f, 0.45f, 0.0f };
}

void DebugBox::Uninit()
{
	delete m_Model;
	m_Model = nullptr;

	if (m_VertexLayout) m_VertexLayout->Release();
	if (m_VertexShader) m_VertexShader->Release();
	if (m_PixelShader) m_PixelShader->Release();
}

void DebugBox::Update()
{
	Player* player = Manager::GetScene()->GetGameObject<Player>();

	Position = player->GetPosition();
	Position += player->GetAxisX() * m_Offset.x;
	Position += player->GetAxisY() * m_Offset.y;
	Position += player->GetAxisZ() * m_Offset.z;
	//Rotation = player->GetRotation();

	RotationMatrix = player->GetRotationMatrix();

}

void DebugBox::DrawShadow()
{

}

void DebugBox::Draw()
{
	Renderer::GetDeviceContext()->IASetInputLayout(m_VertexLayout);

	//shader setting
	Renderer::GetDeviceContext()->VSSetShader(m_VertexShader, NULL, 0);
	Renderer::GetDeviceContext()->PSSetShader(m_PixelShader, NULL, 0);

	Renderer::GetDeviceContext()->PSSetShaderResources(0, 1, &m_Texture);

	//matrix setting
	XMMATRIX ScaleMatrix = XMMatrixScaling(Scale.x, Scale.y, Scale.z);
	XMMATRIX PositionMatrix = XMMatrixTranslation(Position.x, Position.y, Position.z);

	XMMATRIX world = ScaleMatrix * RotationMatrix * PositionMatrix;

	Renderer::SetWorldMatrix(world);

	ModelDraw(m_Model);
}

