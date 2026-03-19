#pragma once
#include "Vector3.h"
#include "component.h"
#include "GameObject.h"

class Collision;

enum class CollisionTag
{
    None,
    Player,
    Enemy,
    Bullet,
    Building,
    Field,
};

enum class BodyType
{
    Static,
    Dynamic
};

struct CollisionInfo
{
    Collider* Self = nullptr;
    Collider* Other = nullptr;
    bool Hit = false;
    Vector3 ResolvedPosition;
    Vector3 Normal;
    float T = 1.0f;
};

class Collider : public Component
{
protected:
    Vector3 m_Offset;
    bool m_IsTrigger = false;
    bool m_IsActive = true;

    CollisionTag m_Tag = CollisionTag::None;
    BodyType m_BodyType = BodyType::Dynamic;

    Vector3 m_PreviousPosition;

public:
    Collider();
    virtual ~Collider();

    GameObject* GetOwner() const { return m_GameObject; }

    void SetOffset(const Vector3& offset) { m_Offset = offset; }

    Vector3 GetCurrentPosition() const;
	Vector3 GetPreviousPosition() const { return m_PreviousPosition; }
    void SavePreviousTransform() { m_PreviousPosition = GetCurrentPosition();}

    void SetTrigger(bool trigger) { m_IsTrigger = trigger; }
    bool IsTrigger() const { return m_IsTrigger; }

    void SetActive(bool active) { m_IsActive = active; }
    bool IsActive()const { return m_IsActive; }

    void SetTag(CollisionTag tag) { m_Tag = tag; }
    CollisionTag GetTag() const { return m_Tag; }

    void SetBodyType(BodyType type) { m_BodyType = type; }
    BodyType GetBodyType() const { return m_BodyType; }

    bool IsStatic() const { return m_BodyType == BodyType::Static; }
    bool IsDynamic() const { return m_BodyType == BodyType::Dynamic; }

    virtual bool Intersects(const Collider* other, CollisionInfo& info) const = 0;
    virtual bool ContainsPoint(const Vector3& point) const = 0;
    virtual void ApplyResolvedPosition(const Vector3& pos) {};

    virtual void OnCollisionEnter(Collider* other)
    {
        if (m_GameObject) m_GameObject->OnCollisionEnter(other);
    }
};
