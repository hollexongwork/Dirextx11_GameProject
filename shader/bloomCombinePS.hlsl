#include "common.hlsl"

Texture2D g_Texture : register(t0);
Texture2D g_Texture1 : register(t1);
SamplerState g_SamplerState : register(s1);

cbuffer BloomParams : register(b8)
{
    float Threshold;
    float Intensity;
    
    float Padding[2];
};

void main(in PS_IN In, out float4 outDiffuse : SV_Target)
{
    float4 sceneColor = g_Texture.Sample(g_SamplerState, In.TexCoord);
    float4 bloom = g_Texture1.Sample(g_SamplerState, In.TexCoord);
    outDiffuse.rgb = sceneColor.rgb + bloom.rgb * Intensity;
    outDiffuse.a = 1.0f;
}
