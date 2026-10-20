#include "Camera.h"
#include <algorithm>
#include <cmath>
#include <stdexcept>
using namespace dx;
using namespace DirectX;

void Camera::SetPerspective(float fovY, float aspect, float nearZ, float farZ)
{
    if (!std::isfinite(fovY) || !std::isfinite(aspect) || !std::isfinite(nearZ) || !std::isfinite(farZ) ||
        fovY <= 0.f || fovY >= XM_PI || aspect <= 0.f || nearZ <= 0.f || farZ <= nearZ)
    {
        throw std::invalid_argument("Invalid camera perspective");
    }
    mFov = fovY;
    mAspect = aspect;
    mNear = nearZ;
    mFar = farZ;
}
void Camera::SetPosition(const XMFLOAT3& position)
{
    mPosition = position;
}
void Camera::SetRotation(float pitch, float yaw, float roll)
{
    if (!std::isfinite(pitch) || !std::isfinite(yaw) || !std::isfinite(roll))
    {
        throw std::invalid_argument("Invalid camera rotation");
    }
    constexpr float pitchLimit = XM_PIDIV2 - 0.01f;
    mPitch = std::clamp(pitch, -pitchLimit, pitchLimit);
    mYaw = std::remainder(yaw, XM_2PI);
    mRoll = std::remainder(roll, XM_2PI);
}
void Camera::Move(const XMFLOAT3& delta)
{
    mPosition.x += delta.x;
    mPosition.y += delta.y;
    mPosition.z += delta.z;
}
void Camera::Rotate(float deltaPitch, float deltaYaw, float deltaRoll)
{
    SetRotation(mPitch + deltaPitch, mYaw + deltaYaw, mRoll + deltaRoll);
}
XMMATRIX Camera::GetView() const
{
    const XMMATRIX rotation = XMMatrixRotationRollPitchYaw(mPitch, mYaw, mRoll);
    const XMVECTOR up = XMVector3TransformNormal(XMVectorSet(0, 1, 0, 0), rotation);
    return XMMatrixLookToLH(XMLoadFloat3(&mPosition), GetForward(), up);
}
XMMATRIX Camera::GetProjection() const
{
    return XMMatrixPerspectiveFovLH(mFov, mAspect, mNear, mFar);
}
XMVECTOR Camera::GetForward() const
{
    return XMVector3TransformNormal(
        XMVectorSet(0, 0, 1, 0), XMMatrixRotationRollPitchYaw(mPitch, mYaw, mRoll));
}
XMVECTOR Camera::GetRight() const
{
    return XMVector3TransformNormal(
        XMVectorSet(1, 0, 0, 0), XMMatrixRotationRollPitchYaw(mPitch, mYaw, mRoll));
}
