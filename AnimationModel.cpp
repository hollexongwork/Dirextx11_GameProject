#include "main.h"
#include "Renderer.h"
#include "AnimationModel.h"



void AnimationModel::Draw()
{
	// プリミティブトポロジ設定
	Renderer::GetDeviceContext()->IASetPrimitiveTopology(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// マテリアル設定
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	material.Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	material.TextureEnable = true;
	Renderer::SetMaterial(material);

	for (unsigned int m = 0; m < m_AiScene->mNumMeshes; m++)
	{
		aiMesh* mesh = m_AiScene->mMeshes[m];


		// マテリアル設定
		aiString texture;
		aiColor3D diffuse;
		float opacity;

		aiMaterial* aimaterial = m_AiScene->mMaterials[mesh->mMaterialIndex];
		aimaterial->GetTexture(aiTextureType_DIFFUSE, 0, &texture);
		aimaterial->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
		aimaterial->Get(AI_MATKEY_OPACITY, opacity);

		if (texture == aiString(""))
		{
			material.TextureEnable = false;
		}
		else
		{
			Renderer::GetDeviceContext()->PSSetShaderResources(0, 1, &m_Texture[texture.data]);
			material.TextureEnable = true;
		}

		material.Diffuse = XMFLOAT4(diffuse.r, diffuse.g, diffuse.b, opacity);
		material.Ambient = material.Diffuse;
		Renderer::SetMaterial(material);


		// 頂点バッファ設定
		UINT stride = sizeof(VERTEX_3D);
		UINT offset = 0;
		Renderer::GetDeviceContext()->IASetVertexBuffers(0, 1, &m_VertexBuffer[m], &stride, &offset);

		// インデックスバッファ設定
		Renderer::GetDeviceContext()->IASetIndexBuffer(m_IndexBuffer[m], DXGI_FORMAT_R32_UINT, 0);

		// ポリゴン描画
		Renderer::GetDeviceContext()->DrawIndexed(mesh->mNumFaces * 3, 0, 0);
	}
}

void AnimationModel::DrawMesh(unsigned int MeshIndex)
{
	if (MeshIndex >= m_AiScene->mNumMeshes) return;

	aiMesh* mesh = m_AiScene->mMeshes[MeshIndex];

	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	Renderer::GetDeviceContext()->IASetVertexBuffers(0, 1, &m_VertexBuffer[MeshIndex], &stride, &offset);

	// インデックスバッファ設定
	Renderer::GetDeviceContext()->IASetIndexBuffer(m_IndexBuffer[MeshIndex], DXGI_FORMAT_R32_UINT, 0);

	// ポリゴン描画
	Renderer::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Renderer::GetDeviceContext()->DrawIndexed(mesh->mNumFaces * 3, 0, 0);
}

void AnimationModel::Load(const char* FileName)
{
	const std::string modelPath(FileName);

	m_AiScene = aiImportFile(FileName, aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_ConvertToLeftHanded);
	assert(m_AiScene);

	m_VertexBuffer = new ID3D11Buffer * [m_AiScene->mNumMeshes];
	m_IndexBuffer = new ID3D11Buffer * [m_AiScene->mNumMeshes];


	//変形後頂点配列生成
	m_DeformVertex = new std::vector<DEFORM_VERTEX>[m_AiScene->mNumMeshes];

	//再帰的にボーン生成
	CreateBone(m_AiScene->mRootNode);

	for (unsigned int m = 0; m < m_AiScene->mNumMeshes; m++)
	{
		aiMesh* mesh = m_AiScene->mMeshes[m];

		// 頂点バッファ生成
		{
			VERTEX_3D* vertex = new VERTEX_3D[mesh->mNumVertices];

			for (unsigned int v = 0; v < mesh->mNumVertices; v++)
			{
				vertex[v].Position = XMFLOAT3(mesh->mVertices[v].x, mesh->mVertices[v].y, mesh->mVertices[v].z);
				vertex[v].Normal = XMFLOAT3(mesh->mNormals[v].x, mesh->mNormals[v].y, mesh->mNormals[v].z);
				vertex[v].TexCoord = XMFLOAT2(mesh->mTextureCoords[0][v].x, mesh->mTextureCoords[0][v].y);
				vertex[v].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			}

			D3D11_BUFFER_DESC bd;
			ZeroMemory(&bd, sizeof(bd));
			bd.Usage = D3D11_USAGE_DYNAMIC;
			bd.ByteWidth = sizeof(VERTEX_3D) * mesh->mNumVertices;
			bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

			D3D11_SUBRESOURCE_DATA sd;
			ZeroMemory(&sd, sizeof(sd));
			sd.pSysMem = vertex;

			Renderer::GetDevice()->CreateBuffer(&bd, &sd,
				&m_VertexBuffer[m]);

			delete[] vertex;
		}


		// インデックスバッファ生成
		{
			unsigned int* index = new unsigned int[mesh->mNumFaces * 3];

			for (unsigned int f = 0; f < mesh->mNumFaces; f++)
			{
				const aiFace* face = &mesh->mFaces[f];

				assert(face->mNumIndices == 3);

				index[f * 3 + 0] = face->mIndices[0];
				index[f * 3 + 1] = face->mIndices[1];
				index[f * 3 + 2] = face->mIndices[2];
			}

			D3D11_BUFFER_DESC bd;
			ZeroMemory(&bd, sizeof(bd));
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.ByteWidth = sizeof(unsigned int) * mesh->mNumFaces * 3;
			bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
			bd.CPUAccessFlags = 0;

			D3D11_SUBRESOURCE_DATA sd;
			ZeroMemory(&sd, sizeof(sd));
			sd.pSysMem = index;

			Renderer::GetDevice()->CreateBuffer(&bd, &sd, &m_IndexBuffer[m]);

			delete[] index;
		}



		//変形後頂点データ初期化
		for (unsigned int v = 0; v < mesh->mNumVertices; v++)
		{
			DEFORM_VERTEX deformVertex;
			deformVertex.Position = mesh->mVertices[v];
			deformVertex.Normal = mesh->mNormals[v];
			deformVertex.BoneNum = 0;

			for (unsigned int b = 0; b < 4; b++)
			{
				deformVertex.BoneName[b] = "";
				deformVertex.BoneWeight[b] = 0.0f;
			}

			m_DeformVertex[m].push_back(deformVertex);
		}


		//ボーンデータ初期化
		for (unsigned int b = 0; b < mesh->mNumBones; b++)
		{
			aiBone* bone = mesh->mBones[b];

			m_Bone[bone->mName.C_Str()].OffsetMatrix = bone->mOffsetMatrix;

			//変形後頂点にボーンデータ格納
			for (unsigned int w = 0; w < bone->mNumWeights; w++)
			{
				aiVertexWeight weight = bone->mWeights[w];

				int num = m_DeformVertex[m][weight.mVertexId].BoneNum;

				m_DeformVertex[m][weight.mVertexId].BoneWeight[num] = weight.mWeight;
				m_DeformVertex[m][weight.mVertexId].BoneName[num] = bone->mName.C_Str();
				m_DeformVertex[m][weight.mVertexId].BoneNum++;

				assert(m_DeformVertex[m][weight.mVertexId].BoneNum <= 4);
			}
		}
	}

	//テクスチャ読み込み
	for (int i = 0; i < m_AiScene->mNumTextures; i++)
	{
		aiTexture* aitexture = m_AiScene->mTextures[i];

		ID3D11ShaderResourceView* texture;

		// テクスチャ読み込み
		TexMetadata metadata;
		ScratchImage image;
		LoadFromWICMemory(aitexture->pcData, aitexture->mWidth, WIC_FLAGS_NONE, &metadata, image);
		CreateShaderResourceView(Renderer::GetDevice(), image.GetImages(), image.GetImageCount(), metadata, &texture);
		assert(texture);

		m_Texture[aitexture->mFilename.data] = texture;
	}
}

void AnimationModel::LoadAnimation(const char* FileName, const char* Name)
{

	m_Animation[Name] = aiImportFile(FileName, aiProcess_ConvertToLeftHanded);
	assert(m_Animation[Name]);

}

void AnimationModel::CreateBone(aiNode* node)
{
	BONE bone;

	bone.Matrix = aiMatrix4x4();
	bone.AnimationMatrix = aiMatrix4x4();
	bone.OffsetMatrix = aiMatrix4x4();

	m_Bone[node->mName.C_Str()] = bone;

	for (unsigned int n = 0; n < node->mNumChildren; n++)
	{
		CreateBone(node->mChildren[n]);
	}
}

void AnimationModel::Uninit()
{
	for (unsigned int m = 0; m < m_AiScene->mNumMeshes; m++)
	{
		m_VertexBuffer[m]->Release();
		m_IndexBuffer[m]->Release();
	}

	delete[] m_VertexBuffer;
	delete[] m_IndexBuffer;

	delete[] m_DeformVertex;


	for (std::pair<const std::string, ID3D11ShaderResourceView*> pair : m_Texture)
	{
		pair.second->Release();
	}

	aiReleaseImport(m_AiScene);

	for (std::pair<const std::string, const aiScene*> pair : m_Animation)
	{
		aiReleaseImport(pair.second);
	}

}

void AnimationModel::Update(const char* AnimationName1, int Frame1,
	const char* AnimationName2, int Frame2, float BlendRate)
{
	if (m_Animation.count(AnimationName1) == 0)
		return;

	if (!m_Animation[AnimationName1]->HasAnimations())
		return;

	if (m_Animation.count(AnimationName2) == 0)
		return;

	if (!m_Animation[AnimationName2]->HasAnimations())
		return;

	//アニメーションデータからボーンマトリクス算出
	aiAnimation* animation1 = m_Animation[AnimationName1]->mAnimations[0];
	aiAnimation* animation2 = m_Animation[AnimationName2]->mAnimations[0];

	for (auto pair : m_Bone)
	{
		BONE* bone = &m_Bone[pair.first];

		aiNodeAnim* nodeAnim1 = nullptr;

		for (unsigned int n = 0; n < animation1->mNumChannels; n++)
		{
			if (animation1->mChannels[n]->mNodeName == aiString(pair.first))
			{
				nodeAnim1 = animation1->mChannels[n];
				break;
			}
		}

		aiNodeAnim* nodeAnim2 = nullptr;

		for (unsigned int n = 0; n < animation2->mNumChannels; n++)
		{
			if (animation2->mChannels[n]->mNodeName == aiString(pair.first))
			{
				nodeAnim2 = animation2->mChannels[n];
				break;
			}
		}

		aiQuaternion rotation1;
		aiVector3D position1;
		int f;
		if (nodeAnim1)
		{
			f = Frame1 % nodeAnim1->mNumRotationKeys;
			rotation1 = nodeAnim1->mRotationKeys[f].mValue;

			f = Frame1 % nodeAnim1->mNumPositionKeys;
			position1 = nodeAnim1->mPositionKeys[f].mValue;
		}

		aiQuaternion rotation2;
		aiVector3D position2;
		if (nodeAnim2)
		{
			f = Frame1 % nodeAnim2->mNumRotationKeys;
			rotation2 = nodeAnim2->mRotationKeys[f].mValue;

			f = Frame1 % nodeAnim2->mNumPositionKeys;
			position2 = nodeAnim2->mPositionKeys[f].mValue;
		}

		aiVector3D position;
		position = position1 * (1.0f - BlendRate) + position2 * BlendRate;//線形補間

		aiQuaternion rotation;
		aiQuaternion::Interpolate(rotation, rotation1, rotation2, BlendRate);//球面線形補間

		bone->AnimationMatrix = aiMatrix4x4(aiVector3D(1.0f, 1.0f, 1.0f), rotation, position);
	}

	//再帰的にボーンマトリクスを更新
	aiMatrix4x4 rootMatrix = aiMatrix4x4(aiVector3D(0.01f, 0.01f, 0.01f),
		aiQuaternion(/*(float)AI_MATH_PI*/ 0.0f, 0.0f, 0.0f),
		aiVector3D(0.0f, 0.0f, 0.0f));

	UpdateBoneMatrix(m_AiScene->mRootNode, rootMatrix);

	//頂点変換（CPUスキニング）
	for (unsigned int m = 0; m < m_AiScene->mNumMeshes; m++)
	{
		aiMesh* mesh = m_AiScene->mMeshes[m];

		D3D11_MAPPED_SUBRESOURCE ms;
		Renderer::GetDeviceContext()->Map(m_VertexBuffer[m], 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);

		VERTEX_3D* vertex = (VERTEX_3D*)ms.pData;

		for (unsigned int v = 0; v < mesh->mNumVertices; v++)
		{
			DEFORM_VERTEX* deformVertex = &m_DeformVertex[m][v];

			aiMatrix4x4 matrix[4];
			for (int b = 0; b < 4; b++)
			{
				matrix[b] = m_Bone[deformVertex->BoneName[b]].Matrix;
			}

			aiMatrix4x4 outMatrix;
			outMatrix = matrix[0] * deformVertex->BoneWeight[0]
				+ matrix[1] * deformVertex->BoneWeight[1]
				+ matrix[2] * deformVertex->BoneWeight[2]
				+ matrix[3] * deformVertex->BoneWeight[3];

			deformVertex->Position = outMatrix * mesh->mVertices[v];
			//deformVertex->Position *= outMatrix;

			outMatrix.a4 = 0.0f;
			outMatrix.b4 = 0.0f;
			outMatrix.c4 = 0.0f;

			deformVertex->Normal = outMatrix * mesh->mNormals[v];

			vertex[v].Position = XMFLOAT3(deformVertex->Position.x, deformVertex->Position.y, deformVertex->Position.z);
			vertex[v].Normal = XMFLOAT3(deformVertex->Normal.x, deformVertex->Normal.y, deformVertex->Normal.z);
			vertex[v].TexCoord = XMFLOAT2(mesh->mTextureCoords[0][v].x, mesh->mTextureCoords[0][v].y);
			vertex[v].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		}

		Renderer::GetDeviceContext()->Unmap(m_VertexBuffer[m], 0);
	}
}

void AnimationModel::Update(aiMatrix4x4 Matrix)
{
	UpdateBoneMatrix(m_AiScene->mRootNode, Matrix);
	//頂点変換（CPUスキニング）
	for (unsigned int m = 0; m < m_AiScene->mNumMeshes; m++)
	{
		aiMesh* mesh = m_AiScene->mMeshes[m];

		D3D11_MAPPED_SUBRESOURCE ms;
		Renderer::GetDeviceContext()->Map(m_VertexBuffer[m], 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);

		VERTEX_3D* vertex = (VERTEX_3D*)ms.pData;

		for (unsigned int v = 0; v < mesh->mNumVertices; v++)
		{
			DEFORM_VERTEX* deformVertex = &m_DeformVertex[m][v];

			aiMatrix4x4 matrix[4];
			for (int b = 0; b < 4; b++)
			{
				matrix[b] = m_Bone[deformVertex->BoneName[b]].Matrix;
			}

			aiMatrix4x4 outMatrix;
			outMatrix = matrix[0] * deformVertex->BoneWeight[0]
				+ matrix[1] * deformVertex->BoneWeight[1]
				+ matrix[2] * deformVertex->BoneWeight[2]
				+ matrix[3] * deformVertex->BoneWeight[3];

			deformVertex->Position = outMatrix * mesh->mVertices[v];
			//deformVertex->Position *= outMatrix;

			outMatrix.a4 = 0.0f;
			outMatrix.b4 = 0.0f;
			outMatrix.c4 = 0.0f;

			deformVertex->Normal = outMatrix * mesh->mNormals[v];

			vertex[v].Position = XMFLOAT3(deformVertex->Position.x, deformVertex->Position.y, deformVertex->Position.z);
			vertex[v].Normal = XMFLOAT3(deformVertex->Normal.x, deformVertex->Normal.y, deformVertex->Normal.z);
			vertex[v].TexCoord = XMFLOAT2(mesh->mTextureCoords[0][v].x, mesh->mTextureCoords[0][v].y);
			vertex[v].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		}

		Renderer::GetDeviceContext()->Unmap(m_VertexBuffer[m], 0);
	}
}

void AnimationModel::UpdateBoneMatrix(const aiNode* node, const aiMatrix4x4& parentTransform)
{
	std::string nodeName(node->mName.C_Str());

	aiMatrix4x4 nodeTransform = node->mTransformation;

	// 対応するボーンがある場合は手動設定された AnimationMatrix を使用
	auto it = m_Bone.find(nodeName);
	if (it != m_Bone.end())
	{
		nodeTransform = it->second.AnimationMatrix;
	}

	aiMatrix4x4 globalTransform = parentTransform * nodeTransform;

	// ボーンが見つかった場合はスキニング行列を更新
	if (it != m_Bone.end())
	{
		it->second.Matrix = globalTransform * it->second.OffsetMatrix;
	}

	// 子ノードを再帰処理
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		UpdateBoneMatrix(node->mChildren[i], globalTransform);
	}
}

