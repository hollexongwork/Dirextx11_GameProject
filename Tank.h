#pragma once
#include"GameObject.h"
#include "AnimationModel.h"
#include "InputState.h"

struct Material
{
	XMFLOAT4 UV = { 0.0f, 0.0f, 0.0f, 0.0f };
};

class Tank : public GameObject
{
protected:
	static constexpr float CULL_RADIUS = 1.8f;

	AnimationModel* m_Model;

	static ID3D11InputLayout* m_VertexLayout;
	static ID3D11VertexShader* m_VertexShader;
	static ID3D11PixelShader* m_PixelShader;

	static ID3D11ShaderResourceView* m_Texture[3];
	static ID3D11ShaderResourceView* m_NormalMap[3];
	static ID3D11ShaderResourceView* m_ARM[3];
	static ID3D11ShaderResourceView* m_Emission;

	static ID3D11Buffer* m_MaterialBuffer;

	Material m_BodyMaterial;
	Material m_LeftTrackMaterial;
	Material m_RightTrackMaterial;



	class Camera* m_Camera;

	void LoadModel();
	static void LoadShader();
	static void LoadTexture();
	static void CreateMaterialBuffer();
	void InitMaterials();

public:
	static void Load();
	virtual void Init()override;
	virtual void Start()override;
	virtual void Uninit()override;
	virtual void Update()override;
	virtual void DrawShadow() override;
	virtual void Draw()override;

	void SetDamageMaterialAlpha(float z) { m_RightTrackMaterial.UV.z = m_LeftTrackMaterial.UV.z = m_BodyMaterial.UV.z = z; }

	void SetLeftTrackUV(float y) { m_LeftTrackMaterial.UV.y = y; }
	void SetRightTrackUV(float y) { m_RightTrackMaterial.UV.y = y; }

	float GetLeftTrackUV() const { return m_LeftTrackMaterial.UV.y; }
	float GetRightTrackUV() const { return m_RightTrackMaterial.UV.y; }

	AnimationModel* GetModel() const { return m_Model; }
};

