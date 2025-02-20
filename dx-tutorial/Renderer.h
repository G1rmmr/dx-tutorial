#pragma once

#include <vector>

#include <Windows.h>
#include <d3d11.h>
#include <DirectXMath.h>

#include "Actor.h"

namespace core
{
    class Renderer
    {
    public:
        Renderer();
        ~Renderer();

        bool Initialize(HWND hWnd, int width, int height);
        void Cleanup();

        void BeginFrame(float red, float green, float blue, float alpha);
        void EndFrame();

        void Draw(std::vector<Actor*>& actors);

        void SetCameraMatrices(const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& proj);

    private:
        class Pipeline* mPipeline;
        class Shader* mShader;

        ID3D11Buffer* mMatrixBuffer;

        DirectX::XMMATRIX mView;
        DirectX::XMMATRIX mProj;

        ID3D11InputLayout* mInputLayout;
    };
}