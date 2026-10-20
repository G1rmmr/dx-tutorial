#pragma once

#include "Component.h"

#include <Windows.h>
#include <d3d11.h>
#include <span>
#include <string>
#include <wrl/client.h>

namespace dx
{
    class Shader : public Component
    {
    public:
        Shader(
            ID3D11Device* device,
            const std::wstring& vsFilename,
            const std::string& vsEntryPoint,
            const std::wstring& psFilename,
            const std::string& psEntryPoint,
            std::span<const D3D11_INPUT_ELEMENT_DESC> inputLayout = {});

        Shader(const Shader& other)
        {
            if (this != &other)
            {
                this->mInputLayout = other.mInputLayout;
                this->mVertexShader = other.mVertexShader;
                this->mPixelShader = other.mPixelShader;
            }
        }

        Shader& operator=(const Shader& other)
        {
            if (this != &other)
            {
                this->mInputLayout = other.mInputLayout;
                this->mVertexShader = other.mVertexShader;
                this->mPixelShader = other.mPixelShader;
            }
            return *this;
        }

        void Bind(ID3D11DeviceContext* context) const;

    private:
        Microsoft::WRL::ComPtr<ID3D11InputLayout> mInputLayout;
        Microsoft::WRL::ComPtr<ID3D11VertexShader> mVertexShader;
        Microsoft::WRL::ComPtr<ID3D11PixelShader> mPixelShader;

        Microsoft::WRL::ComPtr<ID3DBlob> compileShaderFromFile(
            const std::wstring& filename,
            const std::string& entryPoint,
            const std::string& shaderModel);
    };
}