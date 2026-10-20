#include "Renderer.h"
#include "Mesh.h"
#include "Shader.h"
#include "Transform.h"

#include <sstream>
#include <stdexcept>
#include <string>

namespace
{
    void ThrowIfFailed(HRESULT hr, const char* operation)
    {
        if (FAILED(hr))
        {
            std::ostringstream message;
            message << operation << " failed (HRESULT 0x" << std::hex
                << static_cast<unsigned long>(hr) << ")";
            throw std::runtime_error(message.str());
        }
    }
}

using namespace dx;
using namespace std;
using namespace Microsoft::WRL;
using namespace DirectX;

Renderer::Renderer(HWND hWnd, UINT width, UINT height) : mHWnd(hWnd), mWidth(width), mHeight(height)
{
    if (!hWnd || width == 0 || height == 0)
    {
        throw invalid_argument("Renderer requires a window and nonzero dimensions");
    }

    ThrowIfFailed(createDeviceAndSwapChain(), "Create device and swap chain");
    ThrowIfFailed(createRenderTargetView(), "Create render target view");
    ThrowIfFailed(createDepthStencilBuffer(), "Create depth stencil buffer");
    ThrowIfFailed(createDepthStencilState(), "Create depth stencil state");
    ThrowIfFailed(createRasterizerState(), "Create rasterizer state");
    ThrowIfFailed(createConstantBuffer(), "Create constant buffer");
    SetViewport();
}

void Renderer::BeginFrame(XMFLOAT4 color)
{
    mDeviceContext->OMSetDepthStencilState(mDepthStencilState.Get(), 1);
    mDeviceContext->RSSetState(mRasterizerState.Get());

    UINT numViews = 1;

    mDeviceContext->OMSetRenderTargets(
        numViews,
        mRenderTargetView.GetAddressOf(),
        mDepthStencilView.Get());

    float clearColor[4] = { color.x, color.y, color.z, color.w };

    mDeviceContext->ClearRenderTargetView(
        mRenderTargetView.Get(),
        clearColor);

    float depth = 1.f;
    UINT8 stencil = 0;

    mDeviceContext->ClearDepthStencilView(
        mDepthStencilView.Get(),
        D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
        depth,
        stencil);
}

void Renderer::EndFrame()
{
    HRESULT hr = mSwapChain->Present(1, 0);
    mIsOccluded = hr == DXGI_STATUS_OCCLUDED;
    ThrowIfFailed(hr, "Present");
}

bool Renderer::TestVisibility()
{
    HRESULT hr = mSwapChain->Present(0, DXGI_PRESENT_TEST);
    mIsOccluded = hr == DXGI_STATUS_OCCLUDED;
    ThrowIfFailed(hr, "Test swap chain visibility");
    return !mIsOccluded;
}

void Renderer::Resize(UINT width, UINT height)
{
    if (width == 0 || height == 0 || (width == mWidth && height == mHeight))
    {
        return;
    }

    mDeviceContext->OMSetRenderTargets(0, nullptr, nullptr);
    mRenderTargetView.Reset();
    mDepthStencilView.Reset();
    mDepthStencilBuffer.Reset();
    mDeviceContext->Flush();

    ThrowIfFailed(mSwapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0), "Resize swap chain");
    mWidth = width;
    mHeight = height;
    ThrowIfFailed(createRenderTargetView(), "Recreate render target view");
    ThrowIfFailed(createDepthStencilBuffer(), "Recreate depth stencil buffer");
    SetViewport();
    mIsOccluded = false;
}

void Renderer::Draw(const Object& object, const Camera& camera)
{
    // 셰이더 바인딩
    const Shader* shader = object.GetComponent<Shader>();
    if (shader != nullptr)
    {
        shader->Bind(mDeviceContext.Get());
    }

    // 상수 버퍼링
    MatrixBuffer matrices{};
    matrices.World = XMMatrixIdentity();

    const Transform* transform = object.GetComponent<Transform>();
    if (transform != nullptr)
    {
        matrices.World = XMMatrixTranspose(transform->GetWorld());
    }

    matrices.View = XMMatrixTranspose(camera.GetView());
    matrices.Projection = XMMatrixTranspose(camera.GetProjection());

    mDeviceContext->UpdateSubresource(
        mConstantBuffer.Get(),
        0,
        nullptr,
        &matrices,
        0,
        0);

    mDeviceContext->VSSetConstantBuffers(
        0,
        1,
        mConstantBuffer.GetAddressOf());

    // 메쉬 바인딩
    const Mesh* mesh = object.GetComponent<Mesh>();
    if (mesh != nullptr)
    {
        mesh->Bind(mDeviceContext.Get());
        mDeviceContext->DrawIndexed(mesh->GetIndexCount(), 0, 0);
    }
}

