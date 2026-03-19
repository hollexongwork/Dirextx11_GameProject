#include "main.h"
#include "Renderer.h"
#include "ShadowMap.h"

ID3D11Texture2D* ShadowMap::m_ShadowMapTexture[MAX_GLOBAL_LIGHTS];
ID3D11DepthStencilView* ShadowMap::m_ShadowMapDSV[MAX_GLOBAL_LIGHTS][6];
ID3D11ShaderResourceView* ShadowMap::m_ShadowMapSRV[MAX_GLOBAL_LIGHTS];
ID3D11SamplerState* ShadowMap::m_ShadowSamplerState;
ID3D11VertexShader* ShadowMap::m_ShadowVertexShader;
ID3D11InputLayout* ShadowMap::m_ShadowVertexLayout;

void ShadowMap::Init()
{
	HRESULT hr;
	// Create shadow map sampler state
	D3D11_SAMPLER_DESC shadowSamplerDesc = {};
	shadowSamplerDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
	shadowSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	shadowSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	shadowSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	shadowSamplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
	shadowSamplerDesc.BorderColor[0] = 1.0f;
	shadowSamplerDesc.BorderColor[1] = 1.0f;
	shadowSamplerDesc.BorderColor[2] = 1.0f;
	shadowSamplerDesc.BorderColor[3] = 1.0f;
	shadowSamplerDesc.MaxAnisotropy = 1;
	shadowSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = Renderer::GetDevice()->CreateSamplerState(&shadowSamplerDesc, &m_ShadowSamplerState);
	assert(SUCCEEDED(hr));

	// Load shadow vertex shader
	Renderer::CreateVertexShader(&m_ShadowVertexShader, &m_ShadowVertexLayout, "shader/cso/ShadowVS.cso");
}

void ShadowMap::Uninit()
{
	if (m_ShadowSamplerState) { m_ShadowSamplerState->Release(); m_ShadowSamplerState = nullptr; }
	if (m_ShadowVertexShader) { m_ShadowVertexShader->Release(); m_ShadowVertexShader = nullptr; }
	if (m_ShadowVertexLayout) { m_ShadowVertexLayout->Release(); m_ShadowVertexLayout = nullptr; }
	for (int i = 0; i < MAX_GLOBAL_LIGHTS; i++)
	{
		if (m_ShadowMapSRV[i]) m_ShadowMapSRV[i]->Release();
		for (int face = 0; face < 6; face++)
		{
			if (m_ShadowMapDSV[i][face]) m_ShadowMapDSV[i][face]->Release();
		}
		if (m_ShadowMapTexture[i]) m_ShadowMapTexture[i]->Release();
	}
}

void ShadowMap::RenderShadowMap(const LIGHT& light, int lightIndex)
{
	if (!light.CastsShadow) return;

	int numFaces = (light.Type == LIGHT_POINT) ? 6 : 1;

	ID3D11DeviceContext* DeviceContext = Renderer::GetDeviceContext();

	for (int face = 0; face < numFaces; face++)
	{
		DeviceContext->ClearDepthStencilView(m_ShadowMapDSV[lightIndex][face], D3D11_CLEAR_DEPTH, 1.0f, 0);
		DeviceContext->OMSetRenderTargets(0, nullptr, m_ShadowMapDSV[lightIndex][face]);

		D3D11_VIEWPORT shadowViewport = {};
		shadowViewport.Width = SHADOW_MAP_SIZE;
		shadowViewport.Height = SHADOW_MAP_SIZE;
		shadowViewport.MinDepth = 0.0f;
		shadowViewport.MaxDepth = 1.0f;
		shadowViewport.TopLeftX = 0;
		shadowViewport.TopLeftY = 0;
		DeviceContext->RSSetViewports(1, &shadowViewport);

		DeviceContext->VSSetShader(m_ShadowVertexShader, nullptr, 0);
		DeviceContext->IASetInputLayout(m_ShadowVertexLayout);

		XMFLOAT4X4 lightVP;
		XMStoreFloat4x4(&lightVP, XMMatrixTranspose(XMLoadFloat4x4(&light.LightViewProjection[face])));
		DeviceContext->UpdateSubresource(Renderer::GetProjectionBuffer(), 0, nullptr, &lightVP, 0, 0);
	}
}

void ShadowMap::BindShadowMaps()
{
	ID3D11DeviceContext* DeviceContext = Renderer::GetDeviceContext();
	DeviceContext->PSSetShaderResources(StartSlot, MAX_GLOBAL_LIGHTS, m_ShadowMapSRV);
	DeviceContext->PSSetSamplers(2, 1, &m_ShadowSamplerState);
}

void ShadowMap::CreateShadowMapForLight(int index, LightType type)
{
	UINT arraySize = (type == LIGHT_POINT) ? 6 : 1;

	ReleaseShadowMap(index);

	ID3D11Device* Device = Renderer::GetDevice();

	D3D11_TEXTURE2D_DESC shadowTexDesc = {};
	shadowTexDesc.Width = SHADOW_MAP_SIZE;
	shadowTexDesc.Height = SHADOW_MAP_SIZE;
	shadowTexDesc.MipLevels = 1;
	shadowTexDesc.ArraySize = arraySize;
	shadowTexDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	shadowTexDesc.SampleDesc.Count = 1;
	shadowTexDesc.Usage = D3D11_USAGE_DEFAULT;
	shadowTexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	shadowTexDesc.MiscFlags = (type == LIGHT_POINT) ? D3D11_RESOURCE_MISC_TEXTURECUBE : 0;

	HRESULT hr = Device->CreateTexture2D(&shadowTexDesc, nullptr, &m_ShadowMapTexture[index]);
	assert(SUCCEEDED(hr));

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = (arraySize > 1) ? D3D11_DSV_DIMENSION_TEXTURE2DARRAY : D3D11_DSV_DIMENSION_TEXTURE2D;

	for (UINT face = 0; face < arraySize; ++face)
	{
		if (arraySize > 1)
		{
			dsvDesc.Texture2DArray.MipSlice = 0;
			dsvDesc.Texture2DArray.FirstArraySlice = face;
			dsvDesc.Texture2DArray.ArraySize = 1;
		}
		else
		{
			dsvDesc.Texture2D.MipSlice = 0;
		}
		hr = Device->CreateDepthStencilView(m_ShadowMapTexture[index], &dsvDesc, &m_ShadowMapDSV[index][face]);
		assert(SUCCEEDED(hr));
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = (arraySize > 1) ? D3D11_SRV_DIMENSION_TEXTURE2DARRAY : D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2DArray.MostDetailedMip = 0;
	srvDesc.Texture2DArray.MipLevels = 1;
	srvDesc.Texture2DArray.FirstArraySlice = 0;
	srvDesc.Texture2DArray.ArraySize = arraySize;

	hr = Device->CreateShaderResourceView(m_ShadowMapTexture[index], &srvDesc, &m_ShadowMapSRV[index]);
	assert(SUCCEEDED(hr));
}

void ShadowMap::ReleaseShadowMap(int index)
{
	if (m_ShadowMapSRV[index]) { m_ShadowMapSRV[index]->Release(); m_ShadowMapSRV[index] = nullptr; }
	for (int face = 0; face < 6; ++face)
	{
		if (m_ShadowMapDSV[index][face]) { m_ShadowMapDSV[index][face]->Release(); m_ShadowMapDSV[index][face] = nullptr; }
	}
	if (m_ShadowMapTexture[index]) { m_ShadowMapTexture[index]->Release(); m_ShadowMapTexture[index] = nullptr; }
}
