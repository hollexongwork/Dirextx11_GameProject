float ConstantBiasScale(float x, float bias, float scale)
{
    return (x + bias) * scale;
}
float2 ConstantBiasScale(float2 x, float bias, float scale)
{
    return (x + bias) * scale;
}
float3 ConstantBiasScale(float3 x, float bias, float scale)
{
    return (x + bias) * scale;
}
float4 ConstantBiasScale(float4 x, float bias, float scale)
{
    return (x + bias) * scale;
}

float3 DeriveNormalZ(float2 XY)
{
    float Z = sqrt(saturate(1.0 - dot(XY, XY)));
    return float3(XY, Z);
}