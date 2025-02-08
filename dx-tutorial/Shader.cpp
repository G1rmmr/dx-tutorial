#include "Shader.h"
#include <d3dcompiler.h>

using namespace core;

Shader::Shader()
    : mVertexShader(nullptr)
    , mPixelShader(nullptr)
    , mInputLayout(nullptr)
{

}

Shader::~Shader()
{
    Cleanup();
}

bool Shader::Initialize(
    ID3D11Device* device,
    const std::wstring& vsFilename,
    const std::string& vsEntryPoint,
    const std::wstring& psFilename,
    const std::string& psEntryPoint)
{
    ID3DBlob* vsBlob = nullptr;
    if(!compileShaderFromFile(vsFilename, vsEntryPoint, "vs_5_0", &vsBlob))
    {
        return false;
    }

    ID3DBlob* psBlob = nullptr;
    if(!compileShaderFromFile(psFilename, psEntryPoint, "ps_5_0", &psBlob))
    {
        if(vsBlob) vsBlob->Release();
        return false;
    }

    HRESULT hr = device->CreateVertexShader(
        vsBlob->GetBufferPointer(),
        vsBlob->GetBufferSize(),
        nullptr,
        &mVertexShader);

    if(FAILED(hr))
    {
        vsBlob->Release();
        psBlob->Release();
        return false;
    }

    hr = device->CreatePixelShader(
        psBlob->GetBufferPointer(),
        psBlob->GetBufferSize(),
        nullptr,
        &mPixelShader);

    if(FAILED(hr))
    {
        vsBlob->Release();
        psBlob->Release();
        return false;
    }

    if(!createInputLayout(device, vsBlob))
    {
        vsBlob->Release();
        psBlob->Release();
        return false;
    }

    vsBlob->Release();
    psBlob->Release();

    return true;
}

void Shader::Cleanup()
{
    if(mInputLayout)
    {
        mInputLayout->Release();
        mInputLayout = nullptr;
    }
    if(mVertexShader)
    {
        mVertexShader->Release();
        mVertexShader = nullptr;
    }
    if(mPixelShader)
    {
        mPixelShader->Release();
        mPixelShader = nullptr;
    }
}

void Shader::SetShader(ID3D11DeviceContext* deviceContext)
{
    deviceContext->IASetInputLayout(mInputLayout);
    deviceContext->VSSetShader(mVertexShader, nullptr, 0);
    deviceContext->PSSetShader(mPixelShader, nullptr, 0);
}

bool Shader::compileShaderFromFile(
    const std::wstring& filename,
    const std::string& entryPoint,
    const std::string& shaderModel,
    ID3DBlob** blobOut)
{
    UINT compileFlags = 0;
#ifdef _DEBUG
    compileFlags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    ID3DBlob* errorMessage = nullptr;
    HRESULT hr = D3DCompileFromFile(
        filename.c_str(),
        nullptr,
        nullptr,
        entryPoint.c_str(),
        shaderModel.c_str(),
        compileFlags,
        0,
        blobOut,
        &errorMessage);

    if(FAILED(hr))
    {
        if(errorMessage)
        {
            char* compileErrors = (char*)errorMessage->GetBufferPointer();
            OutputDebugStringA(compileErrors);
            errorMessage->Release();
        }
        return false;
    }

    if(errorMessage)
        errorMessage->Release();

    return true;
}

bool Shader::createInputLayout(ID3D11Device* device, ID3DBlob* vsBlob)
{
    D3D11_INPUT_ELEMENT_DESC layoutDesc[] =
    {
        {
            "POSITION",
            0,
            DXGI_FORMAT_R32G32B32_FLOAT,
            0,
            0,
            D3D11_INPUT_PER_VERTEX_DATA,
            0
        }
    };

    UINT numElements = _countof(layoutDesc);

    HRESULT hr = device->CreateInputLayout(
        layoutDesc,
        numElements,
        vsBlob->GetBufferPointer(),
        vsBlob->GetBufferSize(),
        &mInputLayout);

    if(FAILED(hr))
    {
        return false;
    }

    return true;
}
