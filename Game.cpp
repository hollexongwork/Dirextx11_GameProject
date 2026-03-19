#include "main.h"
#include "Manager.h"
#include "Time.h"
#include "Mouse.h"

#include "Game.h"
#include "LoadTitle.h"

#include "Camera.h"
#include "SkyDome.h"
#include "MeshField.h"
#include "Buildings.h"

#include "Player.h"
#include "Enemy.h"

#include "CrossSight.h"
#include "Fade.h"
#include "Bloom.h"
#include "DepthFade.h"
#include "TankEvent.h"
#include "EnemyManager.h"

#include "DebugBox.h"

void Game::Init()
{
	Mouse_SetMode(MOUSE_POSITION_MODE_RELATIVE);

	Scene::Init();

	//Camera
	AddGameObject <Camera>(RENDER_LAYER_CAMERA);

	//BackGround
	AddGameObject <SkyDome>(RENDER_LAYER_SKYBOX);
	AddGameObject <MeshField>(RENDER_LAYER_OPAQUE);

	//Object
	Buildings buildings;
	buildings.Init();

	m_Player = AddGameObject <Player>(RENDER_LAYER_OPAQUE);
	m_Player->SetPosition(Vector3(0.0f, 0.0f, -135.0f));

	{
		m_EnemyManager = new EnemyManager();
		m_EnemyManager->Init();

		m_EnemyManager->CreateEnemyRandomRotation(Vector3(-10.0f, 0.0f, 20.0f));
		m_EnemyManager->CreateEnemyRandomRotation(Vector3(80.0f, 0.0f, 65.0f));
		m_EnemyManager->CreateEnemyRandomRotation(Vector3(-100.0f, 0.0f, -45.0f));
		m_EnemyManager->CreateEnemyRandomRotation(Vector3(-95.0f, 0.0f, 35.0f));
		m_EnemyManager->CreateEnemyRandomRotation(Vector3(25.0f, 0.0f, 100.0f));
		m_EnemyManager->CreateEnemyRandomRotation(Vector3(60.0f, 0.0f, -10.0f));
	}

	//PostProcess
	AddGameObject <Bloom>(RENDER_LAYER_POST);

	//UI
	AddGameObject <CrossSight>(RENDER_LAYER_UI);
}

void Game::Start()
{
	Scene::Start();

	m_PlayerEvent = m_Player->GetComponent<TankEvent>();
}

void Game::Uninit()
{
	Scene::Uninit();
	if (m_Player) { m_Player = nullptr; }
	if (m_PlayerEvent) {  m_Player = nullptr; }
}

void Game::Update()
{
	Scene::Update();

	if (m_Player)
	{
		XMVECTOR followPos = XMVectorSet(m_Player->GetPosition().x, m_Player->GetPosition().y, m_Player->GetPosition().z, 1.0f);
		GlobalLightsManager::UpdateDirectionalLightShadow(0, followPos, 10.0f, 0.1f, 10.0f);
	}

	if (m_PlayerEvent->IsDestory() || EnemyManager::IsAllDestory())
	{
		m_Timer += Time::GetDeltaTime();

		if (m_Timer >= m_Delay)
		{
			if (!m_IsFade)
			{
				m_Fade->PlayFade(FADE_MODE_OUT, 0.35f);
				m_IsFade = true;
			}

			if (m_IsFade && m_Fade->GetFadeState() == FADE_STATE_CHANGE)
			{
				m_IsFade = false;
				m_Fade->PlayFade(FADE_MODE_IN, 0.35f);
				Manager::ChangeScene<LoadTitle>();
			}
		}
	}
}
