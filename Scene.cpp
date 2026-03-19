#include "main.h"
#include "Manager.h"
#include "Renderer.h"
#include "ShadowMap.h"
#include "Scene.h"
#include "Camera.h"
#include "GlobalLightsManager.h"
#include "Collision.h"
#include "DepthFade.h"
#include "Fade.h"
#include "Bloom.h"

void Scene::Init()
{
    m_Fade = AddGameObject <Fade>(RENDER_LAYER_FADE);
    m_IsFade = false;


    Renderer::CreateVertexShader(&m_VertexShader, &m_VertexLayout, "shader/cso/unlitTextureVS.cso");

}

void Scene::Start()
{
    for (const auto& layer : m_GameObject)
    {
        for (const auto& GameObject : layer)
        {
            GameObject->Start();
        }
    }
}

void Scene::Uninit()
{
	Collision::Clear();

	for (auto& layer : m_GameObject)
	{
		for (const auto& GameObject : layer)
		{
			GameObject->Uninit();
		}
		layer.clear();
	}
}

void Scene::Update()
{
    Collision::Update();

	for (const auto& layer : m_GameObject)
	{
		for (const auto& GameObject : layer)
		{
			GameObject->Update();
		}
	}

	for (auto& layer : m_GameObject)
	{
		layer.remove_if([](GameObject* object)
			{
				return object->Destory();
			});
	}
}

