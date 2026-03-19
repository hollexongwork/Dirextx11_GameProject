#include "common.hlsl"

Texture2D g_Texture : register(t0);
SamplerState g_SamplerState : register(s1);

cbuffer BlurParams : register(b7)
{
    float2 texelSize;
    int radius;
    float sigma;
    float weights[64];
};

void main(in PS_IN In, out float4 outDiffuse : SV_Target)
{
    outDiffuse.a = g_Texture.Sample(g_SamplerState, In.TexCoord).a;
    outDiffuse.rgb = g_Texture.Sample(g_SamplerState, In.TexCoord).rgb * weights[0];
    for (int i = -radius; i <= radius; i++)
    {
        outDiffuse.rgb += g_Texture.Sample(g_SamplerState, In.TexCoord + float2(texelSize.x * i, 0.0f)) * weights[abs(i)];
    }
}
