#include "common.hlsl"
#include "PBR.hlsl"
#include "Depth.hlsl"
#include "HDR.hlsl"

struct MATERIAL
{
    float4 UV;
};

Texture2D g_Texture : register(t0);
Texture2D g_NormalMap : register(t1);
Texture2D g_ARM : register(t2);

Texture2D g_TextureD : register(t4);
Texture2D g_NormalMapD : register(t5);
Texture2D g_ARMD : register(t6);

Texture2D g_Emission : register(t7);

Texture2D g_SceneDepth : register(t3);

SamplerState g_SamplerState : register(s0);

cbuffer MaterialBuffer : register(b7)
{
    MATERIAL Material;
}

void main(in PS_IN In, out float4 outDiffuse : SV_Target)
{
    float2 UV = In.TexCoord + Material.UV.xy;
    float damage = Material.UV.z;
    // Texture sampling
    float4 albedoN = g_Texture.Sample(g_SamplerState, UV);
    clip(albedoN.a - 0.5f);
    
    float4 albedoD = g_TextureD.Sample(g_SamplerState, UV);
    clip(albedoD.a - 0.5f);
    
    float4 albedo = lerp(albedoD, albedoN, damage);
    //---------------------------------------------------------
    float normalBias = -0.5f;
    float4 normalMapN = g_NormalMap.SampleBias(g_SamplerState, UV, normalBias);
    float4 normalMapD = g_NormalMapD.SampleBias(g_SamplerState, UV, normalBias);

    float4 normalMap = lerp(normalMapD, normalMapN, damage);
    //---------------------------------------------------------
    float4 ARM_N = g_ARM.Sample(g_SamplerState, UV);
    float4 ARM_D = g_ARMD.Sample(g_SamplerState, UV);
    
    float4 ARM = lerp(ARM_D, ARM_N, damage);
    
    float ao = ARM.r;
    float roughness = ARM.g;
    float metallic = ARM.b;
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
    
    //normal = lerp(In.Normal.xyz, normal, 1.0f);
    //-------------------------------------------------------------------------------
       
    float3 viewDir = normalize(CameraPosition.xyz - In.WorldPosition.xyz);

    // Base reflectivity
    float3 lighting = float3(0.0f, 0.0f, 0.0f);

    [loop]
    for (int i = 0; i < LightCount; i++)
    {
        float shadowFactor = CalculateShadowFactor(Lights[i], In.WorldPosition.xyz, normal);
        lighting += CalculateLight(Lights[i], normal, viewDir, In.WorldPosition.xyz, albedo.rgb, roughness, metallic) * shadowFactor;
    }

    // Ambient
    float3 ambient = AMBIENT * ao * albedo.rgb;
    
    // Emission
    float3 emission = g_Emission.Sample(g_SamplerState, UV).rgb * 1.2f;

    // HDR compositing
    float3 hdrColor = lighting + ambient + emission;

    // Fog
    float fog = Fog(In.Position, g_SceneDepth, In.WorldPosition);
    float3 fogLight = ambient * FogLightCoef;
    float3 fogColorHDR = FogColor * fogLight;
    
    hdrColor = lerp(hdrColor, fogColorHDR, fog);
    
    // Exposure
    hdrColor *= EXPOSURE;
    
    // Tone Mapping
    float3 color = ACESFilm(hdrColor);
    
    // Output
    outDiffuse = float4(color, albedo.a) * In.Diffuse;
}

