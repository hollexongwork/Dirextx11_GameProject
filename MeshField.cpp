#include "main.h"
#include "Texture.h"
#include "MeshField.h"
#include "PerlinNoise.h"
#include "FieldCollider.h"

ID3D11Buffer* MeshField::m_VertexBuffer;
ID3D11Buffer* MeshField::m_IndexBuffer;

ID3D11InputLayout* MeshField::m_VertexLayout;
ID3D11VertexShader* MeshField::m_VertexShader;
ID3D11PixelShader* MeshField::m_PixelShader;

ID3D11ShaderResourceView* MeshField::m_Texture;
ID3D11ShaderResourceView* MeshField::m_NormalMap;
ID3D11ShaderResourceView* MeshField::m_RoughnessMap;
ID3D11ShaderResourceView* MeshField::m_AOMap;

VERTEX_3D MeshField::m_Vertex[FIELD_RES][FIELD_RES];

std::vector<Triangle> MeshField::m_Triangles;

void MeshField::Load()
{
	std::vector<std::vector<float>> FieldHeight(FIELD_RES, std::vector<float>(FIELD_RES, 0.0f));
	PerlinNoise noise;
	for (int x = 0; x <= FIELD_RES - 1; x++)
	{
		for (int z = 0; z <= FIELD_RES - 1; z++)
		{
			FieldHeight[x][z] = (float)noise.noise2D(x * 0.008, z * 0.008) * 70.0f - 35.0f;
		}
	}

	// Vertex Buffer
	{
		for (int x = 0; x <= FIELD_RES - 1; x++)
		{
			for (int z = 0; z <= FIELD_RES - 1; z++)
			{
				float y = FieldHeight[x][z] + 30.0f;
				//float y = 0.0f;
				m_Vertex[x][z].Position = XMFLOAT3((x - FIELD_RES * 0.5f) * MESH_SIZE, y, (z - FIELD_RES * 0.5f) * -MESH_SIZE);
				m_Vertex[x][z].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
				m_Vertex[x][z].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
				m_Vertex[x][z].TexCoord = XMFLOAT2(x * 0.5f, z * 0.5f);
			}
		}

		for (int x = 1; x <= FIELD_RES - 2; x++)
		{
			for (int z = 1; z <= FIELD_RES - 2; z++)
			{
				Vector3 vx, vz, vn;
				vx.x = m_Vertex[x + 1][z].Position.x - m_Vertex[x - 1][z].Position.x;
				vx.y = m_Vertex[x + 1][z].Position.y - m_Vertex[x - 1][z].Position.y;
				vx.z = m_Vertex[x + 1][z].Position.z - m_Vertex[x - 1][z].Position.z;

				vz.x = m_Vertex[x][z - 1].Position.x - m_Vertex[x][z + 1].Position.x;
				vz.y = m_Vertex[x][z - 1].Position.y - m_Vertex[x][z + 1].Position.y;
				vz.z = m_Vertex[x][z - 1].Position.z - m_Vertex[x][z + 1].Position.z;

				vn = Vector3::Cross(vz, vx);
				vn.normalize();

				m_Vertex[x][z].Normal.x = vn.x;
				m_Vertex[x][z].Normal.y = vn.y;
				m_Vertex[x][z].Normal.z = vn.z;
			}
		}


		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(VERTEX_3D) * FIELD_RES * FIELD_RES;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.pSysMem = m_Vertex;

		Renderer::GetDevice()->CreateBuffer(&bd, &sd, &m_VertexBuffer);
	}

	// Index Buffer
	{
		unsigned int index[FIELD_INDEX_COUNT];

		int i = 0;
		for (int x = 0; x < FIELD_RES - 1; x++)
		{
			for (int z = 0; z < FIELD_RES; z++)
			{
				index[i] = x * FIELD_RES + z;
				i++;

				index[i] = (x + 1) * FIELD_RES + z;
				i++;
			}

			if (x == FIELD_RES - 2)
				break;

			index[i] = (x + 1) * FIELD_RES + (FIELD_RES - 1);
			i++;

			index[i] = (x + 1) * FIELD_RES;
			i++;
		}

		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(unsigned int) * FIELD_INDEX_COUNT;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.pSysMem = index;

		Renderer::GetDevice()->CreateBuffer(&bd, &sd, &m_IndexBuffer);
	}

	for (int x = 0; x < FIELD_RES - 1; ++x)
	{
		for (int z = 0; z < FIELD_RES - 1; ++z)
		{
			Triangle t1, t2;

			t1.v0 = Vector3(m_Vertex[x][z].Position.x, m_Vertex[x][z].Position.y, m_Vertex[x][z].Position.z);
			t1.v1 = Vector3(m_Vertex[x + 1][z].Position.x, m_Vertex[x + 1][z].Position.y, m_Vertex[x + 1][z].Position.z);
			t1.v2 = Vector3(m_Vertex[x + 1][z + 1].Position.x, m_Vertex[x + 1][z + 1].Position.y, m_Vertex[x + 1][z + 1].Position.z);
			t1.normal = Vector3::Cross(t1.v1 - t1.v0, t1.v2 - t1.v0);
			t1.normal.normalize();

			t2.v0 = Vector3(m_Vertex[x][z].Position.x, m_Vertex[x][z].Position.y, m_Vertex[x][z].Position.z);
			t2.v1 = Vector3(m_Vertex[x + 1][z + 1].Position.x, m_Vertex[x + 1][z + 1].Position.y, m_Vertex[x + 1][z + 1].Position.z);
			t2.v2 = Vector3(m_Vertex[x][z + 1].Position.x, m_Vertex[x][z + 1].Position.y, m_Vertex[x][z + 1].Position.z);
			t2.normal = Vector3::Cross(t2.v1 - t2.v0, t2.v2 - t2.v0);
			t2.normal.normalize();

			m_Triangles.push_back(t1);
			m_Triangles.push_back(t2);
		}
	}

	//Shader Set
	Renderer::CreateVertexShader(&m_VertexShader, &m_VertexLayout, "shader/cso/PBRVS.cso");
	Renderer::CreatePixelShader(&m_PixelShader, "shader/cso/PBRPS.cso");

	//Texture
	m_Texture = Texture::Load("asset/texture/coast_sand_02_diff_4k.png", true);
	m_NormalMap = Texture::Load("asset/texture/coast_sand_02_nor_dx_4k.png");
	m_RoughnessMap = Texture::Load("asset/texture/coast_sand_02_rough_4k.png");
	m_AOMap = Texture::Load("asset/texture/coast_sand_02_ao_4k.png");
}

