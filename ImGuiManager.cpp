#include "ImGuiManager.h"
#include "Renderer.h"
#include "Manager.h"
#include "Scene.h"
#include "Camera.h"
#include "Player.h"
#include "Enemy.h"
#include "Sight.h"
#include "DepthFade.h"

void ImGuiManager::Init(HWND hWnd)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::StyleColorsDark();

    ImGui_ImplWin32_Init(hWnd);
    ImGui_ImplDX11_Init(Renderer::GetDevice(), Renderer::GetDeviceContext());
}

void ImGuiManager::Uninit()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void ImGuiManager::NewFrame()
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void ImGuiManager::Render()
{
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void ImGuiManager::DrawDebugWindows()
{
    ImGui::Begin("Debug Window");
    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
    //static float clearColor[3] = { 0.3f, 0.3f, 0.3f };
    //ImGui::ColorEdit3("Clear Color", clearColor);

    Camera* camera = Manager::GetScene()->GetGameObject<Camera>();
    if (camera)
    {
        Vector3 rota = camera->GetRotation();
        ImGui::Text("Camera Rotation: X=%.2f, Y=%.2f, Z=%.2f", XMConvertToDegrees(rota.x), XMConvertToDegrees(rota.y), XMConvertToDegrees(rota.z));
    }

    // Player Scale Editor
    Player* player = Manager::GetScene()->GetGameObject<Player>();
    if (player)
    {
        Vector3 pos = player->GetPosition();
        ImGui::Text("Player Position: X=%.2f, Y=%.2f, Z=%.2f", pos.x, pos.y, pos.z);

        //float position[3] = { player->GetPosition().x, player->GetPosition().y, player->GetPosition().z };
        //if (ImGui::SliderFloat3("Player Position", position, -10.0f, 10.0f))
        //{
        //    player->SetPosition(Vector3(position[0], position[1], position[2]));
        //}

        //float rotation[3] = { player->GetRotation().x, player->GetRotation().y, player->GetRotation().z };
		//if (ImGui::SliderFloat3("Player Rotation", rotation, -180.0f, 180.0f))
		//{
		//	player->SetRotation(Vector3(rotation[0], rotation[1], rotation[2]));
		//}

        //float Trotation[3] = { player->m_TurretRotation.x, player->m_TurretRotation.y, player->m_TurretRotation.z };
        //if (ImGui::SliderFloat3("T Rotation", Trotation, -180.0f, 180.0f))
        //{
        //    player->m_TurretRotation = Vector3(rotation[0], rotation[1], rotation[2]);
        //}

        //float scale[3] = { player->GetScale().x, player->GetScale().y, player->GetScale().z};
        //float scale = 0.005f;
        //if (ImGui::SliderFloat("Player Scale", &scale, 0.005f, 1.0f))
        //{
        //    player->SetScale(Vector3(scale, scale, scale));
        //}


        //ImGui::Text("Turret Angle: %.2f", player->turretAngle);
        //if (ImGui::DragFloat("Turret Angle",0.0f)
        //{
        //    player->turretAngle;
        //}

        //float UV[2] = { player->m_TrackUVOffset.x, player->m_TrackUVOffset.y };
        //if (ImGui::SliderFloat2("UV", UV, -1.0f, 1.0f))
        //{
		//	player->m_TrackUVOffset = XMFLOAT2(UV[0], UV[1]);
        //}

    }

    //ImGui::Text("Bloom");
    //Bloom* bloom = Manager::GetScene()->GetGameObject<Bloom>();
    //if (bloom)
    //{
    //    static float threshold = 0.5f;
    //    if (ImGui::SliderFloat("Threshold", &threshold, -1.0f, 1.0f))
    //    {
    //        bloom->SetThreshold(threshold);
    //    }
    //    static float intensity = 0.5f;
    //    if (ImGui::SliderFloat("Intensity", &intensity, 0.0f, 10.0f))
    //    {
    //        bloom->SetIntensity(intensity);
    //    }
    //    static float thresholdSoftness = 0.2f;
    //    if (ImGui::SliderFloat("Threshold Softness", &thresholdSoftness, 0.001f, 1.0f))
    //    {
    //        bloom->SetThresholdSoftness(thresholdSoftness);
    //    }
    //    static float size = 100.0f;
    //    if (ImGui::SliderFloat("Blur Size", &size, 1.0f, 500.0f))
    //    {
    //        bloom->SetBlurSize(size);
    //    }
    //    static int radius = 12;
    //    if (ImGui::SliderInt("Blur Radius", &radius, 1, 64))
    //    {
    //        bloom->SetBlurRadius(radius);
    //    }
    //    static float sigma = 100.0f;
    //    if (ImGui::SliderFloat("Blur Sigma", &sigma, 0.01f, 100.0f))
    //    {
    //        bloom->SetBlurSigma(sigma);
    //    }
    //}

    //ImGui::Text("DepthFade");
    //DepthFade* depthFade = Manager::GetScene()->GetGameObject<DepthFade>();
    //if (depthFade)
    //{
    //    static float FadeDistance = 0.2f;
    //    if (ImGui::SliderFloat("FadeDistance", &FadeDistance, -100.0f, 100.0f))
    //    {
    //        depthFade->SetFadeDistance(FadeDistance);
    //    }

    //    static float nearPlane = 0.001f;
    //    if (ImGui::SliderFloat("nearPlane", &nearPlane, 0.0f, 0.5f))
    //    {
    //        depthFade->SetNearPlane(nearPlane);
    //    }

    //    static float farPlane = 100.0f;
    //    if (ImGui::SliderFloat("farPlane", &farPlane, 0.0f, 100.0f))
    //    {
    //        depthFade->SetFarPlane(FadeDistance);
    //    }
    //}

    Sight* frontSight = Manager::GetScene()->GetGameObject<Sight>();
    if (frontSight)
    {
        Vector3 pos = frontSight->GetPosition();
        ImGui::Text("FrontSight Position: X=%.2f, Y=%.2f, Z=%.2f", pos.x, pos.y, pos.z);
    }

    ImGui::End();
}