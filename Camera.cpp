#include "main.h"
#include "Input.h"
#include "Mouse.h"
#include "Renderer.h"
#include "Manager.h"
#include "Scene.h"
#include "Time.h"
#include "Camera.h"
#include "Player.h"
#include "Functions.h"

void Camera::Init()
{
	Rotation = { XMConvertToRadians(-20.0f),0.0f,0.0f };
	Position = RotateAround(FocusPoint, Rotation, 5.5f);
}

void Camera::Start()
{
	Vector3 desiredPos = RotateAround(FocusPoint, Rotation, CameraDistance);
	Position = desiredPos;
	Renderer::SetCameraPosition(XMFLOAT3(Position.x, Position.y, Position.z));
}

void Camera::Uninit()
{

}

void Camera::Update()
{
	float dt = Time::GetDeltaTime();

	Mouse_State mouseState;
	Mouse_GetState(&mouseState);

	if (mouseState.positionMode == MOUSE_POSITION_MODE_RELATIVE)
	{

		Rotation.y -= XMConvertToRadians(mouseState.x * SENSITIVITY);
		Rotation.x -= XMConvertToRadians(mouseState.y * SENSITIVITY);
		Rotation.x = Clamp(Rotation.x, XMConvertToRadians(-89.0f), XMConvertToRadians(89.0f));
	}

	Player* player= Manager::GetScene()->GetGameObject<Player>();
	if (player)
	{
		FocusPoint = player->GetPosition();
		FocusPoint.y += 1.5f;
	}

	Vector3 desiredPos = RotateAround(FocusPoint, Rotation, CameraDistance);
	Position = Vector3::LerpVector(Position, desiredPos, dt * 35.0f);

	Renderer::SetCameraPosition(XMFLOAT3(Position.x, Position.y, Position.z));
}

void Camera::Draw()
{
	//Projection Matrix
	Projection = XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0f), (float)SCREEN_WIDTH / SCREEN_HEIGHT, 0.1f, 1500.0f);

	Renderer::SetProjectionMatrix(Projection);

	//Camera Matrix setting (View Matrix)
	View = XMMatrixLookAtLH(XMLoadFloat3((XMFLOAT3*) & Position), XMLoadFloat3((XMFLOAT3*)&FocusPoint), XMLoadFloat3((XMFLOAT3*)&Up));
	Renderer::SetViewMatrix(View);

	XMMATRIX ViewProj = XMMatrixMultiply(View, Projection);
	XMMATRIX InvViewProj = XMMatrixInverse(nullptr, ViewProj);
	Renderer::SetInvViewProjMatrix(InvViewProj);
}

bool Camera::isInView(Vector3 targetPosition, float radius)
{
	XMMATRIX vp;
	vp = View * Projection;

	XMMATRIX invVp = XMMatrixInverse(NULL, vp);

	XMFLOAT3 vPos[4];
	vPos[0] = XMFLOAT3(-1.0f, 1.0f, 1.0f); // Left Top
	vPos[1] = XMFLOAT3(1.0f, 1.0f, 1.0f);  // Right Top
	vPos[2] = XMFLOAT3(-1.0f, -1.0f, 1.0f);// Left Bottom
	vPos[3] = XMFLOAT3(1.0f, -1.0f, 1.0f); // Right Bottom

	XMVECTOR vPosVec[4];
	for (int i = 0; i < 4; i++)
	{
		vPosVec[i] = XMLoadFloat3(&vPos[i]);
	}

	XMVECTOR wPosVec[4];
	for (int i = 0; i < 4; i++)
	{
		wPosVec[i] = XMVector3TransformCoord(vPosVec[i], invVp);
	}

	XMFLOAT3 wPos[4];
	for (int i = 0; i < 4; i++)
	{
		XMStoreFloat3(&wPos[i], wPosVec[i]); //farPlene Vertex World Position
	}

	Vector3 v;
	v = targetPosition - Position;

	Vector3 wp[4];
	for (int i = 0; i < 4; i++)
	{
		wp[i] = Vector3(wPos[i].x, wPos[i].y, wPos[i].z);
	}

	//Left Plane
	{
		Vector3 v1, v2;
		v1 = wp[0] - Position;
		v2 = wp[2] - Position;

		Vector3 normal = Vector3::Cross(v1, v2);
		normal.normalize();

		float d = Vector3::Dot(normal, v);//Plane distance

		if (d < -radius)
		{
			return false;
		}
	}

	//Right Plane
	{
		Vector3 v1, v2;
		v1 = wp[3] - Position;
		v2 = wp[1] - Position;

		Vector3 normal = Vector3::Cross(v1, v2);
		normal.normalize();

		float d = Vector3::Dot(normal, v);

		if (d < -radius)
		{
			return false;
		}
	}

	//Top Plane
	{
		Vector3 v1, v2;
		v1 = wp[1] - Position;
		v2 = wp[0] - Position;

		Vector3 normal = Vector3::Cross(v1, v2);
		normal.normalize();

		float d = Vector3::Dot(normal, v);

		if (d < -radius)
		{
			return false;
		}
	}

	//Bottom Plane
	{
		Vector3 v1, v2;
		v1 = wp[2] - Position;
		v2 = wp[3] - Position;

		Vector3 normal = Vector3::Cross(v1, v2);
		normal.normalize();

		float d = Vector3::Dot(normal, v);

		if (d < -radius)
		{
			return false;
		}
	}

	//float length = (targetPosition - Position).lengthSq();
	//if (length > (300.0f * 300.0f))
	//{
	//	return false;
	//}

	return true;
}


