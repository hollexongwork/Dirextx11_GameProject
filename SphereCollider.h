#pragma once
#include "Collider.h"

class SphereCollider : public Collider
{
private:
    class GameObject* m_Owner;
    class MeshField* m_Field;
    class FieldCollider* m_FieldCollider;

    float m_Radius = 1.0f;

    Vector3 m_VelocityThisFrame;

    bool ResolveTerrainCollision(const Vector3& targetPos, Vector3& outResolvedPos);

    bool ResolveInitialPenetration(Vector3& inoutPos);

    bool RaycastOBB(const Vector3& prevCenter, const Vector3& rayDir,   float maxDist,
                    const Vector3& currCenter,    const Vector3& halfSize, 
                    const Vector3& axisX,     const Vector3& axisY,    const Vector3& axisZ,
                    float& outT, Vector3& outNormal)const;

    bool SweptSphereVsOBB(const Vector3& prevCenter, float radius,           const Vector3& currCenter,
                          const Vector3& boxCenter,  const Vector3& boxHalf,
                          const Vector3& axisX,      const Vector3& axisY,   const Vector3& axisZ,
                          Vector3& outResolvedPos, Vector3& outNormal, float& outT)const;

    bool SweptSphereVsSphere(const Vector3& otherPrevCenter,float otherRadius, const Vector3& otherCurrCenter, Vector3& outResolvedPos, Vector3& outNormal, float& outT)const;

public:
    SphereCollider() = default;
    explicit SphereCollider(float radius) : m_Radius(radius) {}

    void Start()override;
    void Update()override;

    void SetRadius(float r) { m_Radius = r; }
    float GetRadius() const { return m_Radius; }

    bool Intersects(const Collider* other, CollisionInfo& info) const override;
    bool ContainsPoint(const Vector3& point) const override
    {
        if (!m_GameObject) return false;
        Vector3 center = GetCurrentPosition();
        return (point - center).lengthSq() <= (m_Radius * m_Radius);
    }

    bool CheckFieldCollision(CollisionInfo& info) const;

    void ResolveCollision(const CollisionInfo& info);

    void ApplyResolvedPosition(const Vector3& pos);
};

