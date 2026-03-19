#include "main.h"
#include "Renderer.h"
#include "Bloom.h"
#include "Input.h"
#include "Functions.h"


void Bloom::Init()
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

    // shaders
    Renderer::CreateVertexShader(&m_VertexShader, &m_VertexLayout, "shader/cso/unlitTextureVS.cso");
    Renderer::CreatePixelShader(&m_ExtractPS, "shader/cso/brightExtractPS.cso");
    Renderer::CreatePixelShader(&m_BlurHPS, "shader/cso/gaussianHorizPS.cso");
    Renderer::CreatePixelShader(&m_BlurVPS, "shader/cso/gaussianVertPS.cso");
    Renderer::CreatePixelShader(&m_CombinePS, "shader/cso/bloomCombinePS.cso");

    // Create textures, RTVs, SRVs for bloom passes
    D3D11_TEXTURE2D_DESC texDesc{};
    texDesc.Width = SCREEN_WIDTH;
    texDesc.Height = SCREEN_HEIGHT;
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 1;
    texDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
    texDesc.SampleDesc.Count = 1;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

    // Bright texture
    Renderer::GetDevice()->CreateTexture2D(&texDesc, nullptr, &m_BrightTexture);
    Renderer::GetDevice()->CreateRenderTargetView(m_BrightTexture, nullptr, &m_BrightRTV);
    Renderer::GetDevice()->CreateShaderResourceView(m_BrightTexture, nullptr, &m_BrightSRV);

    // Blur texture 1
    Renderer::GetDevice()->CreateTexture2D(&texDesc, nullptr, &m_BlurTexture1);
    Renderer::GetDevice()->CreateRenderTargetView(m_BlurTexture1, nullptr, &m_BlurRTV1);
    Renderer::GetDevice()->CreateShaderResourceView(m_BlurTexture1, nullptr, &m_BlurSRV1);

    // Blur texture 2
    Renderer::GetDevice()->CreateTexture2D(&texDesc, nullptr, &m_BlurTexture2);
    Renderer::GetDevice()->CreateRenderTargetView(m_BlurTexture2, nullptr, &m_BlurRTV2);
    Renderer::GetDevice()->CreateShaderResourceView(m_BlurTexture2, nullptr, &m_BlurSRV2);

    //Parameters buffer
    D3D11_BUFFER_DESC bufferDesc{};
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bufferDesc.CPUAccessFlags = 0;
    bufferDesc.MiscFlags = 0;
    bufferDesc.StructureByteStride = sizeof(float);

    bufferDesc.ByteWidth = sizeof(BlurParameters);
    Renderer::GetDevice()->CreateBuffer(&bufferDesc, nullptr, &m_BlurParametersBuffer);

    bufferDesc.ByteWidth = sizeof(BloomParameters);
    Renderer::GetDevice()->CreateBuffer(&bufferDesc, nullptr, &m_BloomParametersBuffer);
}

void Bloom::Uninit()
{
    if (m_BrightTexture) { m_BrightTexture->Release(); }
    if (m_BrightRTV) { m_BrightRTV->Release(); }
    if (m_BrightSRV) { m_BrightSRV->Release(); }

    if (m_BlurTexture1) { m_BlurTexture1->Release(); }
    if (m_BlurRTV1) { m_BlurRTV1->Release(); }
    if (m_BlurSRV1) { m_BlurSRV1->Release(); }

    if (m_BlurTexture2) { m_BlurTexture2->Release(); }
    if (m_BlurRTV2) { m_BlurRTV2->Release(); }
    if (m_BlurSRV2) { m_BlurSRV2->Release(); }

    if (m_VertexBuffer) { m_VertexBuffer->Release(); }
    if (m_VertexLayout) { m_VertexLayout->Release(); }
    if (m_VertexShader) { m_VertexShader->Release(); }
    if (m_ExtractPS) { m_ExtractPS->Release(); }
    if (m_BlurHPS) { m_BlurHPS->Release(); }
    if (m_BlurVPS) { m_BlurVPS->Release(); }
    if (m_CombinePS) { m_CombinePS->Release(); }
    if (m_Sampler) { m_Sampler->Release(); }
}

