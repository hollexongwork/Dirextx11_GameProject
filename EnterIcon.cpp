#include "main.h"
#include "Renderer.h"
#include "Texture.h"
#include "Time.h"
#include "EnterIcon.h"
#include "Functions.h"

void EnterIcon::Init()
{
	Vector3 position = Vector3(SCREEN_WIDTH * 0.35f, SCREEN_HEIGHT * 0.87f, 0.0f);
	Vector3 rotation = Vector3(0.0f, 0.0f, 0.0f);
	Vector3 scale = Vector3(1935.0f * 0.05f, 2771.0f * 0.05f, 1.0f);

	Polygon2D::Init(position, rotation, scale, "asset/texture/Enter.png");

	m_TimeCount = 0.0f;
	m_StartTime = 0.0f;
}

void EnterIcon::Uninit()
{
	Polygon2D::Uninit();
}

void EnterIcon::Update()
{
	m_TimeCount += Time::GetDeltaTime();

	Position.y = sinf(m_TimeCount / 0.5f) *10.0f+ 940.0f;

	if (m_StartTime)
	{
		float t = (m_TimeCount - m_StartTime) / 1.0f; 
		//t = std::min(t, 1.0f);                        
		float value = easeOutElastic(t);
		float scaleFactor = 0.7f + value * 0.3f;
		Scale = Scale * scaleFactor;
	}
}

void EnterIcon::Draw()
{
	Polygon2D::Draw();
}

void EnterIcon::PlaySacleAnimation()
{
	if (!m_StartTime)
	{
		m_StartTime = m_TimeCount;
	}
}