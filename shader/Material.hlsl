struct MATERIAL
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    float4 Emission;
    float Shininess;
    bool TextureEnable;
    float2 UV_Offset;
};

cbuffer MaterialBuffer : register(b6)
{
	MATERIAL Material;
}




