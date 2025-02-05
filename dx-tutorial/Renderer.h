// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once

#include <d3d11.h>
#include <dxgi.h>
#include <DirectXMath.h>

namespace core
{
    struct DirectionalLight
    {
        DirectX::XMFLOAT3 Dir;
        DirectX::XMFLOAT3 DiffColor;
        DirectX::XMFLOAT3 SpecColor;
    };

    class Renderer
    {
    public:
        Renderer();
        ~Renderer();

        bool Initialize(HWND hWnd, int width, int height);
        void Draw();
        void Cleanup();

        inline float GetScreenWidth() const
        {
            return mScreenWidth;
        }

        inline float GetScreenHeight() const
        {
            return mScreenHeight;
        }

        inline void SetViewMatrix(const DirectX::XMFLOAT4X4& view)
        {
            mView = view;
        }

        inline void SetProjectionMatrix(const DirectX::XMFLOAT4X4& proj)
        {
            mProj = proj;
        }

        inline void SetAmbientLight(const DirectX::XMFLOAT3& ambient)
        {
            mAmbLight = ambient;
        }

        inline DirectionalLight& GetDirectionalLight()
        {
            return mDirLight;
        }

    private:
        ID3D11Device* mDevice;
        ID3D11DeviceContext* mDeviceContext;
        IDXGISwapChain* mSwapChain;
        ID3D11RenderTargetView* mRenderTargetView;
        ID3D11Texture2D* mDepthStencilBuffer;
        ID3D11DepthStencilView* mDepthStencilView;

        DirectX::XMFLOAT4X4 mView;
        DirectX::XMFLOAT4X4 mProj;

        DirectX::XMFLOAT3 mAmbLight;
        DirectionalLight mDirLight;

        float mScreenWidth;
        float mScreenHeight;

    };
}