void Bloom::Update()
{
    if (Input::GetKeyPress('1'))m_DebugPass = DebugPass::Bright;
    if (Input::GetKeyPress('2'))m_DebugPass = DebugPass::BlurH;
    if (Input::GetKeyPress('3'))m_DebugPass = DebugPass::BlurV;
    if (Input::GetKeyPress('4'))m_DebugPass = DebugPass::Combined;
    if (Input::GetKeyPress('5'))m_DebugPass = DebugPass::Scene;

    // Update bloom parameters buffer
    std::vector<float> weights = ComputeGaussianWeights(m_BlurParams.radius, m_BlurParams.sigma);
    for (int i = 0; i < weights.size() && i < 64; i++)
    {
        m_BlurParams.weights[i] = weights[i];
    }

    Renderer::GetDeviceContext()->UpdateSubresource(m_BlurParametersBuffer, 0, nullptr, &m_BlurParams, 0, 0);
    Renderer::GetDeviceContext()->UpdateSubresource(m_BloomParametersBuffer, 0, nullptr, &m_BloomParams, 0, 0);
}

void Bloom::Draw()
{
    Renderer::GetDeviceContext()->PSSetConstantBuffers(7, 1, &m_BlurParametersBuffer);
    Renderer::GetDeviceContext()->PSSetConstantBuffers(8, 1, &m_BloomParametersBuffer);

    ID3D11DeviceContext* context = Renderer::GetDeviceContext();

    ID3D11RenderTargetView* backRTV = Renderer::GetRenderTargetView();

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

    // Small viewport for bloom textures
    D3D11_VIEWPORT smallVP{};
    smallVP.Width = static_cast<float>(SCREEN_WIDTH);
    smallVP.Height = static_cast<float>(SCREEN_HEIGHT);
    smallVP.MinDepth = 0.0f;
    smallVP.MaxDepth = 1.0f;
    smallVP.TopLeftX = 0.0f;
    smallVP.TopLeftY = 0.0f;

    // Full viewport for combine
    D3D11_VIEWPORT fullVP{};
    fullVP.Width = static_cast<float>(SCREEN_WIDTH);
    fullVP.Height = static_cast<float>(SCREEN_HEIGHT);
    fullVP.MinDepth = 0.0f;
    fullVP.MaxDepth = 1.0f;
    fullVP.TopLeftX = 0.0f;
    fullVP.TopLeftY = 0.0f;

    // Small ortho projection
    XMMATRIX smallView = XMMatrixIdentity();
    XMMATRIX smallProj = XMMatrixOrthographicOffCenterLH(0.0f, static_cast<float>(SCREEN_WIDTH), static_cast<float>(SCREEN_HEIGHT), 0.0f, 0.0f, 1.0f);
    Renderer::SetViewMatrix(smallView);
    Renderer::SetProjectionMatrix(smallProj);

    XMMATRIX smallWorld = XMMatrixScaling(static_cast<float>(SCREEN_WIDTH), static_cast<float>(SCREEN_HEIGHT), 1.0f) *
        XMMatrixTranslation(static_cast<float>(SCREEN_WIDTH) / 2.0f, static_cast<float>(SCREEN_HEIGHT) / 2.0f, 0.0f);

    // Full projection for final render
    XMMATRIX fullWorld = XMMatrixScaling(static_cast<float>(SCREEN_WIDTH), static_cast<float>(SCREEN_HEIGHT), 1.0f) *
        XMMatrixTranslation(static_cast<float>(SCREEN_WIDTH) / 2.0f, static_cast<float>(SCREEN_HEIGHT) / 2.0f, 0.0f);

    float clearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
    ID3D11ShaderResourceView* nullSRVs[2] = { nullptr, nullptr };

    // Simple pixel shader for debug rendering
    ID3D11PixelShader* debugPS = nullptr;
    Renderer::CreatePixelShader(&debugPS, "shader/cso/unlitTexturePS.cso");

    // Perform passes based on debug mode
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
    else if (m_DebugPass == DebugPass::Bright)
    {
        // Bright pass
        context->OMSetRenderTargets(1, &m_BrightRTV, nullptr);
        context->ClearRenderTargetView(m_BrightRTV, clearColor);
        context->RSSetViewports(1, &smallVP);
        Renderer::SetViewMatrix(smallView);
        Renderer::SetProjectionMatrix(smallProj);
        Renderer::SetWorldMatrix(smallWorld);
        context->PSSetShader(m_ExtractPS, nullptr, 0);
        ID3D11ShaderResourceView* sceneSRV = Renderer::GetSceneSRV();
        context->PSSetShaderResources(0, 1, &sceneSRV);
        context->Draw(4, 0);
        context->PSSetShaderResources(0, 1, nullSRV);

        // Render bright texture to screen
        context->OMSetRenderTargets(1, &backRTV, nullptr);
        context->RSSetViewports(1, &fullVP);
        Renderer::SetWorldViewProjection2D();
        Renderer::SetWorldMatrix(fullWorld);
        context->PSSetShader(debugPS, nullptr, 0);
        context->PSSetShaderResources(0, 1, &m_BrightSRV);
        context->Draw(4, 0);
        context->PSSetShaderResources(0, 1, nullSRV);
    }
    else if (m_DebugPass == DebugPass::BlurH)
    {
        // Bright pass
        context->OMSetRenderTargets(1, &m_BrightRTV, nullptr);
        context->ClearRenderTargetView(m_BrightRTV, clearColor);
        context->RSSetViewports(1, &smallVP);
        Renderer::SetViewMatrix(smallView);
        Renderer::SetProjectionMatrix(smallProj);
        Renderer::SetWorldMatrix(smallWorld);
        context->PSSetShader(m_ExtractPS, nullptr, 0);
        ID3D11ShaderResourceView* sceneSRV = Renderer::GetSceneSRV();
        context->PSSetShaderResources(0, 1, &sceneSRV);
        context->Draw(4, 0);
        context->PSSetShaderResources(0, 1, nullSRV);

        // Horizontal blur pass
        context->OMSetRenderTargets(1, &m_BlurRTV1, nullptr);
        context->ClearRenderTargetView(m_BlurRTV1, clearColor);
        context->RSSetViewports(1, &smallVP);
        Renderer::SetWorldMatrix(smallWorld);
        context->PSSetShader(m_BlurHPS, nullptr, 0);
        context->PSSetShaderResources(0, 1, &m_BrightSRV);
        context->Draw(4, 0);
        context->PSSetShaderResources(0, 1, nullSRV);

        // Render horizontal blur to screen
        context->OMSetRenderTargets(1, &backRTV, nullptr);
        context->RSSetViewports(1, &fullVP);
        Renderer::SetWorldViewProjection2D();
        Renderer::SetWorldMatrix(fullWorld);
        context->PSSetShader(debugPS, nullptr, 0);
        context->PSSetShaderResources(0, 1, &m_BlurSRV1);
        context->Draw(4, 0);
        context->PSSetShaderResources(0, 1, nullSRV);
    }
    else if (m_DebugPass == DebugPass::BlurV)
    {
        // Bright pass
        context->OMSetRenderTargets(1, &m_BrightRTV, nullptr);
        context->ClearRenderTargetView(m_BrightRTV, clearColor);
        context->RSSetViewports(1, &smallVP);
        Renderer::SetViewMatrix(smallView);
        Renderer::SetProjectionMatrix(smallProj);
        Renderer::SetWorldMatrix(smallWorld);
        context->PSSetShader(m_ExtractPS, nullptr, 0);
        ID3D11ShaderResourceView* sceneSRV = Renderer::GetSceneSRV();
        context->PSSetShaderResources(0, 1, &sceneSRV);
        context->Draw(4, 0);
        context->PSSetShaderResources(0, 1, nullSRV);

        // Horizontal blur pass
        context->OMSetRenderTargets(1, &m_BlurRTV1, nullptr);
        context->ClearRenderTargetView(m_BlurRTV1, clearColor);
        context->RSSetViewports(1, &smallVP);
        Renderer::SetWorldMatrix(smallWorld);
        context->PSSetShader(m_BlurHPS, nullptr, 0);
        context->PSSetShaderResources(0, 1, &m_BrightSRV);
        context->Draw(4, 0);
        context->PSSetShaderResources(0, 1, nullSRV);

        // Vertical blur pass
        context->OMSetRenderTargets(1, &m_BlurRTV2, nullptr);
        context->ClearRenderTargetView(m_BlurRTV2, clearColor);
        context->RSSetViewports(1, &smallVP);
        Renderer::SetWorldMatrix(smallWorld);
        context->PSSetShader(m_BlurVPS, nullptr, 0);
        context->PSSetShaderResources(0, 1, &m_BlurSRV1);
        context->Draw(4, 0);
        context->PSSetShaderResources(0, 1, nullSRV);

        // Render vertical blur to screen
        context->OMSetRenderTargets(1, &backRTV, nullptr);
        context->RSSetViewports(1, &fullVP);
        Renderer::SetWorldViewProjection2D();
        Renderer::SetWorldMatrix(fullWorld);
        context->PSSetShader(debugPS, nullptr, 0);
        context->PSSetShaderResources(0, 1, &m_BlurSRV2);
        context->Draw(4, 0);
        context->PSSetShaderResources(0, 1, nullSRV);
    }
    else // DebugPass::Combined (default)
    {
        // Full bloom pipeline
        // 1. Brightness extraction pass
        context->OMSetRenderTargets(1, &m_BrightRTV, nullptr);
        context->ClearRenderTargetView(m_BrightRTV, clearColor);
        context->RSSetViewports(1, &smallVP);
        Renderer::SetViewMatrix(smallView);
        Renderer::SetProjectionMatrix(smallProj);
        Renderer::SetWorldMatrix(smallWorld);
        context->PSSetShader(m_ExtractPS, nullptr, 0);
        ID3D11ShaderResourceView* sceneSRV = Renderer::GetSceneSRV();
        context->PSSetShaderResources(0, 1, &sceneSRV);
        context->Draw(4, 0);
        context->PSSetShaderResources(0, 1, nullSRV);

        // 2. Horizontal blur pass
        context->OMSetRenderTargets(1, &m_BlurRTV1, nullptr);
        context->ClearRenderTargetView(m_BlurRTV1, clearColor);
        context->RSSetViewports(1, &smallVP);
        Renderer::SetWorldMatrix(smallWorld);
        context->PSSetShader(m_BlurHPS, nullptr, 0);
        context->PSSetShaderResources(0, 1, &m_BrightSRV);
        context->Draw(4, 0);
        context->PSSetShaderResources(0, 1, nullSRV);

        // 3. Vertical blur pass
        context->OMSetRenderTargets(1, &m_BlurRTV2, nullptr);
        context->ClearRenderTargetView(m_BlurRTV2, clearColor);
        context->RSSetViewports(1, &smallVP);
        Renderer::SetWorldMatrix(smallWorld);
        context->PSSetShader(m_BlurVPS, nullptr, 0);
        context->PSSetShaderResources(0, 1, &m_BlurSRV1);
        context->Draw(4, 0);
        context->PSSetShaderResources(0, 1, nullSRV);

        // 4. Combine pass
        context->OMSetRenderTargets(1, &backRTV, nullptr);
        context->RSSetViewports(1, &fullVP);
        Renderer::SetWorldViewProjection2D();
        Renderer::SetWorldMatrix(fullWorld);
        context->PSSetShader(m_CombinePS, nullptr, 0);
        ID3D11ShaderResourceView* combineSRVs[2] = { Renderer::GetSceneSRV(), m_BlurSRV2 };
        context->PSSetShaderResources(0, 2, combineSRVs);
        context->Draw(4, 0);
        context->PSSetShaderResources(0, 2, nullSRVs);
    }
}

std::vector<float> Bloom::ComputeGaussianWeights(int radius, float sigma)
{
    std::vector<float> weights(radius * 2 + 1);
    float sum = 0.0f;

    for (int i = -radius; i <= radius; i++)
    {
        float w = expf(-(i * i) / (2.0f * sigma * sigma));
        weights[i + radius] = w;
        sum += w;
    }

    // Normalize 
    for (int i = 0; i < weights.size(); i++)
        weights[i] /= sum;

    return weights;
}