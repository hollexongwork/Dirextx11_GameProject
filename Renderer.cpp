#include "main.h"
#include "Renderer.h"
#include "ShadowMap.h"
#include <io.h>


D3D_FEATURE_LEVEL       Renderer::m_FeatureLevel = D3D_FEATURE_LEVEL_11_0;

ID3D11Device* Renderer::m_Device{};
ID3D11DeviceContext* Renderer::m_DeviceContext{};
IDXGISwapChain* Renderer::m_SwapChain{};
ID3D11RenderTargetView* Renderer::m_RenderTargetView{};
ID3D11DepthStencilView* Renderer::m_DepthStencilView{};
ID3D11DepthStencilView* Renderer::m_DepthStencilMainView{};

ID3D11Buffer* Renderer::m_WorldBuffer{};
ID3D11Buffer* Renderer::m_ViewBuffer{};
ID3D11Buffer* Renderer::m_ProjectionBuffer{};
ID3D11Buffer* Renderer::m_InvViewProjBuffer{};
ID3D11Buffer* Renderer::m_MaterialBuffer{};
ID3D11Buffer* Renderer::m_LightBuffer{};
ID3D11Buffer* Renderer::m_CameraBuffer{};

ID3D11DepthStencilState* Renderer::m_DepthStateEnable{};
ID3D11DepthStencilState* Renderer::m_DepthStateDisable{};
ID3D11DepthStencilState* Renderer::m_DepthWriteMaskDisable{};

ID3D11RasterizerState* Renderer::m_RasterizerStateCullNone{};
ID3D11RasterizerState* Renderer::m_RasterizerStateCullBack{};

ID3D11BlendState* Renderer::m_BlendState{};
ID3D11BlendState* Renderer::m_BlendStateAlphaBlend{};
ID3D11BlendState* Renderer::m_BlendStateAdd{};

ID3D11Texture2D* Renderer::m_SceneTexture{};
ID3D11RenderTargetView* Renderer::m_SceneRTV{};
ID3D11ShaderResourceView* Renderer::m_SceneSRV{};

ID3D11Texture2D* Renderer::m_RenderTexture{};
ID3D11RenderTargetView* Renderer::m_RenderRTV{};
ID3D11ShaderResourceView* Renderer::m_RenderSRV{};

ID3D11Texture2D* Renderer::m_DepthTexture{};
ID3D11ShaderResourceView* Renderer::m_DepthSRV{};

