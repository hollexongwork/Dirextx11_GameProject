#include "RayCast.h"
#include "Camera.h"
#include "Manager.h"
#include "Scene.h"
#include "MeshField.h"
#include "Mouse.h"
#include "Enemy.h"

static Camera* GetCamera()
{
    return Manager::GetScene()->GetGameObject<Camera>();
}

Ray CameraRay(XMFLOAT2 screenPos, float maxDistance)
{
    Camera* camera = GetCamera();
    const XMMATRIX view = camera->GetCmeraView();
    const XMMATRIX proj = camera->GetCameraProjection();
    XMMATRIX viewProj = XMMatrixMultiply(view, proj);
    XMMATRIX invViewProj = XMMatrixInverse(nullptr, viewProj);

    float ndcX = (2.0f * screenPos.x) / SCREEN_WIDTH - 1.0f;
    float ndcY = 1.0f - (2.0f * screenPos.y) / SCREEN_HEIGHT;

    XMVECTOR ndcNear = XMVectorSet(ndcX, ndcY, 0.0f, 1.0f);
    XMVECTOR ndcFar = XMVectorSet(ndcX, ndcY, 1.0f, 1.0f);

    XMVECTOR worldNear = XMVector3TransformCoord(ndcNear, invViewProj);
    XMVECTOR worldFar = XMVector3TransformCoord(ndcFar, invViewProj);

    Vector3 rayOrigin = Vector3(XMVectorGetX(worldNear), XMVectorGetY(worldNear), XMVectorGetZ(worldNear));
    Vector3 rayEnd = Vector3(XMVectorGetX(worldFar), XMVectorGetY(worldFar), XMVectorGetZ(worldFar));

    Vector3 rayDir = (rayEnd - rayOrigin);
    float rayLength = rayDir.length();

    if (rayLength > 0.001f)
    {
        rayDir.normalize();
    }
    
    if (rayLength > maxDistance)
    {
        rayEnd = rayOrigin + rayDir * maxDistance;
        rayLength = maxDistance;
    }

	Ray ray;
	ray.origin = rayOrigin;
	ray.end = rayEnd;
	ray.direction = rayDir;
	ray.length = rayLength;

	return ray;
}

bool IntersectTriangle(const Ray& Ray,
	const Vector3& v0,
	const Vector3& v1,
	const Vector3& v2,
	float& outDistance,
	Vector3& hitPoint,
	Vector3& outNormal)
{
	const float EPSILON = 1e-6f;
	const Vector3 edge1 = v1 - v0;
	const Vector3 edge2 = v2 - v0;
	const Vector3 pvec = Vector3::Cross(Ray.direction, edge2);
	const float det = Vector3::Dot(edge1, pvec);

	if (fabs(det) < EPSILON)
	{
		return false;
	}

	const Vector3 tvec = Ray.origin - v0;
	const float   u = Vector3::Dot(tvec, pvec) / det;
	if (u < 0.0f || u > 1.0f)
	{
		return false;
	}

	const Vector3 qvec = Vector3::Cross(tvec, edge1);
	const float   v = Vector3::Dot(Ray.direction, qvec) / det;
	if (v < 0.0f || u + v > 1.0f)
	{
		return false;
	}

	outDistance = Vector3::Dot(edge2, qvec) / det;
	if (outDistance < EPSILON)
	{
		return false;
	}

	hitPoint = Ray.origin + Ray.direction * outDistance;

	outNormal = Vector3::Cross(edge1, edge2);
	outNormal.normalize();

	return true;
}

