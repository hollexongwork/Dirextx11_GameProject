#include "main.h"
#include "Renderer.h"
#include "math.h"
#include <DirectXMath.h>
using namespace DirectX;
#include <cmath>
#include <iostream>
#include "Vector3.h"
#include "Functions.h"

float Clamp(float quality,float Min,float Max) 
{
    quality = fminf(fmaxf(quality, Min), Max);
	return quality;
}

float easeInOutSine(float t)
{
	return 0.5f * (1.0f + sinf(XM_PI * (t - 0.5f)));
}

float easeOutElastic(float t)
{
	const float c4 = (2 * 3.14159) / 3;
	return t == 0.0f ? 0.0f : t == 1.0f ? 1.0f : pow(2, -10 * t) * sinf((t * 10 - 0.75) * c4) + 1;
}

XMFLOAT3 RotateAround(XMFLOAT3 center, XMFLOAT3 rotate, float len)
{
	XMFLOAT3 position;
	position.x = center.x + sinf(rotate.y) * cosf(rotate.x) * len;
	position.y = center.y + sinf(rotate.x) * len;
	position.z = center.z - cosf(rotate.y) * cosf(rotate.x) * len;
	return position;
}

XMFLOAT3 NormalizeVector3D(XMFLOAT3 vec)
{
	XMFLOAT3 nv = XMFLOAT3(0, 0, 0);

	float len = sqrtf((vec.x * vec.x) + (vec.y * vec.y) + (vec.z * vec.z));

	if (len > 0.0f)
	{
		nv.x = vec.x / len;
		nv.y = vec.y / len;
		nv.z = vec.z / len;
	}
	return nv;
}

XMVECTOR NormalizeVector(XMVECTOR vec)
{
	XMVECTOR nv = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

	float len = sqrtf((vec.m128_f32[0] * vec.m128_f32[0]) + (vec.m128_f32[1] * vec.m128_f32[1]) + (vec.m128_f32[2] * vec.m128_f32[2]) + (vec.m128_f32[3] * vec.m128_f32[3]));

	if (len > 0.0f)
	{
		nv.m128_f32[0] = vec.m128_f32[0] / len;
		nv.m128_f32[1] = vec.m128_f32[1] / len;
		nv.m128_f32[2] = vec.m128_f32[2] / len;
		nv.m128_f32[3] = vec.m128_f32[3] / len;
	}
	return nv;
}

XMFLOAT3 Cross(XMFLOAT3 V1, XMFLOAT3 V2)
{
	XMFLOAT3 NormalVector;
	NormalVector.x = V1.y * V2.z - V1.z * V2.y;
	NormalVector.y = V1.z * V2.x - V1.x * V2.z;
	NormalVector.z = V1.x * V2.y - V1.y * V2.x;
	return NormalVector;
}

float Dot(XMFLOAT3 V1, XMFLOAT3 V2)
{
	float dot = V1.x * V2.x + V1.y * V2.y + V1.z * V2.z;
	return dot;
}

Vector3 RotateAround(Vector3 center, Vector3 rotate, float len)
{
	Vector3 position;
	position.x = center.x + sinf(rotate.y) * cosf(rotate.x) * len;
	position.y = center.y - sinf(rotate.x) * len;
	position.z = center.z - cosf(rotate.y) * cosf(rotate.x) * len;
	return position;
}

XMFLOAT2 AnimationUV(int no, int WidthDivision, int HeightDivision)
{
	XMFLOAT2 uvTopLeft;

	float w = 1.0f / WidthDivision;
	float h = 1.0f / HeightDivision;

	uvTopLeft.x = (no % WidthDivision) * w;
	uvTopLeft.y = (no / HeightDivision) * h;

	return uvTopLeft;
}

float AngleBetweenVector(Vector3 V1, Vector3 V2)
{
    V1.normalize();
    V2.normalize();

    float cos_theta = Vector3::Dot(V1, V2);

    cos_theta = Clamp(cos_theta, -1.0f, 1.0f);

    float sin_theta = Vector3::Cross(V1, V2).y;

    float angle = std::atan2(sin_theta, cos_theta);

    return XMConvertToDegrees(angle);
}

float Lerp(float start, float end, float t)
{
	return start + t * (end - start);
}

float NormalizeAngle(float angle)
{
	while (angle > 180.0f) angle -= 360.0f;
	while (angle < -180.0f) angle += 360.0f;
	return angle;
}

float NormalizeEulerAngle(float angle)
{
	const float PI = 3.14159265358979323846f;
	while (angle > PI) angle -= 2.0f * PI;
	while (angle < -PI) angle += 2.0f * PI;
	return angle;
}

float RandRange(float min, float max)
{
	return min + (max - min) * (float(rand()) / RAND_MAX);
}

