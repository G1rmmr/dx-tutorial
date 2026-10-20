#include "SkyBox.h"
#include "Shader.h"

#include <stdexcept>
#include <fstream>
#include <vector>
#include <array>
#include <filesystem>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

const float BOX_SIZE = 50.f;

using namespace DirectX;
using namespace dx;
using Microsoft::WRL::ComPtr;

SkyBox::SkyBox(ID3D11Device* device, ID3D11DeviceContext* /*context*/)
{
    if (!device)
    {
        throw std::invalid_argument("SkyBox requires a device");
    }
    initCubeMesh(device);
    loadCubeMap(device);
    initShaders(device);
    D3D11_DEPTH_STENCIL_DESC depth{};
    depth.DepthEnable = TRUE;
    depth.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    depth.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
    D3D11_RASTERIZER_DESC raster{};
    raster.FillMode = D3D11_FILL_SOLID;
    raster.CullMode = D3D11_CULL_NONE;
    raster.DepthClipEnable = TRUE;
    if (FAILED(device->CreateDepthStencilState(&depth, mDepthState.GetAddressOf())) ||
        FAILED(device->CreateRasterizerState(&raster, mRasterizerState.GetAddressOf())))
    {
        throw std::runtime_error("Failed to create skybox render states");
    }
}

SkyBox::~SkyBox() = default;

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
    if (FAILED(device->CreateBuffer(&buffDsc, &initVData, mVertexBuffer.GetAddressOf())))
    {
        throw std::runtime_error("Failed to create skybox vertex buffer");
    }

    buffDsc.ByteWidth = sizeof(indices);
    buffDsc.BindFlags = D3D11_BIND_INDEX_BUFFER;

    D3D11_SUBRESOURCE_DATA initIData{};
    initIData.pSysMem = indices;

    if (FAILED(device->CreateBuffer(&buffDsc, &initIData, mIndexBuffer.GetAddressOf())))
    {
        throw std::runtime_error("Failed to create skybox index buffer");
    }
}

bool SkyBox::loadCubeTex(std::vector<uint8_t>& outPixels, UINT& width, UINT& height,
    const std::string& filePath)
{
    int imageWidth = 0, imageHeight = 0, channels = 0;
    std::unique_ptr<stbi_uc, decltype(&stbi_image_free)> pixels(
        stbi_load(filePath.c_str(), &imageWidth, &imageHeight, &channels, 4), stbi_image_free);
    if (!pixels || imageWidth <= 0 || imageHeight <= 0 ||
        imageWidth > D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION ||
        imageHeight > D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION)
    {
        return false;
    }
    width = static_cast<UINT>(imageWidth);
    height = static_cast<UINT>(imageHeight);
    const size_t count = static_cast<size_t>(width) * height * 4;
    outPixels.assign(pixels.get(), pixels.get() + count);
    return true;
}

