#include "main.h"
#include "Renderer.h"
#include "Model.h"
#include "Texture.h"
#include "Manager.h"
#include "Scene.h"
#include "BoxCollider.h"
#include "Camera.h"
#include "Building.h"
#include "MeshField.h"

MODEL* Building::m_Model[5];

ID3D11InputLayout* Building::m_VertexLayout;
ID3D11VertexShader* Building::m_VertexShader;
ID3D11PixelShader* Building::m_PixelShader;

ID3D11ShaderResourceView* Building::m_Texture[5];

void Building::Load()
{
	// Load Model
	m_Model[0] = ModelLoad("asset/model/Hangar_v0.fbx");
	m_Model[1] = ModelLoad("asset/model/Hangar_v1.fbx");
	m_Model[2] = ModelLoad("asset/model/Hangar_v2.fbx");
	m_Model[3] = ModelLoad("asset/model/Hangar_v3.fbx");
	m_Model[4] = ModelLoad("asset/model/Hangar_v4.fbx");

	// Load Shader
	Renderer::CreateVertexShader(&m_VertexShader, &m_VertexLayout, "shader/cso/unlitTextureVS.cso");
	Renderer::CreatePixelShader(&m_PixelShader, "shader/cso/unlitTextureFog.cso");

	// Load Texture
	m_Texture[0] = Texture::Load("asset/texture/Hangar_v0.png", true);
	m_Texture[1] = Texture::Load("asset/texture/Hangar_v1.png", true);
	m_Texture[2] = Texture::Load("asset/texture/Hangar_v2.png", true);
	m_Texture[3] = Texture::Load("asset/texture/Hangar_v3.png", true);
	m_Texture[4] = Texture::Load("asset/texture/Hangar_v4.png", true);
}

void Building::Init()
{
	RotationMatrix = XMMatrixRotationRollPitchYaw(XMConvertToRadians(Rotation.x), XMConvertToRadians(Rotation.y), XMConvertToRadians(Rotation.z));

	m_ColliderScale[0] = Vector3(6.2f, 2.5f, 3.45f);  
	m_ColliderScale[1] = Vector3(2.7f, 2.4f, 3.1f);   
	m_ColliderScale[2] = Vector3(2.75f, 3.2f, 5.5f);  
	m_ColliderScale[3] = Vector3(6.6f, 2.7f, 3.3f);   
	m_ColliderScale[4] = Vector3(9.38f, 2.7f, 5.36f); 

	auto* col = AddComponent<BoxCollider>(m_ColliderScale[m_Index]);
	col->SetTag(CollisionTag::Building);
	col->SetActive(false);
	col->SetBodyType(BodyType::Static);
}

void Building::Start()
{
	m_Camera = Manager::GetScene()->GetGameObject<Camera>();

	MeshField* field = Manager::GetScene()->GetGameObject<MeshField>();
	FieldHit hit = field->GetFieldHeightAndNormal(Position);
	Position.y = hit.Height;

	Vector3 forward = GetAxisZ();
	Vector3 up = hit.Normal;
	up.normalize();
	Vector3 right = Vector3::Cross(up, forward);
	right.normalize();
	forward = Vector3::Cross(right, up);
	forward.normalize();
	RotationMatrix =
	{
		right.x, right.y, right.z, 0.0f,
		up.x, up.y, up.z, 0.0f,
		forward.x, forward.y, forward.z, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
	Position += up * 2.2f;
}

void Building::Uninit()
{
	for (int i = 0; i < 5; i++)
	{
		if (m_Model[i]) { m_Model[i] = nullptr; }
	}

	if (m_Camera) { m_Camera = nullptr; }

	if (m_VertexLayout) { m_VertexLayout->Release(); m_VertexLayout = nullptr; }
	if (m_VertexShader) { m_VertexShader->Release(); m_VertexShader = nullptr; }
	if (m_PixelShader) { m_PixelShader->Release(); m_PixelShader = nullptr; }
}

void Building::DrawShadow()
{
	ModelDraw(m_Model[m_Index]);
}

void Building::Draw()
{
	if (!m_Camera->isInView(Position, CULL_RADIUS))
	{
		return;
	}

	// Shader Setting
	Renderer::GetDeviceContext()->IASetInputLayout(m_VertexLayout);
	Renderer::GetDeviceContext()->VSSetShader(m_VertexShader, NULL, 0);
	Renderer::GetDeviceContext()->PSSetShader(m_PixelShader, NULL, 0);

	// Texture Setting
	Renderer::GetDeviceContext()->PSSetShaderResources(0, 1, &m_Texture[m_Index]);
	ID3D11ShaderResourceView* depthSRV = Renderer::GetDepthSRV();
	Renderer::GetDeviceContext()->PSSetShaderResources(1, 1, &depthSRV);

	// Matrix Setting
	XMMATRIX ScaleMatrix = XMMatrixScaling(Scale.x, Scale.y, Scale.z);
	XMMATRIX PositionMatrix = XMMatrixTranslation(Position.x, Position.y, Position.z);

	XMMATRIX world = ScaleMatrix * RotationMatrix * PositionMatrix;

	Renderer::SetWorldMatrix(world);

	// Material Setting
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	material.TextureEnable = true;
	material.UV_Offset = XMFLOAT2(1.0f, 1.0f);
	Renderer::SetMaterial(material);

	ModelDraw(m_Model[m_Index]);
}

void Building::SetModelIndex(int index)
{
	m_Index = index;
	auto* col = GetComponent<BoxCollider>();
	col->SetHalfSize(m_ColliderScale[m_Index]);
}

