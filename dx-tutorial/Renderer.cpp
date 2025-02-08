#include "Renderer.h"

#include "Shader.h"

#include <assert.h>

using namespace core;

Renderer::Renderer()
    : mDevice(nullptr)
    , mDeviceContext(nullptr)
    , mSwapChain(nullptr)
    , mRenderTargetView(nullptr)
    , mDepthStencilBuffer(nullptr)
    , mDepthStencilView(nullptr)
    , mDepthStencilState(nullptr)
    , mRasterizerState(nullptr)
{

}

Renderer::~Renderer()
{
    Cleanup();
}

bool Renderer::Initialize(HWND hWnd, int width, int height)
{
    if(!createDeviceAndSwapChain(hWnd, width, height))
    {
        return false;
    }

    if(!createRenderTargetView())
    {
        return false;
    }

    if(!createDepthStencilBuffer(width, height))
    {
        return false;
    }

    if(!createDepthStencilState())
    {
        return false;
    }

    if(!createRasterizerState())
    {
        return false;
    }

    mShader = new Shader();
    if(!mShader->Initialize(mDevice, L"VertexShader.hlsl", "VSMain", L"PixelShader.hlsl", "PSMain"))
    {
        return false;
    }

    mDeviceContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);

    D3D11_VIEWPORT viewport;
    viewport.Width = static_cast<FLOAT>(width);
    viewport.Height = static_cast<FLOAT>(height);
    viewport.MinDepth = 0.f;
    viewport.MaxDepth = 1.f;
    viewport.TopLeftX = 0.f;
    viewport.TopLeftY = 0.f;

    mDeviceContext->RSSetViewports(1, &viewport);
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

    if(mRasterizerState)
    {
        mRasterizerState->Release();
        mRasterizerState = nullptr;
    }

    if(mDepthStencilState)
    {
        mDepthStencilState->Release();
        mDepthStencilState = nullptr;
    }

    if(mDepthStencilView)
    {
        mDepthStencilView->Release();
        mDepthStencilView = nullptr;
    }

    if(mDepthStencilBuffer)
    {
        mDepthStencilBuffer->Release();
        mDepthStencilBuffer = nullptr;
    }

    if(mRenderTargetView)
    {
        mRenderTargetView->Release();
        mRenderTargetView = nullptr;
    }

    if(mSwapChain)
    {
        mSwapChain->Release();
        mSwapChain = nullptr;
    }

    if(mDeviceContext)
    {
        mDeviceContext->Release();
        mDeviceContext = nullptr;
    }

    if(mDevice)
    {
        mDevice->Release();
        mDevice = nullptr;
    }
}

void Renderer::BeginFrame(float red, float green, float blue, float alpha)
{
    float clearColor[4] = {red, green, blue, alpha};
    mDeviceContext->ClearRenderTargetView(mRenderTargetView, clearColor);

    mDeviceContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void Renderer::EndFrame()
{
    mSwapChain->Present(1, 0);
}

void Renderer::Draw()
{
    mShader->SetShader(mDeviceContext);
    // 실제 드로우 호출이 들어갈 자리
    // ex) m_deviceContext->IASetInputLayout( ... );
    //     m_deviceContext->IASetVertexBuffers( ... );
    //     m_deviceContext->IASetIndexBuffer( ... );
    //     m_deviceContext->DrawIndexed(...);

    // 여기서는 간단히 예시만 표시
    // "실제 게임 로직 or Mesh/Shader 호출"을 여기서 해줍니다.
}

bool Renderer::createDeviceAndSwapChain(HWND hWnd, int width, int height)
{
    DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
    swapChainDesc.BufferDesc.Width = width;
    swapChainDesc.BufferDesc.Height = height;
    swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
    swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;

    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = 1;
    swapChainDesc.OutputWindow = hWnd;
    swapChainDesc.Windowed = TRUE;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    swapChainDesc.Flags = 0;

    UINT createDeviceFlags = 0;

    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0
    };
    D3D_FEATURE_LEVEL featureLevel;

    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        createDeviceFlags,
        featureLevels,
        _countof(featureLevels),
        D3D11_SDK_VERSION,
        &swapChainDesc,
        &mSwapChain,
        &mDevice,
        &featureLevel,
        &mDeviceContext
    );

    if(FAILED(hr))
    {
        return false;
    }

    return true;
}

bool Renderer::createRenderTargetView()
{
    ID3D11Texture2D* backBuffer = nullptr;
    HRESULT hr = mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
    if(FAILED(hr))
    {
        return false;
    }

    hr = mDevice->CreateRenderTargetView(backBuffer, nullptr, &mRenderTargetView);
    backBuffer->Release();

    if(FAILED(hr))
    {
        return false;
    }

    return true;
}

bool Renderer::createDepthStencilBuffer(int width, int height)
{
    D3D11_TEXTURE2D_DESC depthBufferDesc = {};
    depthBufferDesc.Width = width;
    depthBufferDesc.Height = height;
    depthBufferDesc.MipLevels = 1;
    depthBufferDesc.ArraySize = 1;
    depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthBufferDesc.SampleDesc.Count = 1;
    depthBufferDesc.SampleDesc.Quality = 0;
    depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthBufferDesc.CPUAccessFlags = 0;
    depthBufferDesc.MiscFlags = 0;

    HRESULT hr = mDevice->CreateTexture2D(&depthBufferDesc, nullptr, &mDepthStencilBuffer);
    if(FAILED(hr))
    {
        return false;
    }

    D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
    dsvDesc.Format = depthBufferDesc.Format;
    dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Texture2D.MipSlice = 0;

    hr = mDevice->CreateDepthStencilView(mDepthStencilBuffer, &dsvDesc, &mDepthStencilView);
    if(FAILED(hr))
    {
        return false;
    }

    return true;
}

bool Renderer::createDepthStencilState()
{
    D3D11_DEPTH_STENCIL_DESC dsDesc = {};
    dsDesc.DepthEnable = TRUE;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS;

    dsDesc.StencilEnable = FALSE;

    HRESULT hr = mDevice->CreateDepthStencilState(&dsDesc, &mDepthStencilState);
    if(FAILED(hr))
    {
        return false;
    }

    mDeviceContext->OMSetDepthStencilState(mDepthStencilState, 1);

    return true;
}

bool Renderer::createRasterizerState()
{
    D3D11_RASTERIZER_DESC rasterDesc = {};
    rasterDesc.FillMode = D3D11_FILL_SOLID;
    rasterDesc.CullMode = D3D11_CULL_BACK;
    rasterDesc.FrontCounterClockwise = FALSE;
    rasterDesc.DepthBias = 0;
    rasterDesc.SlopeScaledDepthBias = 0.0f;
    rasterDesc.DepthBiasClamp = 0.0f;
    rasterDesc.DepthClipEnable = TRUE;
    rasterDesc.ScissorEnable = FALSE;
    rasterDesc.MultisampleEnable = FALSE;
    rasterDesc.AntialiasedLineEnable = FALSE;

    HRESULT hr = mDevice->CreateRasterizerState(&rasterDesc, &mRasterizerState);
    if(FAILED(hr))
    {
        return false;
    }

    mDeviceContext->RSSetState(mRasterizerState);
    return true;
}
