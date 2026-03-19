#pragma once
#include"GameObject.h"

class SkyDome : public GameObject
{
private:
	static class MODEL* m_Model;

	static ID3D11InputLayout* m_VertexLayout;
	static ID3D11VertexShader* m_VertexShader;
	static ID3D11PixelShader* m_PixelShader;

	static ID3D11ShaderResourceView* m_Texture;

public:
	static void Load();

	void Init()override;
	void Uninit()override;
	void Update()override;
	void Draw()override;
};

