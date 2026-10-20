#pragma once

#include "Component.h"

#include <DirectXMath.h>

namespace dx
{
    struct Transform : public Component
    {
        DirectX::XMFLOAT3 Position{ 0.f, 0.f, 0.f };
        DirectX::XMFLOAT3 Rotation{0.f, 0.f, 0.f};
        DirectX::XMFLOAT3 Scale{ 1.f, 1.f, 1.f };

        inline DirectX::XMMATRIX GetWorld() const
        {
            DirectX::XMMATRIX scale = DirectX::XMMatrixScaling(
                Scale.x,
                Scale.y,
                Scale.z
            );

            DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationRollPitchYaw(
                Rotation.x,
                Rotation.y,
                Rotation.z
            );

            DirectX::XMMATRIX translation = DirectX::XMMatrixTranslation(
                Position.x,
                Position.y,
                Position.z
            );

            return scale * rotation * translation;
        }
    };
}
