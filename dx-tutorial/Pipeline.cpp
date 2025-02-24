#include "Pipeline.h"
#include <cassert>

using namespace core;

Pipeline::Pipeline()
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

Pipeline::~Pipeline()
{
    Cleanup();
}

bool Pipeline::Initialize(HWND hWnd, int width, int height)
{
    if(!createDeviceAndSwapChain(hWnd, width, height))
    {
        MessageBox(nullptr, L"Device init failed", L"Error", MB_OK);
        return false;
    }
        
    if(!createRenderTargetView())
    {
        MessageBox(nullptr, L"RTV init failed", L"Error", MB_OK);
        return false;
    }
        
    if(!createDepthStencilBuffer(width, height))
    {
        MessageBox(nullptr, L"Depth stencil buf init failed", L"Error", MB_OK);
        return false;
    }
        
    if(!createDepthStencilState())
    {
        MessageBox(nullptr, L"Depth stencil state init failed", L"Error", MB_OK);
        return false;
    }
        
    if(!createRasterizerState())
    {
        MessageBox(nullptr, L"rasterizer state init failed", L"Error", MB_OK);
        return false;
    }
        
    return true;
}

void Pipeline::Cleanup()
{
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

bool Pipeline::createDeviceAndSwapChain(HWND hWnd, int width, int height)
{
    DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
    swapChainDesc.BufferCount = 1;
    swapChainDesc.BufferDesc.Width = width;
    swapChainDesc.BufferDesc.Height = height;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
    swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.OutputWindow = hWnd;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.Windowed = TRUE;

    UINT createDeviceFlags = 0;

#if defined(_DEBUG)
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3,
        D3D_FEATURE_LEVEL_9_2,
        D3D_FEATURE_LEVEL_9_1
    };

    D3D_FEATURE_LEVEL featureLevel;

    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        createDeviceFlags,
        featureLevels,
        1,
        D3D11_SDK_VERSION,
        &swapChainDesc,
        &mSwapChain,
        &mDevice,
        &featureLevel,
        &mDeviceContext
    );

    if(FAILED(hr))
    {
        MessageBox(nullptr, L"hr", L"Error", MB_OK);
        return false;
    }

    return true;
}

bool Pipeline::createRenderTargetView()
{
    ID3D11Texture2D* backBufferPtr = nullptr;
    HRESULT hr = mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBufferPtr);
    if(FAILED(hr))
    {
        return false;
    }

    hr = mDevice->CreateRenderTargetView(backBufferPtr, nullptr, &mRenderTargetView);
    backBufferPtr->Release();

    if(FAILED(hr))
    {
        return false;
    }
    return true;
}

bool Pipeline::createDepthStencilBuffer(int width, int height)
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

    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
    depthStencilViewDesc.Format = depthBufferDesc.Format;
    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depthStencilViewDesc.Texture2D.MipSlice = 0;

    hr = mDevice->CreateDepthStencilView(mDepthStencilBuffer, &depthStencilViewDesc, &mDepthStencilView);
    if(FAILED(hr))
    {
        return false;
    }

    mDeviceContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);

    return true;
}

bool Pipeline::createDepthStencilState()
{
    D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
    depthStencilDesc.DepthEnable = true;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

    depthStencilDesc.StencilEnable = true;
    depthStencilDesc.StencilReadMask = 0xFF;
    depthStencilDesc.StencilWriteMask = 0xFF;

    // 전면 패스
    depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    // 후면 패스
    depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    HRESULT hr = mDevice->CreateDepthStencilState(&depthStencilDesc, &mDepthStencilState);
    if(FAILED(hr))
    {
        return false;
    }

    mDeviceContext->OMSetDepthStencilState(mDepthStencilState, 1);

    return true;
}

bool Pipeline::createRasterizerState()
{
    D3D11_RASTERIZER_DESC rasterDesc = {};
    rasterDesc.AntialiasedLineEnable = false;
    rasterDesc.CullMode = D3D11_CULL_BACK;
    rasterDesc.DepthBias = 0;
    rasterDesc.DepthBiasClamp = 0.0f;
    rasterDesc.DepthClipEnable = true;
    rasterDesc.FillMode = D3D11_FILL_SOLID;
    rasterDesc.FrontCounterClockwise = false;
    rasterDesc.MultisampleEnable = false;
    rasterDesc.ScissorEnable = false;
    rasterDesc.SlopeScaledDepthBias = 0.0f;

    HRESULT hr = mDevice->CreateRasterizerState(&rasterDesc, &mRasterizerState);
    if(FAILED(hr))
    {
        return false;
    }

    mDeviceContext->RSSetState(mRasterizerState);

    return true;
}