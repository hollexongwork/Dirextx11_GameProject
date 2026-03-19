#include "main.h"
#include "Renderer.h"
#include "Model.h"

MODEL* ModelLoad( const char *FileName )
{
    MODEL* model = new MODEL;

    model->AiScene = aiImportFile(
        FileName,
        aiProcessPreset_TargetRealtime_MaxQuality |
        aiProcess_ConvertToLeftHanded |
        aiProcess_CalcTangentSpace // Tangent/Binormal生成をAssimpに依頼
    );
    assert(model->AiScene);

    model->VertexBuffer = new ID3D11Buffer * [model->AiScene->mNumMeshes];
    model->IndexBuffer = new ID3D11Buffer * [model->AiScene->mNumMeshes];

    for (unsigned int m = 0; m < model->AiScene->mNumMeshes; m++)
    {
        aiMesh* mesh = model->AiScene->mMeshes[m];

        // 頂点バッファ作成
        VERTEX_3D* vertex = new VERTEX_3D[mesh->mNumVertices];

        for (unsigned int v = 0; v < mesh->mNumVertices; v++)
        {
            // 位置変換（Y up → Z up）
            vertex[v].Position = XMFLOAT3(mesh->mVertices[v].x, -mesh->mVertices[v].z, mesh->mVertices[v].y);
            vertex[v].Normal = XMFLOAT3(mesh->mNormals[v].x, -mesh->mNormals[v].z, mesh->mNormals[v].y);

            // UV
            if (mesh->HasTextureCoords(0))
                vertex[v].TexCoord = XMFLOAT2(mesh->mTextureCoords[0][v].x, mesh->mTextureCoords[0][v].y);
            else
                vertex[v].TexCoord = XMFLOAT2(0.0f, 0.0f);

            vertex[v].Diffuse = XMFLOAT4(1, 1, 1, 1);

            // Tangent / Binormal
            if (mesh->HasTangentsAndBitangents())
            {
                vertex[v].Tangent = XMFLOAT3(mesh->mTangents[v].x, -mesh->mTangents[v].z, mesh->mTangents[v].y);
                vertex[v].Binormal = XMFLOAT3(mesh->mBitangents[v].x, -mesh->mBitangents[v].z, mesh->mBitangents[v].y);
            }
            else
            {
                // クロス積による簡易計算（あまり推奨されないが最低限）
                XMVECTOR normal = XMLoadFloat3(&vertex[v].Normal);
                XMVECTOR tangent = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f); // 仮のTangent
                XMVECTOR binormal = XMVector3Normalize(XMVector3Cross(normal, tangent));

                XMStoreFloat3(&vertex[v].Tangent, tangent);
                XMStoreFloat3(&vertex[v].Binormal, binormal);
            }
        }

        // バッファ作成
        D3D11_BUFFER_DESC bd = {};
        bd.Usage = D3D11_USAGE_DYNAMIC;
        bd.ByteWidth = sizeof(VERTEX_3D) * mesh->mNumVertices;
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        D3D11_SUBRESOURCE_DATA sd = {};
        sd.pSysMem = vertex;

        Renderer::GetDevice()->CreateBuffer(&bd, &sd, &model->VertexBuffer[m]);

        delete[] vertex;

        // インデックスバッファ
        unsigned int* index = new unsigned int[mesh->mNumFaces * 3];

        for (unsigned int f = 0; f < mesh->mNumFaces; f++)
        {
            const aiFace* face = &mesh->mFaces[f];
            assert(face->mNumIndices == 3);

            index[f * 3 + 0] = face->mIndices[0];
            index[f * 3 + 1] = face->mIndices[1];
            index[f * 3 + 2] = face->mIndices[2];
        }

        D3D11_BUFFER_DESC ibd = {};
        ibd.Usage = D3D11_USAGE_DEFAULT;
        ibd.ByteWidth = sizeof(unsigned int) * mesh->mNumFaces * 3;
        ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;

        D3D11_SUBRESOURCE_DATA isd = {};
        isd.pSysMem = index;

        Renderer::GetDevice()->CreateBuffer(&ibd, &isd, &model->IndexBuffer[m]);

        delete[] index;
    }

    // テクスチャ読み込み（Assimp埋め込みテクスチャ対応）
    for (UINT i = 0; i < model->AiScene->mNumTextures; i++)
    {
        aiTexture* aitexture = model->AiScene->mTextures[i];

        ID3D11ShaderResourceView* texture;
        TexMetadata metadata;
        ScratchImage image;
        LoadFromWICMemory(aitexture->pcData, aitexture->mWidth, WIC_FLAGS_NONE, &metadata, image);
        CreateShaderResourceView(Renderer::GetDevice(), image.GetImages(), image.GetImageCount(), metadata, &texture);
        assert(texture);

        model->Texture[aitexture->mFilename.data] = texture;
    }
    return model;
}

void ModelRelease(MODEL* model)
{
	for (unsigned int m = 0; m < model->AiScene->mNumMeshes; m++)
	{
		model->VertexBuffer[m]->Release();
		model->IndexBuffer[m]->Release();
	}

	delete[] model->VertexBuffer;
	delete[] model->IndexBuffer;


	for (std::pair<const std::string, ID3D11ShaderResourceView*> pair : model->Texture)
	{
		pair.second->Release();
	}


	aiReleaseImport(model->AiScene);


	delete model;
}


void ModelDraw(MODEL* model)
{
	// プリミティブトポロジ設定
	Renderer::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	for (unsigned int m = 0; m < model->AiScene->mNumMeshes; m++)
	{
		aiMesh* mesh = model->AiScene->mMeshes[m];

		// テクスチャ設定
		aiString texture;
		aiMaterial* aimaterial = model->AiScene->mMaterials[mesh->mMaterialIndex];
		aimaterial->GetTexture(aiTextureType_DIFFUSE, 0, &texture);

		if (texture != aiString(""))
			Renderer::GetDeviceContext()->PSSetShaderResources(0, 1, &model->Texture[texture.data]);

		// 頂点バッファ設定
		UINT stride = sizeof(VERTEX_3D);
		UINT offset = 0;
		Renderer::GetDeviceContext()->IASetVertexBuffers(0, 1, &model->VertexBuffer[m], &stride, &offset);

		// インデックスバッファ設定
		Renderer::GetDeviceContext()->IASetIndexBuffer(model->IndexBuffer[m], DXGI_FORMAT_R32_UINT, 0);

		// ポリゴン描画
		Renderer::GetDeviceContext()->DrawIndexed(mesh->mNumFaces * 3, 0, 0);
	}
}




