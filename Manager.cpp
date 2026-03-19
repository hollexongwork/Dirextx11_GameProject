#include "main.h"
#include "Time.h"
#include "Manager.h"
#include "Renderer.h"
#include "Input.h"
#include "Mouse.h"
#include "Scene.h"
#include "Game.h"
#include "Title.h"
#include "LoadTitle.h"
#include "LoadGame.h"
#include "Fade.h"

#include "ImGuiManager.h"

Scene* Manager::m_Scene = nullptr;
Scene* Manager::m_NextScene = nullptr;

void Manager::Init(HWND hWnd)
{
	Time::Init();
	Renderer::Init();
	Input::Init();
	Mouse_Initialize(hWnd);
	Mouse_SetMode(MOUSE_POSITION_MODE_ABSOLUTE);

	ImGuiManager::Init(hWnd);

	m_Scene = new LoadTitle();
	m_Scene->Init();
	m_Scene->Start();
}

void Manager::Uninit()
{
	m_Scene->Uninit();
	delete m_Scene;

	ImGuiManager::Uninit();

	Mouse_SetMode(MOUSE_POSITION_MODE_ABSOLUTE);
	Mouse_SetVisible(true);
	Mouse_Finalize();
	Input::Uninit();
	Renderer::Uninit();
}

void Manager::Update()
{
	Time::Update();
	Input::Update();

	if (m_NextScene != nullptr)
	{
		m_Scene->Uninit();
		delete m_Scene;

		m_Scene = m_NextScene;
		m_Scene->Init();
		m_Scene->Start();
		m_NextScene = nullptr;
	}

	m_Scene->Update();

	Mouse_UpdatePrevState();
}

void Manager::Draw()
{	
	Renderer::Begin();
	m_Scene->Draw();

	//ImGuiManager::NewFrame();  
	//ImGuiManager::DrawDebugWindows();  
	//ImGuiManager::Render();

	Renderer::End();
}