void MeshField::Init()
{
	auto* col = AddComponent<FieldCollider>();
	col->SetTag(CollisionTag::Field);
	col->SetBodyType(BodyType::Static);
	col->SetTrigger(true);
}

void MeshField::Uninit()
{
	if (m_VertexLayout) { m_VertexLayout->Release(); m_VertexLayout = nullptr; }
	if (m_VertexShader) { m_VertexShader->Release(); m_VertexShader = nullptr; }
	if (m_PixelShader) { m_PixelShader->Release(); m_PixelShader = nullptr; }

	if (m_VertexBuffer) { m_VertexBuffer->Release(); m_VertexBuffer = nullptr; }
	if (m_IndexBuffer) { m_IndexBuffer->Release(); m_IndexBuffer = nullptr; }
}

void MeshField::Update()
{

}

void MeshField::DrawShadow()
{
	// Set world matrix
	XMMATRIX PivotMatrix = XMMatrixTranslation(Pivot.x, Pivot.y, Pivot.z);
	XMMATRIX RotationMatrix = XMMatrixRotationRollPitchYaw(XMConvertToRadians(Rotation.x), XMConvertToRadians(Rotation.y), XMConvertToRadians(Rotation.z));
	XMMATRIX ScaleMatrix = XMMatrixScaling(Scale.x, Scale.y, Scale.z);
	XMMATRIX PositionMatrix = XMMatrixTranslation(Position.x, Position.y, Position.z);

	XMMATRIX world = ScaleMatrix * RotationMatrix * PositionMatrix;
	Renderer::SetWorldMatrix(world);

	// Draw
	Renderer::GetDeviceContext()->DrawIndexed(FIELD_INDEX_COUNT, 0, 0);
}

