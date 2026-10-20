#ifndef NOMINMAX
#define NOMINMAX
#endif

#include "Mesh.h"
#include "Shader.h"
#include "MatrixBuffer.h"
#include "ModelAsset.h"

#include <d3d11.h>
#include <d3d11sdklayers.h>
#include <DirectXMath.h>
#include <wrl/client.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using Microsoft::WRL::ComPtr;
using namespace DirectX;

namespace
{
    constexpr UINT TileSize = 384;
    constexpr UINT ViewCount = 3;
    constexpr UINT ModelCount = 5;

    void Check(HRESULT result, const char* operation)
    {
        if (FAILED(result))
        {
            std::ostringstream message;
            message << operation << " failed: 0x" << std::hex
                << static_cast<unsigned long>(result);
            throw std::runtime_error(message.str());
        }
    }

    void WriteBitmap(const std::filesystem::path& path,
        UINT width, UINT height, const std::vector<std::uint8_t>& rgba)
    {
        BITMAPFILEHEADER fileHeader{};
        BITMAPINFOHEADER imageHeader{};
        imageHeader.biSize = sizeof(imageHeader);
        imageHeader.biWidth = static_cast<LONG>(width);
        imageHeader.biHeight = -static_cast<LONG>(height);
        imageHeader.biPlanes = 1;
        imageHeader.biBitCount = 32;
        imageHeader.biCompression = BI_RGB;
        imageHeader.biSizeImage = width * height * 4;
        fileHeader.bfType = 0x4d42;
        fileHeader.bfOffBits = sizeof(fileHeader) + sizeof(imageHeader);
        fileHeader.bfSize = fileHeader.bfOffBits + imageHeader.biSizeImage;

        std::vector<std::uint8_t> bgra(rgba);
        for (std::size_t offset = 0; offset < bgra.size(); offset += 4)
        {
            std::swap(bgra[offset], bgra[offset + 2]);
            bgra[offset + 3] = 255;
        }
        if (!path.parent_path().empty())
        {
            std::filesystem::create_directories(path.parent_path());
        }
        std::ofstream output(path, std::ios::binary);
        output.write(reinterpret_cast<const char*>(&fileHeader), sizeof(fileHeader));
        output.write(reinterpret_cast<const char*>(&imageHeader), sizeof(imageHeader));
        output.write(reinterpret_cast<const char*>(bgra.data()),
            static_cast<std::streamsize>(bgra.size()));
        if (!output)
        {
            throw std::runtime_error("Cannot write preview bitmap");
        }
    }

    void CheckDebugMessages(ID3D11InfoQueue* queue)
    {
        if (!queue)
        {
            return;
        }
        const UINT64 count = queue->GetNumStoredMessagesAllowedByRetrievalFilter();
        for (UINT64 index = 0; index < count; ++index)
        {
            SIZE_T size = 0;
            Check(queue->GetMessage(index, nullptr, &size), "Measure debug message");
            std::vector<std::uint8_t> storage(size);
            auto* message = reinterpret_cast<D3D11_MESSAGE*>(storage.data());
            Check(queue->GetMessage(index, message, &size), "Read debug message");
            if (message->Severity == D3D11_MESSAGE_SEVERITY_CORRUPTION ||
                message->Severity == D3D11_MESSAGE_SEVERITY_ERROR)
            {
                throw std::runtime_error(message->pDescription);
            }
        }
    }
}

