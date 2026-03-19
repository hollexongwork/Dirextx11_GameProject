#pragma once
#include "main.h"
#include "Vector3.h"

struct Ray
{
    Vector3 origin;
    Vector3 end;
    Vector3 direction;
    float length;
};

struct RayHit
{
    bool hit = false;
    Vector3 point;
    Vector3 normal;
    float distance = 0.0f;
    class GameObject* object = nullptr; 
};

Ray CameraRay(XMFLOAT2 screenPos, float maxDistance);

bool IntersectTriangle(const Ray& ray, const Vector3& v0, const Vector3& v1, const Vector3& v2, float& outDistance, Vector3& hitPoint, Vector3& outNormal);

bool RayIntersectAABB(
    const Vector3& rayOrigin,
    const Vector3& rayDir,
    const Vector3& minAABB,
    const Vector3& maxAABB,
    float& tmin,
    float& tmax);

bool RayIntersectOBB(
    const Ray& ray,
    class Enemy* enemy,
    float& outDist,
    Vector3& outHitPos,
    Vector3& outNormal);