void Renderer::Init()
{
	HRESULT hr = S_OK;

	// SWAP CHAIN
	DXGI_SWAP_CHAIN_DESC swapChainDesc{};
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width = SCREEN_WIDTH;
	swapChainDesc.BufferDesc.Height = SCREEN_HEIGHT;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = GetWindow();
	swapChainDesc.SampleDesc.Count = 1; //MSAA Sample Num Setting
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.Windowed = TRUE;

	hr = D3D11CreateDeviceAndSwapChain(NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		0,
		NULL,
		0,
		D3D11_SDK_VERSION,
		&swapChainDesc,
		&m_SwapChain,
		&m_Device,
		&m_FeatureLevel,
		&m_DeviceContext);//

	// Render Target
	ID3D11Texture2D* renderTarget{};
	m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&renderTarget);
	m_Device->CreateRenderTargetView(renderTarget, NULL, &m_RenderTargetView);
	renderTarget->Release();

	// Depth Texture
	D3D11_TEXTURE2D_DESC depthDesc{};
	depthDesc.Width = swapChainDesc.BufferDesc.Width;
	depthDesc.Height = swapChainDesc.BufferDesc.Height;
	depthDesc.MipLevels = 1;
	depthDesc.ArraySize = 1;
	depthDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	depthDesc.SampleDesc = swapChainDesc.SampleDesc;
	depthDesc.Usage = D3D11_USAGE_DEFAULT;
	depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	depthDesc.CPUAccessFlags = 0;
	depthDesc.MiscFlags = 0;
	hr = m_Device->CreateTexture2D(&depthDesc, NULL, &m_DepthTexture);
	assert(SUCCEEDED(hr));

	// DEPTH STENCIL VIEW
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
	depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D; 
	depthStencilViewDesc.Flags = 0;
	hr = m_Device->CreateDepthStencilView(m_DepthTexture, &depthStencilViewDesc, &m_DepthStencilView);
	assert(SUCCEEDED(hr));

	//Depth ShaderResourceView
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;
	hr = m_Device->CreateShaderResourceView(m_DepthTexture, &srvDesc, &m_DepthSRV);
	assert(SUCCEEDED(hr));

	// Depth Stencile Buff
	ID3D11Texture2D* depthStencile{};
	D3D11_TEXTURE2D_DESC depthMainDesc{};
	depthMainDesc.Width = swapChainDesc.BufferDesc.Width;
	depthMainDesc.Height = swapChainDesc.BufferDesc.Height;
	depthMainDesc.MipLevels = 1;
	depthMainDesc.ArraySize = 1;
	depthMainDesc.Format = DXGI_FORMAT_D16_UNORM;
	depthMainDesc.SampleDesc = swapChainDesc.SampleDesc;
	depthMainDesc.Usage = D3D11_USAGE_DEFAULT;
	depthMainDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthMainDesc.CPUAccessFlags = 0;
	depthMainDesc.MiscFlags = 0;
	hr = m_Device->CreateTexture2D(&depthMainDesc, NULL, &depthStencile);
	assert(SUCCEEDED(hr));

	// Depth Stencile Main View
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilMainViewDesc{};
	depthStencilMainViewDesc.Format = depthMainDesc.Format;
	depthStencilMainViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;// MSAA Setting
	depthStencilMainViewDesc.Flags = 0;
	hr = m_Device->CreateDepthStencilView(depthStencile, &depthStencilMainViewDesc, &m_DepthStencilMainView);
	assert(SUCCEEDED(hr));

	m_DeviceContext->OMSetRenderTargets(1, &m_RenderTargetView, m_DepthStencilMainView);


	// Scene texture for post-processing
	D3D11_TEXTURE2D_DESC sceneTexDesc{};
	sceneTexDesc.Width = SCREEN_WIDTH;
	sceneTexDesc.Height = SCREEN_HEIGHT;
	sceneTexDesc.MipLevels = 1;
	sceneTexDesc.ArraySize = 1;
	sceneTexDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	sceneTexDesc.SampleDesc.Count = 1;
	sceneTexDesc.SampleDesc.Quality = 0;
	sceneTexDesc.Usage = D3D11_USAGE_DEFAULT;
	sceneTexDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	sceneTexDesc.CPUAccessFlags = 0;
	sceneTexDesc.MiscFlags = 0;
	hr = m_Device->CreateTexture2D(&sceneTexDesc, NULL, &m_SceneTexture);
	assert(SUCCEEDED(hr));

	hr = m_Device->CreateRenderTargetView(m_SceneTexture, NULL, &m_SceneRTV);
	assert(SUCCEEDED(hr));

	hr = m_Device->CreateShaderResourceView(m_SceneTexture, NULL, &m_SceneSRV);
	assert(SUCCEEDED(hr));

	// Render texture
	D3D11_TEXTURE2D_DESC renderTexDesc{};
	renderTexDesc.Width = SCREEN_WIDTH;
	renderTexDesc.Height = SCREEN_HEIGHT;
	renderTexDesc.MipLevels = 1;
	renderTexDesc.ArraySize = 1;
	renderTexDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	renderTexDesc.SampleDesc.Count = 1;
	renderTexDesc.SampleDesc.Quality = 0;
	renderTexDesc.Usage = D3D11_USAGE_DEFAULT;
	renderTexDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	renderTexDesc.CPUAccessFlags = 0;
	renderTexDesc.MiscFlags = 0;
	hr = m_Device->CreateTexture2D(&renderTexDesc, nullptr, &m_RenderTexture);
	assert(SUCCEEDED(hr));

	hr = m_Device->CreateRenderTargetView(m_RenderTexture, nullptr, &m_RenderRTV);
	assert(SUCCEEDED(hr));

	hr = m_Device->CreateShaderResourceView(m_RenderTexture, nullptr, &m_RenderSRV);
	assert(SUCCEEDED(hr));

	// VIEWPORT
	D3D11_VIEWPORT viewport;
	viewport.Width = (FLOAT)SCREEN_WIDTH;
	viewport.Height = (FLOAT)SCREEN_HEIGHT;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	m_DeviceContext->RSSetViewports(1, &viewport);

	// RASTERIZER
	D3D11_RASTERIZER_DESC rasterizerDesc{};
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	rasterizerDesc.DepthClipEnable = TRUE;
	rasterizerDesc.MultisampleEnable = FALSE;
	rasterizerDesc.DepthBias = 1;
	rasterizerDesc.SlopeScaledDepthBias = 1.0f;
	rasterizerDesc.DepthBiasClamp = 0.0f;

	m_Device->CreateRasterizerState(&rasterizerDesc, &m_RasterizerStateCullBack);

	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	m_Device->CreateRasterizerState(&rasterizerDesc, &m_RasterizerStateCullNone);

	m_DeviceContext->RSSetState(m_RasterizerStateCullBack);

	// --------------------------------------------------------------- Blend ---------------------------------------------------------------
	D3D11_BLEND_DESC blendDesc{};
	blendDesc.AlphaToCoverageEnable = FALSE;
	blendDesc.IndependentBlendEnable = FALSE;
	blendDesc.RenderTarget[0].BlendEnable = FALSE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	m_Device->CreateBlendState(&blendDesc, &m_BlendState);//Opaque

	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	m_Device->CreateBlendState(&blendDesc, &m_BlendStateAlphaBlend);//Translucent

	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	m_Device->CreateBlendState(&blendDesc, &m_BlendStateAdd);

	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	m_DeviceContext->OMSetBlendState(m_BlendState, blendFactor, 0xffffffff);

	// Depth Stencil
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc{};
	depthStencilDesc.DepthEnable = TRUE;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilDesc.StencilEnable = FALSE;

	m_Device->CreateDepthStencilState(&depthStencilDesc, &m_DepthStateEnable);//Depth On (Opaque)

	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	m_Device->CreateDepthStencilState(&depthStencilDesc, &m_DepthWriteMaskDisable);//Depth No Write (Translucent)

	depthStencilDesc.DepthEnable = FALSE;
	m_Device->CreateDepthStencilState(&depthStencilDesc, &m_DepthStateDisable);//Depth Off

	m_DeviceContext->OMSetDepthStencilState(m_DepthStateEnable, NULL);


	// Sampler
	D3D11_SAMPLER_DESC samplerDesc{};
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MaxAnisotropy = 4;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	ID3D11SamplerState* samplerState{};
	m_Device->CreateSamplerState(&samplerDesc, &samplerState);

	m_DeviceContext->PSSetSamplers(0, 1, &samplerState);

	// Buffer
	D3D11_BUFFER_DESC bufferDesc{};
	bufferDesc.ByteWidth = sizeof(XMFLOAT4X4);
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = sizeof(float);

	m_Device->CreateBuffer(&bufferDesc, NULL, &m_WorldBuffer);
	m_DeviceContext->VSSetConstantBuffers(0, 1, &m_WorldBuffer);

	m_Device->CreateBuffer(&bufferDesc, NULL, &m_ViewBuffer);
	m_DeviceContext->VSSetConstantBuffers(1, 1, &m_ViewBuffer);

	m_Device->CreateBuffer(&bufferDesc, NULL, &m_ProjectionBuffer);
	m_DeviceContext->VSSetConstantBuffers(2, 1, &m_ProjectionBuffer);

	m_Device->CreateBuffer(&bufferDesc, NULL, &m_InvViewProjBuffer);
	m_DeviceContext->VSSetConstantBuffers(3, 1, &m_InvViewProjBuffer);

	m_Device->CreateBuffer(&bufferDesc, NULL, &m_CameraBuffer);
	m_DeviceContext->PSSetConstantBuffers(4, 1, &m_CameraBuffer);

	bufferDesc.ByteWidth = sizeof(LightBufferData);
	m_Device->CreateBuffer(&bufferDesc, NULL, &m_LightBuffer);
	m_DeviceContext->PSSetConstantBuffers(5, 1, &m_LightBuffer);

	bufferDesc.ByteWidth = sizeof(MATERIAL);
	m_Device->CreateBuffer(&bufferDesc, NULL, &m_MaterialBuffer);
	m_DeviceContext->VSSetConstantBuffers(6, 1, &m_MaterialBuffer);
	m_DeviceContext->PSSetConstantBuffers(6, 1, &m_MaterialBuffer);


	bufferDesc.ByteWidth = sizeof(XMFLOAT4);


	GlobalLightsManager::Init();

	// Material
	MATERIAL material{};
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	material.Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	material.TextureEnable = true;
	SetMaterial(material);

	ShadowMap::Init();
}



