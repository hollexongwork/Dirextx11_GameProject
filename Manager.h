#pragma once

class Manager
{
private:
	static class Scene* m_Scene;
	static class Scene* m_NextScene;
public:
	static void Init(HWND hWnd);
	static void Uninit();
	static void Update();
	static void Draw();

	static Scene* GetScene()
	{
		return m_Scene;
	}

	template <typename T>
	static void ChangeScene()
	{
		m_NextScene = new T();
	}
};