void SkyBox::loadCubeMap(ID3D11Device* device)
{
    // DXGI cube slices: +X, -X, +Y, -Y, +Z, -Z.
    const std::array<std::string, 6> faces{"lf.tga", "rt.tga", "up.tga", "dn.tga", "ft.tga", "bk.tga"};
    std::array<std::vector<uint8_t>, 6> pixels;
    std::array<D3D11_SUBRESOURCE_DATA, 6> subresources{};
    UINT width = 0, height = 0;
    for (size_t i = 0; i < faces.size(); ++i)
    {
        UINT faceWidth = 0, faceHeight = 0;
        if (!loadCubeTex(pixels[i], faceWidth, faceHeight, faces[i]) || faceWidth != faceHeight)
        {
            throw std::runtime_error("Missing or invalid skybox cube face: " + faces[i]);
        }
        if (i == 0)
        {
            width = faceWidth;
            height = faceHeight;
        }
        if (faceWidth != width || faceHeight != height)
        {
            throw std::runtime_error("Skybox cube faces must have matching dimensions");
        }
        subresources[i].pSysMem = pixels[i].data();
        subresources[i].SysMemPitch = width * 4;
    }
    D3D11_TEXTURE2D_DESC description{};
    description.Width = width;
    description.Height = height;
    description.MipLevels = 1;
    description.ArraySize = 6;
    description.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    description.SampleDesc.Count = 1;
    description.Usage = D3D11_USAGE_DEFAULT;
    description.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    description.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
    ComPtr<ID3D11Texture2D> texture;
    if (FAILED(device->CreateTexture2D(&description, subresources.data(), texture.GetAddressOf())))
    {
        throw std::runtime_error("Failed to create skybox cube texture");
    }
    D3D11_SHADER_RESOURCE_VIEW_DESC view{};
    view.Format = description.Format;
    view.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
    view.TextureCube.MipLevels = 1;
    if (FAILED(device->CreateShaderResourceView(texture.Get(), &view, mCubeMapSRV.GetAddressOf())))
    {
        throw std::runtime_error("Failed to create skybox cube view");
    }
    D3D11_SAMPLER_DESC sampler{};
    sampler.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampler.AddressU = sampler.AddressV = sampler.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampler.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampler.MaxLOD = D3D11_FLOAT32_MAX;
    if (FAILED(device->CreateSamplerState(&sampler, mSamplerState.GetAddressOf())))
    {
        throw std::runtime_error("Failed to create skybox sampler");
    }
}

bool SkyBox::initShaders(ID3D11Device* device)
{
    const D3D11_INPUT_ELEMENT_DESC position{
        "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0};
    mShader = std::make_unique<Shader>(device, L"SkyBoxVS.hlsl", "VSMain",
        L"SkyBoxPS.hlsl", "PSMain", std::span<const D3D11_INPUT_ELEMENT_DESC>(&position, 1));
    return true;
}

void SkyBox::Render(ID3D11DeviceContext* context, ID3D11Buffer* matrixBuf,
    const XMMATRIX& view, const XMMATRIX& projection)
{
    if (!context || !matrixBuf)
    {
        throw std::invalid_argument("SkyBox rendering requires a context and matrix buffer");
    }
    struct Matrices { XMMATRIX World, View, Projection; };
    XMMATRIX centeredView = view;
    centeredView.r[3] = XMVectorSet(0, 0, 0, 1);
    const Matrices matrices{XMMatrixIdentity(), XMMatrixTranspose(centeredView), XMMatrixTranspose(projection)};
    context->UpdateSubresource(matrixBuf, 0, nullptr, &matrices, 0, 0);
    context->VSSetConstantBuffers(0, 1, &matrixBuf);

    ComPtr<ID3D11DepthStencilState> previousDepth;
    ComPtr<ID3D11RasterizerState> previousRasterizer;
    UINT previousStencil = 0;
    context->OMGetDepthStencilState(previousDepth.GetAddressOf(), &previousStencil);
    context->RSGetState(previousRasterizer.GetAddressOf());
    context->OMSetDepthStencilState(mDepthState.Get(), 0);
    context->RSSetState(mRasterizerState.Get());
    mShader->Bind(context);
    const UINT stride = sizeof(float) * 5, offset = 0;
    context->IASetVertexBuffers(0, 1, mVertexBuffer.GetAddressOf(), &stride, &offset);
    context->IASetIndexBuffer(mIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    context->PSSetShaderResources(0, 1, mCubeMapSRV.GetAddressOf());
    context->PSSetSamplers(0, 1, mSamplerState.GetAddressOf());
    context->DrawIndexed(36, 0, 0);
    ID3D11ShaderResourceView* emptyView = nullptr;
    context->PSSetShaderResources(0, 1, &emptyView);
    context->OMSetDepthStencilState(previousDepth.Get(), previousStencil);
    context->RSSetState(previousRasterizer.Get());
}
