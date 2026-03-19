#include "main.h"
#include "Renderer.h"
#include "GlobalLightsManager.h"

std::vector<LIGHT> GlobalLightsManager::Lights;

void GlobalLightsManager::Init()
{
	Lights.clear();
	Lights.reserve(MAX_GLOBAL_LIGHTS);

	//-----------------------DirectionalLight-----------------------
	LIGHT DirectionalLight_0 = {};
	DirectionalLight_0.Enable = true;
	DirectionalLight_0.Type = LIGHT_DIRECTIONAL;
	DirectionalLight_0.Position = XMFLOAT4(0.0f, 5.0f, 0.0f, 1.0f);
	DirectionalLight_0.Direction = XMFLOAT4(-1.0f, -1.0f, 0.0f, 0.0f);
	DirectionalLight_0.Diffuse = XMFLOAT4(8.0f, 7.5f, 6.5f, 1.0f);
	DirectionalLight_0.Ambient = XMFLOAT4(0.02f, 0.02f, 0.02f, 1.0f);
	DirectionalLight_0.CastsShadow = true;
	DirectionalLight_0.LightIndex = 0;

	// Compute light view-projection matrix
	float orthoSize = 500.0f;
	float nearPlane = 0.1f;
	float farPlane = 1000.0f;
	XMStoreFloat4x4(&DirectionalLight_0.LightViewProjection[0], 
		ComputeLightViewProjectionMatrix(DirectionalLight_0.Direction, DirectionalLight_0.Position, nearPlane, farPlane, orthoSize, orthoSize, true));

	Lights.push_back(DirectionalLight_0);

	//-----------------------SpotLight-----------------------
	//LIGHT SpotLight_0 = {};
	//SpotLight_0.Enable = true;
	//SpotLight_0.Type = LIGHT_SPOT;
	//SpotLight_0.Position = XMFLOAT4(0.0f, 3.5f, 0.0f, 1.0f);
	//SpotLight_0.Direction = XMFLOAT4(0.0f, -1.0f, 0.0f, 0.0f);
	//SpotLight_0.Diffuse = XMFLOAT4(25.0f, 25.0f, 25.0f, 1.0f);
	//SpotLight_0.Ambient = XMFLOAT4(0.03f, 0.03f, 0.03f, 1.0f);
	//SpotLight_0.Attenuation = XMFLOAT4(1.0f, 0.7f, 1.8f, 0.0f);
	//SpotLight_0.AngleDeg = 60.0f;
	//SpotLight_0.InnerAngleMul = 0.8f;
	//SpotLight_0.Angle = XMFLOAT4(XMConvertToRadians(SpotLight_0.AngleDeg * SpotLight_0.InnerAngleMul), XMConvertToRadians(SpotLight_0.AngleDeg), 0.0f, 0.0f);
	//SpotLight_0.CastsShadow = false;
	//SpotLight_0.LightIndex = 1;

	//Lights.push_back(SpotLight_0);
		
	//LIGHT SpotLight_1 = {};
	//SpotLight_1.Enable = true;
	//SpotLight_1.Type = LIGHT_SPOT;
	//SpotLight_1.Position = XMFLOAT4(-3.0f, 5.0f, -3.0f, 1.0f);
	//SpotLight_1.Direction = XMFLOAT4(1.0f, -1.0f, 1.0f, 0.0f);
	//SpotLight_1.Diffuse = XMFLOAT4(2.0f, 2.0f, 500.0f, 1.0f);
	//SpotLight_1.Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	//SpotLight_1.Attenuation = XMFLOAT4(1.0f, 0.7f, 1.8f, 0.0f);
	//SpotLight_1.AngleDeg = 60.0f;
	//SpotLight_1.InnerAngleMul = 0.8f;
	//SpotLight_1.Angle = XMFLOAT4(XMConvertToRadians(SpotLight_1.AngleDeg * SpotLight_1.InnerAngleMul), XMConvertToRadians(SpotLight_1.AngleDeg), 0.0f, 0.0f);
	//SpotLight_1.CastsShadow = true;
	//SpotLight_1.LightIndex = 2;
	//XMStoreFloat4x4(&SpotLight_1.LightViewProjection[0],
	//	ComputeLightViewProjectionMatrix(SpotLight_1.Direction, SpotLight_1.Position, 0.1f, 100.0f, XMConvertToRadians(120.0f), 1.0f, false));
	//Lights.push_back(SpotLight_1);
	
	//-----------------------PointLight-----------------------
	//LIGHT PointLight_0 = {};
	//PointLight_0.Enable = true;
	//PointLight_0.Type = LIGHT_POINT;
	//PointLight_0.Position = XMFLOAT4(2.0f, 2.0f, 2.0f, 1.0f);
	//PointLight_0.Diffuse = XMFLOAT4(100.0f, 100.0f, 100.0f, 1.0f);
	//PointLight_0.Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	//PointLight_0.Attenuation = XMFLOAT4(1.5f, 0.7f, 1.8f, 0.0f);
	//PointLight_0.CastsShadow = true;
	//PointLight_0.LightIndex = 3;
	//PointLight_0.Range = 10000.0f; // Set range for shadow projection
	// Initialize cube map faces (+X, -X, +Y, -Y, +Z, -Z)
	//XMVECTOR pos = XMLoadFloat4(&PointLight_0.Position);
	//for (int face = 0; face < 6; face++)
	//{
	//	XMVECTOR lookAtDir, up;
	//	switch (face)
	//	{
	//	case 0: lookAtDir = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f); up = XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f); break; // +X
	//	case 1: lookAtDir = XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f); up = XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f); break; // -X
	//	case 2: lookAtDir = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f); up = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f); break; // +Y
	//	case 3: lookAtDir = XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f); up = XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f); break; // -Y
	//	case 4: lookAtDir = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f); up = XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f); break; // +Z
	//	case 5: lookAtDir = XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f); up = XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f); break; // -Z
	//	}
	//	lightView = XMMatrixLookAtLH(pos, pos + lookAtDir, up);
	//	lightProj = XMMatrixPerspectiveFovLH(XM_PI / 2.0f, 1.0f, 0.1f, PointLight_0.Range);
	//	XMStoreFloat4x4(&PointLight_0.LightViewProjection[face], lightView * lightProj);
	//}
	//Lights.push_back(PointLight_0);

	Renderer::SetLights(Lights);
}

