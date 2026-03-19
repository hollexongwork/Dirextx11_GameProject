#include "common.hlsl"
#include "PBR.hlsl"
#include "SubUV.hlsl"
#include "Depth.hlsl"
#include "HDR.hlsl"

struct MATERIAL
{
    float4 Color;
    int Frame;
    int WidthDivision;
    int HeightDivision;
    float DepthFadeDistance;
};

Texture2D g_Texture : register(t0);
Texture2D g_NormalMap : register(t1);
Texture2D g_SceneDepth : register(t2);

SamplerState g_SamplerState : register(s0);

cbuffer MaterialBuffer : register(b7)
{
    MATERIAL Material;
}

void main(in PS_IN In, out float4 outDiffuse : SV_Target)
{
    float2 UV = SubUV(In.TexCoord, Material.Frame, Material.WidthDivision, Material.HeightDivision);
    // Texture sampling
    float4 albedo = g_Texture.Sample(g_SamplerState, UV);
    float4 normalMap = g_NormalMap.Sample(g_SamplerState, In.TexCoord);
    float roughness = 0.5f;
    float metallic = 0.0f;
    //-------------------------------------------------------------------------------
    
    //Normal map processing
    float3 tangentNormal = normalMap.xyz * 2.0f - 1.0f;
    tangentNormal = normalize(tangentNormal);
    //tangentNormal.y = -tangentNormal.y;
    
    float3 N = normalize(In.Normal.xyz);
    float3 T = normalize(In.Tangent);
    T = normalize(T - dot(T, N) * N);
    float3 B = cross(N, T);
    float3x3 TBN = float3x3(T, B, N);
    float3 normal = normalize(mul(tangentNormal, TBN));
    
    normal = lerp(In.Normal.xyz, normal, 0.7f);
    //-------------------------------------------------------------------------------
       
    float3 viewDir = normalize(CameraPosition.xyz - In.WorldPosition.xyz);

    // Base reflectivity
    float3 lighting = float3(0.0f, 0.0f, 0.0f);

    [loop]
    for (int i = 0; i < LightCount; i++)
    {
        lighting += CalculateLight(Lights[i], normal, viewDir, In.WorldPosition.xyz, albedo.rgb, roughness, metallic) ;
    }

    // Ambient
    float3 ambient = AMBIENT * albedo.rgb;
    
    // HDR compositing
    float3 hdrColor = lighting + ambient;

    outDiffuse = float4(hdrColor, albedo.a) * Material.Color;
    outDiffuse.a *= DepthFade(In.Position, Material.DepthFadeDistance, g_SceneDepth);
}



