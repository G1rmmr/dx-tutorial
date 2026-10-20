#pragma once

#include <vector>

#include <Windows.h>
#include <d3d11.h>

#include "Object.h"
#include "Camera.h"
#include "MatrixBuffer.h"

#include "wrl/client.h"

namespace dx
{
    class Renderer
    {
    public:
        Renderer(HWND hWnd, UINT width, UINT height);

        void BeginFrame(DirectX::XMFLOAT4 color);
        void EndFrame();
        void Resize(UINT width, UINT height);
        bool TestVisibility();

        bool IsOccluded() const noexcept { return mIsOccluded; }

        void Draw(const Object& object, const Camera& camera);

        inline ID3D11Device* GetDevice() const noexcept
        {
            return mDevice.Get();
        }
        ID3D11DeviceContext* GetContext() const noexcept { return mDeviceContext.Get(); }
        IDXGISwapChain* GetSwapChain() const noexcept { return mSwapChain.Get(); }
        ID3D11Buffer* GetMatrixBuffer() const noexcept { return mConstantBuffer.Get(); }
        UINT GetWidth() const noexcept { return mWidth; }
        UINT GetHeight() const noexcept { return mHeight; }

    private:
        HWND mHWnd;
        UINT mWidth;
        UINT mHeight;
        bool mIsOccluded = false;

        Microsoft::WRL::ComPtr<ID3D11Device> mDevice;
        Microsoft::WRL::ComPtr<ID3D11DeviceContext> mDeviceContext;
        Microsoft::WRL::ComPtr<IDXGISwapChain> mSwapChain;
        Microsoft::WRL::ComPtr<ID3D11RenderTargetView> mRenderTargetView;
        Microsoft::WRL::ComPtr<ID3D11Texture2D> mDepthStencilBuffer;
        Microsoft::WRL::ComPtr<ID3D11DepthStencilView> mDepthStencilView;
        Microsoft::WRL::ComPtr<ID3D11DepthStencilState> mDepthStencilState;
        Microsoft::WRL::ComPtr<ID3D11RasterizerState> mRasterizerState;
        Microsoft::WRL::ComPtr<ID3D11Buffer> mConstantBuffer;

        void SetViewport();

        std::vector<Microsoft::WRL::ComPtr<IDXGIAdapter1>> enumrateAdapters();

        HRESULT createDeviceAndSwapChain();
        HRESULT createRenderTargetView();
        HRESULT createDepthStencilBuffer();
        HRESULT createDepthStencilState();
        HRESULT createRasterizerState();
        HRESULT createConstantBuffer();
    };
}