void MeshField::Draw()
{
	Renderer::GetDeviceContext()->PSSetShaderResources(0, 1, &m_Texture);
	Renderer::GetDeviceContext()->PSSetShaderResources(1, 1, &m_NormalMap);
	Renderer::GetDeviceContext()->PSSetShaderResources(2, 1, &m_RoughnessMap);
	Renderer::GetDeviceContext()->PSSetShaderResources(3, 1, &m_AOMap);

	ID3D11ShaderResourceView* depthSRV = Renderer::GetDepthSRV();
	Renderer::GetDeviceContext()->PSSetShaderResources(4, 1, &depthSRV);

	Renderer::GetDeviceContext()->IASetInputLayout(m_VertexLayout);

	//shader setting
	Renderer::GetDeviceContext()->VSSetShader(m_VertexShader, NULL, 0);
	Renderer::GetDeviceContext()->PSSetShader(m_PixelShader, NULL, 0);

	// Material Setting
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	material.TextureEnable = true;
	material.UV_Offset = XMFLOAT2(1.0f, 1.0f);
	Renderer::SetMaterial(material);

	//matrix setting
	XMMATRIX PivotMatrix = XMMatrixTranslation(Pivot.x, Pivot.y, Pivot.z);
	XMMATRIX RotationMatrix = XMMatrixRotationRollPitchYaw(XMConvertToRadians(Rotation.x), XMConvertToRadians(Rotation.y), XMConvertToRadians(Rotation.z));
	XMMATRIX ScaleMatrix = XMMatrixScaling(Scale.x, Scale.y, Scale.z);
	XMMATRIX PositionMatrix = XMMatrixTranslation(Position.x, Position.y, Position.z);

	XMMATRIX world = ScaleMatrix * RotationMatrix * PositionMatrix;

	Renderer::SetWorldMatrix(world);

	// Vertex Buffer Setting
	UINT stride = sizeof( VERTEX_3D );
	UINT offset = 0;
	Renderer::GetDeviceContext()->IASetVertexBuffers( 0, 1, &m_VertexBuffer, &stride, &offset );

	// Index Buffer Setting
	Renderer::GetDeviceContext()->IASetIndexBuffer( 
		m_IndexBuffer, DXGI_FORMAT_R32_UINT, 0 );

	// Primitive Setting
	Renderer::GetDeviceContext()->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );

	// Draw
	Renderer::GetDeviceContext()->DrawIndexed(FIELD_INDEX_COUNT, 0, 0);
}

FieldHit MeshField::GetFieldHeightAndNormal(const Vector3& Position) const
{
	FieldHit out = { 0.0f, Vector3(0,1,0) };

	float fx = Position.x / MESH_SIZE + FIELD_RES * 0.5f;
	float fz = Position.z / -MESH_SIZE + FIELD_RES * 0.5f;

	int x = (int)fx;
	int z = (int)fz;

	float tx = fx - x;   // 0 - 1
	float tz = fz - z;


	x = Clamp(x, 0, FIELD_RES - 2);
	z = Clamp(z, 0, FIELD_RES - 2);

	const XMFLOAT3& p0 = m_Vertex[x][z].Position;
	const XMFLOAT3& p1 = m_Vertex[x + 1][z].Position;
	const XMFLOAT3& p2 = m_Vertex[x][z + 1].Position;
	const XMFLOAT3& p3 = m_Vertex[x + 1][z + 1].Position;

	float height =
		p0.y * (1 - tx) * (1 - tz) +
		p1.y * tx * (1 - tz) +
		p2.y * (1 - tx) * tz +
		p3.y * tx * tz;

	out.Height = height;

	//Normal smoothing
	Vector3 n0(m_Vertex[x][z].Normal.x, m_Vertex[x][z].Normal.y, m_Vertex[x][z].Normal.z);
	Vector3 n1(m_Vertex[x + 1][z].Normal.x, m_Vertex[x + 1][z].Normal.y, m_Vertex[x + 1][z].Normal.z);
	Vector3 n2(m_Vertex[x][z + 1].Normal.x, m_Vertex[x][z + 1].Normal.y, m_Vertex[x][z + 1].Normal.z);
	Vector3 n3(m_Vertex[x + 1][z + 1].Normal.x, m_Vertex[x + 1][z + 1].Normal.y, m_Vertex[x + 1][z + 1].Normal.z);

	Vector3 normal =
		n0 * (1 - tx) * (1 - tz) +
		n1 * tx * (1 - tz) +
		n2 * (1 - tx) * tz +
		n3 * tx * tz;

	normal.normalize();
	out.Normal = normal;

	return out;
}