void Renderer::Uninit()
{
	ShadowMap::Uninit();


	if (m_SceneSRV) { m_SceneSRV->Release(); m_SceneSRV = nullptr; }
	if (m_SceneRTV) { m_SceneRTV->Release(); m_SceneRTV = nullptr; }
	if (m_SceneTexture) { m_SceneTexture->Release(); m_SceneTexture = nullptr; }

	if (m_WorldBuffer)			m_WorldBuffer->Release();
	if (m_ViewBuffer)			m_ViewBuffer->Release();
	if (m_ProjectionBuffer)		m_ProjectionBuffer->Release();
	if (m_LightBuffer)			m_LightBuffer->Release();
	if (m_MaterialBuffer)		m_MaterialBuffer->Release();

	if (m_DeviceContext)			m_DeviceContext->ClearState();
	if (m_RenderTargetView)		m_RenderTargetView->Release();
	if (m_SwapChain)			m_SwapChain->Release();
	if (m_DeviceContext)		m_DeviceContext->Release();
	if (m_Device)				m_Device->Release();

}

void Renderer::Begin()
{
	float clearColor[4] = { 0.0f, 0.3f, 0.8f, 1.0f };
	m_DeviceContext->ClearRenderTargetView(m_RenderTargetView, clearColor);
	m_DeviceContext->ClearDepthStencilView(m_DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}



void Renderer::End()
{
	m_SwapChain->Present(1, 0);
}


// --- Depth Stencil State ---
void Renderer::SetDepthEnable(bool Enable)
{
	if (Enable)
		m_DeviceContext->OMSetDepthStencilState(m_DepthStateEnable, NULL);
	else
		m_DeviceContext->OMSetDepthStencilState(m_DepthStateDisable, NULL);

}

void Renderer::SetDepthWriteMaskEnable(bool Enable)
{
	if (Enable)
		m_DeviceContext->OMSetDepthStencilState(m_DepthStateEnable, NULL);
	else
		m_DeviceContext->OMSetDepthStencilState(m_DepthWriteMaskDisable, NULL);
}

// --- Blend State ---
void Renderer::SetAlphaBlendEnable(bool Enable)
{
	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	if (Enable)
		m_DeviceContext->OMSetBlendState(m_BlendStateAlphaBlend, blendFactor, 0xffffffff);
	else
		m_DeviceContext->OMSetBlendState(m_BlendState, blendFactor, 0xffffffff);

}

void Renderer::SetAddBlendEnable(bool Enable)
{
	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	if (Enable)
		m_DeviceContext->OMSetBlendState(m_BlendStateAdd, blendFactor, 0xffffffff);
	else
		m_DeviceContext->OMSetBlendState(m_BlendState, blendFactor, 0xffffffff);

}

// --- Rasterizer State ---
void Renderer::SetCullMode(D3D11_CULL_MODE CullMode)
{
	if (CullMode == D3D11_CULL_BACK)
		m_DeviceContext->RSSetState(m_RasterizerStateCullBack);
	else if (CullMode == D3D11_CULL_NONE)
		m_DeviceContext->RSSetState(m_RasterizerStateCullNone);
}



void Renderer::SetWorldViewProjection2D()
{
	SetWorldMatrix(XMMatrixIdentity());
	SetViewMatrix(XMMatrixIdentity());

	XMMATRIX projection;
	projection = XMMatrixOrthographicOffCenterLH(0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f);
	SetProjectionMatrix(projection);
}

void Renderer::SetWorldMatrix(XMMATRIX WorldMatrix)
{
	XMFLOAT4X4 worldf;
	XMStoreFloat4x4(&worldf, XMMatrixTranspose(WorldMatrix));
	m_DeviceContext->UpdateSubresource(m_WorldBuffer, 0, NULL, &worldf, 0, 0);
}

void Renderer::SetViewMatrix(XMMATRIX ViewMatrix)
{
	XMFLOAT4X4 viewf;
	XMStoreFloat4x4(&viewf, XMMatrixTranspose(ViewMatrix));
	m_DeviceContext->UpdateSubresource(m_ViewBuffer, 0, NULL, &viewf, 0, 0);
}

void Renderer::SetProjectionMatrix(XMMATRIX ProjectionMatrix)
{
	XMFLOAT4X4 projectionf;
	XMStoreFloat4x4(&projectionf, XMMatrixTranspose(ProjectionMatrix));
	m_DeviceContext->UpdateSubresource(m_ProjectionBuffer, 0, NULL, &projectionf, 0, 0);
}

void Renderer::SetInvViewProjMatrix(XMMATRIX InvViewProjMatrix)
{
	XMFLOAT4X4 invViewProjf;
	XMStoreFloat4x4(&invViewProjf, XMMatrixTranspose(InvViewProjMatrix));
	m_DeviceContext->UpdateSubresource(m_InvViewProjBuffer, 0, NULL, &invViewProjf, 0, 0);
}

void Renderer::SetMaterial(MATERIAL Material)
{
	m_DeviceContext->UpdateSubresource(m_MaterialBuffer, 0, NULL, &Material, 0, 0);
}

void Renderer::SetLight(LIGHT Light)
{
	m_DeviceContext->UpdateSubresource(m_LightBuffer, 0, NULL, &Light, 0, 0);
}

void Renderer::SetLights(const LIGHT* pLights, int count)
{
	if (count > MAX_GLOBAL_LIGHTS) count = MAX_GLOBAL_LIGHTS;
	m_DeviceContext->UpdateSubresource(m_LightBuffer, 0, NULL, pLights, 0, 0);
}

void Renderer::SetLights(const std::vector<LIGHT>& lights)
{
	LightBufferData bufferData = {};
	int count = static_cast<int>(lights.size());
	if (count > MAX_GLOBAL_LIGHTS) count = MAX_GLOBAL_LIGHTS;

	for (int i = 0; i < count; ++i)
	{
		bufferData.Lights[i] = lights[i];

		// Shadow Map 
		if (lights[i].CastsShadow)
		{
			UINT arraySize = (lights[i].Type == LIGHT_POINT) ? 6 : 1;
			ID3D11Texture2D* tex = ShadowMap::GetShadowMapTexture(i);

			bool needRecreate = false;
			if (!tex) needRecreate = true;
			else
			{
				D3D11_TEXTURE2D_DESC desc;
				tex->GetDesc(&desc);
				if (desc.ArraySize != arraySize) needRecreate = true;
			}

			if (needRecreate)
			{
				ShadowMap::CreateShadowMapForLight(i, lights[i].Type);
			}
		}
		else
		{
			ShadowMap::ReleaseShadowMap(i);
		}
	}

	bufferData.LightCount = count;
	m_DeviceContext->UpdateSubresource(m_LightBuffer, 0, nullptr, &bufferData, 0, 0);
}

void Renderer::SetCameraPosition(XMFLOAT3 CameraPosition)
{
	XMFLOAT4	temp = XMFLOAT4(CameraPosition.x, CameraPosition.y, CameraPosition.z, 0.0f);
	GetDeviceContext()->UpdateSubresource(m_CameraBuffer, 0, NULL, &temp, 0, 0);
}

void Renderer::CreateVertexShader(ID3D11VertexShader** VertexShader, ID3D11InputLayout** VertexLayout, const char* FileName)
{

	FILE* file;
	long int fsize;

	file = fopen(FileName, "rb");
	assert(file);

	fsize = _filelength(_fileno(file));
	unsigned char* buffer = new unsigned char[fsize];
	fread(buffer, fsize, 1, file);
	fclose(file);

	m_Device->CreateVertexShader(buffer, fsize, NULL, VertexShader);


	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 4 * 3, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 4 * 6, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 4 * 10, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BINORMAL",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 44,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = ARRAYSIZE(layout);

	m_Device->CreateInputLayout(layout,
		numElements,
		buffer,
		fsize,
		VertexLayout);

	delete[] buffer;
}

void Renderer::CreatePixelShader(ID3D11PixelShader** PixelShader, const char* FileName)
{
	FILE* file;
	long int fsize;

	file = fopen(FileName, "rb");
	assert(file);

	fsize = _filelength(_fileno(file));
	unsigned char* buffer = new unsigned char[fsize];
	fread(buffer, fsize, 1, file);
	fclose(file);

	m_Device->CreatePixelShader(buffer, fsize, NULL, PixelShader);

	delete[] buffer;
}
