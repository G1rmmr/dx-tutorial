#include "SkyBox.h"
#include <stdexcept>
#include <fstream>
#include <vector>
#include <array>

using namespace core;

struct Vertex
{ 
    XMFLOAT3 Pos;
};

SkyBox::SkyBox(
    ID3D11Device* device, 
    ID3D11DeviceContext* context)
    : mVertexBuffer(nullptr)
    , mIndexBuffer(nullptr)
    , mSamplerState(nullptr)
    , mCubeMapSRV(nullptr)
{
    initCubeMesh(device);
    loadCubeMap(device);

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
        mVertexBuffer = nullptr;
    }

    if(mIndexBuffer)
    {
        mIndexBuffer->Release();
        mIndexBuffer = nullptr;
    }

    if(mSamplerState)
    {
        mSamplerState->Release();
        mSamplerState = nullptr;
    }

    if(mCubeMapSRV)
    {
        mCubeMapSRV->Release();
        mCubeMapSRV = nullptr;
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
        0, 1, 2, 0, 2, 3,
        4, 6, 5, 4, 7, 6,
        4, 5, 1, 4, 1, 0,
        3, 2, 6, 3, 6, 7,
        1, 5, 6, 1, 6, 2,
        4, 0, 3, 4, 3, 7
    };

    D3D11_BUFFER_DESC vbd = {sizeof(vertices), D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER};
    D3D11_SUBRESOURCE_DATA vertexData = {vertices};

    HRESULT hr = device->CreateBuffer(&vbd, &vertexData, &mVertexBuffer);
    if(FAILED(hr))
    {
        throw std::runtime_error("Failed to create vertex buffer.");
    }

    D3D11_BUFFER_DESC ibd = {sizeof(indices), D3D11_USAGE_DEFAULT, D3D11_BIND_INDEX_BUFFER};
    D3D11_SUBRESOURCE_DATA indexData = {indices};

    hr = device->CreateBuffer(&ibd, &indexData, &mIndexBuffer);
    if(FAILED(hr))
    {
        throw std::runtime_error("Failed to create index buffer.");
    }
}

bool SkyBox::loadCubeTex(
    ID3D11Device* device, 
    std::vector<uint8_t>& outPixels, 
    UINT& outRowPitch, 
    const std::string& filePath)
{
    std::ifstream file(filePath, std::ios::binary);
    if(!file)
    {
        MessageBox(nullptr, L"Failed to open TGA file", L"Error", MB_OK);
        return false;
    }

    TGAHeader header;
    file.read(reinterpret_cast<char*>(&header), sizeof(TGAHeader));

    if(header.ImageType != 2) 
    {
        file.close();
        MessageBox(nullptr, L"Unsupported TGA format", L"Error", MB_OK);
        return false;
    }

    int width = header.Width;
    int height = header.Height;
    int bpp = header.BPP / 8;


    std::vector<uint8_t> imageData(width * height * bpp);
    file.read(reinterpret_cast<char*>(imageData.data()), imageData.size());
    file.close();

    outPixels.resize(width * height * 4);
    for(int i = 0; i < width * height; i++)
    {
        outPixels[i * 4 + 0] = imageData[i * bpp + 2]; // Red
        outPixels[i * 4 + 1] = imageData[i * bpp + 1]; // Green
        outPixels[i * 4 + 2] = imageData[i * bpp + 0]; // Blue
        outPixels[i * 4 + 3] = (bpp == 4) ? imageData[i * bpp + 3] : 255; // Alpha
    }

    outRowPitch = width * 4;
    return true;
}

void SkyBox::loadCubeMap(ID3D11Device* device)
{
    const std::array<std::string, 6> files = {
        "bk.tga",
        "dn.tga",
        "ft.tga",
        "lf.tga",
        "rt.tga",
        "up.tga"
    };

    std::array<std::vector<uint8_t>, 6> pixelData;
    std::array<UINT, 6> rowPitch;

    for(int i = 0; i < 6; ++i)
    {
        if(!loadCubeTex(device, pixelData[i], rowPitch[i], files[i]))
        {
            throw std::runtime_error("Failed to load cube map texture: " + files[i]);
        }
    }

    for(int i = 0; i < 6; ++i)
    {
        if(pixelData[i].empty() || rowPitch[i] == 0)
        {
            throw std::runtime_error("Invalid pixel data or row pitch for texture: " + files[i]);
        }
    }

    D3D11_TEXTURE2D_DESC texDesc = {};
    texDesc.Width = 1024;
    texDesc.Height = 1024;
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 6;
    texDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    texDesc.SampleDesc.Count = 1;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    texDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

    std::vector<D3D11_SUBRESOURCE_DATA> subresourceData(6);
    for(int i = 0; i < 6; ++i)
    {
        subresourceData[i].pSysMem = pixelData[i].data();
        subresourceData[i].SysMemPitch = rowPitch[i];
        subresourceData[i].SysMemSlicePitch = 0;
    }

    ID3D11Texture2D* cubeTexture = nullptr;
    HRESULT hr = device->CreateTexture2D(&texDesc, subresourceData.data(), &cubeTexture);
    if(FAILED(hr))
    {
        throw std::runtime_error("Failed to create cube texture.");
    }

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = texDesc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
    srvDesc.TextureCube.MipLevels = texDesc.MipLevels;
    srvDesc.TextureCube.MostDetailedMip = 0;

    hr = device->CreateShaderResourceView(cubeTexture, &srvDesc, &mCubeMapSRV);
    if(FAILED(hr))
    {
        throw std::runtime_error("Failed to create CubeMap SRV.");
    }

    cubeTexture->Release();
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

    struct MatrixBufferType
    {
        XMMATRIX View;
        XMMATRIX Proj;
    };

    MatrixBufferType Matbuf;

    Matbuf.View = XMMatrixTranspose(modifiedView);
    Matbuf.Proj = XMMatrixTranspose(proj);

    context->UpdateSubresource(matrixBuf, 0, nullptr, &Matbuf, 0, 0);

    context->IASetVertexBuffers(0, 1, &mVertexBuffer, &stride, &offset);
    context->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    for(int i = 0; i < 6; ++i)
    {
        context->PSSetShaderResources(0, 1, &mCubeMapSRV);
        context->PSSetSamplers(0, 1, &mSamplerState);
        context->DrawIndexed(6, i * 6, 0);
    }
}
