#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;
#include <vector>
#include <memory>
#include <string>
#include "Vector3.h"
#include "Functions.h"
#include "Component.h"

class Collider;

class GameObject
{
private:
	void UpdateColliderScales(Vector3 targetScale);

protected:
	bool IsDestory = false;

	Vector3 Pivot    { 0.0f, 0.0f, 0.0f };
	Vector3 Position { 0.0f, 0.0f, 0.0f };
	Vector3 Rotation { 0.0f, 0.0f, 0.0f };
	Vector3 Scale    { 1.0f, 1.0f, 1.0f };

	XMMATRIX RotationMatrix = XMMatrixRotationRollPitchYaw(XMConvertToRadians(Rotation.x), XMConvertToRadians(Rotation.y), XMConvertToRadians(Rotation.z));

	std::vector<std::unique_ptr<Component>> Components;

public:
	virtual ~GameObject() = default;

	virtual void Init()   { for (auto& c : Components) c->Init(); };
	virtual void Start()  { for (auto& c : Components) c->Start(); };
	virtual void Uninit() { for (auto& c : Components) c->Uninit(); };
	virtual void Update() { for (auto& c : Components) c->Update(); };
	virtual void DrawShadow() {};
	virtual void Draw()   { for (auto& c : Components) c->Draw(); };

	virtual void OnCollisionEnter(Collider* otherCollider) {};

	template<typename T, typename... Args>
	T* AddComponent(Args&&... args)
	{
		auto comp = std::make_unique<T>(std::forward<Args>(args)...);
		T* ptr = comp.get();
		comp->SetOwner(this);
		Components.push_back(std::move(comp));
		ptr->Init();
		return ptr;
	}

	template<typename T>
	T* GetComponent()
	{
		for (auto& comp : Components)
		{
			if (T* casted = dynamic_cast<T*>(comp.get()))
				return casted;
		}
		return nullptr;
	}

	template<typename T>
	void RemoveComponent()
	{
		Components.erase(
			std::remove_if(Components.begin(), Components.end(),
				[](const std::unique_ptr<Component>& c) {
					return dynamic_cast<T*>(c.get()) != nullptr;
				}),
			Components.end());
	}

	void SetDestory()
	{ 
		IsDestory = true; 
	}

	bool GetIsDestory() const
	{
		return IsDestory;
	}

	bool Destory()
	{
		if (IsDestory)
		{
			Uninit();
			delete this;
			return true;
		}
		else
		{
			return false;
		}
	}

	Vector3 GetPosition() const { return Position; }
	Vector3 GetRotation() const { return Rotation; }
	Vector3 GetScale() const { return Scale; }
	XMVECTOR GetPositionXM() const { return XMVectorSet(Position.x, Position.y, Position.z, 1.0f); }

	void SetPosition(const Vector3& position) { Position = position; }
	void SetRotation(const Vector3& rotation) { Rotation = rotation; }
	void SetScale(const Vector3& scale) { UpdateColliderScales(scale); Scale = scale; }
	void SetRotationMatrix(const XMMATRIX& rotationMatrix) { RotationMatrix = rotationMatrix; }
	void UpdateRotationMatrix() { RotationMatrix = XMMatrixRotationRollPitchYaw(XMConvertToRadians(Rotation.x), XMConvertToRadians(Rotation.y), XMConvertToRadians(Rotation.z)); }

	Vector3 GetAxisX() const
	{
		Vector3 X;
		XMStoreFloat3((XMFLOAT3*)&X, RotationMatrix.r[0]);

		return X;
	}

	Vector3 GetAxisY() const
	{
		Vector3 Y;
		XMStoreFloat3((XMFLOAT3*)&Y, RotationMatrix.r[1]);

		return Y;
	}

	Vector3 GetAxisZ() const
	{
		Vector3 Z;
		XMStoreFloat3((XMFLOAT3*)&Z, RotationMatrix.r[2]);

		return Z;
	}

	XMMATRIX GetRotationMatrix() const
	{ 
		return RotationMatrix; 
	}
};

