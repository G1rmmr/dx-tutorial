#pragma once

#include <d3d11.h>
#include <DirectXMath.h>

#include <string>
#include <vector>

using namespace DirectX;

struct TGAHeader
{
    uint8_t  IDLength;
    uint8_t  ColorMapType;
    uint8_t  ImageType;
    uint16_t ColorMapStart;
    uint16_t ColorMapLength;
    uint8_t  ColorMapDepth;
    uint16_t XOrigin;
    uint16_t YOrigin;
    uint16_t Width;
    uint16_t Height;
    uint8_t  BPP;
    uint8_t  ImageDescriptor;
};

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
            const XMMATRIX& view,
            const XMMATRIX& proj);

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

        ID3D11Buffer* mVertexBuffer;
        ID3D11Buffer* mIndexBuffer;
        ID3D11ShaderResourceView* mCubeMapSRV;
        ID3D11SamplerState* mSamplerState;
    };
}