#pragma once

#include <vector>

#include <Windows.h>
#include <d3d11.h>
#include <DirectXMath.h>

#include "Actor.h"
#include "Pipeline.h"
#include "Shader.h"
#include "SkyBox.h"
#include "UI.h"

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

        void SetCameraMatrices(
            const DirectX::XMMATRIX& view,
            const DirectX::XMMATRIX& proj);

        bool InitSkyBox();

        inline Pipeline* GetPipeline() const
        {
            return mPipeline;
        }

        inline Shader* GetShader() const
        {
            return mShader;
        }

        inline ID3D11Buffer* GetMatrixBuffer() const
        {
            return mMatrixBuffer;
        }

    private:
        DirectX::XMMATRIX mView;
        DirectX::XMMATRIX mProj;

        ID3D11Buffer* mMatrixBuffer;
        ID3D11InputLayout* mInputLayout;

        Shader* mShader;
        SkyBox* mSkyBox;

        Pipeline* mPipeline;
    };
}