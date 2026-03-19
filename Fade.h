#pragma once
#include "Polygon.h"

enum FADE_MODE
{
	FADE_MODE_NONE,
	FADE_MODE_IN,
	FADE_MODE_OUT
};

enum FADE_STATE
{
	FADE_STATE_STANDBY,
	FADE_STATE_IN,
	FADE_STATE_OUT,
	FADE_STATE_CHANGE,
};

class Fade : public Polygon2D
{
private:
	ID3D11Buffer* m_VertexBuffer;

	ID3D11InputLayout* m_VertexLayout;
	ID3D11VertexShader* m_VertexShader;
	ID3D11PixelShader* m_PixelShader;

	ID3D11ShaderResourceView* m_Texture;

	static FADE_MODE  m_FadeMode;
	static FADE_STATE m_FadeState;

	MATERIAL Fade;

	static float m_PlayTime;
	float m_TimeCount;

protected:

public:
	void Init()override;
	void Uninit()override;
	void Update()override;
	void Draw()override;

	void PlayFade(FADE_MODE mode, float playTime);
	FADE_STATE GetFadeState() const { return m_FadeState; }
};

