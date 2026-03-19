#include "main.h"
#include "Manager.h"
#include "Scene.h"
#include "Time.h"
#include "AnimationModel.h"
#include "TankPosture.h"
#include "Tank.h"
#include "TankEvent.h"
#include "TankMovement.h"
#include "MeshField.h"

const std::vector<std::string> TankPosture::LEFT_TRACK_JOINT_NAMES =
{
	"lf_wheel_02_track_jnt", "lf_wheel_03_track_jnt", "lf_wheel_04_track_jnt",
	"lf_wheel_05_track_jnt", "lf_wheel_06_track_jnt", "lf_wheel_07_track_jnt", "lf_wheel_08_track_jnt"
};
const std::vector<std::string> TankPosture::RIGHT_TRACK_JOINT_NAMES =
{
	"rt_wheel_02_track_jnt", "rt_wheel_03_track_jnt", "rt_wheel_04_track_jnt",
	"rt_wheel_05_track_jnt", "rt_wheel_06_track_jnt", "rt_wheel_07_track_jnt", "rt_wheel_08_track_jnt"
};

const std::vector<std::string> TankPosture::LEFT_WHEEL_JOINT_NAMES =
{
	"lf_wheel_01_jnt", "lf_wheel_02_jnt", "lf_wheel_03_jnt", "lf_wheel_04_jnt",
	"lf_wheel_05_jnt", "lf_wheel_06_jnt", "lf_wheel_07_jnt", "lf_wheel_08_jnt", "lf_wheel_09_jnt"
};
const std::vector<std::string> TankPosture::RIGHT_WHEEL_JOINT_NAMES =
{
	"rt_wheel_01_jnt", "rt_wheel_02_jnt", "rt_wheel_03_jnt", "rt_wheel_04_jnt",
	"rt_wheel_05_jnt", "rt_wheel_06_jnt", "rt_wheel_07_jnt", "rt_wheel_08_jnt", "rt_wheel_09_jnt"
};

void TankPosture::Init()
{
	
}

void TankPosture::Start()
{
	m_Owner = GetOwnerAs<Tank>();
	if (!m_Owner)
		return;

	m_Model = m_Owner->GetModel();

	m_Event = m_Owner->GetComponent<TankEvent>();

	m_Movement = m_Owner->GetComponent<TankMovement>();

	m_Field = Manager::GetScene()->GetGameObject<MeshField>();

	m_LeftWheelTrackPositions.resize(LEFT_TRACK_JOINT_NAMES.size());
	m_RightWheelTrackPositions.resize(RIGHT_TRACK_JOINT_NAMES.size());

	m_CogNode = m_Model->GetAiScene()->mRootNode->FindNode("cog_jnt");

	m_GunNode = m_Model->GetAiScene()->mRootNode->FindNode("gun_1_jnt");

	RootPosture();
}

void TankPosture::Uninit()
{
	m_LeftWheelTrackPositions.clear();
	m_RightWheelTrackPositions.clear();

	if (m_Field) { m_Field = nullptr; }
	if (m_Movement) { m_Movement = nullptr; }
	if (m_Event) { m_Event = nullptr; }
	if (m_Model) { m_Model = nullptr; }
	if (m_Owner) { m_Owner = nullptr; }
}

void TankPosture::Update()
{
	m_DeltaTime = Time::GetDeltaTime();

	RootPosture();
	CogPosture();
	GunPosture();
	TracksAndWheelsPosture();
	TurretAndBarrelPosture();
}

