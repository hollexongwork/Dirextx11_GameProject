#include "main.h"
#include <thread>
#include "Manager.h"
#include "Scene.h"
#include "Fade.h"
#include "LoadGame.h"
#include "Game.h"
#include "SkyDome.h"
#include "MeshField.h"
#include "Building.h"
#include "Tank.h"

void LoadGame::Init()
{
	Loading::Init();

	std::thread th([=] 
	{
		SkyDome::Load();
		MeshField::Load();
		Building::Load();
		Tank::Load();
		m_LoadFinish = true; 
	});
	th.detach();
}

void LoadGame::Start()
{
	Loading::Start();
}

void LoadGame::Uninit()
{
	Loading::Uninit();
}

void LoadGame::Update()
{
	Loading::Update();

	if (m_IsFade && m_Fade->GetFadeState() == FADE_STATE_CHANGE)
	{
		m_IsFade = false;
		m_Fade->PlayFade(FADE_MODE_IN, 1.5f);
		Manager::ChangeScene<Game>();
	}
}