void GlobalLightsManager::UpdateLight(int index, const LIGHT& light)
{
	if (index >= 0 && index < Lights.size())
	{
		Lights[index] = light;

		if (light.CastsShadow)
		{
			if (light.Type == LIGHT_POINT)
			{
				XMVECTOR pos = XMLoadFloat4(&light.Position);
				for (int face = 0; face < 6; face++)
				{
					XMVECTOR lookAtDir, up;
					switch (face)
					{
					case 0: lookAtDir = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f); up = XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f); break;
					case 1: lookAtDir = XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f); up = XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f); break;
					case 2: lookAtDir = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f); up = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f); break;
					case 3: lookAtDir = XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f); up = XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f); break;
					case 4: lookAtDir = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f); up = XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f); break;
					case 5: lookAtDir = XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f); up = XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f); break;
					}
					XMMATRIX lightView = XMMatrixLookAtLH(pos, pos + lookAtDir, up);
					XMMATRIX lightProj = XMMatrixPerspectiveFovLH(XM_PI / 2.0f, 1.0f, 0.1f, light.Range);
					XMStoreFloat4x4(&Lights[index].LightViewProjection[face], lightView * lightProj);
				}
			}
			else if (light.Type == LIGHT_SPOT)
			{
				XMMATRIX lightView = XMMatrixLookAtLH(
					XMLoadFloat4(&light.Position),
					XMLoadFloat4(&light.Position) + XMLoadFloat4(&light.Direction),
					XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)
				);
				XMMATRIX lightProj = XMMatrixPerspectiveFovLH(XMConvertToRadians(light.AngleDeg), 1.0f, 0.5f, 50.0f);
				XMStoreFloat4x4(&Lights[index].LightViewProjection[0], lightView * lightProj);
			}
			else if (light.Type == LIGHT_DIRECTIONAL)
			{
				XMMATRIX lightView = XMMatrixLookAtLH(
					XMVectorSet(10.0f, 10.0f, 0.0f, 1.0f),
					XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f),
					XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)
				);
				XMMATRIX lightProj = XMMatrixOrthographicLH(20.0f, 20.0f, 0.1f, 100.0f);
				XMStoreFloat4x4(&Lights[index].LightViewProjection[0], lightView * lightProj);
			}
		}

		Renderer::SetLights(Lights);
	}
}

