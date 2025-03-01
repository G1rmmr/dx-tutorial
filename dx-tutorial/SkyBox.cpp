#include "SkyBox.h"
#include "Shader.h"

#include <stdexcept>
#include <fstream>
#include <vector>
#include <array>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

const float BOX_SIZE = 50.f;

using namespace DirectX;
using namespace core;

SkyBox::SkyBox(
    ID3D11Device* device, 
    ID3D11DeviceContext* context)
    : mVertexBuffer(nullptr)
    , mIndexBuffer(nullptr)
    , mSamplerState(nullptr)
    , mCubeMapSRV(nullptr)
    , mIL(nullptr)
    , mShader(nullptr)
{
    initCubeMesh(device);
    loadCubeMap(device);
    initShaders(device);
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

    if(mIL)
    {
        mIL->Release();
        mIL = nullptr;
    }
}

void SkyBox::initCubeMesh(ID3D11Device* device)
{
    struct Vertex
    {
        float X;
        float Y;
        float Z;
        float U;
        float V;
    };

    Vertex vertices[] =
    {
        // front face
        { -BOX_SIZE,  BOX_SIZE, -BOX_SIZE, 0.f, 0.f },
        {  BOX_SIZE,  BOX_SIZE, -BOX_SIZE, 1.f, 0.f },
        {  BOX_SIZE, -BOX_SIZE, -BOX_SIZE, 1.f, 1.f },
        { -BOX_SIZE, -BOX_SIZE, -BOX_SIZE, 0.f, 1.f },

        // back face
        {  BOX_SIZE,  BOX_SIZE,  BOX_SIZE, 0.f, 0.f },
        { -BOX_SIZE,  BOX_SIZE,  BOX_SIZE, 1.f, 0.f },
        { -BOX_SIZE, -BOX_SIZE,  BOX_SIZE, 1.f, 1.f },
        {  BOX_SIZE, -BOX_SIZE,  BOX_SIZE, 0.f, 1.f },

        // left face
        { -BOX_SIZE,  BOX_SIZE,  BOX_SIZE, 0.f, 0.f },
        { -BOX_SIZE,  BOX_SIZE, -BOX_SIZE, 1.f, 0.f },
        { -BOX_SIZE, -BOX_SIZE, -BOX_SIZE, 1.f, 1.f },
        { -BOX_SIZE, -BOX_SIZE,  BOX_SIZE, 0.f, 1.f },

        // right face
        {  BOX_SIZE,  BOX_SIZE, -BOX_SIZE, 0.f, 0.f },
        {  BOX_SIZE,  BOX_SIZE,  BOX_SIZE, 1.f, 0.f },
        {  BOX_SIZE, -BOX_SIZE,  BOX_SIZE, 1.f, 1.f },
        {  BOX_SIZE, -BOX_SIZE, -BOX_SIZE, 0.f, 1.f },

        // top face
        { -BOX_SIZE,  BOX_SIZE,  BOX_SIZE, 0.f, 0.f },
        {  BOX_SIZE,  BOX_SIZE,  BOX_SIZE, 1.f, 0.f },
        {  BOX_SIZE,  BOX_SIZE, -BOX_SIZE, 1.f, 1.f },
        { -BOX_SIZE,  BOX_SIZE, -BOX_SIZE, 0.f, 1.f },

        // bottom face
        { -BOX_SIZE, -BOX_SIZE, -BOX_SIZE, 0.f, 0.f },
        {  BOX_SIZE, -BOX_SIZE, -BOX_SIZE, 1.f, 0.f },
        {  BOX_SIZE, -BOX_SIZE,  BOX_SIZE, 1.f, 1.f },
        { -BOX_SIZE, -BOX_SIZE,  BOX_SIZE, 0.f, 1.f }
    };

    UINT indices[] =
    {
        0, 2, 1, 0, 3, 2, // front
        4, 6, 5, 4, 7, 6, // back
        8, 10, 9, 8, 11, 10, // left
        12, 14, 13, 12, 15, 14, // right
        16, 18, 17, 16, 19, 18, // top
        20, 22, 21, 20, 23, 22  // bottom
    };

    D3D11_BUFFER_DESC buffDsc{};
    buffDsc.Usage = D3D11_USAGE_DEFAULT;
    buffDsc.ByteWidth = sizeof(vertices);
    buffDsc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA initVData{};
    initVData.pSysMem = vertices;
    device->CreateBuffer(&buffDsc, &initVData, &mVertexBuffer);

    buffDsc.ByteWidth = sizeof(indices);
    buffDsc.BindFlags = D3D11_BIND_INDEX_BUFFER;

    D3D11_SUBRESOURCE_DATA initIData{};
    initIData.pSysMem = indices;

    device->CreateBuffer(&buffDsc, &initIData, &mIndexBuffer);
}

