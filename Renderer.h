#pragma once
#include "GlobalLightsManager.h"

struct VERTEX_3D
{
	XMFLOAT3 Position;
	XMFLOAT3 Normal;
	XMFLOAT4 Diffuse;
	XMFLOAT2 TexCoord;
	XMFLOAT3 Tangent; 
	XMFLOAT3 Binormal;
};

struct MATERIAL
{
	XMFLOAT4	Ambient;
	XMFLOAT4	Diffuse;
	XMFLOAT4	Specular;
	XMFLOAT4	Emission;
	float		Shininess;
	BOOL		TextureEnable;
	XMFLOAT2	UV_Offset;
};

struct LightBufferData
{
	LIGHT Lights[MAX_GLOBAL_LIGHTS];
	int LightCount;
	XMFLOAT3 Padding;
};

class Renderer
{
private:

	static D3D_FEATURE_LEVEL         m_FeatureLevel;
								     
	static ID3D11Device*             m_Device;
	static ID3D11DeviceContext*      m_DeviceContext;
	static IDXGISwapChain*           m_SwapChain;
	static ID3D11RenderTargetView*   m_RenderTargetView;
	static ID3D11DepthStencilView*   m_DepthStencilMainView;
	static ID3D11DepthStencilView*   m_DepthStencilView;
									 
	static ID3D11Buffer*			 m_WorldBuffer;
	static ID3D11Buffer*			 m_ViewBuffer;
	static ID3D11Buffer*			 m_ProjectionBuffer;
	static ID3D11Buffer*			 m_InvViewProjBuffer;
	static ID3D11Buffer*			 m_MaterialBuffer;
	static ID3D11Buffer*			 m_LightBuffer;
	static ID3D11Buffer*			 m_CameraBuffer;
									 
	static ID3D11DepthStencilState*  m_DepthStateEnable;
	static ID3D11DepthStencilState*  m_DepthStateDisable;
	static ID3D11DepthStencilState * m_DepthWriteMaskDisable;

	static ID3D11RasterizerState*    m_RasterizerStateCullNone;
	static ID3D11RasterizerState*    m_RasterizerStateCullBack;
									 
	static ID3D11BlendState*		 m_BlendState;
	static ID3D11BlendState*		 m_BlendStateAlphaBlend;
	static ID3D11BlendState*         m_BlendStateAdd;

	// Scene texture for post-processing
	static ID3D11Texture2D*          m_SceneTexture;
	static ID3D11RenderTargetView*   m_SceneRTV;
	static ID3D11ShaderResourceView* m_SceneSRV;

	static ID3D11Texture2D*          m_RenderTexture;
	static ID3D11RenderTargetView*   m_RenderRTV;
	static ID3D11ShaderResourceView* m_RenderSRV;

	static ID3D11Texture2D*          m_DepthTexture;
	static ID3D11ShaderResourceView* m_DepthSRV;

public:
	static void Init();
	static void Uninit();
	static void Begin();
	static void End();

	static void SetDepthEnable(bool Enable);
	static void SetDepthWriteMaskEnable(bool Enable);
	static void SetAlphaBlendEnable(bool Enable);
	static void SetAddBlendEnable(bool Enable);
	static void SetCullMode(D3D11_CULL_MODE CullMode);
	static void SetWorldViewProjection2D();
	static void SetWorldMatrix(XMMATRIX WorldMatrix);
	static void SetViewMatrix(XMMATRIX ViewMatrix);
	static void SetProjectionMatrix(XMMATRIX ProjectionMatrix);
	static void SetInvViewProjMatrix(XMMATRIX InvViewProjMatrix);
	static void SetMaterial(MATERIAL Material);
	static void SetLight(LIGHT Light);
	static void SetLights(const LIGHT* pLights, int count);
	static void SetLights(const std::vector<LIGHT>& lights);
	static void SetCameraPosition(XMFLOAT3 CameraPosition);

	static ID3D11Device* GetDevice( void ){ return m_Device; }
	static ID3D11DeviceContext* GetDeviceContext( void ){ return m_DeviceContext; }

	static void CreateVertexShader(ID3D11VertexShader** VertexShader, ID3D11InputLayout** VertexLayout, const char* FileName);
	static void CreatePixelShader(ID3D11PixelShader** PixelShader, const char* FileName);

	static ID3D11RenderTargetView* GetRenderTargetView() { return m_RenderTargetView; }
	static ID3D11DepthStencilView* GetDepthStencilView() { return m_DepthStencilView; }
	static ID3D11DepthStencilView* GetDepthStencilMainView() { return m_DepthStencilMainView; }

	static ID3D11Buffer* GetProjectionBuffer() { return m_ProjectionBuffer; }

	static ID3D11RenderTargetView* GetSceneRTV() { return m_SceneRTV; }
	static ID3D11ShaderResourceView* GetSceneSRV() { return m_SceneSRV; }

	static ID3D11RenderTargetView* GetRenderRTV() { return m_RenderRTV; }
	static ID3D11ShaderResourceView* GetRenderSRV() { return m_RenderSRV; }

	static ID3D11ShaderResourceView* GetDepthSRV() { return m_DepthSRV; }
};
