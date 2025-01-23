#include "FPSCamera.h"

#include <algorithm>

core::FPSCamera::FPSCamera()
    : mPos(0.0f, 1.6f, 0.0f)
    , mYaw(0.0f)
    , mPitch(0.0f)
    , mMoveSpeed(0.1f)
    , mMouseSensitivity(0.002f)
{
    mForward = DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f);
    mRight = DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f);
    mUp = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
}

void core::FPSCamera::MoveForward(float delta)
{
    DirectX::XMVECTOR forward = DirectX::XMLoadFloat3(&mForward);
    DirectX::XMVECTOR pos = DirectX::XMLoadFloat3(&mPos);
    
    DirectX::XMStoreFloat3(&mPos,
        DirectX::XMVectorAdd(pos,
        DirectX::XMVectorScale(forward, delta * mMoveSpeed)));
}

void core::FPSCamera::MoveRight(float delta)
{
    DirectX::XMVECTOR right = DirectX::XMLoadFloat3(&mRight);
    DirectX::XMVECTOR pos = DirectX::XMLoadFloat3(&mPos);
    
    DirectX::XMStoreFloat3(&mPos,
        DirectX::XMVectorAdd(pos,
        DirectX::XMVectorScale(right, delta * mMoveSpeed)));
}

void core::FPSCamera::Rotate(float dx, float dy)
{
    mYaw += dx * mMouseSensitivity;

    mPitch += dy * mMouseSensitivity;
    mPitch = std::max(-1.5f, std::min(1.5f, mPitch));

    DirectX::XMVECTOR dir = DirectX::XMVectorSet(
        cosf(mPitch) * sinf(mYaw),
        sinf(mPitch),
        cosf(mPitch) * cosf(mYaw),
        0.f
    );

    DirectX::XMStoreFloat3(&mForward, dir);
    DirectX::XMStoreFloat3(&mRight,
        DirectX::XMVector3Cross(dir,
        DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)));
}

DirectX::XMMATRIX core::FPSCamera::GetViewMatrix()
{
    return DirectX::XMMatrixLookToLH(
        DirectX::XMLoadFloat3(&mPos),
        DirectX::XMLoadFloat3(&mForward),
        DirectX::XMLoadFloat3(&mUp));
}
