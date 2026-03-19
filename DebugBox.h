#pragma once
#include"GameObject.h"

class DebugBox : public GameObject
{
private:
	class MODEL* m_Model;

	ID3D11InputLayout* m_VertexLayout;
	ID3D11VertexShader* m_VertexShader;
	ID3D11PixelShader* m_PixelShader;

	ID3D11ShaderResourceView* m_Texture;

	Vector3 m_Offset;

protected:

public:
	void Init()override;
	void Uninit()override;
	void Update()override;
	void DrawShadow() override;
	void Draw()override;
};

