#include "main.h"
#include <thread>
#include "Manager.h"
#include "Scene.h"
#include "Fade.h"
#include "LoadTitle.h"
#include "Title.h"
#include "SkyDome.h"
#include "MeshField.h"
#include "Tank.h"

void LoadTitle::Init()
{
	Loading::Init();

	std::thread th([=] 
	{
		SkyDome::Load();
		MeshField::Load();
		Tank::Load();
		m_LoadFinish = true; 
	});
	th.detach();
}

void LoadTitle::Start()
{
	Loading::Start();
}

void LoadTitle::Uninit()
{
	Loading::Uninit();
}

void LoadTitle::Update()
{
	Loading::Update();

	if (m_IsFade && m_Fade->GetFadeState() == FADE_STATE_CHANGE)
	{
		m_IsFade = false;
		m_Fade->PlayFade(FADE_MODE_IN, 1.5f);
		Manager::ChangeScene<Title>();
	}
}
