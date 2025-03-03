#pragma once

#include <d3d11.h>
#include <DirectXMath.h>

#include <string>
#include <vector>

namespace core
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
            return mCubeMapSRV;
        }

        inline ID3D11SamplerState* GetSamplerState() const
        {
            return mSamplerState;
        }

    private:
        void initCubeMesh(ID3D11Device* device);

        bool loadCubeTex(
            ID3D11Device* device,
            std::vector<uint8_t>& outPixels,
            UINT& outRowPitch,
            const std::string& filePath);

        void loadCubeMap(ID3D11Device* device);

        bool initShaders(ID3D11Device* device);

        class Shader* mShader;
        ID3D11InputLayout* mIL;

        ID3D11Buffer* mVertexBuffer;
        ID3D11Buffer* mIndexBuffer;

        ID3D11ShaderResourceView* mCubeMapSRV;
        ID3D11SamplerState* mSamplerState;
    };
}