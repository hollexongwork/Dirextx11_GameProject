#pragma once
float Clamp(float quality, float Min, float Max);

float easeInOutSine(float t);
float easeOutElastic(float t);

XMFLOAT3 RotateAround(XMFLOAT3 center, XMFLOAT3 rotate, float len);

XMFLOAT3  NormalizeVector3D(XMFLOAT3 vec);

XMVECTOR  NormalizeVector(XMVECTOR vec);

XMFLOAT3 Cross(XMFLOAT3 V1, XMFLOAT3 V2);

float Dot(XMFLOAT3 V1, XMFLOAT3 V2);

Vector3 RotateAround(Vector3 center, Vector3 rotate, float len);

XMFLOAT2 AnimationUV(int no, int WidthDivision, int HeightDivision);

float AngleBetweenVector(Vector3 V1, Vector3 V2);

float Lerp(float start, float end, float t);

float NormalizeAngle(float angle);

float NormalizeEulerAngle(float angle);

float RandRange(float min, float max);

