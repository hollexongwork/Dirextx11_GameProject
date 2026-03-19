#define MAX_LIGHTS        5
#define LIGHT_POINT       0
#define LIGHT_SPOT        1
#define LIGHT_DIRECTIONAL 2

struct LIGHT
{
    bool Enable;
    int Type; // 0:Point, 1:Spot, 2:Directional
    float AngleDeg;
    float InnerAngleMul;
    
    float4 Position;
    float4 Direction;
    
    float4 Diffuse;
    float4 Ambient;
    float4 Attenuation;
    float4 Angle;
    
    bool CastsShadow;
    int LightIndex;
    int Padding;
    float Range;
    matrix LightViewProjection[6];
};

cbuffer LightBuffer : register(b5)
{
	LIGHT Lights[MAX_LIGHTS];
    int LightCount;
    float3 padding;
}
