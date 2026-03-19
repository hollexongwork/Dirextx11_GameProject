#pragma once
#include "component.h"
#include "AnimationModel.h"

namespace 
{
	constexpr float PITCH_MIN_ANGLE = 133.0f;
	constexpr float PITCH_DEFAULTS_ANGLE = 145.0f;

	constexpr float GUN_PITCH_MIN = -10.0f;
	constexpr float GUN_PITCH_DEFAULTS = 0.0f;
	constexpr float GUN_PITCH_MAX = 20.0f;
}

class TankPosture : public Component
{
private:
	static constexpr float BOUNDARY = 145.0f;

	class Tank* m_Owner;
	class AnimationModel* m_Model;
	class TankEvent* m_Event;
	class TankMovement* m_Movement;
	class MeshField* m_Field;

	aiQuaternion m_RootOrientation;
	aiMatrix4x4 m_RootMatrix;

	aiNode* m_CogNode;
	aiNode* m_GunNode;

	static const std::vector<std::string> LEFT_TRACK_JOINT_NAMES;
	static const std::vector<std::string> RIGHT_TRACK_JOINT_NAMES;

	static const std::vector<std::string> LEFT_WHEEL_JOINT_NAMES;
	static const std::vector<std::string> RIGHT_WHEEL_JOINT_NAMES;

	std::vector<Vector3> m_LeftWheelTrackPositions;
	std::vector<Vector3> m_RightWheelTrackPositions;

	Vector3 m_TargetPointion;

	float m_TurretAngle;
	float m_GunAngle;

	float m_DeltaTime;

	void RootPosture();
	void CogPosture();
	void GunPosture();
	void TracksAndWheelsPosture();
	void TurretAndBarrelPosture();

public:
	void Init() override;
	void Start() override;
	void Uninit() override;
	void Update() override;

	void InitTurretAndBarrelPosture(float turretAngle, float gunAngle);

	void SetTargetPoint(const Vector3& targetPos) { m_TargetPointion = targetPos; }

	const std::vector<Vector3>& GetLeftWheelTrackPositions() const { return m_LeftWheelTrackPositions; }
	const std::vector<Vector3>& GetRightWheelTrackPositions() const { return m_RightWheelTrackPositions; }

	Vector3 GetBoneWorldPosition(const std::string& bone) const;
	aiMatrix4x4 GetBoneMatrix(const std::string& bone) const;
};

