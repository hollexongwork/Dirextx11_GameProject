#include "main.h"
#include "Renderer.h"
#include "Texture.h"
#include "Sight.h"

void Sight::Init()
{
	Billboard::Init();

	// Load Shader
	Renderer::CreateVertexShader(&m_VertexShader, &m_VertexLayout, "shader/cso/unlitTextureVS.cso");
	Renderer::CreatePixelShader(&m_PixelShader, "shader/cso/unlitTexturePS.cso");
	
	// Load Texture
	m_Texture = Texture::Load("asset/texture/Sight.png");

	Scale = Vector3(1.5f, 1.5f, 1.5f);
}

void Sight::Start()
{
	Billboard::Start();
}

void Sight::Uninit()
{
	Billboard::Uninit();
	if (m_VertexLayout) m_VertexLayout->Release();
	if (m_VertexShader) m_VertexShader->Release();
	if (m_PixelShader)  m_PixelShader->Release();
}

void Sight::Update()
{

}

void Sight::Draw()
{
	//Shader Setting
	Renderer::GetDeviceContext()->IASetInputLayout(m_VertexLayout);
	Renderer::GetDeviceContext()->VSSetShader(m_VertexShader, NULL, 0);
	Renderer::GetDeviceContext()->PSSetShader(m_PixelShader, NULL, 0);

	Billboard::MatrixSetting();

	//Material Setting
	MATERIAL material{};
	material.Diffuse = { 1.0f,1.0f,1.0f,1.0f };
	material.TextureEnable = true;
	Renderer::SetMaterial(material);

	//Vertex Buffer Setting
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	Renderer::GetDeviceContext()->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);

	//Texture Setting
	Renderer::GetDeviceContext()->PSSetShaderResources(0, 1, &m_Texture);

	//Primitive Topology Setting
	Renderer::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	Renderer::GetDeviceContext()->Draw(4, 0);
}
