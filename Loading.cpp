#include "main.h"
#include <thread>
#include "Loading.h"
#include "Polygon.h"
#include "Game.h"
#include "Fade.h"
#include "Camera.h"

void Loading::Init()
{
	Scene::Init();

	AddGameObject <Camera>(RENDER_LAYER_CAMERA);

	Vector3 position = Vector3(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f, 0.0f);
	Vector3 rotation = Vector3(0.0f, 0.0f, 0.0f);
	Vector3 scale = Vector3(SCREEN_WIDTH, SCREEN_HEIGHT, 1.0f);
	AddGameObject<Polygon2D>(RENDER_LAYER_UI)->Init(position, rotation, scale, "asset/texture/Load.png");

	position = Vector3(SCREEN_WIDTH * 0.85f, SCREEN_HEIGHT * 0.87f, 0.0f);
	scale = Vector3(793.0f * 0.5f, 259.0f * 0.5f, 1.0f);
	AddGameObject<Polygon2D>(RENDER_LAYER_UI)->Init(position, rotation, scale, "asset/texture/Loading.png");

	position = Vector3(SCREEN_WIDTH * 0.72f, SCREEN_HEIGHT * 0.87f, 0.0f);
	rotation = Vector3(0.0f, 0.0f, 40.0f);
	scale = Vector3(125.0f, 125.0f, 1.0f);
	m_Circle = AddGameObject<Polygon2D>(RENDER_LAYER_UI);
	m_Circle->Init(position, rotation, scale, "asset/texture/Circle.png");

	m_LoadFinish = false;
}

void Loading::Start()
{
	Scene::Start();
}

void Loading::Uninit()
{
	Scene::Uninit();
	if (m_Circle) { m_Circle = nullptr; }
}

void Loading::Update()
{
	Scene::Update();

	m_Rota -= 5.0f;
	m_Rota = NormalizeAngle(m_Rota);
	m_Circle->SetRotation(Vector3(0.0f, 0.0f, m_Rota));

	if (m_LoadFinish && !m_IsFade)
	{
		m_IsFade = true;
		m_Fade->PlayFade(FADE_MODE_OUT, 0.35f);
	}
}