void TankPosture::RootPosture()
{
	if (!m_Field)
		return;

	Vector3 position = m_Owner->GetPosition();
	Vector3 rotation = m_Owner->GetRotation();
	Vector3 scale = m_Owner->GetScale();

	FieldHit hit = m_Field->GetFieldHeightAndNormal(position);

	Vector3 forward = m_Owner->GetAxisZ();
	Vector3 up = hit.Normal;
	up.normalize();
	Vector3 right = Vector3::Cross(up, forward);
	right.normalize();
	forward = Vector3::Cross(right, up);
	forward.normalize();

	// --- Rotation Update --- 
	aiMatrix3x3 basis
	(
		right.x, up.x, forward.x,
		right.y, up.y, forward.y,
		right.z, up.z, forward.z
	);

	aiQuaternion qBasis(basis);

	aiVector3D rotAxis(up.x, up.y, up.z);

	aiQuaternion qRot;

	if (!m_Event->IsYawInitialized())
	{
		qRot = aiQuaternion(aiVector3D(0.0f, 1.0f, 0.0f), XMConvertToRadians(rotation.y));
		m_Event->SetYawInitialized(true);
	}
	else
	{
		qRot = aiQuaternion(rotAxis, m_Movement->GetAngularVelocity() * m_DeltaTime);
	}

	m_RootOrientation = qBasis * qRot;
	m_RootOrientation.Normalize();

	// --- RotationMatrix Update --- 
	XMVECTOR quat = XMVectorSet(m_RootOrientation.x, m_RootOrientation.y, m_RootOrientation.z, m_RootOrientation.w);
	m_Owner->SetRotationMatrix(XMMatrixRotationQuaternion(quat));

	// --- Position Update ---
	position += forward * m_Movement->GetVelocity() * m_DeltaTime;
	position.x = Clamp(position.x, -BOUNDARY, BOUNDARY);
	position.z = Clamp(position.z, -BOUNDARY, BOUNDARY);
	position.y = hit.Height;
	m_Owner->SetPosition(position);

	// --- Matrix Update --- 
	m_RootMatrix = aiMatrix4x4
	(
		aiVector3D(scale.x, scale.y, scale.z),
		m_RootOrientation,
		aiVector3D(position.x, position.y, position.z)
	);

	// --- Posture Update ---
	m_Model->Update(m_RootMatrix);
}

void TankPosture::CogPosture()
{
	aiQuaternion qRoll(aiVector3D(1, 0, 0), XMConvertToRadians(m_Movement->GetCogRot().x));
	aiQuaternion qPitch(aiVector3D(0, 0, 1), XMConvertToRadians(m_Movement->GetCogRot().z));

	// --- Combined Pitch & Roll --- 
	aiQuaternion qCombined = qPitch * qRoll;
	aiMatrix4x4 combinedRotMat(qCombined.GetMatrix());

	// --- Get Bind Pose --- 
	aiMatrix4x4 baseTransform = m_CogNode ? m_CogNode->mTransformation : aiMatrix4x4();

	// Final Local Matrix = Bind Pose x Combined Rotation Mattrix
	aiMatrix4x4 finalLocalMatrix = baseTransform * combinedRotMat;

	m_Model->SetBoneMatrix("cog_jnt", finalLocalMatrix);
}

void TankPosture::GunPosture()
{
	if (!m_Event)
		return;

	if (m_Event->IsGunRecoiling())
	{
		if (m_GunNode)
		{
			aiMatrix4x4 baseLocal = m_GunNode->mTransformation;
			aiMatrix4x4 finalLocal = baseLocal;
			finalLocal.c4 += m_Movement->GetRecoilOffset();
			m_Model->SetBoneMatrix("gun_1_jnt", finalLocal);
		}
	}
}

void TankPosture::TracksAndWheelsPosture()
{
	float speed = m_Movement->GetVelocity();
	float omega = m_Movement->GetAngularVelocity();

	// --- Moving ---
	if (std::fabsf(speed) > 0.01f)
	{
		float uvSpeed = speed * TRACK_UV_MOVE_COEF;

		float leftTrackUV = m_Owner->GetLeftTrackUV();
		float rightTrackUV = m_Owner->GetRightTrackUV();

		m_Owner->SetLeftTrackUV(leftTrackUV - uvSpeed * m_DeltaTime);
		m_Owner->SetRightTrackUV(rightTrackUV - uvSpeed * m_DeltaTime);


		float wheelTurnRate = speed / WHEEL_RADIUS;
		for (const auto& wheel : LEFT_WHEEL_JOINT_NAMES)
		{
			m_Model->RotateBoneLocal(wheel, wheelTurnRate, aiVector3D(-1, 0, 0));
		}
		for (const auto& wheel : RIGHT_WHEEL_JOINT_NAMES)
		{
			m_Model->RotateBoneLocal(wheel, wheelTurnRate, aiVector3D(-1, 0, 0));
		}
	}
	// --- Turning ---
	else if (std::fabsf(omega) > 0.01f)
	{
		float uvSpeed = omega * TRACK_UV_TURN_COEF;

		float leftTrackUV = m_Owner->GetLeftTrackUV();
		float rightTrackUV = m_Owner->GetRightTrackUV();

		m_Owner->SetLeftTrackUV(leftTrackUV - uvSpeed * m_DeltaTime);
		m_Owner->SetRightTrackUV(rightTrackUV + uvSpeed * m_DeltaTime);

		float wheelTurnRate = omega / WHEEL_RADIUS;
		for (const auto& wheel : LEFT_WHEEL_JOINT_NAMES)
		{
			m_Model->RotateBoneLocal(wheel, wheelTurnRate, aiVector3D(-1, 0, 0));
		}
		for (const auto& wheel : RIGHT_WHEEL_JOINT_NAMES)
		{
			m_Model->RotateBoneLocal(wheel, wheelTurnRate, aiVector3D(1, 0, 0));
		}
	}


	for (int i = 0; i < LEFT_TRACK_JOINT_NAMES.size(); ++i)
	{
		m_LeftWheelTrackPositions[i] = GetBoneWorldPosition(LEFT_TRACK_JOINT_NAMES[i]);
	}
	
	for (int i = 0; i < RIGHT_TRACK_JOINT_NAMES.size(); ++i)
	{
		m_RightWheelTrackPositions[i] = GetBoneWorldPosition(RIGHT_TRACK_JOINT_NAMES[i]);
	}
}

