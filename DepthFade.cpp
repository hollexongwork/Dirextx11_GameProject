#include "main.h"
#include "Renderer.h"
#include "DepthFade.h"
#include "Input.h"
#include "Functions.h"

void DepthFade::Init()
{
    VERTEX_3D vertex[4];

    vertex[0].Position = XMFLOAT3(-0.5f, -0.5f, 0.0f);
    vertex[0].Normal = XMFLOAT3(0.0f, 0.0f, 0.0f);
    vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    vertex[0].TexCoord = XMFLOAT2(0.0f, 0.0f);

    vertex[1].Position = XMFLOAT3(0.5f, -0.5f, 0.0f);
    vertex[1].Normal = XMFLOAT3(0.0f, 0.0f, 0.0f);
    vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    vertex[1].TexCoord = XMFLOAT2(1.0f, 0.0f);

    vertex[2].Position = XMFLOAT3(-0.5f, 0.5f, 0.0f);
    vertex[2].Normal = XMFLOAT3(0.0f, 0.0f, 0.0f);
    vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    vertex[2].TexCoord = XMFLOAT2(0.0f, 1.0f);

    vertex[3].Position = XMFLOAT3(0.5f, 0.5f, 0.0f);
    vertex[3].Normal = XMFLOAT3(0.0f, 0.0f, 0.0f);
    vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    vertex[3].TexCoord = XMFLOAT2(1.0f, 1.0f);

    D3D11_BUFFER_DESC bd{};
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.ByteWidth = sizeof(VERTEX_3D) * 4;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    D3D11_SUBRESOURCE_DATA sd{};
    sd.pSysMem = vertex;
    Renderer::GetDevice()->CreateBuffer(&bd, &sd, &m_VertexBuffer);

    // Shaders
    Renderer::CreateVertexShader(&m_VertexShader, &m_VertexLayout, "shader/cso/unlitTextureVS.cso");
    Renderer::CreatePixelShader(&m_DepthPS, "shader/cso/depthVisualizePS.cso");  
    Renderer::CreatePixelShader(&m_FadePS, "shader/cso/depthFadePS.cso");  

    // Create texture, RTV, SRV for fade pass
    D3D11_TEXTURE2D_DESC texDesc{};
    texDesc.Width = SCREEN_WIDTH;
    texDesc.Height = SCREEN_HEIGHT;
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 1;
    texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    texDesc.SampleDesc.Count = 1;
    texDesc.SampleDesc.Quality = 0;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    texDesc.CPUAccessFlags = 0;
    texDesc.MiscFlags = 0;

    Renderer::GetDevice()->CreateTexture2D(&texDesc, nullptr, &m_FadeTexture);
    Renderer::GetDevice()->CreateRenderTargetView(m_FadeTexture, nullptr, &m_FadeRTV);
    Renderer::GetDevice()->CreateShaderResourceView(m_FadeTexture, nullptr, &m_FadeSRV);

    // Parameters buffer
    D3D11_BUFFER_DESC bufferDesc{};
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bufferDesc.CPUAccessFlags = 0;
    bufferDesc.MiscFlags = 0;
    bufferDesc.StructureByteStride = sizeof(float);

    bufferDesc.ByteWidth = sizeof(DepthFadeParameters);
    Renderer::GetDevice()->CreateBuffer(&bufferDesc, nullptr, &m_DepthFadeParametersBuffer);
}

void DepthFade::Uninit()
{
    if (m_FadeTexture) { m_FadeTexture->Release(); }
    if (m_FadeRTV) { m_FadeRTV->Release(); }
    if (m_FadeSRV) { m_FadeSRV->Release(); }

    if (m_VertexBuffer) { m_VertexBuffer->Release(); }
    if (m_VertexLayout) { m_VertexLayout->Release(); }
    if (m_VertexShader) { m_VertexShader->Release(); }
    if (m_DepthPS) { m_DepthPS->Release(); }
    if (m_FadePS) { m_FadePS->Release(); }
    if (m_Sampler) { m_Sampler->Release(); }
    if (m_DepthFadeParametersBuffer) { m_DepthFadeParametersBuffer->Release(); }
}

void DepthFade::Update()
{
    // Update parameters if needed, e.g., from input
    if (Input::GetKeyPress('6')) m_DebugPass = DebugPass::Scene;
    if (Input::GetKeyPress('7')) m_DebugPass = DebugPass::Depth;
    if (Input::GetKeyPress('8')) m_DebugPass = DebugPass::Faded;

    Renderer::GetDeviceContext()->UpdateSubresource(m_DepthFadeParametersBuffer, 0, nullptr, &m_Params, 0, 0);
}