void GlobalLightsManager::AddLight(const LIGHT& light)
{
	if (Lights.size() < MAX_GLOBAL_LIGHTS)
	{
		Lights.push_back(light);
		Renderer::SetLights(Lights);
	}
}

void GlobalLightsManager::DisableLight(int index)
{
	if (index >= 0 && index < Lights.size())
	{
		Lights[index].Enable = false;
		Renderer::SetLights(Lights);
	}
}

const std::vector<LIGHT>& GlobalLightsManager::GetLights() 
{ 
	return Lights; 
}

LIGHT& GlobalLightsManager::GetLightByID(LightID id)
{
	assert(id >= 0 && id < Lights.size());
	return Lights[static_cast<size_t>(id)];
}

XMMATRIX GlobalLightsManager::ComputeLightViewProjectionMatrix(
    const XMFLOAT4& lightDirection,  
    const XMFLOAT4& lightPosition,   
    float nearPlane,                 
    float farPlane,                  
    float fovAngleY,                 
    float aspectRatio,               
	bool isDirectional // Directional Light fovAngleY = orthoWidth  aspectRatio = orthoHeight
)
{
    // View Matrix
    XMVECTOR lightDir = XMLoadFloat4(&lightDirection);
    XMVECTOR lightPos = XMLoadFloat4(&lightPosition);
    
    XMVECTOR targetPos = XMVectorAdd(lightPos, lightDir);
    XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    
    XMVECTOR lightDirNorm = XMVector3Normalize(lightDir);
    XMVECTOR dotUp = XMVector3Dot(up, lightDirNorm);
    if (XMVectorGetX(XMVectorAbs(dotUp)) > 0.99f)
    {
        up = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
    }

	XMMATRIX viewMatrix = XMMatrixLookAtLH(lightPos, targetPos, up);

	// Projection Matrix
    XMMATRIX projectionMatrix;
    
    if (isDirectional)
    {
		// Directional Light 
        projectionMatrix = XMMatrixOrthographicLH(
			fovAngleY,	
			aspectRatio,
            nearPlane,	
            farPlane    
        );
    }
    else
    {
        // Point Light & Spot Light
        projectionMatrix = XMMatrixPerspectiveFovLH(
            fovAngleY,  
            aspectRatio,
            nearPlane,  
            farPlane    
        );
    }

    return viewMatrix * projectionMatrix;
}

void GlobalLightsManager::UpdateDirectionalLightShadow(
	int index,
	const XMVECTOR followPos,
	float orthoSize,
	float nearPlane,
	float farPlane
)
{
	LIGHT& light = Lights[index];
	assert(light.Type == LIGHT_DIRECTIONAL && light.CastsShadow);

	XMVECTOR lightDir = XMLoadFloat4(&light.Direction);
	XMVECTOR lightPos = XMVectorAdd(followPos, XMVectorSet(0, 5, 0, 0)); // Distance
	XMVECTOR targetPos = XMVectorAdd(lightPos, lightDir);
	XMVECTOR up = XMVectorSet(0, 1, 0, 0);

	XMVECTOR lightDirNorm = XMVector3Normalize(lightDir);
	XMVECTOR dotUp = XMVector3Dot(up, lightDirNorm);
	if (XMVectorGetX(XMVectorAbs(dotUp)) > 0.99f)
		up = XMVectorSet(0, 0, 1, 0); 

	XMMATRIX lightView = XMMatrixLookAtLH(lightPos, targetPos, up);

	XMMATRIX lightProj = XMMatrixOrthographicLH(orthoSize, orthoSize, nearPlane, farPlane);

	XMStoreFloat4x4(&light.LightViewProjection[0], lightView * lightProj);

	Renderer::SetLights(Lights);
}