void Scene::Draw()
{
    bool hasCamera = !m_GameObject[RENDER_LAYER_CAMERA].empty();

    if (hasCamera)
    {
        Camera* camera = GetGameObject<Camera>();
        Vector3 cameraPos = camera->GetPosition();
        XMMATRIX view = camera->GetCmeraView();

        if (camera != nullptr)
        {
            // Set Camera Matrix
            camera->Draw();

            // Render shadow maps for all lights
            const std::vector<LIGHT>& lights = GlobalLightsManager::GetLights();
            for (size_t i = 0; i < lights.size(); ++i)
            {
                RenderShadowPass(lights[i], static_cast<int>(i));
            }

            // Bind shadow maps for the main pass
            ShadowMap::BindShadowMaps();


            // Restore main viewport
            D3D11_VIEWPORT mainViewport = {};
            mainViewport.Width = (float)SCREEN_WIDTH;
            mainViewport.Height = (float)SCREEN_HEIGHT;
            mainViewport.MinDepth = 0.0f;
            mainViewport.MaxDepth = 1.0f;
            mainViewport.TopLeftX = 0;
            mainViewport.TopLeftY = 0;
            Renderer::GetDeviceContext()->RSSetViewports(1, &mainViewport);

            ID3D11DeviceContext* context = Renderer::GetDeviceContext();
            ID3D11RenderTargetView* postRTV = Renderer::GetRenderTargetView();
            ID3D11RenderTargetView* sceneRTV = Renderer::GetSceneRTV();
            ID3D11DepthStencilView* depthView = Renderer::GetDepthStencilView();
            ID3D11DepthStencilView* depthMainView = Renderer::GetDepthStencilMainView();

            float clearColor[4] = { 0.0f, 0.35f, 0.5f, 1.0f };

            camera->Draw();

            // Render Depth texture
            context->OMSetRenderTargets(0, nullptr, depthView);
            context->ClearDepthStencilView(depthView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

            Renderer::GetDeviceContext()->IASetInputLayout(m_VertexLayout);
            Renderer::GetDeviceContext()->VSSetShader(m_VertexShader, NULL, 0);

            for (const auto& gameObject : m_GameObject[RENDER_LAYER_OPAQUE])
            {
                gameObject->Draw();
            }

            // Render 3D layers
            context->OMSetRenderTargets(1, &sceneRTV, depthMainView);
            context->ClearRenderTargetView(sceneRTV, clearColor);
            context->ClearDepthStencilView(depthMainView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

            for (int i = RENDER_LAYER_OPAQUE; i <= RENDER_LAYER_XRAY; i++)
            {
                switch (i)
                {
                case RENDER_LAYER_OPAQUE:
                    break;

                case RENDER_LAYER_SKYBOX:
                    Renderer::SetDepthWriteMaskEnable(false);
                    break;

                case RENDER_LAYER_TRANSLUCENT:
                {
                    Renderer::SetAlphaBlendEnable(true);

                    auto& list = m_GameObject[i];

                    list.sort([&](const GameObject* a, const GameObject* b)
                        {
                            float da = (a->GetPosition() - cameraPos).lengthSq();
                            float db = (b->GetPosition() - cameraPos).lengthSq();
                            return da > db;
                        });

                    break;
                }

                case RENDER_LAYER_PARTICLE:
                {
                    auto& list = m_GameObject[i];
                    list.sort([&](const GameObject* a, const GameObject* b)
                        {
                            float za = XMVectorGetZ(XMVector3Transform(a->GetPositionXM(), view));
                            float zb = XMVectorGetZ(XMVector3Transform(b->GetPositionXM(), view));
                            return za > zb;
                        });
                    break;
                }

                case RENDER_LAYER_XRAY:
                {
                    Renderer::SetDepthEnable(false);
                    break;
                }
                }

                for (const auto& gameObject : m_GameObject[i])
                {
                    gameObject->Draw();
                }
            }

            // Render 2D layer
            context->OMSetRenderTargets(1, &postRTV, nullptr);
            context->ClearRenderTargetView(postRTV, clearColor);
            for (int i = RENDER_LAYER_POST; i <= RENDER_LAYER_FADE; i++)
            {
                switch (i)
                {
                case RENDER_LAYER_POST:
                    Renderer::SetAlphaBlendEnable(false);
                    break;

                case RENDER_LAYER_UI:
                    Renderer::SetAlphaBlendEnable(true);
                    break;
                }

                for (const auto& gameObject : m_GameObject[i])
                {
                    gameObject->Draw();
                }
            }

            Renderer::SetDepthEnable(true);
            Renderer::SetAlphaBlendEnable(false);
        }
    }
}

void Scene::RenderShadowPass(const LIGHT& light, int lightIndex)
{
    if (!light.CastsShadow) return;

    int numFaces = (light.Type == LIGHT_POINT) ? 6 : 1;

    for (int face = 0; face < numFaces; face++)
    {
        // Set light's view-projection matrix for the current face
        XMFLOAT4X4 lightVP;
        XMStoreFloat4x4(&lightVP, XMMatrixTranspose(XMLoadFloat4x4(&light.LightViewProjection[face])));
        Renderer::GetDeviceContext()->UpdateSubresource(Renderer::GetProjectionBuffer(), 0, nullptr, &lightVP, 0, 0);

        // Clear depth
        Renderer::GetDeviceContext()->ClearDepthStencilView(ShadowMap::GetShadowMapDSV(lightIndex, face), D3D11_CLEAR_DEPTH, 1.0f, 0);

        // Set render target to shadow map
        Renderer::GetDeviceContext()->OMSetRenderTargets(0, nullptr, ShadowMap::GetShadowMapDSV(lightIndex, face));

        // Set viewport
        D3D11_VIEWPORT shadowViewport = {};
        shadowViewport.Width = SHADOW_MAP_SIZE;
        shadowViewport.Height = SHADOW_MAP_SIZE;
        shadowViewport.MinDepth = 0.0f;
        shadowViewport.MaxDepth = 1.0f;
        shadowViewport.TopLeftX = 0;
        shadowViewport.TopLeftY = 0;
        Renderer::GetDeviceContext()->RSSetViewports(1, &shadowViewport);

        // Set shadow vertex shader and input layout
        Renderer::GetDeviceContext()->VSSetShader(ShadowMap::GetShadowVertexShader(), nullptr, 0);
        Renderer::GetDeviceContext()->IASetInputLayout(ShadowMap::GetShadowVertexLayout());

        // Render opaque game objects
        for (const auto& gameObject : m_GameObject[RENDER_LAYER_OPAQUE])
        {
            gameObject->DrawShadow();
        }
    }

    // Restore render target for the main pass
    ID3D11RenderTargetView* renderTargetView = Renderer::GetRenderTargetView();
    ID3D11DepthStencilView* depthStencilView = Renderer::GetDepthStencilView();
    Renderer::GetDeviceContext()->OMSetRenderTargets(1, &renderTargetView, depthStencilView);
}

