#include "main.h"
#include "Manager.h"
#include "Renderer.h"
#include "Texture.h"

std::unordered_map<std::string, ID3D11ShaderResourceView*> Texture::m_TexturePool;

ID3D11ShaderResourceView* Texture::Load(const char* FileName, bool isColorTexture)
{
	if (m_TexturePool.count(FileName) > 0)
	{
		return m_TexturePool[FileName];
	}

	wchar_t wFileName[512];
	mbstowcs(wFileName, FileName, strlen(FileName) + 1);

	//Texture Load
	WIC_FLAGS wicFlags = isColorTexture ? WIC_FLAGS_DEFAULT_SRGB : WIC_FLAGS_NONE;
	CREATETEX_FLAGS createFlags = isColorTexture ? CREATETEX_FORCE_SRGB : CREATETEX_DEFAULT;

	TexMetadata metadata;
	ScratchImage image;
	ScratchImage mipChain;
	ID3D11ShaderResourceView* texture;

	LoadFromWICFile(wFileName, wicFlags, &metadata, image);

	GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), TEX_FILTER_DEFAULT, 0, mipChain);

	CreateShaderResourceViewEx(
		Renderer::GetDevice(),
		mipChain.GetImages(), mipChain.GetImageCount(), metadata,
		D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, 0,
		createFlags,
		&texture);

	assert(texture);

	m_TexturePool[FileName] = texture;

	return texture;
}

void Texture::Release()
{
	if (!m_TexturePool.empty())
	{
		for (auto& it : m_TexturePool)
		{
			if (it.second)
			{
				it.second->Release();
				it.second = nullptr;
			}
		}
		m_TexturePool.clear();
	}
}