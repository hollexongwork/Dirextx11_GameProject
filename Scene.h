#pragma once
#include "Renderer.h"
#include "GameObject.h"
#include <list>

enum RENDER_LAYER
{
	RENDER_LAYER_CAMERA,
	RENDER_LAYER_OPAQUE,
	RENDER_LAYER_SKYBOX,
	RENDER_LAYER_TRANSLUCENT,
	RENDER_LAYER_PARTICLE,
	RENDER_LAYER_XRAY,
	RENDER_LAYER_POST,
	RENDER_LAYER_UI,
	RENDER_LAYER_FADE,
	RENDER_LAYER_NUM,
};

class Scene
{
private:
	std::list<GameObject*> m_GameObject[RENDER_LAYER_NUM];
	void RenderShadowPass(const LIGHT& light, int lightIndex);

protected:
	class Fade* m_Fade;
	bool m_IsFade = false;


	ID3D11VertexShader* m_VertexShader;
	ID3D11InputLayout* m_VertexLayout;


public:

	virtual void Init();
	virtual void Start();
	virtual void Uninit();
	virtual void Update();
	virtual void Draw();

	template <typename T>
	T* AddGameObject(int Layer)
	{
		T* gameObject = new T();
		gameObject->Init();
		gameObject->Start();
		m_GameObject[Layer].push_back(gameObject);
		return gameObject;
	}

	template <typename T>
	T* GetGameObject()
	{
		for (int i = 0; i < RENDER_LAYER_NUM; i++)
		{
			for (const auto gameObject : m_GameObject[i])
			{
				T* find = dynamic_cast<T*>(gameObject);

				if (find != nullptr)
					return find;
			}
		}
		return nullptr;
	}

	template <typename T>
	std::vector<T*> GetGameObjects()
	{
		std::vector<T*> objects;
		for (int i = 0; i < RENDER_LAYER_NUM; i++)
		{
			for (const auto gameObject : m_GameObject[i])
			{
				T* find = dynamic_cast<T*>(gameObject);

				if (find != nullptr)
					objects.push_back(find);
			}
		}
		return objects;
	}
};

