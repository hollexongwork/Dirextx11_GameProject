#include "common.hlsl"
#include "Material.hlsl"
#include "Depth.hlsl"
#include "HDR.hlsl"

Texture2D g_Texture : register(t0);
Texture2D g_SceneDepth : register(t1);
SamplerState g_SamplerState : register(s0);

void main(in PS_IN In, out float4 outDiffuse : SV_Target)
{
    float3 color = g_Texture.Sample(g_SamplerState, In.TexCoord);
    
    float fog = Fog(In.Position, g_SceneDepth, In.WorldPosition);
    float3 fogLight = AMBIENT * FogLightCoef;
    float3 fogColorHDR = FogColor * fogLight;
    
    color = lerp(color, fogColorHDR, fog);
    
    color *= EXPOSURE;
    
    color = ACESFilm(color);
    
    outDiffuse = float4(color, 1.0f) * In.Diffuse;
}