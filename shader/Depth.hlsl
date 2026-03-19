#define SCREEN_WIDTH	(1920)
#define SCREEN_HEIGHT	(1080)

cbuffer InvViewProjBuffer : register(b3)
{
    matrix InvViewProj;
}

SamplerState g_DepthSampler : register(s1)
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Clamp;
    AddressV = Clamp;
};

static const float NearZ = 0.1f;
static const float FarZ = 1500.0f;

static const float3 FogColor = float3(1.5f, 1.0f, 0.8f);
static const float FogDensity = 2.5f;
static const float FogHeight = 25.0f;
static const float FogHeightRange = 80.0f;
static const float HeightDensity = 1.5f;
static const float FogLightCoef = 2.0f;

float LinearizeDepth(float depth)
{   
    float z = depth * 2.0f - 1.0f;
    return (2.0f * NearZ * FarZ) / (FarZ + NearZ - z * (FarZ - NearZ));
}

float Fog(float4 inPosition, Texture2D depthTexture,float4 worldPos)
{
    float2 screenUV = inPosition.xy / float2(SCREEN_WIDTH, SCREEN_HEIGHT);
    float sceneDepth = depthTexture.Sample(g_DepthSampler, screenUV).r;
    float linearSceneDepth = LinearizeDepth(sceneDepth);
    
    float distFog = 1.0f - exp(-linearSceneDepth * FogDensity);
    
    float height = worldPos.y;
    float heightDiff = ((FogHeight - height) / FogHeightRange);
    float heightFog = 1.0f - exp(-max(heightDiff, 0.0f) * HeightDensity);
    
    return saturate(distFog * heightFog);
}

float DepthFade(float4 inPosition, float fadeDistance, Texture2D depthTexture)
{
    float2 screenUV = inPosition.xy / float2(SCREEN_WIDTH, SCREEN_HEIGHT);
    float sceneDepth = depthTexture.Sample(g_DepthSampler, screenUV).r;
    float linearSceneDepth = LinearizeDepth(sceneDepth);
    
    float pixelDepth = inPosition.z;
    float linearPixelDepth = LinearizeDepth(pixelDepth);
        
    float depthDiff = linearSceneDepth - linearPixelDepth;
    
    return saturate(depthDiff / max(fadeDistance, 0.001f)); 
}