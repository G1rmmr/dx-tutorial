#pragma once

#include <Windows.h>
#include <d3d11.h>
#include <DirectXMath.h>

namespace core
{
    class Shader;

    class Renderer
    {
    public:
        Renderer();
        ~Renderer();

        bool Initialize(HWND hWnd, int width, int height);
        void Cleanup();

        void BeginFrame(float red, float green, float blue, float alpha);
        void EndFrame();

        void Draw();

        void SetCameraMatrices(const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& proj);

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

        ID3D11Buffer* mMatrixBuffer;

        ID3D11Buffer* mTriangleVertexBuffer;
        ID3D11InputLayout* mInputLayout;

        Shader* mShader;
    };
}