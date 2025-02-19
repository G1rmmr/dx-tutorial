#pragma once

#include <Windows.h>
#include <d3d11.h>
#include <DirectXMath.h>

namespace core
{
    class Shader;
    class Enemy;
    class Pipeline;

    class Renderer
    {
    public:
        Renderer();
        ~Renderer();

        bool Initialize(HWND hWnd, int width, int height);
        void Cleanup();

        void BeginFrame(float red, float green, float blue, float alpha);
        void EndFrame();

        void Draw(Enemy* enemy);

        void SetCameraMatrices(const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& proj);

    private:
        Pipeline* mPipeline;

        ID3D11Buffer* mMatrixBuffer;

        DirectX::XMMATRIX mView;
        DirectX::XMMATRIX mProj;

        ID3D11Buffer* mFloorVertex;
        ID3D11Buffer* mEnemyVertex;
        ID3D11InputLayout* mInputLayout;

        Shader* mShader;
    };
}