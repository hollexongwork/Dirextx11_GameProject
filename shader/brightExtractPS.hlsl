#include "common.hlsl"

Texture2D		g_Texture : register(t0);
SamplerState	g_SamplerState : register(s1);

cbuffer BloomParams : register(b8)
{
    float Threshold;
    float Intensity;
    float ThresholdSoftness;
    float Padding;
};

float SoftKneeWeight(float lum, float threshold, float knee)
{
    // knee is usually threshold * 0.5 or a small constant
    // compute a smooth ramp from threshold-knee to threshold+knee
    float lower = threshold - knee;
    float upper = threshold + knee;
    // avoid divide by zero
    if (knee <= 0.0001)
        return lum > threshold ? 1.0 : 0.0;
    // smooth Hermite
    float x = saturate((lum - lower) / (upper - lower));
    return x * x * (3.0 - 2.0 * x); // smoothstep
}

//void main(in PS_IN In, out float4 outDiffuse : SV_Target)
//{
//    float4 color = g_Texture.Sample(g_SamplerState, In.TexCoord);
//    float brightness = dot(color.rgb, float3(0.2126, 0.7152, 0.0722)); 
    
//    float contribution = saturate((brightness - Threshold) / ThresholdSoftness);
//    contribution = pow(contribution, Padding);
    
//    outDiffuse = color * contribution ;
//    outDiffuse.a = 1.0f;
//}

void main(in PS_IN In, out float4 outDiffuse : SV_Target)
{
    float4 color = g_Texture.Sample(g_SamplerState, In.TexCoord);
    float brightness = dot(color.rgb, float3(0.2126, 0.7152, 0.0722));
    
    float w = SoftKneeWeight(brightness, Threshold, ThresholdSoftness);    
    
    outDiffuse.rgb = color.rgb * w;
    outDiffuse.a = 1.0f;
}
