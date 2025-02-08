#pragma once

#include <Windows.h>
#include <d3d11.h>
#include <string>

namespace core
{
    class Shader
    {
    public:
        Shader();
        ~Shader();
        bool Initialize(
            ID3D11Device* device,
            const std::wstring& vsFilename,
            const std::string& vsEntryPoint,
            const std::wstring& psFilename,
            const std::string& psEntryPoint);

        void Cleanup();

        void SetShader(ID3D11DeviceContext* deviceContext);

    private:
        bool compileShaderFromFile(
            const std::wstring& filename,
            const std::string& entryPoint,
            const std::string& shaderModel,
            ID3DBlob** blobOut);

        bool createInputLayout(ID3D11Device* device, ID3DBlob* vsBlob);

    private:
        ID3D11VertexShader* mVertexShader;
        ID3D11PixelShader* mPixelShader;
        ID3D11InputLayout* mInputLayout;
    };
}