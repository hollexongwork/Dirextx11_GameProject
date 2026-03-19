#pragma once
#include "Collider.h"

class BoxCollider : public Collider
{
private:
    struct MTV
    {
        Vector3 axis = Vector3(0, 0, 0);
        float depth = FLT_MAX;
    };

    class GameObject* m_Owner;

    Vector3 m_HalfSize{ 1,1,1 }; 

    bool TestAxis(const Vector3& axis, float projA, float projB, float distance, MTV& mtv) const;

    bool ObbVsObb(const BoxCollider* other, MTV& mtv) const;

public:
    BoxCollider() = default;
    BoxCollider(const Vector3& halfSize) : m_HalfSize(halfSize) {}

    void Start()override;

    void SetHalfSize(const Vector3& half) { m_HalfSize = half;}
    Vector3 GetHalfSize() const { return m_HalfSize; }

    bool Intersects(const Collider* other, CollisionInfo& info) const override;
    bool ContainsPoint(const Vector3& point) const override
    {
        if (!m_GameObject) return false;
        Vector3 center = GetCurrentPosition();
        Vector3 min = center - m_HalfSize;
        Vector3 max = center + m_HalfSize;

        return point.x >= min.x && point.x <= max.x &&
               point.y >= min.y && point.y <= max.y &&
               point.z >= min.z && point.z <= max.z;
    }

    void ApplyResolvedPosition(const Vector3& pos);
};