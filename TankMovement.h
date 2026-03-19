#pragma once
#include "Component.h"
#include "InputProvider.h"
#include "Vector3.h"
#include "AnimationModel.h"

namespace
{
	constexpr float G = 9.80665f;
	constexpr float MASS = 59000.0f;												// kg
	constexpr float MAX_SPEED = 18.6f;												// m/s (約67km/h)
	constexpr float MAX_REVERSE_SPEED = -MAX_SPEED * 0.5f;							// 後退速度
	constexpr float MAX_FORCE = 180000.0f;											// 最大推進力 N
	constexpr float ROLL_RESISTANCE = 0.02f;										// 転がり抵抗係数
	constexpr float DRAG_COEF = 0.8f;												// 空気抵抗係数
	constexpr float FRONTAL_AREA = 10.0f;											// 正面投影面積 m^2
	constexpr float AIR_DENSITY = 1.225f;											// 空気密度 kg/m^3
	constexpr float TURN_COEF = 1.2f;												// 旋回係数
	constexpr float MAX_BRAKE_FORCE = 300000.0f;									// 最大制動力 N
	constexpr float STATIONARY_TURN_RATE = XMConvertToRadians(40.0f);				// 停車時旋回角速度 (rad/s)
	constexpr float STATIONARY_TURN_ACCEL = XMConvertToRadians(120.0f);				// 停車時旋回角加速度 (rad/s^2)

	constexpr float WHEEL_RADIUS = 0.311f;											//車輪半径
	constexpr float TRACK_UV_MOVE_COEF = 1.85f;										//移動UVスクロール係数
	constexpr float TRACK_UV_TURN_COEF = 1.3f;										//回転UVスクロール係数

	constexpr float TURRET_TURN_RATE = 45.0f;										// 砲塔最大角速度 (deg/sec)
	constexpr float GUN_PITCH_RATE = 20.0f;											// 砲身最大俯仰速度 (deg/sec)

	constexpr float SPRING_COEF = 35.0f;											// スプリング係数
	constexpr float SPRING_DAMPING = 5.0f;											// ダンパー係数
	constexpr float RECOIL_SPRING_COEF = 10.0f;										// リコイル スプリング係数

	constexpr float  RECOIL_DURATION = 0.04f;										//砲身後退時間
	constexpr float  RECOIL_BACK_DURATION = 0.08f;									//砲身戻り時間
	constexpr float  RECOIL_TOTAL_TIME = RECOIL_DURATION + RECOIL_BACK_DURATION;    //リコイル時間
	constexpr float  RECOIL_DISTANCE = 0.1f;										//砲身後退距離

	constexpr float DAMPING_COEF = 0.8f;											// 慣性ダンピング係数
	constexpr float ANGULAR_DAMPING_COEF = 3.0f;									// 角慣性ダンピング係数
}

class TankMovement : public Component
{
private:
	static constexpr float THRESHOLD = 0.01f;

	class Tank* m_Owner;
	class TankEvent* m_Event;

	const InputProvider* m_InputProvider;
	InputState m_Input;

	Collider* m_Collider;
	
	float m_DeltaTime;

	float m_Acceleration;
	float m_Velocity;
	float m_AngularVelocity;
	void Movement();

	Vector3 m_CogRot;      // rotation (deg)
	Vector3 m_CogRotVel;   // angular velocity (deg/sec)
	void Spring();

	float m_RecoilOffset;
	float m_GunRecoilTime;
	void Recoil();

	void InertialDampening();

public:
	void Start() override;
	void Uninit() override;
	void Update() override;

	float GetAcceleration() const { return m_Acceleration; }
	void SetAcceleration(const float& accel) { m_Acceleration = accel; }

	float GetVelocity() const { return m_Velocity; }
	void SetVelocity(const float& speed) { m_Velocity = speed; }

	float GetAngularVelocity() const { return m_AngularVelocity; }

	Vector3 GetCogRot() const { return m_CogRot; }
	Vector3 GetCogRotVel() const { return m_CogRotVel; }
	float GetRecoilOffset() const { return m_RecoilOffset; }

	void SetCogRotVel(const Vector3& vel) { m_CogRotVel = vel; }

	void ReetRecoilTime() { m_GunRecoilTime = 0.0f; }
};

