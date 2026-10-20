#pragma once

#include <d3d11.h>
#include <DirectXMath.h>

#include <string>
#include <vector>
#include <cstdint>
#include <memory>
#include <wrl/client.h>

namespace dx
{
    class SkyBox
    {
    public:
        SkyBox(ID3D11Device* device, ID3D11DeviceContext* context);
        ~SkyBox();

        void Render(
            ID3D11DeviceContext* context,
            ID3D11Buffer* matrixBuf,
            const DirectX::XMMATRIX& view,
            const DirectX::XMMATRIX& proj);

        inline ID3D11ShaderResourceView* GetCubeMapSRV() const
        {
            return mCubeMapSRV.Get();
        }

        inline ID3D11SamplerState* GetSamplerState() const
        {
            return mSamplerState.Get();
        }

    private:
        void initCubeMesh(ID3D11Device* device);

        bool loadCubeTex(
            std::vector<uint8_t>& outPixels,
            UINT& width,
            UINT& height,
            const std::string& filePath);

        void loadCubeMap(ID3D11Device* device);

        bool initShaders(ID3D11Device* device);

        std::unique_ptr<class Shader> mShader;

        Microsoft::WRL::ComPtr<ID3D11Buffer> mVertexBuffer;
        Microsoft::WRL::ComPtr<ID3D11Buffer> mIndexBuffer;

        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mCubeMapSRV;
        Microsoft::WRL::ComPtr<ID3D11SamplerState> mSamplerState;
        Microsoft::WRL::ComPtr<ID3D11DepthStencilState> mDepthState;
        Microsoft::WRL::ComPtr<ID3D11RasterizerState> mRasterizerState;
    };
}
