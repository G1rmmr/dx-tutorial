#pragma once
#include <DirectXMath.h>
#include <d3d11.h>

namespace core
{
    class Actor
    {
    public:
        Actor() = default;
        virtual ~Actor() = default;

        virtual void Update(float deltaTime) = 0;

        virtual void Render(
            ID3D11DeviceContext* context,
            ID3D11Buffer* matrixBuf,
            const DirectX::XMMATRIX& view,
            const DirectX::XMMATRIX& proj) = 0;

        inline DirectX::XMFLOAT3 GetPos() const
        {
            return mPos;
        }

        inline void SetPos(const DirectX::XMFLOAT3& pos)
        {
            mPos = pos;
        }

    protected:
        DirectX::XMFLOAT3 mPos = {0.f, 0.f, 0.f};
    };
}