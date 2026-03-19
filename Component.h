#pragma once
class GameObject;
class Collider;

class Component
{
protected:
    GameObject* m_GameObject = nullptr;

public:
    Component() = default;
    virtual ~Component() = default;

    Component(const Component&) = delete;
    Component& operator=(const Component&) = delete;

    Component(Component&&) = default;
    Component& operator=(Component&&) = default;

    void SetOwner(GameObject* owner) { m_GameObject = owner; }

    const GameObject* GetOwner() const { return m_GameObject; }

    template<typename T>
    T* GetOwnerAs() const { return static_cast<T*>(m_GameObject); }

    virtual void Init() {}
    virtual void Start() {}
    virtual void Uninit() {}
    virtual void Update() {}
    virtual void Draw() {}
};