void TankPosture::TurretAndBarrelPosture()
{
	if (!m_Event->HasTarget() || m_Event->IsDestory())
		return;

	Vector3 gunPos = GetBoneWorldPosition("gun_jnt");

	Vector3 toTarget = m_TargetPointion - gunPos;
	toTarget.normalize();

	// --- Transform to local space by inverse root rotation ---
	aiQuaternion invRoot = m_RootOrientation;
	invRoot.Conjugate();

	aiVector3D v(toTarget.x, toTarget.y, toTarget.z);
	aiVector3D vLocal = invRoot.Rotate(v);

	Vector3 localDir(vLocal.x, vLocal.y, vLocal.z);

	// --- Local yaw / pitch ---
	float targetYaw = atan2f(localDir.x, localDir.z); // Y
	float targetPitch = -atan2f(localDir.y, sqrtf(localDir.x * localDir.x + localDir.z * localDir.z)); // X

	float targetTurretAngle = XMConvertToDegrees(targetYaw);

	// --- Yaw ---
	float maxDelta = TURRET_TURN_RATE * m_DeltaTime;
	float delta = targetTurretAngle - m_TurretAngle;

	delta = NormalizeAngle(delta);
	delta = Clamp(delta, -maxDelta, maxDelta);

	m_TurretAngle += delta;
	m_TurretAngle = NormalizeAngle(m_TurretAngle);

	// --- Set Turret Angle --- 
	m_Model->SetBoneAngle("turret_jnt", m_TurretAngle, aiVector3D(0, 1, 0));

	// --- Pitch ---
	float absTurret = std::fabsf(m_TurretAngle);
	float t = Clamp((absTurret - PITCH_MIN_ANGLE) / (PITCH_DEFAULTS_ANGLE - PITCH_MIN_ANGLE), 0.0f, 1.0f);
	float gunMin = Lerp(GUN_PITCH_MIN, GUN_PITCH_DEFAULTS, t);

	float targetGunAngle = Clamp(XMConvertToDegrees(-targetPitch), gunMin, GUN_PITCH_MAX);
	float maxGunDelta = GUN_PITCH_RATE * m_DeltaTime;

	float gunDelta = Clamp(targetGunAngle - m_GunAngle, -maxGunDelta, maxGunDelta);

	m_GunAngle += gunDelta;

	// --- Set Gun Angle --- 
	m_Model->SetBoneAngle("gun_jnt", m_GunAngle, aiVector3D(1, 0, 0));
}

Vector3 TankPosture::GetBoneWorldPosition(const std::string& bone) const
{
	return m_Model->GetBoneWorldPosition(bone, m_RootMatrix);
}

aiMatrix4x4 TankPosture::GetBoneMatrix(const std::string& bone) const
{ 
	BONE* b = m_Model->GetBone(bone);
	return b->Matrix;
}

void TankPosture::InitTurretAndBarrelPosture(float turretAngle,float gunAngle)
{
	m_TurretAngle = turretAngle;
	m_Model->SetBoneAngle("turret_jnt", m_TurretAngle, aiVector3D(0, 1, 0));

	m_GunAngle = gunAngle;
	m_Model->SetBoneAngle("gun_jnt", m_GunAngle, aiVector3D(1, 0, 0));
}