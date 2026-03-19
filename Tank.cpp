#include "main.h"
#include "Renderer.h"
#include "Texture.h"
#include "Manager.h"
#include "Scene.h"
#include "Tank.h"
#include "Camera.h"

ID3D11InputLayout* Tank::m_VertexLayout;
ID3D11VertexShader* Tank::m_VertexShader;
ID3D11PixelShader* Tank::m_PixelShader;

ID3D11ShaderResourceView* Tank::m_Texture[3];
ID3D11ShaderResourceView* Tank::m_NormalMap[3];
ID3D11ShaderResourceView* Tank::m_ARM[3];
ID3D11ShaderResourceView* Tank::m_Emission;

ID3D11Buffer* Tank::m_MaterialBuffer;


void Tank::Load()
{
	LoadShader();
	LoadTexture();

	CreateMaterialBuffer();
}

void Tank::Init()
{
	GameObject::Init();
	LoadModel();
	InitMaterials();
}

void Tank::Start()
{
	GameObject::Start();
	m_Camera = Manager::GetScene()->GetGameObject<Camera>();
}

void Tank::Uninit()
{
	if (m_Model) { m_Model = nullptr; }

	if (m_VertexLayout) { m_VertexLayout->Release(); m_VertexLayout = nullptr; }
	if (m_VertexShader) { m_VertexShader->Release(); m_VertexShader = nullptr; }
	if (m_PixelShader) { m_PixelShader->Release(); m_PixelShader = nullptr; }
	if (m_MaterialBuffer) { m_MaterialBuffer->Release(); m_MaterialBuffer = nullptr; }
}

void Tank::Update()
{
	GameObject::Update();
}

void Tank::DrawShadow()
{
	m_Model->Draw();
}

void Tank::Draw()
{
	if (!m_Camera->isInView(Position, CULL_RADIUS))
	{
		return;
	}

	//shader setting
	Renderer::GetDeviceContext()->IASetInputLayout(m_VertexLayout);
	Renderer::GetDeviceContext()->VSSetShader(m_VertexShader, NULL, 0);
	Renderer::GetDeviceContext()->PSSetShader(m_PixelShader, NULL, 0);

	//matrix setting
	XMMATRIX RotationMatrix = XMMatrixRotationRollPitchYaw(0.0f, 0.0f, 0.0f);
	XMMATRIX ScaleMatrix = XMMatrixScaling(1.0f, 1.0f, 1.0f);
	XMMATRIX PositionMatrix = XMMatrixTranslation(0.0f, 0.0f, 0.0f);

	XMMATRIX world = ScaleMatrix * RotationMatrix * PositionMatrix;

	Renderer::SetWorldMatrix(world);

	ID3D11ShaderResourceView* depthSRV = Renderer::GetDepthSRV();
	Renderer::GetDeviceContext()->PSSetShaderResources(3, 1, &depthSRV);

	// Draw each mesh with appropriate textures based on material index
	for (unsigned int m = 0; m < m_Model->GetAiScene()->mNumMeshes; m++)
	{
		aiMesh* mesh = m_Model->GetAiScene()->mMeshes[m];

		// Set textures based on material index
		switch (mesh->mMaterialIndex)
		{
		case 0:
			Renderer::GetDeviceContext()->UpdateSubresource(m_MaterialBuffer, 0, nullptr, &m_BodyMaterial, 0, 0);
			Renderer::GetDeviceContext()->PSSetConstantBuffers(7, 1, &m_MaterialBuffer);

			Renderer::GetDeviceContext()->PSSetShaderResources(0, 1, &m_Texture[mesh->mMaterialIndex]);
			Renderer::GetDeviceContext()->PSSetShaderResources(1, 1, &m_NormalMap[mesh->mMaterialIndex]);
			Renderer::GetDeviceContext()->PSSetShaderResources(2, 1, &m_ARM[mesh->mMaterialIndex]);

			Renderer::GetDeviceContext()->PSSetShaderResources(4, 1, &m_Texture[2]);
			Renderer::GetDeviceContext()->PSSetShaderResources(5, 1, &m_NormalMap[2]);
			Renderer::GetDeviceContext()->PSSetShaderResources(6, 1, &m_ARM[2]);

			Renderer::GetDeviceContext()->PSSetShaderResources(7, 1, &m_Emission);
			m_Model->DrawMesh(m);
			break;
		case 1:
			Renderer::GetDeviceContext()->UpdateSubresource(m_MaterialBuffer, 0, nullptr, &m_LeftTrackMaterial, 0, 0);
			Renderer::GetDeviceContext()->PSSetConstantBuffers(7, 1, &m_MaterialBuffer);

			Renderer::GetDeviceContext()->PSSetShaderResources(0, 1, &m_Texture[mesh->mMaterialIndex]);
			Renderer::GetDeviceContext()->PSSetShaderResources(1, 1, &m_NormalMap[mesh->mMaterialIndex]);
			Renderer::GetDeviceContext()->PSSetShaderResources(2, 1, &m_ARM[mesh->mMaterialIndex]);

			Renderer::SetCullMode(D3D11_CULL_NONE);
			m_Model->DrawMesh(m);
			break;
		case 2:
			Renderer::GetDeviceContext()->UpdateSubresource(m_MaterialBuffer, 0, nullptr, &m_RightTrackMaterial, 0, 0);
			Renderer::GetDeviceContext()->PSSetConstantBuffers(7, 1, &m_MaterialBuffer);

			Renderer::GetDeviceContext()->PSSetShaderResources(0, 1, &m_Texture[1]);
			Renderer::GetDeviceContext()->PSSetShaderResources(1, 1, &m_NormalMap[1]);
			Renderer::GetDeviceContext()->PSSetShaderResources(2, 1, &m_ARM[1]);

			m_Model->DrawMesh(m);
			Renderer::SetCullMode(D3D11_CULL_BACK);
			break;
		}
	}
}

