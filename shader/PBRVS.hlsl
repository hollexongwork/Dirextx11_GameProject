#include "common.hlsl"

void main(in VS_IN In, out PS_IN Out)
{
    matrix wvp = mul(mul(World, View), Projection);
    Out.Position = mul(In.Position, wvp);
    Out.WorldPosition = mul(In.Position, World);

    float4 Normal = float4(In.Normal.xyz, 0.0f);
    float4 worldNormal = normalize(mul(Normal, World));
    Out.Normal = worldNormal;

    float4 T = float4(In.Tangent.xyz, 0.0f);
    Out.Tangent = normalize(mul(T, World)).xyz;

    Out.TexCoord = In.TexCoord;
    Out.Diffuse = In.Diffuse;
}