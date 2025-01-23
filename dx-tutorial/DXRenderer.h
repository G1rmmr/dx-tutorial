#pragma once

#include <d3d11.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

namespace core
{
    class DXRenderer
    {
    public:
        bool Initialize(HWND hwnd);
        void Render();
        void Cleanup();

        ID3D11Device* GetDevice() const;
        ID3D11DeviceContext* GetContext() const;
        IDXGISwapChain* GetSwapChain() const;
        ID3D11RenderTargetView* GetRenderTargetView() const;

    private:
        ID3D11Device* mD3dDevice;
        ID3D11DeviceContext* mD3dDeviceContext;
        IDXGISwapChain* mSwapChain;
        ID3D11RenderTargetView* mRenderTargetView;
    };
}