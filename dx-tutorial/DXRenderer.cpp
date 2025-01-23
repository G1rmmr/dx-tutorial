#include "DXRenderer.h"

#include <sstream>

bool core::DXRenderer::Initialize(HWND hwnd)
{
    UINT createDeviceFlags = 0;

#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    DXGI_SWAP_CHAIN_DESC scd = {};
    scd.BufferCount = 2;
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scd.BufferDesc.Width = 1280;
    scd.BufferDesc.Height = 720;
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.OutputWindow = hwnd;
    scd.SampleDesc.Count = 1;
    scd.Windowed = TRUE;
    scd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    scd.Flags = 0;

    D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0
    };
    
    D3D_FEATURE_LEVEL selectedFeatureLevel;

    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        NULL, D3D_DRIVER_TYPE_HARDWARE,
        NULL, createDeviceFlags, featureLevels, _countof(featureLevels),
        D3D11_SDK_VERSION, &scd,
        &mSwapChain, &mD3dDevice, &selectedFeatureLevel, &mD3dDeviceContext
    );

    if(FAILED(hr))
    {
        std::wstringstream ss;
        ss << L"DirectX 11 하드웨어 가속 실패! HRESULT: 0x" << std::hex << hr
            << L"\n지원되는 Feature Level: " << selectedFeatureLevel;
        MessageBox(hwnd, ss.str().c_str(), L"Error", MB_OK);
        return false;
    }

    ID3D11Texture2D* backBuffer;
    hr = mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);

    if(FAILED(hr) || backBuffer == nullptr)
    {
        std::wstringstream ss;
        ss << L"BackBuffer 가져오기 실패! HRESULT: 0x" << std::hex << hr;
        MessageBox(hwnd, ss.str().c_str(), L"Error", MB_OK);
        return false;
    }

    hr = mD3dDevice->CreateRenderTargetView(backBuffer, NULL, &mRenderTargetView);
    backBuffer->Release();

    if(FAILED(hr))
    {
        std::wstringstream ss;
        ss << L"CreateRenderTargetView 실패! HRESULT: 0x" << std::hex << hr;
        MessageBox(hwnd, ss.str().c_str(), L"Error", MB_OK);
        return false;
    }

    mD3dDeviceContext->OMSetRenderTargets(1, &mRenderTargetView, NULL);
    return true;
}

void core::DXRenderer::Render()
{
    float clearColor[4] = {0.f, 0.f, 0.f, 1.0f};
    mD3dDeviceContext->ClearRenderTargetView(mRenderTargetView, clearColor);
    mSwapChain->Present(1, 0);
}

void core::DXRenderer::Cleanup()
{
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

    if(mD3dDeviceContext)
    {
        mD3dDeviceContext->Release();
        mD3dDeviceContext = nullptr;
    }

    if(mD3dDevice)
    {
        mD3dDevice->Release();
        mD3dDevice = nullptr;
    }
}

ID3D11Device* core::DXRenderer::GetDevice() const
{
    return mD3dDevice;
}

ID3D11DeviceContext* core::DXRenderer::GetContext() const
{
    return mD3dDeviceContext;
}

IDXGISwapChain* core::DXRenderer::GetSwapChain() const
{
    return mSwapChain;
}

ID3D11RenderTargetView* core::DXRenderer::GetRenderTargetView() const
{
    return mRenderTargetView;
}