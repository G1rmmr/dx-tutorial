#pragma once

#include "Object.h"
#include "Vertex.h"

#include <DirectXMath.h>
#include <vector>

namespace dx
{
    class Floor : public Object
    {
    public:
        Floor(
            const DirectX::XMFLOAT3& position,
            const DirectX::XMFLOAT3& size,
            const DirectX::XMFLOAT4& color);

        void Update(const float /*deltaTime*/) override
        {
            return;
        }

        inline const std::vector<Vertex>& GetVertices() const
        {
            return mVertices;
        }

    private:
        std::vector<Vertex> mVertices;
    };
}