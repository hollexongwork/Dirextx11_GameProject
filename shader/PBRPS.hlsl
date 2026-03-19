#include "common.hlsl"
#include "PBR.hlsl"
#include "Material.hlsl"
#include "Depth.hlsl"
#include "HDR.hlsl"

Texture2D g_Texture : register(t0);
Texture2D g_NormalMap : register(t1);
Texture2D g_RoughnessMap : register(t2);
Texture2D g_AOMap : register(t3);

Texture2D g_SceneDepth : register(t4);

SamplerState g_SamplerState : register(s0);

void main(in PS_IN In, out float4 outDiffuse : SV_Target)
{
    float2 UV = In.TexCoord * Material.UV_Offset;
    // Texture sampling
    float4 albedo = g_Texture.Sample(g_SamplerState, UV);
    float normalBias = -0.5f;
    float4 normalMap = g_NormalMap.SampleBias(g_SamplerState, UV, normalBias);
    float roughness = g_RoughnessMap.Sample(g_SamplerState, UV).r;
    float ao = g_AOMap.Sample(g_SamplerState, UV).r;
    float metallic = 0.01f;
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
    
    normal = lerp(In.Normal.xyz, normal, 1.5f);
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
    
    // HDR compositing
    float3 hdrColor = lighting + ambient;

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