void Tank::LoadModel()
{ 
	m_Model = new AnimationModel();
	m_Model->Load("asset/model/SK_West_Tank_M1A1Abrams.fbx");
	// Initial Joint Pose
	m_Model->Pose();
}

void Tank::LoadShader()
{
	Renderer::CreateVertexShader(&m_VertexShader, &m_VertexLayout, "shader/cso/PBRVS.cso");
	Renderer::CreatePixelShader(&m_PixelShader, "shader/cso/PBR_ARM_PS.cso");
}

void Tank::LoadTexture()
{
	m_Texture[0] = Texture::Load("asset/texture/T_West_Tank_M1A1Abrams_D_Desert.png",true);
	m_Texture[1] = Texture::Load("asset/texture/T_West_Tank_M1A1Abrams_Track_D.png", true);
	m_Texture[2] = Texture::Load("asset/texture/T_West_Tank_M1A1Abrams_Damaged_D.png", true);

	m_NormalMap[0] = Texture::Load("asset/texture/T_West_Tank_M1A1Abrams_N.png");
	m_NormalMap[1] = Texture::Load("asset/texture/T_West_Tank_M1A1Abrams_Track_N.png");
	m_NormalMap[2] = Texture::Load("asset/texture/T_West_Tank_M1A1Abrams_Damaged_N.png");

	m_ARM[0] = Texture::Load("asset/texture/T_West_Tank_M1A1Abrams_ARM.png");
	m_ARM[1] = Texture::Load("asset/texture/T_West_Tank_M1A1Abrams_Track_ARM.png");
	m_ARM[2] = Texture::Load("asset/texture/T_West_Tank_M1A1Abrams_Damaged_ARM.png");

	m_Emission = Texture::Load("asset/texture/T_West_Tank_M1A1Abrams_E.png");
}

void Tank::InitMaterials()
{
	auto initMaterial = [](Material& mat)
		{
			mat.UV = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
		};
	initMaterial(m_BodyMaterial);
	initMaterial(m_LeftTrackMaterial);
	initMaterial(m_RightTrackMaterial);
}

void Tank::CreateMaterialBuffer()
{
	D3D11_BUFFER_DESC bufferDesc{};
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = sizeof(float);

	bufferDesc.ByteWidth = sizeof(Material);
	Renderer::GetDevice()->CreateBuffer(&bufferDesc, nullptr, &m_MaterialBuffer);
}