void DepthFade::Draw()
{
    Renderer::GetDeviceContext()->PSSetConstantBuffers(1, 1, &m_DepthFadeParametersBuffer);

    ID3D11DeviceContext* context = Renderer::GetDeviceContext();
    ID3D11RenderTargetView* backRTV = Renderer::GetRenderTargetView();
    ID3D11RenderTargetView* renderRTV = Renderer::GetRenderRTV();
    // Common settings
    context->VSSetShader(m_VertexShader, nullptr, 0);
    context->IASetInputLayout(m_VertexLayout);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    UINT stride = sizeof(VERTEX_3D);
    UINT offset = 0;
    context->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);

    MATERIAL material{};
    material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    material.TextureEnable = true;
    Renderer::SetMaterial(material);

    D3D11_VIEWPORT fullVP{};
    fullVP.Width = static_cast<float>(SCREEN_WIDTH);
    fullVP.Height = static_cast<float>(SCREEN_HEIGHT);
    fullVP.MinDepth = 0.0f;
    fullVP.MaxDepth = 1.0f;
    fullVP.TopLeftX = 0.0f;
    fullVP.TopLeftY = 0.0f;

    XMMATRIX fullWorld = XMMatrixScaling(static_cast<float>(SCREEN_WIDTH), static_cast<float>(SCREEN_HEIGHT), 1.0f) *
        XMMatrixTranslation(static_cast<float>(SCREEN_WIDTH) / 2.0f, static_cast<float>(SCREEN_HEIGHT) / 2.0f, 0.0f);

    ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
    ID3D11ShaderResourceView* nullSRVs[2] = { nullptr, nullptr };

    float clearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

    ID3D11PixelShader* debugPS = nullptr;
    Renderer::CreatePixelShader(&debugPS, "shader/cso/unlitTexturePS.cso");

    if (m_DebugPass == DebugPass::Scene)
    {
        context->OMSetRenderTargets(1, &backRTV, nullptr);
        context->RSSetViewports(1, &fullVP);
        Renderer::SetWorldViewProjection2D();
        Renderer::SetWorldMatrix(fullWorld);
        context->PSSetShader(debugPS, nullptr, 0);
        ID3D11ShaderResourceView* sceneSRV = Renderer::GetSceneSRV();
        context->PSSetShaderResources(0, 1, &sceneSRV);
        context->Draw(4, 0);
        context->PSSetShaderResources(0, 1, nullSRV);
    }

    else if (m_DebugPass == DebugPass::Depth)
    {
        // Visualize depth
        context->OMSetRenderTargets(1, &renderRTV, nullptr);
        Renderer::SetWorldViewProjection2D();
        Renderer::SetWorldMatrix(fullWorld);
        context->PSSetShader(m_DepthPS, nullptr, 0);
        ID3D11ShaderResourceView* depthSRV = Renderer::GetDepthSRV();
        context->PSSetShaderResources(0, 1, &depthSRV);
        context->Draw(4, 0);
        context->PSSetShaderResources(0, 1, nullSRV);
    }

    else if (m_DebugPass == DebugPass::Faded)
    {
        // Apply depth fade (simulate soft particle fade on scene)
        // First, render fade pass
        context->OMSetRenderTargets(1, &m_FadeRTV, nullptr);
        context->ClearRenderTargetView(m_FadeRTV, clearColor);
        Renderer::SetWorldViewProjection2D();
        Renderer::SetWorldMatrix(fullWorld);
        context->PSSetShader(m_FadePS, nullptr, 0);
        ID3D11ShaderResourceView* fadeSRVs[2] = { Renderer::GetSceneSRV(), Renderer::GetDepthSRV() };
        context->PSSetShaderResources(0, 2, fadeSRVs);
        context->Draw(4, 0);
        context->PSSetShaderResources(0, 2, nullSRVs);

        // Then, render to back buffer
        context->OMSetRenderTargets(1, &backRTV, nullptr);
        Renderer::SetWorldViewProjection2D();
        Renderer::SetWorldMatrix(fullWorld);
        context->PSSetShader(m_DepthPS, nullptr, 0);  // Passthrough or combine
        context->PSSetShaderResources(0, 1, &m_FadeSRV);
        context->Draw(4, 0);
        context->PSSetShaderResources(0, 1, nullSRV);
    }
}
