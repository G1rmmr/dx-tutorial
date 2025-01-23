#pragma once

#include <DirectXMath.h>

namespace core
{
    class FPSCamera
    {
    public:
        FPSCamera();

        void MoveForward(float delta);
        void MoveRight(float delta);
        void Rotate(float dx, float dy);

        DirectX::XMMATRIX GetViewMatrix();

    private:
        DirectX::XMFLOAT3 mPos;
        DirectX::XMFLOAT3 mForward;
        DirectX::XMFLOAT3 mRight;
        DirectX::XMFLOAT3 mUp;

        float mYaw;
        float mPitch;
        float mMoveSpeed;
        float mMouseSensitivity;
    };
}