void Renderer::SetViewport()
{
    D3D11_VIEWPORT viewport{};
    viewport.Width = static_cast<FLOAT>(mWidth);
    viewport.Height = static_cast<FLOAT>(mHeight);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    mDeviceContext->RSSetViewports(1, &viewport);
}

vector<ComPtr<IDXGIAdapter1>> Renderer::enumrateAdapters()
{
    ComPtr<IDXGIFactory1> factory;
    ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(factory.GetAddressOf())), "Create DXGI factory");

    vector<ComPtr<IDXGIAdapter1>> adapters;
    for (UINT i = 0; ; ++i)
    {
        ComPtr<IDXGIAdapter1> adapter;
        HRESULT hr = factory->EnumAdapters1(i, adapter.GetAddressOf());
        if (hr == DXGI_ERROR_NOT_FOUND)
        {
            break;
        }
        ThrowIfFailed(hr, "Enumerate DXGI adapter");
        adapters.push_back(adapter);
    }
    return adapters;
}

HRESULT Renderer::createDeviceAndSwapChain()
{
    DXGI_SWAP_CHAIN_DESC swapChainDescriptor{};
    swapChainDescriptor.BufferCount = 2;
    swapChainDescriptor.BufferDesc.Width = mWidth;
    swapChainDescriptor.BufferDesc.Height = mHeight;
    swapChainDescriptor.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDescriptor.BufferDesc.RefreshRate.Numerator = 60;
    swapChainDescriptor.BufferDesc.RefreshRate.Denominator = 1;
    swapChainDescriptor.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDescriptor.OutputWindow = mHWnd;
    swapChainDescriptor.SampleDesc.Count = 1;
    swapChainDescriptor.Windowed = TRUE;
    swapChainDescriptor.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

    ComPtr<IDXGIAdapter1> adapter;
    for (const auto& candidate : enumrateAdapters())
    {
        DXGI_ADAPTER_DESC1 description{};
        ThrowIfFailed(candidate->GetDesc1(&description), "Get adapter description");
        if ((description.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) == 0)
        {
            adapter = candidate;
            break;
        }
    }

    UINT createDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#if defined(_DEBUG) || defined(DEBUG)
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    const D3D_FEATURE_LEVEL requestedFeatureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0
    };

    auto createDevice = [&](IDXGIAdapter* selectedAdapter, D3D_DRIVER_TYPE driverType, UINT flags)
    {
        mSwapChain.Reset();
        mDeviceContext.Reset();
        mDevice.Reset();
        D3D_FEATURE_LEVEL supportedFeatureLevel{};
        HRESULT hr = D3D11CreateDeviceAndSwapChain(
            selectedAdapter, driverType, nullptr, flags,
            requestedFeatureLevels, _countof(requestedFeatureLevels), D3D11_SDK_VERSION,
            &swapChainDescriptor, mSwapChain.GetAddressOf(), mDevice.GetAddressOf(),
            &supportedFeatureLevel, mDeviceContext.GetAddressOf());
        if (hr == E_INVALIDARG)
        {
            mSwapChain.Reset();
            mDeviceContext.Reset();
            mDevice.Reset();
            hr = D3D11CreateDeviceAndSwapChain(
                selectedAdapter, driverType, nullptr, flags,
                requestedFeatureLevels + 1, _countof(requestedFeatureLevels) - 1, D3D11_SDK_VERSION,
                &swapChainDescriptor, mSwapChain.GetAddressOf(), mDevice.GetAddressOf(),
                &supportedFeatureLevel, mDeviceContext.GetAddressOf());
        }
        return hr;
    };

    auto createWithDebugFallback = [&](IDXGIAdapter* selectedAdapter, D3D_DRIVER_TYPE driverType)
    {
        HRESULT hr = createDevice(selectedAdapter, driverType, createDeviceFlags);
        if (hr == DXGI_ERROR_SDK_COMPONENT_MISSING && (createDeviceFlags & D3D11_CREATE_DEVICE_DEBUG) != 0)
        {
            OutputDebugStringA("D3D11 debug layer unavailable; retrying without debug layer.\n");
            createDeviceFlags &= ~D3D11_CREATE_DEVICE_DEBUG;
            hr = createDevice(selectedAdapter, driverType, createDeviceFlags);
        }
        return hr;
    };

    HRESULT hr = adapter
        ? createWithDebugFallback(adapter.Get(), D3D_DRIVER_TYPE_UNKNOWN)
        : createWithDebugFallback(nullptr, D3D_DRIVER_TYPE_WARP);
    if (FAILED(hr) && adapter)
    {
        hr = createWithDebugFallback(nullptr, D3D_DRIVER_TYPE_WARP);
    }
    return hr;
}

