#include "Shader.h"

#include <d3dcompiler.h>
#include <sstream>
#include <stdexcept>

namespace
{
    void ThrowIfFailed(HRESULT hr, const char* operation)
    {
        if (FAILED(hr))
        {
            std::ostringstream message;
            message << operation << " failed (HRESULT 0x" << std::hex
                << static_cast<unsigned long>(hr) << ")";
            throw std::runtime_error(message.str());
        }
    }
}

using namespace dx;
using namespace std;
using namespace Microsoft::WRL;

Shader::Shader(
    ID3D11Device* device,
    const wstring& vsFilename,
    const string& vsEntryPoint,
    const wstring& psFilename,
    const string& psEntryPoint,
    span<const D3D11_INPUT_ELEMENT_DESC> inputLayout)
{
    if (!device)
    {
        throw invalid_argument("Shader requires a device");
    }
    // ¡§¡° ºŒ¿Ã¥ı ∑ŒµÂ
    ComPtr<ID3DBlob> vsBlob = compileShaderFromFile(vsFilename, vsEntryPoint, "vs_5_0");
    if (!vsBlob)
    {
        throw runtime_error("Vertex shader compilation returned no bytecode");
    }

    // «»ºø ºŒ¿Ã¥ı ∑ŒµÂ
    ComPtr<ID3DBlob> psBlob = compileShaderFromFile(psFilename, psEntryPoint, "ps_5_0");
    if (!psBlob)
    {
        throw runtime_error("Pixel shader compilation returned no bytecode");
    }

    HRESULT hr = S_OK;

    // ¿‘∑¬ ∑π¿Ãæ∆øÙ º≥¡§
    D3D11_INPUT_ELEMENT_DESC layoutDesc[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,   0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT,0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    if (inputLayout.empty())
    {
        inputLayout = layoutDesc;
    }
    UINT numElements = static_cast<UINT>(inputLayout.size());

    if (FAILED(hr = device->CreateInputLayout(
        inputLayout.data(),
        numElements,
        vsBlob->GetBufferPointer(),
        vsBlob->GetBufferSize(),
        mInputLayout.GetAddressOf())))
    {
        ThrowIfFailed(hr, "Create input layout");
    }

    // ¡§¡° ºŒ¿Ã¥ı «“¥Á
    if (FAILED(hr = device->CreateVertexShader(
        vsBlob->GetBufferPointer(),
        vsBlob->GetBufferSize(),
        nullptr,
        mVertexShader.GetAddressOf())))
    {
        ThrowIfFailed(hr, "Create vertex shader");
    }

    // «»ºø ºŒ¿Ã¥ı «“¥Á
    if (FAILED(hr = device->CreatePixelShader(
        psBlob->GetBufferPointer(),
        psBlob->GetBufferSize(),
        nullptr,
        mPixelShader.GetAddressOf())))
    {
        ThrowIfFailed(hr, "Create pixel shader");
    }
}

void Shader::Bind(ID3D11DeviceContext* context) const
{
    if (!context)
    {
        throw invalid_argument("Shader::Bind requires a context");
    }
    context->IASetInputLayout(mInputLayout.Get());
    context->VSSetShader(mVertexShader.Get(), nullptr, 0);
    context->PSSetShader(mPixelShader.Get(), nullptr, 0);
}

ComPtr<ID3DBlob> Shader::compileShaderFromFile(
    const wstring& filename,
    const string& entryPoint,
    const string& shaderModel)
{
    UINT compileFlags = 0;
#ifdef _DEBUG
    compileFlags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    Microsoft::WRL::ComPtr<ID3DBlob> errorMessage = nullptr;
    Microsoft::WRL::ComPtr<ID3DBlob> blobOut = nullptr;

    HRESULT hr = D3DCompileFromFile(
        filename.c_str(),
        nullptr,
        nullptr,
        entryPoint.c_str(),
        shaderModel.c_str(),
        compileFlags,
        0,
        blobOut.GetAddressOf(),
        errorMessage.GetAddressOf());

    if (errorMessage)
    {
        const char* compileErrors = static_cast<const char*>(errorMessage->GetBufferPointer());
        OutputDebugStringA(compileErrors);
    }
    ThrowIfFailed(hr, "Compile shader");

    return blobOut;
}