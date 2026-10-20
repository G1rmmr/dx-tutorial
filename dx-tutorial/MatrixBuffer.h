#pragma once

#include <DirectXMath.h>

namespace dx
{
    struct MatrixBuffer
    {
        DirectX::XMMATRIX World;
        DirectX::XMMATRIX View;
        DirectX::XMMATRIX Projection;
    };
}