int wmain(int argc, wchar_t** argv)
{
    try
    {
        if (argc < 3 || argc > 4 ||
            (argc == 4 && std::wstring(argv[3]) != L"--debug"))
        {
            std::cerr << "Usage: RenderAssets <project-source-folder> <output.bmp> [--debug]\n";
            return 2;
        }

        const std::filesystem::path sourceFolder = std::filesystem::absolute(argv[1]);
        const std::filesystem::path outputPath = std::filesystem::absolute(argv[2]);
        const bool debug = argc == 4;

        ComPtr<ID3D11Device> device;
        ComPtr<ID3D11DeviceContext> context;
        D3D_FEATURE_LEVEL featureLevel{};
        const D3D_FEATURE_LEVEL requestedLevel = D3D_FEATURE_LEVEL_11_0;
        Check(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_WARP, nullptr,
            debug ? D3D11_CREATE_DEVICE_DEBUG : 0,
            &requestedLevel, 1, D3D11_SDK_VERSION,
            device.GetAddressOf(), &featureLevel, context.GetAddressOf()), "Create WARP device");
        ComPtr<ID3D11InfoQueue> debugQueue;
        if (debug)
        {
            Check(device.As(&debugQueue), "Get D3D debug queue");
        }

        D3D11_TEXTURE2D_DESC colorDescription{};
        colorDescription.Width = TileSize;
        colorDescription.Height = TileSize;
        colorDescription.MipLevels = 1;
        colorDescription.ArraySize = 1;
        colorDescription.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        colorDescription.SampleDesc.Count = 1;
        colorDescription.Usage = D3D11_USAGE_DEFAULT;
        colorDescription.BindFlags = D3D11_BIND_RENDER_TARGET;
        ComPtr<ID3D11Texture2D> colorTexture;
        Check(device->CreateTexture2D(&colorDescription, nullptr,
            colorTexture.GetAddressOf()), "Create color target");
        ComPtr<ID3D11RenderTargetView> colorView;
        Check(device->CreateRenderTargetView(colorTexture.Get(), nullptr,
            colorView.GetAddressOf()), "Create color view");

        D3D11_TEXTURE2D_DESC depthDescription = colorDescription;
        depthDescription.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        depthDescription.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        ComPtr<ID3D11Texture2D> depthTexture;
        Check(device->CreateTexture2D(&depthDescription, nullptr,
            depthTexture.GetAddressOf()), "Create depth target");
        ComPtr<ID3D11DepthStencilView> depthView;
        Check(device->CreateDepthStencilView(depthTexture.Get(), nullptr,
            depthView.GetAddressOf()), "Create depth view");

        D3D11_TEXTURE2D_DESC stagingDescription = colorDescription;
        stagingDescription.Usage = D3D11_USAGE_STAGING;
        stagingDescription.BindFlags = 0;
        stagingDescription.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
        ComPtr<ID3D11Texture2D> stagingTexture;
        Check(device->CreateTexture2D(&stagingDescription, nullptr,
            stagingTexture.GetAddressOf()), "Create staging texture");

        D3D11_RASTERIZER_DESC rasterizerDescription{};
        rasterizerDescription.FillMode = D3D11_FILL_SOLID;
        rasterizerDescription.CullMode = D3D11_CULL_BACK;
        rasterizerDescription.FrontCounterClockwise = FALSE;
        rasterizerDescription.DepthClipEnable = TRUE;
        ComPtr<ID3D11RasterizerState> rasterizer;
        Check(device->CreateRasterizerState(&rasterizerDescription,
            rasterizer.GetAddressOf()), "Create clockwise rasterizer");
        context->RSSetState(rasterizer.Get());

        D3D11_DEPTH_STENCIL_DESC depthStateDescription{};
        depthStateDescription.DepthEnable = TRUE;
        depthStateDescription.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        depthStateDescription.DepthFunc = D3D11_COMPARISON_LESS;
        ComPtr<ID3D11DepthStencilState> depthState;
        Check(device->CreateDepthStencilState(&depthStateDescription,
            depthState.GetAddressOf()), "Create depth state");
        context->OMSetDepthStencilState(depthState.Get(), 0);

        D3D11_BUFFER_DESC matrixDescription{};
        matrixDescription.ByteWidth = sizeof(dx::MatrixBuffer);
        matrixDescription.Usage = D3D11_USAGE_DEFAULT;
        matrixDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        ComPtr<ID3D11Buffer> matrixBuffer;
        Check(device->CreateBuffer(&matrixDescription, nullptr,
            matrixBuffer.GetAddressOf()), "Create matrix buffer");
        ID3D11Buffer* matrixPointer = matrixBuffer.Get();
        context->VSSetConstantBuffers(0, 1, &matrixPointer);

        dx::Shader shader(device.Get(),
            (sourceFolder / "VertexShader.hlsl").wstring(), "VSMain",
            (sourceFolder / "PixelShader.hlsl").wstring(), "PSMain");
        shader.Bind(context.Get());

        const D3D11_VIEWPORT viewport{ 0, 0,
            static_cast<float>(TileSize), static_cast<float>(TileSize), 0, 1 };
        context->RSSetViewports(1, &viewport);

        const std::array<const char*, ModelCount> names{
            "enemy", "elite", "machine-gun", "pistol", "grenades" };
        const std::array<float, ViewCount> yaw{
            -XM_PIDIV4, XM_PIDIV4, XM_PI + XM_PIDIV4 };
        constexpr float background[4]{ 0.025f, 0.035f, 0.05f, 1.0f };
        const UINT sheetWidth = TileSize * ViewCount;
        const UINT sheetHeight = TileSize * ModelCount;
        std::vector<std::uint8_t> sheet(
            static_cast<std::size_t>(sheetWidth) * sheetHeight * 4);

        for (UINT row = 0; row < ModelCount; ++row)
        {
            dx::ModelImportOptions options;
            options.Size = 2.0f;
            options.SizeAxis = row < 2 ? dx::ModelSizeAxis::Height : dx::ModelSizeAxis::Longest;
            options.Origin = dx::ModelOrigin::BottomCenter;
            options.BakeLighting = true;
            const dx::ModelAsset model = dx::LoadFbxModel(
                sourceFolder / "assets" / "models" / (std::string(names[row]) + ".fbx"), options);
            dx::Mesh mesh(device.Get(), model.Vertices, model.Indices);

            const XMVECTOR lower = XMLoadFloat3(&model.Min);
            const XMVECTOR upper = XMLoadFloat3(&model.Max);
            const XMVECTOR center = XMVectorScale(XMVectorAdd(lower, upper), 0.5f);
            const float radius = XMVectorGetX(XMVector3Length(XMVectorSubtract(upper, lower))) * 0.5f;
            if (!std::isfinite(radius) || radius <= 0.0f)
            {
                throw std::runtime_error("Invalid model bounds");
            }
            const float distance = radius / std::sin(XM_PIDIV4 * 0.5f) * 1.12f;
            mesh.Bind(context.Get());
            std::cout << names[row] << ": " << model.Vertices.size() << " vertices, "
                << model.Indices.size() / 3 << " triangles; pixels";

            for (UINT column = 0; column < ViewCount; ++column)
            {
                constexpr float pitch = 0.23f;
                const XMVECTOR offset = XMVectorSet(
                    std::sin(yaw[column]) * std::cos(pitch) * distance,
                    std::sin(pitch) * distance,
                    -std::cos(yaw[column]) * std::cos(pitch) * distance, 0);
                dx::MatrixBuffer matrices{};
                matrices.World = XMMatrixIdentity();
                matrices.View = XMMatrixTranspose(XMMatrixLookAtLH(
                    XMVectorAdd(center, offset), center, XMVectorSet(0, 1, 0, 0)));
                matrices.Projection = XMMatrixTranspose(XMMatrixPerspectiveFovLH(
                    XM_PIDIV4, 1.0f, radius * 0.01f, distance + radius * 3.0f));
                context->UpdateSubresource(matrixBuffer.Get(), 0, nullptr, &matrices, 0, 0);

                context->ClearRenderTargetView(colorView.Get(), background);
                context->ClearDepthStencilView(depthView.Get(),
                    D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
                ID3D11RenderTargetView* targetPointer = colorView.Get();
                context->OMSetRenderTargets(1, &targetPointer, depthView.Get());
                context->DrawIndexed(mesh.GetIndexCount(), 0, 0);
                context->OMSetRenderTargets(0, nullptr, nullptr);
                context->CopyResource(stagingTexture.Get(), colorTexture.Get());

                D3D11_MAPPED_SUBRESOURCE mapped{};
                Check(context->Map(stagingTexture.Get(), 0, D3D11_MAP_READ, 0,
                    &mapped), "Read rendered tile");
                const auto* pixels = static_cast<const std::uint8_t*>(mapped.pData);
                const std::array<std::uint8_t, 3> backgroundBytes{
                    pixels[0], pixels[1], pixels[2] };
                std::size_t visiblePixels = 0;
                for (UINT y = 0; y < TileSize; ++y)
                {
                    const auto* scanline = pixels + static_cast<std::size_t>(y) * mapped.RowPitch;
                    const auto destination = ((static_cast<std::size_t>(row) * TileSize + y) * sheetWidth
                        + static_cast<std::size_t>(column) * TileSize) * 4;
                    std::memcpy(sheet.data() + destination, scanline, TileSize * 4);
                    for (UINT x = 0; x < TileSize; ++x)
                    {
                        const auto* pixel = scanline + x * 4;
                        if (std::abs(static_cast<int>(pixel[0]) - backgroundBytes[0]) > 3 ||
                            std::abs(static_cast<int>(pixel[1]) - backgroundBytes[1]) > 3 ||
                            std::abs(static_cast<int>(pixel[2]) - backgroundBytes[2]) > 3)
                        {
                            ++visiblePixels;
                        }
                    }
                }
                context->Unmap(stagingTexture.Get(), 0);
                std::cout << ' ' << visiblePixels;
                if (visiblePixels < 300 || visiblePixels > TileSize * TileSize * 9 / 10)
                {
                    throw std::runtime_error(std::string("Empty or clipped render: ") + names[row]);
                }
            }
            std::cout << '\n';
        }

        CheckDebugMessages(debugQueue.Get());
        WriteBitmap(outputPath, sheetWidth, sheetHeight, sheet);
        std::cout << "PASS: 15 views with depth test and clockwise back-face culling.\n";
        std::wcout << L"Contact sheet: " << outputPath.wstring() << L'\n';
        return 0;
    }
    catch (const std::exception& error)
    {
        std::cerr << "FAIL: " << error.what() << '\n';
        return 1;
    }
}
