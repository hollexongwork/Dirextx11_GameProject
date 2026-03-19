#pragma once
#include"GameObject.h"

class Building : public GameObject
{
private:
	static constexpr float CULL_RADIUS = 17.5f;

	static class MODEL* m_Model[5];

	static ID3D11InputLayout* m_VertexLayout;
	static ID3D11VertexShader* m_VertexShader;
	static ID3D11PixelShader* m_PixelShader;

	static ID3D11ShaderResourceView* m_Texture[5];

	Vector3 m_ColliderScale[5];

	int m_Index = 0;

	class Camera* m_Camera;

protected:

public:
	static void Load();

	void Init()override;
	void Start()override;
	void Uninit()override;
	void DrawShadow() override;
	void Draw()override;

	void SetModelIndex(int index);
};

