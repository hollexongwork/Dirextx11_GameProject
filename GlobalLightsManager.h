#pragma once

#define MAX_GLOBAL_LIGHTS 5

enum LightType 
{
	LIGHT_POINT		  = 0,
	LIGHT_SPOT		  = 1,
	LIGHT_DIRECTIONAL = 2
};

enum LightID
{
	//-----DirectionalLight-----
	DirectionalLight_0,

	//---------SpotLight---------
	SpotLight_0,
	SpotLight_1,

	//--------PointLight--------
	PointLight_0,
};

struct LIGHT
{
	BOOL		Enable;
	LightType 	Type;
	float       AngleDeg;
	float		InnerAngleMul;

	XMFLOAT4	Position;
	XMFLOAT4	Direction;

	XMFLOAT4	Diffuse;
	XMFLOAT4	Ambient;
	XMFLOAT4	Attenuation;
	XMFLOAT4    Angle;

	BOOL 		CastsShadow;
	int			LightIndex;
	int			Padding; // Padding to ensure proper alignment
	float		Range;
	XMFLOAT4X4  LightViewProjection[6];
};

class GlobalLightsManager
{
private:
	static std::vector<LIGHT> Lights;
	
public:
	static void Init();
	static void UpdateLight(int index, const LIGHT& light);
	static void AddLight(const LIGHT& light);
	static void DisableLight(int index);
	static const std::vector<LIGHT>& GetLights();
	static LIGHT& GetLightByID(LightID id);
	static XMMATRIX ComputeLightViewProjectionMatrix(
		const XMFLOAT4& lightDirection, 
		const XMFLOAT4& lightPosition,  
		float nearPlane,                
		float farPlane,                 
		float fovAngleY,                
		float aspectRatio,              
		bool isDirectional              
	);

	static void UpdateDirectionalLightShadow(
		int index,
		const XMVECTOR followPos,
		float orthoSize,
		float nearPlane,
		float farPlane);
};

