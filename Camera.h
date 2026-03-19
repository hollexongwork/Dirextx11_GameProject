#pragma once

#include "main.h"
#include "GameObject.h"

class Camera : public GameObject
{
private:
	static constexpr float SENSITIVITY = 0.15f;

	Vector3 FocusPoint{ 0.0f, 0.0f, 0.0f };
	Vector3 Up{ 0.0f, 1.0f, 0.0f };
	XMMATRIX Projection, View;
	float CameraDistance = 5.0f;

public:
	void Init()override;
	void Start()override;
	void Uninit()override;
	void Update()override;
	void Draw()override;

	Vector3 GetCameraFocusPoint() const { return FocusPoint; }
	Vector3 GetCameraDirection() const { return FocusPoint - Position; }
	XMMATRIX GetCmeraView() const { return View; };
	XMMATRIX GetCameraProjection() const { return Projection; }

	void SetCameraFocusPoint(Vector3 focusPoint) { FocusPoint = focusPoint; }
	void SetCameraDistance(float distance) 
	{
		CameraDistance = distance; 	
		CameraDistance = Clamp(CameraDistance, 1.0f, 50.0f);
	}
	bool isInView(Vector3 targetPosition, float radius);

};

