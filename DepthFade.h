#pragma once
#include "GameObject.h"

class DepthFade : public GameObject
{
private:
    struct DepthFadeParameters
    {
        float NearPlane;
        float FarPlane;
        float FadeDistance;
        float Padding;
    };

    enum DebugPass
    {
        Scene,
        Depth,
        Faded,
    };

    ID3D11Texture2D* m_FadeTexture = nullptr;
    ID3D11RenderTargetView* m_FadeRTV = nullptr;
    ID3D11ShaderResourceView* m_FadeSRV = nullptr;

    ID3D11Buffer* m_VertexBuffer = nullptr;

    ID3D11InputLayout* m_VertexLayout = nullptr;
    ID3D11VertexShader* m_VertexShader = nullptr;
    ID3D11PixelShader* m_DepthPS = nullptr;
    ID3D11PixelShader* m_FadePS = nullptr;

    ID3D11SamplerState* m_Sampler = nullptr;

    ID3D11Buffer* m_DepthFadeParametersBuffer = nullptr;

    DepthFadeParameters m_Params = { 5.0f, 1000.0f, 0.2f, 0.0f };

    DebugPass m_DebugPass = DebugPass::Depth;

public:
    void Init() override;
    void Uninit() override;
    void Update() override;
    void Draw() override;

    void SetNearPlane(float nearPlane) { m_Params.NearPlane = nearPlane; }
    void SetFarPlane(float farPlane) { m_Params.FarPlane = farPlane; }
    void SetFadeDistance(float distance) { m_Params.FadeDistance = distance; }
};