BONE* AnimationModel::GetBone(std::string name)
{
	auto it = m_Bone.find(name);
	if (it != m_Bone.end())
	{
		return &it->second;
	}
	return nullptr;
}

void AnimationModel::SetBoneMatrix(const std::string& boneName, const aiMatrix4x4& matrix)
{
	auto it = m_Bone.find(boneName);
	if (it == m_Bone.end())
		return;

	it->second.AnimationMatrix = matrix;
}

void AnimationModel::Pose()
{
	auto setBindPose = [&](aiNode* node, auto&& self) -> void {
		m_Bone[node->mName.C_Str()].AnimationMatrix = node->mTransformation;
		for (unsigned int i = 0; i < node->mNumChildren; ++i) {
			self(node->mChildren[i], self);
		}
		};
	setBindPose(m_AiScene->mRootNode, setBindPose);
}

void AnimationModel::SetBoneAngle(const std::string& boneName, float angleDeg, const aiVector3D& axis)
{
	auto joint = m_Bone.find(boneName);
	if (joint == m_Bone.end())
		return;

	const aiNode* node = m_AiScene->mRootNode->FindNode(boneName.c_str());
	if (!node)
		return;

	// 初期ローカル行列（BindPose）
	aiMatrix4x4 baseTransform = node->mTransformation;

	// 回転行列（ローカル軸で回転）
	aiQuaternion rot(axis, XMConvertToRadians(angleDeg));
	aiMatrix4x4 rotMat(rot.GetMatrix());

	// 初期ローカルに回転を掛ける
	joint->second.AnimationMatrix = baseTransform * rotMat;
}