HRESULT Renderer::createRenderTargetView()
{
    ComPtr<ID3D11Texture2D> backBufferPtr;
    HRESULT hr = mSwapChain->GetBuffer(0, IID_PPV_ARGS(backBufferPtr.GetAddressOf()));
    if (FAILED(hr))
    {
        return hr;
    }

    D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDescriptor{};
    renderTargetViewDescriptor.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    renderTargetViewDescriptor.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    renderTargetViewDescriptor.Texture2D.MipSlice = 0;
    return mDevice->CreateRenderTargetView(
        backBufferPtr.Get(), &renderTargetViewDescriptor, mRenderTargetView.GetAddressOf());
}

HRESULT Renderer::createDepthStencilBuffer()
{
    // 깊이 버퍼 디스크립터 빌드
    D3D11_TEXTURE2D_DESC depthBufferDesc{};
    depthBufferDesc.Width = mWidth;
    depthBufferDesc.Height = mHeight;
    depthBufferDesc.MipLevels = 1;
    depthBufferDesc.ArraySize = 1;
    depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthBufferDesc.SampleDesc.Count = 1;
    depthBufferDesc.SampleDesc.Quality = 0;
    depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthBufferDesc.CPUAccessFlags = 0;
    depthBufferDesc.MiscFlags = 0;

    // 버퍼 기록
    D3D11_SUBRESOURCE_DATA* subresource = nullptr;
    HRESULT textureResult = mDevice->CreateTexture2D(
        &depthBufferDesc,
        subresource,
        mDepthStencilBuffer.GetAddressOf());
    if (FAILED(textureResult))
    {
        return textureResult;
    }

    // 깊이 스텐실 뷰 디스크립터 빌드
    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
    depthStencilViewDesc.Format = depthBufferDesc.Format;
    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depthStencilViewDesc.Texture2D.MipSlice = 0;

    HRESULT hr = mDevice->CreateDepthStencilView(
        mDepthStencilBuffer.Get(),
        &depthStencilViewDesc,
        mDepthStencilView.GetAddressOf());

    return hr;
}

HRESULT Renderer::createDepthStencilState()
{
    D3D11_DEPTH_STENCIL_DESC depthStencilDesc{};
    depthStencilDesc.DepthEnable = true;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
    depthStencilDesc.StencilEnable = true;
    depthStencilDesc.StencilReadMask = 0xFF;
    depthStencilDesc.StencilWriteMask = 0xFF;

    depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    HRESULT hr = mDevice->CreateDepthStencilState(&depthStencilDesc, mDepthStencilState.GetAddressOf());

    if (FAILED(hr))
    {
        return hr;
    }

    UINT stencilRef = 1;
    mDeviceContext->OMSetDepthStencilState(
        mDepthStencilState.Get(),
        stencilRef);

    return hr;
}

HRESULT Renderer::createRasterizerState()
{
    D3D11_RASTERIZER_DESC rasterDesc{};
    rasterDesc.AntialiasedLineEnable = false;
    rasterDesc.CullMode = D3D11_CULL_BACK;
    rasterDesc.DepthBias = 0;
    rasterDesc.DepthBiasClamp = 0.f;
    rasterDesc.DepthClipEnable = true;
    rasterDesc.FillMode = D3D11_FILL_SOLID;
    rasterDesc.FrontCounterClockwise = false;
    rasterDesc.MultisampleEnable = false;
    rasterDesc.ScissorEnable = false;
    rasterDesc.SlopeScaledDepthBias = 0.f;

    HRESULT hr = mDevice->CreateRasterizerState(&rasterDesc, mRasterizerState.GetAddressOf());
    if (SUCCEEDED(hr))
    {
        mDeviceContext->RSSetState(mRasterizerState.Get());
    }

    return hr;
}

HRESULT Renderer::createConstantBuffer()
{
    D3D11_BUFFER_DESC constantBufferDescriptor = {};
    constantBufferDescriptor.Usage = D3D11_USAGE_DEFAULT;
    constantBufferDescriptor.ByteWidth = sizeof(MatrixBuffer);
    constantBufferDescriptor.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

    return mDevice->CreateBuffer(
        &constantBufferDescriptor,
        nullptr,
        mConstantBuffer.GetAddressOf());
}