#pragma once

#include <unordered_map>

#include "assimp/cimport.h"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "assimp/matrix4x4.h"
#pragma comment (lib, "assimp-vc143-mt.lib")

#include "component.h"


//変形後頂点構造体
struct DEFORM_VERTEX
{
	aiVector3D      Position;
	aiVector3D      Normal;
	int				BoneNum;
	std::string		BoneName[4];//本来はボーンインデックスで管理するべき
	float			BoneWeight[4];
};

//ボーン構造体
struct BONE
{
	aiMatrix4x4 Matrix;
	aiMatrix4x4 AnimationMatrix;
	aiMatrix4x4 OffsetMatrix;
};

class AnimationModel : public Component
{
private:
	const aiScene* m_AiScene = nullptr;
	std::unordered_map<std::string, const aiScene*> m_Animation;

	ID3D11Buffer** m_VertexBuffer;
	ID3D11Buffer** m_IndexBuffer;

	std::unordered_map<std::string, ID3D11ShaderResourceView*> m_Texture;

	std::vector<DEFORM_VERTEX>* m_DeformVertex;//変形後頂点データ
	std::unordered_map<std::string, BONE> m_Bone;//ボーンデータ（名前で参照）

	void CreateBone(aiNode* Node);
	void UpdateBoneMatrix(const aiNode* node, const aiMatrix4x4& parentTransform);

public:
	using Component::Component;

	void Load(const char* FileName);
	void LoadAnimation(const char* FileName, const char* Name);
	void Uninit() override;
	void Update(const char* AnimationName1, int Frame1,
		const char* AnimationName2, int Frame2, float BlendRate);
	void Update(aiMatrix4x4 Matrix);
	void Draw() override;
	void DrawMesh(unsigned int MeshIndex);

	BONE* GetBone(std::string name);

	void SetBoneMatrix(const std::string& boneName, const aiMatrix4x4& matrix);
	void SetBoneAngle(const std::string& boneName, float angleDeg, const aiVector3D& axis);
	void RotateBoneLocal(const std::string& boneName, float angleSpeedDeg, const aiVector3D& axis);
	void Pose();

	Vector3 GetBoneWorldPosition(const std::string& boneName, const aiMatrix4x4& rootMatrix);


	aiScene* GetAiScene() { return (aiScene*)m_AiScene; }

	static XMMATRIX MultiplyAiMatrixWithXMMatrix(aiMatrix4x4 aiMat, XMMATRIX xmMat);

	void SetBoneWorldMatrix(const std::string& boneName, const aiMatrix4x4& worldRotationMatrix);

	aiMatrix4x4 XMMatrixToAiMatrix(XMMATRIX& xm);

};