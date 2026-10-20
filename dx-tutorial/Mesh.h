#pragma once

#include "Vertex.h"
#include "Component.h"

#include <d3d11.h>
#include <wrl/client.h>

#include <span>
#include <cstdint>

namespace dx
{
    class Mesh : public Component
    {
    public:
        Mesh(
            ID3D11Device* device,
            std::span<const Vertex> vertices,
            std::span<const std::uint32_t> indices);

        inline UINT GetIndexCount() const noexcept
        {
            return mIndexCount;
        }

        void Bind(
            ID3D11DeviceContext* context,
            D3D11_PRIMITIVE_TOPOLOGY topologyType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST) const;

    private:
        Microsoft::WRL::ComPtr<ID3D11Buffer> mVertexBuffer;
        Microsoft::WRL::ComPtr<ID3D11Buffer> mIndexBuffer;

        UINT mStride = sizeof(Vertex);
        UINT mIndexCount = 0;
    };
}