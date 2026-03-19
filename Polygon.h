#pragma once
#include "GameObject.h"

class Polygon2D : public GameObject
{
protected:
	ID3D11Buffer* m_VertexBuffer;

	ID3D11InputLayout* m_VertexLayout;
	ID3D11VertexShader* m_VertexShader;
	ID3D11PixelShader* m_PixelShader;

	ID3D11ShaderResourceView* m_Texture;

	void InitVertex();

public:
	virtual void Init() override {};
	void Init(Vector3 position, Vector3 rotation, Vector3 scale, const char* FileName);
	virtual void Uninit()override;
	virtual void Update()override;
	virtual void Draw()override;
};