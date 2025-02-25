#include "Skybox.h"

#include <vector>
#include <stdexcept>

using namespace DirectX;
using namespace core;

struct Vertex
{
    XMFLOAT3 Pos;
};

SkyBox::SkyBox(ID3D11Device* device, ID3D11DeviceContext* context)
    : mVertexBuffer(nullptr)
    , mIndexBuffer(nullptr)
    , mCubeMapSRV(nullptr)
    , mSamplerState(nullptr)
{
    initCubeMesh(device);
    loadCubeMap(device, "skybox.dds");

    D3D11_SAMPLER_DESC samplerDesc = {};
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    HRESULT hr = device->CreateSamplerState(&samplerDesc, &mSamplerState);
    if(FAILED(hr))
    {
        throw std::runtime_error("Failed to create sampler state.");
    }
}

SkyBox::~SkyBox()
{
    if(mVertexBuffer)
    {
        mVertexBuffer->Release();
    }

    if(mIndexBuffer)
    {
        mIndexBuffer->Release();
    }

    if(mCubeMapSRV)
    {
        mCubeMapSRV->Release();
    }

    if(mSamplerState)
    {
        mSamplerState->Release();
    }
}

void SkyBox::initCubeMesh(ID3D11Device* device)
{
    Vertex vertices[] = {
        {{-10.f, -10.f, -10.f}},
        {{-10.f,  10.f, -10.f}},
        {{10.f,  10.f, -10.f}},
        {{10.f, -10.f, -10.f}},
        {{-10.f, -10.f,  10.f}},
        {{-10.f,  10.f,  10.f}},
        {{10.f,  10.f,  10.f}},
        {{10.f, -10.f,  10.f}}
    };

    unsigned int indices[] = {
        0, 1, 2, 0, 2, 3, // Front
        4, 6, 5, 4, 7, 6, // Back
        4, 5, 1, 4, 1, 0, // Left
        3, 2, 6, 3, 6, 7, // Right
        1, 5, 6, 1, 6, 2, // Top
        4, 0, 3, 4, 3, 7  // Bottom
    };

    D3D11_BUFFER_DESC vbd = {};
    vbd.Usage = D3D11_USAGE_DEFAULT;
    vbd.ByteWidth = sizeof(vertices);
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA vertexData = {};
    vertexData.pSysMem = vertices;

    HRESULT hr = device->CreateBuffer(&vbd, &vertexData, &mVertexBuffer);
    if(FAILED(hr))
    {
        throw std::runtime_error("Failed to create vertex buffer.");
    }

    // Index Buffer »ý¼º
    D3D11_BUFFER_DESC ibd = {};
    ibd.Usage = D3D11_USAGE_DEFAULT;
    ibd.ByteWidth = sizeof(indices);
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;

    D3D11_SUBRESOURCE_DATA indexData = {};
    indexData.pSysMem = indices;

    hr = device->CreateBuffer(&ibd, &indexData, &mIndexBuffer);
    if(FAILED(hr))
    {
        throw std::runtime_error("Failed to create index buffer.");
    }
}

void SkyBox::loadCubeMap(ID3D11Device* device, const std::string& filePath)
{
    //
}

void SkyBox::Render(
    ID3D11DeviceContext* context,
    ID3D11Buffer* matrixBuf,
    const XMMATRIX& view,
    const XMMATRIX& proj)
{
    UINT stride = sizeof(Vertex);
    UINT offset = 0;

    XMMATRIX modifiedView = view;
    modifiedView.r[3] = XMVectorSet(0, 0, 0, 1);

    context->IASetVertexBuffers(0, 1, &mVertexBuffer, &stride, &offset);
    context->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

    context->UpdateSubresource(matrixBuf, 0, nullptr, &modifiedView, 0, 0);

    context->PSSetShaderResources(0, 1, &mCubeMapSRV);
    context->PSSetSamplers(0, 1, &mSamplerState);

    context->DrawIndexed(36, 0, 0);
}