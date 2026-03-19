#pragma once
#include "Collider.h"

struct HitResult
{
    bool Hit = false;
    Vector3 HitPoint;
    Vector3 HitNormal;
    float T = 1.0f;
};

class MeshField;

class FieldCollider : public Collider
{
private:
	MeshField* m_Field;

public:
    FieldCollider() = default;

    void Start() override;

    MeshField* GetField() const { return m_Field; }

    bool Intersects(const Collider* other, CollisionInfo& info) const override;

    virtual bool ContainsPoint(const Vector3& point) const override { return false; }

    bool MeshFieldVsSweptSphere(const Vector3& start, const Vector3& end, const float& radius, HitResult& outHit) const;
};

