#include "common.hlsl"
#include "PBR.hlsl"
#include "SubUV.hlsl"
#include "Depth.hlsl"
#include "HDR.hlsl"
#include "Functions.hlsl"

struct MATERIAL
{
    float4 Color;
    int Frame;
    int WidthDivision;
    int HeightDivision;
    float DepthFadeDistance;
};

Texture2D g_Texture : register(t0);
Texture2D g_ColorMap : register(t1);
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
    float4 tex = g_Texture.Sample(g_SamplerState, UV);
    float3 albedo = Material.Color.rgb;
    float roughness = 0.0f;
    float metallic = 0.0f;
    //-------------------------------------------------------------------------------
    //Normal map processing
    float2 xy = ConstantBiasScale(tex.rg, -0.5f, 2.0f);
    float3 normal = DeriveNormalZ(xy);
    //-------------------------------------------------------------------------------
       
    float3 viewDir = normalize(CameraPosition.xyz - In.WorldPosition.xyz);

    // Base reflectivity
    float3 lighting = float3(0.0f, 0.0f, 0.0f);

    [loop]
    for (int i = 0; i < LightCount; i++)
    {
        lighting += CalculateLight(Lights[i], normal, viewDir, In.WorldPosition.xyz, albedo.rgb, roughness, metallic) ;
    }
    //-------------------------------------------------------------------------------
    float emissionMask = pow(tex.b, 0.35f);
    float3 emissionColor = g_ColorMap.Sample(g_SamplerState, emissionMask);
    float3 emission = emissionMask * emissionColor * 15.0f;
    
    // Ambient
    float3 ambient = AMBIENT * albedo;
    
    // HDR compositing
    float3 hdrColor = lighting + ambient + emission;

    float alpha = tex.a * Material.Color.a;
    
    outDiffuse = float4(hdrColor, alpha);
    outDiffuse.a *= DepthFade(In.Position, Material.DepthFadeDistance, g_SceneDepth);
}



