#pragma once

#define SHADOW_MAP_SIZE (1024.0f)

class ShadowMap
{
private:
    static ID3D11Texture2D* m_ShadowMapTexture[MAX_GLOBAL_LIGHTS];
    static ID3D11DepthStencilView* m_ShadowMapDSV[MAX_GLOBAL_LIGHTS][6];
    static ID3D11ShaderResourceView* m_ShadowMapSRV[MAX_GLOBAL_LIGHTS];
    static ID3D11SamplerState* m_ShadowSamplerState;
    static ID3D11VertexShader* m_ShadowVertexShader;
    static ID3D11InputLayout* m_ShadowVertexLayout;

    static constexpr int StartSlot = 8;

public:
    static void Init();
    static void Uninit();
    static void RenderShadowMap(const LIGHT& light, int lightIndex);
    static void BindShadowMaps();
    static void CreateShadowMapForLight(int index, LightType type);
    static void ReleaseShadowMap(int index);

    static ID3D11Texture2D* GetShadowMapTexture(int index) { return m_ShadowMapTexture[index]; }
    static ID3D11VertexShader* GetShadowVertexShader() { return m_ShadowVertexShader; }
    static ID3D11InputLayout* GetShadowVertexLayout() { return m_ShadowVertexLayout; }
    static ID3D11DepthStencilView* GetShadowMapDSV(int lightIndex, int face) { return m_ShadowMapDSV[lightIndex][face]; }
};