void AnimationModel::RotateBoneLocal(const std::string& boneName, float angleSpeedDeg, const aiVector3D& axis)
{
	auto joint = m_Bone.find(boneName);
	if (joint == m_Bone.end())
		return;

	// 回転行列を作成
	aiQuaternion rot(axis, XMConvertToRadians(angleSpeedDeg));
	aiMatrix4x4 rotMat(rot.GetMatrix());

	// 「元のローカル姿勢 × 追加回転 × 既存の手動変換」
	joint->second.AnimationMatrix = joint->second.AnimationMatrix * rotMat;
}

Vector3 AnimationModel::GetBoneWorldPosition(const std::string& boneName, const aiMatrix4x4& rootMatrix)
{
	const aiNode* node = m_AiScene->mRootNode->FindNode(boneName.c_str());
	if (!node)
	{
		return Vector3(0.0f, 0.0f, 0.0f);
	}

	aiMatrix4x4 globalTransform;
	const aiNode* current = node;
	while (current)
	{
		std::string currentName(current->mName.C_Str());

		aiMatrix4x4 nodeTransform = current->mTransformation;

		auto boneIt = m_Bone.find(currentName);
		if (boneIt != m_Bone.end())
		{
			nodeTransform = boneIt->second.AnimationMatrix;
		}

		globalTransform = nodeTransform * globalTransform;

		current = current->mParent;
	}

	globalTransform = rootMatrix * globalTransform;

	return Vector3(globalTransform.a4, globalTransform.b4, globalTransform.c4);
}

