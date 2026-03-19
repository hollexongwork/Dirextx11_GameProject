#include "Light.hlsl"

Texture2D g_ShadowMaps[MAX_LIGHTS] : register(t8);
SamplerComparisonState g_ShadowSampler : register(s2);

float SampleShadow(int lightIndex, float2 shadowUV, float depth, float3 normal, float3 lightDir, int face)
{
    
    float shadow = 0.0f;
    float2 texelSize = 1.0f / 1024.0f;
    float baseBias = 0.005f;
    // Slope-scaled bias
    float slopeBias = tan(acos(saturate(dot(normal, lightDir)))) * 0.002f;
    float bias = max(baseBias, slopeBias);
    
    const int sampleRange = 1;    
    const int sideLength = sampleRange * 2 + 1;
    const int kernelSize = sideLength * sideLength; 
    
    [unroll]
    for (int i = 0; i < kernelSize; i++)
    {
        int x = (i / sideLength) - sampleRange;
        int y = (i % sideLength) - sampleRange;
        float2 offset = float2(x, y) * texelSize;
        // Explicitly sample based on lightIndex to avoid dynamic indexing issues
        float sampleValue = 0.0f;
        [branch]
        switch (lightIndex)
        {
            case 0:
                sampleValue = g_ShadowMaps[0].SampleCmpLevelZero(g_ShadowSampler, float3(shadowUV + offset, face), depth - bias);
                break;
            //case 1:
            //    sampleValue = g_ShadowMaps[1].SampleCmpLevelZero(g_ShadowSampler, float3(shadowUV + offset, face), depth - bias);
            //    break;
            //case 2:
            //    sampleValue = g_ShadowMaps[2].SampleCmpLevelZero(g_ShadowSampler, float3(shadowUV + offset, face), depth - bias);
            //    break;
            //case 3:
            //    sampleValue = g_ShadowMaps[3].SampleCmpLevelZero(g_ShadowSampler, float3(shadowUV + offset, face), depth - bias);
            //    break;
            default:
                sampleValue = 1.0f; // No shadow if lightIndex is invalid
                break;
        }
        shadow += sampleValue;
    }
       
    return shadow / kernelSize;
}

float CalculateShadowFactor(LIGHT light, float3 worldPos, float3 normal)
{
    if (!light.CastsShadow)
        return 1.0f;

    int face = 0;
    float3 toLightDir;

    if (light.Type == LIGHT_POINT)
    {
        float3 toLightVec = light.Position.xyz - worldPos;
        toLightDir = normalize(toLightVec);
        float3 dir = -toLightDir; // From light to pixel
        float3 absDir = abs(dir);
        float maxComp = max(max(absDir.x, absDir.y), absDir.z);
        if (maxComp == absDir.x)
            face = (dir.x > 0) ? 0 : 1; // +X, -X
        else if (maxComp == absDir.y)
            face = (dir.y > 0) ? 2 : 3; // +Y, -Y
        else
            face = (dir.z > 0) ? 4 : 5; // +Z, -Z
    }
    else
    {
        toLightDir = (light.Type == LIGHT_DIRECTIONAL) ? normalize(-light.Direction.xyz) : normalize(light.Position.xyz - worldPos);
    }

    float4 lightSpacePos = mul(float4(worldPos, 1.0f), transpose(light.LightViewProjection[face]));
    lightSpacePos /= lightSpacePos.w;

    float2 shadowUV = float2(
        (lightSpacePos.x + 1.0f) * 0.5f,
        (1.0f - lightSpacePos.y) * 0.5f
    );

    if (shadowUV.x < 0.0f || shadowUV.x > 1.0f || shadowUV.y < 0.0f || shadowUV.y > 1.0f || lightSpacePos.z < 0.0f || lightSpacePos.z > 1.0f)
    {
        return 1.0f; // Outside shadow map, no shadow
    }

    return SampleShadow(light.LightIndex, shadowUV, lightSpacePos.z, normal, toLightDir, face);
}