bool RayIntersectAABB(
    const Vector3& rayOrigin,
    const Vector3& rayDir,
    const Vector3& minAABB,
    const Vector3& maxAABB,
    float& tmin,
    float& tmax)
{
    tmin = 0.0f;
    tmax = FLT_MAX;

    // X軸
    if (fabs(rayDir.x) < 1e-6f)
    {
        if (rayOrigin.x < minAABB.x || rayOrigin.x > maxAABB.x)
            return false;
    }
    else
    {
        float ood = 1.0f / rayDir.x;
        float t1 = (minAABB.x - rayOrigin.x) * ood;
        float t2 = (maxAABB.x - rayOrigin.x) * ood;
        if (t1 > t2) std::swap(t1, t2);

        tmin = fmax(tmin, t1);
        tmax = fmin(tmax, t2);
        if (tmin > tmax) return false;
    }

    // Y軸
    if (fabs(rayDir.y) < 1e-6f)
    {
        if (rayOrigin.y < minAABB.y || rayOrigin.y > maxAABB.y)
            return false;
    }
    else
    {
        float ood = 1.0f / rayDir.y;
        float t1 = (minAABB.y - rayOrigin.y) * ood;
        float t2 = (maxAABB.y - rayOrigin.y) * ood;
        if (t1 > t2) std::swap(t1, t2);

        tmin = fmax(tmin, t1);
        tmax = fmin(tmax, t2);
        if (tmin > tmax) return false;
    }

    // Z軸
    if (fabs(rayDir.z) < 1e-6f)
    {
        if (rayOrigin.z < minAABB.z || rayOrigin.z > maxAABB.z)
            return false;
    }
    else
    {
        float ood = 1.0f / rayDir.z;
        float t1 = (minAABB.z - rayOrigin.z) * ood;
        float t2 = (maxAABB.z - rayOrigin.z) * ood;
        if (t1 > t2) std::swap(t1, t2);

        tmin = fmax(tmin, t1);
        tmax = fmin(tmax, t2);
        if (tmin > tmax) return false;
    }

    return true;
}

bool RayIntersectOBB(
    const Ray& ray,
    class Enemy* enemy,
    float& outDist,
    Vector3& outHitPos,
    Vector3& outNormal)
{
    // OBB center
    Vector3 boxCenter = enemy->GetPosition();

    // OBB axes (normalized)
    Vector3 axisX = enemy->GetAxisX();
    Vector3 axisY = enemy->GetAxisY();
    Vector3 axisZ = enemy->GetAxisZ();

    // OBB half-size
    Vector3 half = enemy->GetScale(); // scale が半径扱いで OK

    // === 世界座標の Ray を OBB ローカル空間に変換 ===

    // Ray から OBB 中心へのベクトル
    Vector3 d = ray.origin - boxCenter;

    // ローカル空間に座標変換（回転だけ）
    Vector3 localOrigin(
        Vector3::Dot(d, axisX),
        Vector3::Dot(d, axisY),
        Vector3::Dot(d, axisZ)
    );

    Vector3 localDir(
        Vector3::Dot(ray.direction, axisX),
        Vector3::Dot(ray.direction, axisY),
        Vector3::Dot(ray.direction, axisZ)
    );

    // === Local AABB (±half) に対し Ray 判定 ===
    Vector3 minAABB = -half;
    Vector3 maxAABB = half;

    float tmin, tmax;
    if (!RayIntersectAABB(localOrigin, localDir, minAABB, maxAABB, tmin, tmax))
        return false;

    // 衝突距離
    outDist = tmin;
    if (outDist < 0.0f)
        outDist = tmax; // レイが内部スタートの場合

    if (outDist < 0.0f)
        return false;

    // === ヒット座標をワールドへ変換 ===
    Vector3 hitLocal = localOrigin + localDir * outDist;

    outHitPos =
        boxCenter +
        axisX * hitLocal.x +
        axisY * hitLocal.y +
        axisZ * hitLocal.z;

    // === 法線計算（AABB 側から求める） ===
    Vector3 normal(0, 0, 0);
    const float bias = 1e-4f;

    if (fabs(hitLocal.x - minAABB.x) < bias) normal = Vector3(-1, 0, 0);
    else if (fabs(hitLocal.x - maxAABB.x) < bias) normal = Vector3(1, 0, 0);
    else if (fabs(hitLocal.y - minAABB.y) < bias) normal = Vector3(0, -1, 0);
    else if (fabs(hitLocal.y - maxAABB.y) < bias) normal = Vector3(0, 1, 0);
    else if (fabs(hitLocal.z - minAABB.z) < bias) normal = Vector3(0, 0, -1);
    else if (fabs(hitLocal.z - maxAABB.z) < bias) normal = Vector3(0, 0, 1);

    // ローカル法線 → ワールド法線へ
    outNormal =
        axisX * normal.x +
        axisY * normal.y +
        axisZ * normal.z;
    outNormal.normalize();

    return true;
}