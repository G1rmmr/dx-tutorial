#pragma once

#include <d3d11.h>
#include <DirectXMath.h>

#pragma comment(lib, "d3d11.lib")

namespace core
{
    struct Vertex
    {
        DirectX::XMFLOAT3 Pos;
        DirectX::XMFLOAT4 Color;
    };

    class Cube
    {
    public:
        Cube();
        ~Cube();

        bool Initialize(ID3D11Device* device);
        void Render(ID3D11DeviceContext* deviceContext);
        void Cleanup();

    private:
        ID3D11Buffer* mVertexBuffer;
        ID3D11Buffer* mIndexBuffer;
        ID3D11Buffer* mConstantBuffer;

        ID3D11InputLayout* mInputLayout;

        ID3D11VertexShader* mVertexShader;
        ID3D11PixelShader* mPixelShader;
    };
}