bool SkyBox::loadCubeTex(
    ID3D11Device* device, 
    std::vector<uint8_t>& outPixels, 
    UINT& outRowPitch, 
    const std::string& filePath)
{
    int width;
    int height;
    int comp;

    unsigned char* data = stbi_load(filePath.c_str(), &width, &height, &comp, 4);
    if(!data)
    {
        return false;
    }

    outRowPitch = static_cast<UINT>(width * 4);
    size_t totalSize = static_cast<size_t>(width) * height * 4;
    outPixels.resize(totalSize);
    memcpy(outPixels.data(), data, totalSize);

    stbi_image_free(data);
    return true;
}

void SkyBox::loadCubeMap(ID3D11Device* device)
{
    const std::array<std::string, 6> faces = {
        "lf.tga", // -X: 왼쪽
        "rt.tga", // +X: 오른쪽
        "up.tga", // +Y: 위
        "dn.tga", // -Y: 아래
        "ft.tga", // +Z: 앞
        "bk.tga"  // -Z: 뒤
    };

    std::vector<std::vector<std::uint8_t>> pixelData(6);
    UINT rowPitch = 0;
    for(int i = 0; i < 6; ++i)
    {
        if(!loadCubeTex(device, pixelData[i], rowPitch, faces[i]))
        {
            return;
        }
    }

    D3D11_TEXTURE2D_DESC texDesc{};
    texDesc.Width = 1024;
    texDesc.Height = 1024;
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 6;
    texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    texDesc.SampleDesc.Count = 1;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    texDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

    D3D11_SUBRESOURCE_DATA subData[6];
    for(int i = 0; i < 6; ++i)
    {
        subData[i].pSysMem = pixelData[i].data();
        subData[i].SysMemPitch = rowPitch;
        subData[i].SysMemSlicePitch = 0;
    }

    ID3D11Texture2D* texArray = nullptr;
    device->CreateTexture2D(&texDesc, subData, &texArray);

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format = texDesc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
    srvDesc.TextureCube.MostDetailedMip = 0;
    srvDesc.TextureCube.MipLevels = 1;

    device->CreateShaderResourceView(texArray, &srvDesc, &mCubeMapSRV);

    if(texArray) 
    {
        texArray->Release();
    }

    D3D11_SAMPLER_DESC sampDesc{};
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    device->CreateSamplerState(&sampDesc, &mSamplerState);
}

bool SkyBox::initShaders(ID3D11Device* device)
{
    mShader = new Shader();
    if(!mShader->Initialize(device, L"SkyBoxVS.hlsl", "VSMain", L"SkyBoxPS.hlsl", "PSMain"))
    {
        return false;
    }

    ID3DBlob* vsBlob = mShader->GetVSBlob();
    if(!vsBlob)
    {
        return false;
    };

    D3D11_INPUT_ELEMENT_DESC layoutDesc[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    HRESULT hr = device->CreateInputLayout(
        layoutDesc,
        _countof(layoutDesc),
        vsBlob->GetBufferPointer(),
        vsBlob->GetBufferSize(),
        &mIL);

    if(FAILED(hr)) 
    {
        return false;
    }

    return true;
}


void SkyBox::Render(
    ID3D11DeviceContext* context,
    ID3D11Buffer* matrixBuf,
    const DirectX::XMMATRIX& view,
    const DirectX::XMMATRIX& proj)
{
    struct MatrixBufferType
    {
        DirectX::XMMATRIX World;
        DirectX::XMMATRIX View;
        DirectX::XMMATRIX Proj;
    };

    MatrixBufferType matrices;

    matrices.World = DirectX::XMMatrixIdentity();
    matrices.View = DirectX::XMMatrixTranspose(view);
    matrices.Proj = DirectX::XMMatrixTranspose(proj);

    context->UpdateSubresource(matrixBuf, 0, nullptr, &matrices, 0, 0);

    mShader->SetShader(context);
    context->IASetInputLayout(mIL);

    UINT stride = sizeof(float) * 5;
    UINT offset = 0;

    context->IASetVertexBuffers(0, 1, &mVertexBuffer, &stride, &offset);
    context->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    context->PSSetShaderResources(0, 1, &mCubeMapSRV);
    context->PSSetSamplers(0, 1, &mSamplerState);

    context->DrawIndexed(36, 0, 0);
}