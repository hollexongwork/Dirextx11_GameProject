#include "main.h"
#include "Manager.h"
#include "Scene.h"
#include "Renderer.h"
#include "Input.h"
#include "Mouse.h"
#include "Fade.h"
#include "Title.h"
#include "LoadGame.h"
#include "Camera.h"
#include "SkyDome.h"
#include "MeshField.h"
#include "Enemy.h"
#include "TankEvent.h"
#include "TankPosture.h"
#include "Bloom.h"
#include "Polygon.h"
#include "EnterIcon.h"
#include "Time.h"

void Title::Init()
{
	Mouse_SetMode(MOUSE_POSITION_MODE_ABSOLUTE);

	Scene::Init();
	// Camera
	m_Camera = AddGameObject <Camera>(RENDER_LAYER_CAMERA);

	// BackGround
	AddGameObject <SkyDome>(RENDER_LAYER_SKYBOX);
	AddGameObject <MeshField>(RENDER_LAYER_OPAQUE);

	// Object
	m_Tank = AddGameObject <Enemy>(RENDER_LAYER_OPAQUE);
	m_Tank->SetPosition(Vector3(50.0f, 0.0f, 0.0f));
	m_Tank->SetRotation(Vector3(0.0f, 180.0f, 0.0f));
	m_Tank->UpdateRotationMatrix();

	// PostProcess
	AddGameObject <Bloom>(RENDER_LAYER_POST);

	// UI
	Vector3 position = Vector3(SCREEN_WIDTH * 0.18f, SCREEN_HEIGHT * 0.2f, 0.0f);
	Vector3 rotation = Vector3(0.0f, 0.0f, 0.0f);
	Vector3 scale = Vector3(SCREEN_WIDTH, SCREEN_HEIGHT, 1.0f);
	scale *= 0.35f;
	AddGameObject <Polygon2D>(RENDER_LAYER_UI)->Init(position, rotation, scale, "asset/texture/Logo.png");

	position = Vector3(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.87f, 0.0f);
	scale = Vector3(793.0f * 0.5f, 259.0f * 0.5f, 1.0f);
	AddGameObject <Polygon2D>(RENDER_LAYER_UI)->Init(position, rotation, scale, "asset/texture/Start.png");

	m_EnterIcon = AddGameObject <EnterIcon>(RENDER_LAYER_UI);
}

void Title::Start()
{
	auto event = m_Tank->GetComponent<TankEvent>();
	event->SetHasTarget(true);

	m_Posture = m_Tank->GetComponent<TankPosture>();
	m_Posture->Start();
	m_Posture->InitTurretAndBarrelPosture(85.0f, 0.0f);
	m_Posture->SetTargetPoint(Vector3(0.0f, 15.0f, 10.0f));

	Vector3 focusPoint = m_Tank->GetPosition();
	focusPoint.x -= 1.5f;
	m_Camera->SetCameraFocusPoint(focusPoint);
	m_Camera->SetRotation(Vector3(XMConvertToRadians(-15.0f), XMConvertToRadians(-40.0f), 0.0f));

	XMVECTOR followPos = XMVectorSet(m_Tank->GetPosition().x, m_Tank->GetPosition().y, m_Tank->GetPosition().z, 1.0f);
	GlobalLightsManager::UpdateDirectionalLightShadow(0, followPos, 10.0f, 0.1f, 10.0f);

	Scene::Start();
}

void Title::Uninit()
{
	Scene::Uninit();
}

void Title::Update()
{
	Scene::Update();

	m_TimeCount += Time::GetDeltaTime();

	if (m_TimeCount > m_Duration)
	{
		float y = RandRange(15.0f, 18.0f);
		float z = RandRange(-30.0f, 30.0f);

		m_Posture->SetTargetPoint(Vector3(0.0f, y, z));

		m_TimeCount = 0.0f;
		m_Duration = 2.0f + 3.0f * (float(rand()) / RAND_MAX);
	}

	if (Input::GetKeyTrigger(VK_RETURN) && !m_IsFade)
	{
		m_EnterIcon->PlaySacleAnimation();

		m_IsFade = true;
		m_Fade->PlayFade(FADE_MODE_OUT, 0.35f);
	}

	if (m_IsFade && m_Fade->GetFadeState() == FADE_STATE_CHANGE)
	{
		m_IsFade = false;
		m_Fade->PlayFade(FADE_MODE_IN, 0.35f);
		Manager::ChangeScene<LoadGame>();
	}
}
