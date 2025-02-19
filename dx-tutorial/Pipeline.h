#pragma once

#include <Windows.h>
#include <d3d11.h>
#include <DirectXMath.h>

namespace core
{
    class Pipeline
    {
    public:
        Pipeline();
        ~Pipeline();

        bool Initialize(HWND hWnd, int width, int height);
        void Cleanup();

        inline ID3D11Device* GetDevice() const
        {
            return mDevice;
        }
        
        inline ID3D11DeviceContext* GetDeviceContext() const
        {
            return mDeviceContext;
        }

        inline IDXGISwapChain* GetSwapChain() const
        {
            return mSwapChain;
        }
        
        inline ID3D11RenderTargetView* GetRenderTargetView() const
        {
            return mRenderTargetView;
        }
        
        inline ID3D11DepthStencilView* GetDepthStencilView() const
        {
            return mDepthStencilView;
        }
        
        inline ID3D11RasterizerState* GetRasterizerState() const
        {
            return mRasterizerState;
        }
        
        inline ID3D11DepthStencilState* GetDepthStencilState() const
        {
            return mDepthStencilState;
        }

    private:
        bool createDeviceAndSwapChain(HWND hWnd, int width, int height);
        bool createRenderTargetView();
        bool createDepthStencilBuffer(int width, int height);
        bool createDepthStencilState();
        bool createRasterizerState();

    private:
        ID3D11Device* mDevice;
        ID3D11DeviceContext* mDeviceContext;
        IDXGISwapChain* mSwapChain;
        ID3D11RenderTargetView* mRenderTargetView;

        ID3D11Texture2D* mDepthStencilBuffer;
        ID3D11DepthStencilView* mDepthStencilView;
        ID3D11DepthStencilState* mDepthStencilState;
        ID3D11RasterizerState* mRasterizerState;
    };
}
