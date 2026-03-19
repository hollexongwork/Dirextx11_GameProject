#include "common.hlsl"
#include "SubUV.hlsl"
#include "Depth.hlsl"

struct MATERIAL
{
    float4 Color;
    int Frame;
    int WidthDivision;
    int HeightDivision;
    float DepthFadeDistance;
};

Texture2D g_Texture : register(t0);
Texture2D g_SceneDepth : register(t1);

SamplerState g_SamplerState : register(s0);

cbuffer MaterialBuffer : register(b7)
{
    MATERIAL Material;
}

void main(in PS_IN In, out float4 outDiffuse : SV_Target)
{
    float2 UV = SubUV(In.TexCoord, Material.Frame, Material.WidthDivision, Material.HeightDivision);
    
    float4 color = g_Texture.Sample(g_SamplerState, UV ) * Material.Color;
    color *= In.Diffuse;
    
    outDiffuse = color;
    outDiffuse.a *= DepthFade(In.Position, Material.DepthFadeDistance, g_SceneDepth);
}
