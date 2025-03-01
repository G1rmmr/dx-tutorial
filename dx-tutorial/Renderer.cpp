#include "Renderer.h"
#include "Shader.h"
#include "Enemy.h"
#include "Pipeline.h"
#include "SkyBox.h"

#include <d3dcompiler.h>

using namespace core;

struct Vertex
{
    DirectX::XMFLOAT3 Pos;
    DirectX::XMFLOAT4 Color;
};

struct MatrixBuffer
{
    DirectX::XMMATRIX World;
    DirectX::XMMATRIX View;
    DirectX::XMMATRIX Proj;
};

Renderer::Renderer()
    : mPipeline(nullptr)
    , mMatrixBuffer(nullptr)
    , mInputLayout(nullptr)
    , mShader(nullptr)
    , mView(DirectX::XMMatrixIdentity())
    , mProj(DirectX::XMMatrixIdentity())
    , mSkyBox(nullptr)
{
}

Renderer::~Renderer()
{
    Cleanup();
}

bool Renderer::Initialize(HWND hWnd, int width, int height)
{
    mPipeline = new Pipeline();
    if(!mPipeline->Initialize(hWnd, width, height))
    {
        MessageBox(nullptr, L"Pipeline init failed", L"Error", MB_OK);
        return false;
    }

    ID3D11Device* device = mPipeline->GetDevice();

    mShader = new Shader();
    if(!mShader->Initialize(device, L"VertexShader.hlsl", "VSMain", L"PixelShader.hlsl", "PSMain"))
    {
        MessageBox(nullptr, L"Shader init failed", L"Error", MB_OK);
        return false;
    }

    D3D11_INPUT_ELEMENT_DESC layoutDesc[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,   0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT,0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    UINT numElements = _countof(layoutDesc);
    ID3DBlob* vsBlob = mShader->GetVSBlob();

    HRESULT hr = device->CreateInputLayout(
        layoutDesc, 
        numElements,
        vsBlob->GetBufferPointer(),
        vsBlob->GetBufferSize(),
        &mInputLayout);

    if(FAILED(hr))
    {
        MessageBox(nullptr, L"CreateInputLayout failed", L"Error", MB_OK);
        return false;
    }

    D3D11_BUFFER_DESC cbd = {};
    cbd.Usage = D3D11_USAGE_DEFAULT;
    cbd.ByteWidth = sizeof(MatrixBuffer);
    cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

    hr = device->CreateBuffer(&cbd, nullptr, &mMatrixBuffer);
    if(FAILED(hr))
    {
        return false;
    }

    ID3D11DeviceContext* context = mPipeline->GetDeviceContext();
    ID3D11RenderTargetView* rtv = mPipeline->GetRenderTargetView();
    context->OMSetRenderTargets(1, &rtv, mPipeline->GetDepthStencilView());

    D3D11_VIEWPORT viewport = {};
    viewport.Width = static_cast<FLOAT>(width);
    viewport.Height = static_cast<FLOAT>(height);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;

    context->RSSetViewports(1, &viewport);

    if(!InitSkyBox())
    {
        MessageBox(nullptr, L"SkyBox rendering init failed", L"Error", MB_OK);
        return false;
    }

    return true;
}

void Renderer::Cleanup()
{
    if(mShader)
    {
        mShader->Cleanup();
        delete mShader;
        mShader = nullptr;
    }

    if(mInputLayout)
    {
        mInputLayout->Release();
        mInputLayout = nullptr;
    }

    if(mMatrixBuffer)
    {
        mMatrixBuffer->Release();
        mMatrixBuffer = nullptr;
    }

    if(mSkyBox)
    {
        delete mSkyBox;
        mSkyBox = nullptr;
    }

    if(mPipeline)
    {
        mPipeline->Cleanup();
        delete mPipeline;
        mPipeline = nullptr;
    }
}

void Renderer::BeginFrame(float red, float green, float blue, float alpha)
{
    float clearColor[4] = {red, green, blue, alpha};

    ID3D11DeviceContext* context = mPipeline->GetDeviceContext();
    ID3D11RenderTargetView* rtv = mPipeline->GetRenderTargetView();
    ID3D11DepthStencilView* dsv = mPipeline->GetDepthStencilView();

    context->OMSetRenderTargets(1, &rtv, dsv);
    context->ClearRenderTargetView(rtv, clearColor);
    context->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void Renderer::EndFrame()
{
    mPipeline->GetSwapChain()->Present(1, 0);
}

void Renderer::Draw(std::vector<Actor*>& actors)
{
    ID3D11DeviceContext* context = mPipeline->GetDeviceContext();

    mSkyBox->Render(context, mMatrixBuffer, mView, mProj);

    // 일반 오브젝트 렌더링
    mShader->SetShader(context);
    context->IASetInputLayout(mInputLayout);

    for(auto& actor : actors)
    {
        actor->Render(context, mMatrixBuffer, mView, mProj);
    }
}

void Renderer::SetCameraMatrices(const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& proj)
{
    mView = view;
    mProj = proj;
}

bool Renderer::InitSkyBox()
{
    auto device = mPipeline->GetDevice();
    auto context = mPipeline->GetDeviceContext();

    mSkyBox = new SkyBox(device, context);
    return mSkyBox != nullptr;
}