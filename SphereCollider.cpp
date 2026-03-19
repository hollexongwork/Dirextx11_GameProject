#include "main.h"
#include "Manager.h"
#include "Scene.h"
#include "SphereCollider.h"
#include "BoxCollider.h"
#include "MeshField.h"
#include "FieldCollider.h"

void SphereCollider::Start()
{
    m_Owner = GetOwner();
    m_Field = Manager::GetScene()->GetGameObject<MeshField>();
    if (!m_Field)
        return;
    m_FieldCollider = m_Field->GetComponent<FieldCollider>();
}

void SphereCollider::Update()
{
    if (m_BodyType == BodyType::Static)
        return;
    m_PreviousPosition = GetCurrentPosition();
}

bool SphereCollider::Intersects(const Collider* other, CollisionInfo& info) const
{
    // Sphere vs OBB
    if (auto* obb = dynamic_cast<const BoxCollider*>(other))
    {
        Vector3 newPos;
        Vector3 hitNormal;
        float outT;
        if(SweptSphereVsOBB
        (
            m_PreviousPosition, m_Radius,
            GetCurrentPosition(),
            obb->GetCurrentPosition(),
            obb->GetHalfSize(),
            obb->GetOwner()->GetAxisX(),
            obb->GetOwner()->GetAxisY(),
            obb->GetOwner()->GetAxisZ(),
            newPos, hitNormal, outT
        ))
        {
            info.Hit = true;
            info.T = outT;
            info.Normal = hitNormal;
            info.ResolvedPosition = newPos;
            return true;
        }
    }

    // Sphere vs Sphere
    else if (auto* sphere = dynamic_cast<const SphereCollider*>(other))
    {
        Vector3 newPos;
        Vector3 hitNormal;
        float outT;
        if (SweptSphereVsSphere(sphere->m_PreviousPosition, sphere->m_Radius, sphere->GetCurrentPosition(), 
            newPos, hitNormal, outT))
        {
            info.Hit = true;
            info.T = outT;
            info.Normal = hitNormal;
            info.ResolvedPosition = newPos;
            return true;
        }
    }

    // Sphere vs Field
    else if (auto* field = dynamic_cast<const FieldCollider*>(other))
    {
        HitResult hit;

        if (field->MeshFieldVsSweptSphere(
            m_PreviousPosition,
            GetCurrentPosition(),
            m_Radius,
            hit))
        {
            info.Hit = true;
            info.T = hit.T;
            info.Normal = hit.HitNormal;
            info.ResolvedPosition = hit.HitPoint;
            return true;
        }
    }

    return false;
}

void SphereCollider::ResolveCollision(const CollisionInfo& info)
{
    if (!info.Hit) return;

    if (IsStatic()) return;

    m_Owner->SetPosition(info.ResolvedPosition);
}

void SphereCollider::ApplyResolvedPosition(const Vector3& pos)
{
    if (m_BodyType == BodyType::Static) 
        return;

    m_Owner->SetPosition(pos - m_Offset);
}

bool SphereCollider::RaycastOBB(const Vector3& prevCenter, const Vector3& rayDir, float maxDist, const Vector3& currCenter, const Vector3& halfSize, const Vector3& axisX, const Vector3& axisY, const Vector3& axisZ, float& outT, Vector3& outNormal) const
{
    Vector3 p = currCenter - prevCenter;

    float tMin = 0.0f;
    float tMax = maxDist;

    Vector3 axes[3] = { axisX, axisY, axisZ };
    float ext[3] = { halfSize.x, halfSize.y, halfSize.z };

    int hitAxis = -1;
    float sign = 1.0f;

    for (int i = 0; i < 3; i++)
    {
        float e = Vector3::Dot(axes[i], p);
        float f = Vector3::Dot(axes[i], rayDir);

        if (fabs(f) > 1e-6f)
        {
            float t1 = (e - ext[i]) / f;
            float t2 = (e + ext[i]) / f;

            float enter = t1;
            float exit = t2;
            float axisSign = -1.0f;

            if (t1 > t2)
            {
                std::swap(enter, exit);
                axisSign = 1.0f;
            }

            if (enter > tMin)
            {
                tMin = enter;
                hitAxis = i;
                sign = axisSign;
            }

            if (exit < tMax)
                tMax = exit;

            if (tMin > tMax)
                return false;
        }
        else
        {
            if (-ext[i] > e || e > ext[i])
                return false;
        }
    }

    outT = tMin;
    outNormal = axes[hitAxis] * sign;

    return true;
}

bool SphereCollider::SweptSphereVsOBB(const Vector3& prevCenter, float radius, const Vector3& currCenter, const Vector3& boxCenter, const Vector3& boxHalf, const Vector3& axisX, const Vector3& axisY, const Vector3& axisZ, Vector3& outResolvedPos, Vector3& outNormal, float& outT) const
{
    Vector3 move = currCenter - prevCenter;
    float dist = move.length();

    if (dist < 1e-6f)
    {
        outResolvedPos = currCenter;
        return false;
    }

    Vector3 dir = move / dist;

    Vector3 inflatedHalf = boxHalf + Vector3(radius, radius, radius);

    float t;
    Vector3 normal;

    if (!RaycastOBB(prevCenter, dir, dist,
        boxCenter, inflatedHalf,
        axisX, axisY, axisZ,
        t, normal))
    {
        return false;
    }

    Vector3 hitPoint = prevCenter + dir * t;

    outResolvedPos = hitPoint - normal * radius;
    outNormal = normal;
    outT = 1- (t / dist);

    return true;
}

bool SphereCollider::SweptSphereVsSphere(const Vector3& otherPrevCenter, float otherRadius, const Vector3& otherCurrCenter, Vector3& outResolvedPos, Vector3& outNormal, float& outT) const
{
    Vector3 prev0 = m_PreviousPosition;
    Vector3 prev1 = otherPrevCenter;

    Vector3 curr0 = GetCurrentPosition();
    Vector3 curr1 = otherCurrCenter;

    Vector3 v0 = curr0 - prev0;
    Vector3 v1 = curr1 - prev1;

    Vector3 relPrev = prev0 - prev1;
    Vector3 relVel = v0 - v1;

    float radiusSum = m_Radius + otherRadius;

    float a = Vector3::Dot(relVel, relVel);
    float b = 2.0f * Vector3::Dot(relPrev, relVel);
    float c = Vector3::Dot(relPrev, relPrev) - radiusSum * radiusSum;

    float D = b * b - 4 * a * c;
    if (D < 0) return false;

    float sqrtD = sqrt(D);
    float t = (-b - sqrtD) / (2 * a);

    if (t < 0 || t > 1) return false;

    Vector3 hitPos = prev0 + v0 * t;
    Vector3 otherHitPos = prev1 + v1 * t;

    outNormal = hitPos - otherHitPos;
    outNormal.normalize();
    outResolvedPos = otherHitPos + outNormal * radiusSum;
    outT = t;

    return true;
}

