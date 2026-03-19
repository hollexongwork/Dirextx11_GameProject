#include "ShadowMapping.hlsl"

#define PI              (3.14159265359f)

// GGX Distribution
float CalculateGGX(float roughness, float nh)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float nh2 = nh * nh;
    float denom = nh2 * (a2 - 1.0f) + 1.0f;
    return a2 / (PI * denom * denom);
}

// Fresnel-Schlick
float3 CalculateFresnel(float3 f0, float vh)
{
    return f0 + (1.0f - f0) * pow(1.0f - vh, 5.0f);
}

// Geometry Schlick-GGX
float GeometrySchlickGGX(float nd, float roughness)
{
    float r = roughness + 1.0f;
    float k = (r * r) / 8.0f;
    return nd / (nd * (1.0f - k) + k);
}

float GeometrySmith(float3 normal, float3 viewDir, float3 lightDir, float roughness)
{
    float nv = max(dot(normal, viewDir), 0.0);
    float nl = max(dot(normal, lightDir), 0.0);
    return GeometrySchlickGGX(nv, roughness) * GeometrySchlickGGX(nl, roughness);
}

float CalculateSpotAttenuation(float3 lightDir, float3 dirToPixel, float2 angles)
{
    float cosTheta = dot(normalize(lightDir), normalize(dirToPixel));
    float inner = cos(angles.x);
    float outer = cos(angles.y);
    return saturate((cosTheta - outer) / max(inner - outer, 0.001f));
}

float3 CalculateLight(LIGHT light, float3 normal, float3 viewDir, float3 worldPos, float3 albedo, float roughness, float metallic)
{
    if (!light.Enable)
        return float3(0, 0, 0);

    float3 lightDir;
    float attenuation = 1.0f;

    if (light.Type == LIGHT_DIRECTIONAL)
    {
        lightDir = normalize(-light.Direction.xyz);
    }
    else
    {
        float3 toLight = light.Position.xyz - worldPos;
        float dist = length(toLight);
        lightDir = toLight / dist;

        float a = light.Attenuation.x;
        float b = light.Attenuation.y;
        float c = light.Attenuation.z;
        attenuation = 1.0f / (a + b * dist + c * dist * dist);
        attenuation = max(attenuation, 0.0f);

        if (light.Type == LIGHT_SPOT) // Spot
        {
            float spot = CalculateSpotAttenuation(light.Direction.xyz, -lightDir, light.Angle.xy);
            attenuation *= spot;
        }
    }

    float3 halfVec = normalize(lightDir + viewDir);
    float nv = max(dot(normal, viewDir), 0.0);
    float nl = max(dot(normal, lightDir), 0.0);
    float nh = max(dot(normal, halfVec), 0.0);
    float vh = max(dot(viewDir, halfVec), 0.0);

    float3 f0 = lerp(float3(0.04f, 0.04f, 0.04f), albedo, metallic);
    float3 F = CalculateFresnel(f0, vh);
    float D = CalculateGGX(roughness, nh);
    float G = GeometrySmith(normal, viewDir, lightDir, roughness);

    float3 kS = F;
    float3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;

    float3 diffuse = kD * albedo / PI;
    float3 specular = (D * F * G) / max(4.0 * nv * nl, 0.001f);

    return (diffuse + specular) * light.Diffuse.rgb * nl * attenuation;
}