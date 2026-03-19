cbuffer WorldBuffer : register(b0)
{
    matrix World;
}
cbuffer ProjectionBuffer : register(b2)
{
    matrix LightViewProj;
};

struct VS_IN
{
    float3 Pos : POSITION;
};

struct VS_OUT
{
    float4 Pos : SV_POSITION;
};

VS_OUT main(VS_IN input)
{
    VS_OUT output;
    float4 worldPos = mul(float4(input.Pos, 1.0f), World);
    output.Pos = mul(worldPos, LightViewProj);
    return output;
}