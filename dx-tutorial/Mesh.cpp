#include "Mesh.h"

#include <stdexcept>
#include <limits>

using namespace dx;
using namespace std;
using namespace Microsoft::WRL;

Mesh::Mesh(
    ID3D11Device* device,
    span<const Vertex> vertices,
    span<const uint32_t> indices)
    : mIndexCount(static_cast<UINT>(indices.size()))
{
    if (!device || vertices.empty() || indices.empty() ||
        vertices.size_bytes() > (std::numeric_limits<UINT>::max)() ||
        indices.size_bytes() > (std::numeric_limits<UINT>::max)())
    {
        throw std::invalid_argument("Mesh requires a device and nonempty buffers of valid size");
    }

    // 정점 버퍼 선언
    D3D11_BUFFER_DESC vertexBufferDescriptor{};
    vertexBufferDescriptor.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDescriptor.ByteWidth = static_cast<UINT>(vertices.size_bytes());
    vertexBufferDescriptor.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA vertexSubresourceData{};
    vertexSubresourceData.pSysMem = vertices.data();

    if (FAILED(device->CreateBuffer(
        &vertexBufferDescriptor,
        &vertexSubresourceData,
        mVertexBuffer.GetAddressOf())))
    {
        throw runtime_error("Failed to create vertex buffer");
    }

    // 색인 버퍼 선언
    D3D11_BUFFER_DESC indexBufferDescriptor{};
    indexBufferDescriptor.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDescriptor.ByteWidth = static_cast<UINT>(indices.size_bytes());
    indexBufferDescriptor.BindFlags = D3D11_BIND_INDEX_BUFFER;

    D3D11_SUBRESOURCE_DATA indexSubresourceData{};
    indexSubresourceData.pSysMem = indices.data();

    if (FAILED(device->CreateBuffer(
        &indexBufferDescriptor,
        &indexSubresourceData,
        mIndexBuffer.GetAddressOf())))
    {
        throw runtime_error("Failed to create index buffer");
    }
}

void Mesh::Bind(
    ID3D11DeviceContext* context,
    D3D11_PRIMITIVE_TOPOLOGY topologyType) const
{
    if (!context)
    {
        throw std::invalid_argument("Mesh binding requires a context");
    }
    UINT vertextOffset = 0;
    UINT startSlot = 0;
    UINT numBuffer = 1;

    context->IASetVertexBuffers(
        startSlot,
        numBuffer,
        mVertexBuffer.GetAddressOf(),
        &mStride,
        &vertextOffset);

    UINT indexOffset = 0;
    context->IASetIndexBuffer(
        mIndexBuffer.Get(),
        DXGI_FORMAT_R32_UINT,
        indexOffset);

    context->IASetPrimitiveTopology(topologyType);
}