XMMATRIX AnimationModel::MultiplyAiMatrixWithXMMatrix(aiMatrix4x4 aiMat, XMMATRIX xmMat)
{
	// 1. aiMatrix4x4をXMFLOAT4X4に変換
	XMFLOAT4X4 float4x4;
	float4x4._11 = aiMat.a1; float4x4._12 = aiMat.a2; float4x4._13 = aiMat.a3; float4x4._14 = aiMat.a4;
	float4x4._21 = aiMat.b1; float4x4._22 = aiMat.b2; float4x4._23 = aiMat.b3; float4x4._24 = aiMat.b4;
	float4x4._31 = aiMat.c1; float4x4._32 = aiMat.c2; float4x4._33 = aiMat.c3; float4x4._34 = aiMat.c4;
	float4x4._41 = aiMat.d1; float4x4._42 = aiMat.d2; float4x4._43 = aiMat.d3; float4x4._44 = aiMat.d4;

	// 2. XMFLOAT4X4をXMMATRIXに変換
	XMMATRIX aiMatrix = XMLoadFloat4x4(&float4x4);

	// 3. 乗算（aiMatrix × xmMat）
	XMMATRIX result = XMMatrixMultiply(aiMatrix, xmMat);

	return result;
}

void AnimationModel::SetBoneWorldMatrix(const std::string& boneName, const aiMatrix4x4& worldRotationMatrix)
{
	auto it = m_Bone.find(boneName);
	if (it == m_Bone.end()) return;

	const aiNode* node = m_AiScene->mRootNode->FindNode(boneName.c_str());
	if (!node) return;

	// 1. 親までのワールド逆行列を求める（親の影響をキャンセル）
	aiMatrix4x4 parentWorldInv = aiMatrix4x4();
	const aiNode* parent = node->mParent;
	while (parent)
	{
		aiMatrix4x4 local = parent->mTransformation;
		auto boneIt = m_Bone.find(std::string(parent->mName.C_Str()));
		if (boneIt != m_Bone.end())
		{
			local = boneIt->second.AnimationMatrix;
		}
		parentWorldInv = local * parentWorldInv;
		parent = parent->mParent;
	}
	if (parentWorldInv.Determinant() != 0.0f)
	{
		parentWorldInv.Inverse();
	}

	// 2. 欲しいワールド回転をローカルに変換
	aiMatrix4x4 localRot = parentWorldInv * worldRotationMatrix;

	// 3. BindPose の平行移動・スケールを保持しつつ、回転だけ置き換える
	aiMatrix4x4 bind = node->mTransformation;
	bind.a1 = localRot.a1; bind.a2 = localRot.a2; bind.a3 = localRot.a3;
	bind.b1 = localRot.b1; bind.b2 = localRot.b2; bind.b3 = localRot.b3;
	bind.c1 = localRot.c1; bind.c2 = localRot.c2; bind.c3 = localRot.c3;
	// d1-d4（平行移動）は bind のまま残す

	it->second.AnimationMatrix = bind;
}

aiMatrix4x4 AnimationModel::XMMatrixToAiMatrix(XMMATRIX& xm)
{
	XMFLOAT4X4 f;
	XMStoreFloat4x4(&f, xm);

	aiMatrix4x4 ai;
	ai.a1 = f._11; ai.a2 = f._12; ai.a3 = f._13; ai.a4 = f._14;
	ai.b1 = f._21; ai.b2 = f._22; ai.b3 = f._23; ai.b4 = f._24;
	ai.c1 = f._31; ai.c2 = f._32; ai.c3 = f._33; ai.c4 = f._34;
	ai.d1 = f._41; ai.d2 = f._42; ai.d3 = f._43; ai.d4 = f._44;
	return ai;
}

