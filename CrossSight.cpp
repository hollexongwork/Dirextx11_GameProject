#include "main.h"
#include "Renderer.h"
#include "Texture.h"
#include "Manager.h"
#include "Scene.h"
#include "CrossSight.h"
#include "MeshField.h"
#include "Enemy.h"

void CrossSight::Init()
{
	Vector3 position = Vector3(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.35f, 0.0f);
	Vector3 rotation = Vector3(0.0f, 0.0f, 0.0f);
	Vector3 scale = Vector3(128.0f, 128.0f, 1.0f);

	Polygon2D::Init(position, rotation, scale, "asset/texture/FrontSight.png");

	m_Ray.end = Vector3(0.0f, 0.0f, m_MaxDistance);
}

void CrossSight::Uninit()
{
	Polygon2D::Uninit();
}

void CrossSight::Update()
{
	m_Ray = CameraRay({ SCREEN_WIDTH * 0.5f,SCREEN_HEIGHT * 0.35f }, m_MaxDistance);
}

void CrossSight::Draw()
{
	Polygon2D::Draw();
}