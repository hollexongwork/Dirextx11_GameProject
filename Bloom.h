#pragma once
#include "GameObject.h"

class Bloom : public GameObject
{
private:
    struct BlurParameters
    {
        XMFLOAT2 texelSize;
        int radius;
        float sigma;
        float weights[64];
    };

    struct BloomParameters
    {
        float Threshold;
        float Intensity;
        float ThresholdSoftness;
        float Padding;
    };

    enum DebugPass
    {
        Scene,
        Bright,
        BlurH,
        BlurV,
        Combined,
    };

    ID3D11Texture2D* m_BrightTexture;
    ID3D11RenderTargetView* m_BrightRTV;
    ID3D11ShaderResourceView* m_BrightSRV;

    ID3D11Texture2D* m_BlurTexture1;
    ID3D11RenderTargetView* m_BlurRTV1;
    ID3D11ShaderResourceView* m_BlurSRV1;

    ID3D11Texture2D* m_BlurTexture2;
    ID3D11RenderTargetView* m_BlurRTV2;
    ID3D11ShaderResourceView* m_BlurSRV2;

    ID3D11Buffer* m_VertexBuffer;

    ID3D11InputLayout* m_VertexLayout;
    ID3D11VertexShader* m_VertexShader;

    ID3D11PixelShader* m_ExtractPS;
    ID3D11PixelShader* m_BlurHPS;
    ID3D11PixelShader* m_BlurVPS;
    ID3D11PixelShader* m_CombinePS;

    ID3D11SamplerState* m_Sampler;

    ID3D11Buffer* m_BlurParametersBuffer;
    ID3D11Buffer* m_BloomParametersBuffer;

                                //Threshold, Intensity, ThresholdSoftness
    BloomParameters m_BloomParams = { 0.75f, 2.5f, 0.35f, 0.0f };
    BlurParameters m_BlurParams = { {1.0f / SCREEN_WIDTH,1.0f / SCREEN_HEIGHT},16,10.0f,{} };

    DebugPass m_DebugPass = DebugPass::Combined;

    std::vector<float> ComputeGaussianWeights(int radius, float sigma);

public:
    void Init() override;
    void Uninit() override;
    void Update() override;
    void Draw() override;

    void SetThreshold(float threshold)
    {
        m_BloomParams.Threshold = threshold;
    }

    void SetIntensity(float intensity)
    {
        m_BloomParams.Intensity = intensity;
    }

    void SetThresholdSoftness(float thresholdSoftness)
    {
        m_BloomParams.ThresholdSoftness = thresholdSoftness;
    }

    void SetBlurRadius(int radius)
    {
        m_BlurParams.radius = radius;
    }

    void SetBlurSigma(float sigma)
    {
        m_BlurParams.sigma = sigma;
    }
};