#pragma once

#include "GameObject.h"
#include "Renderer.h"

struct Triangle
{
	Vector3 v0, v1, v2;
	Vector3 normal;
};

struct FieldHit
{
	float Height;
	Vector3 Normal;
};

class MeshField : public GameObject
{
private:
	static constexpr int FIELD_RES = 150;
	static constexpr int FIELD_INDEX_COUNT = ((FIELD_RES + 1) * 2 * (FIELD_RES - 1)) - 2;
	static constexpr float MESH_SIZE = 2.5f;

	static ID3D11Buffer* m_VertexBuffer;
	static ID3D11Buffer* m_IndexBuffer;

	static ID3D11InputLayout* m_VertexLayout;
	static ID3D11VertexShader* m_VertexShader;
	static ID3D11PixelShader* m_PixelShader;

	static ID3D11ShaderResourceView* m_Texture;
	static ID3D11ShaderResourceView* m_NormalMap;
	static ID3D11ShaderResourceView* m_RoughnessMap;
	static ID3D11ShaderResourceView* m_AOMap;

	static VERTEX_3D m_Vertex[FIELD_RES][FIELD_RES];

	static std::vector<Triangle> m_Triangles;

public:
	static void Load();
	void Init() override;
	void Uninit() override;
	void Update() override;
	void DrawShadow() override;
	void Draw() override;

	FieldHit GetFieldHeightAndNormal(const Vector3& Position) const;
	VERTEX_3D(&GetVertex())[FIELD_RES][FIELD_RES]{ return m_Vertex; }

	int GetFieldRes() const { return FIELD_RES; }

	const std::vector<Triangle>& GetTriangles() const { return m_Triangles; }
};