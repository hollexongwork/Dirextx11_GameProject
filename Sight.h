#pragma once
#include "Billboard.h"

class Sight : public Billboard
{
private:
	ID3D11InputLayout* m_VertexLayout;
	ID3D11VertexShader* m_VertexShader;
	ID3D11PixelShader* m_PixelShader;

	ID3D11ShaderResourceView* m_Texture;

public:
	void Init()override;
	void Start()override;
	void Uninit()override;
	void Update()override;
	void Draw()override;

};

