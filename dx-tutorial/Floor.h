#pragma once
#include "Actor.h"

namespace core
{
    class Floor : public Actor
    {
    public:
        Floor(ID3D11Device* device);
        virtual ~Floor();

        virtual void Update(float deltaTime) override;
        virtual void Render(
            ID3D11DeviceContext* context,
            ID3D11Buffer* matrixBuf,
            const DirectX::XMMATRIX& view,
            const DirectX::XMMATRIX& proj) override;

    private:
        ID3D11Buffer* mVertexBuf = nullptr;
    };
}