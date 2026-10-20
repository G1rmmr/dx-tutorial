#pragma once
#include "Component.h"
#include <DirectXMath.h>
namespace dx
{
    class Camera : public Component
    {
    public:
        // Perspective and rotation angles use radians.
        void SetPerspective(float fovY, float aspect, float nearZ, float farZ);
        void SetPosition(const DirectX::XMFLOAT3& position);
        void SetRotation(float pitch, float yaw, float roll);
        void Move(const DirectX::XMFLOAT3& delta);
        void Rotate(float deltaPitch, float deltaYaw, float deltaRoll);
        const DirectX::XMFLOAT3& GetPosition() const noexcept { return mPosition; }
        DirectX::XMMATRIX GetView() const;
        DirectX::XMMATRIX GetProjection() const;
        DirectX::XMVECTOR GetForward() const;
        DirectX::XMVECTOR GetRight() const;
    private:
        DirectX::XMFLOAT3 mPosition{ 0.f, 0.f, 0.f };
        float mPitch = 0.f;
        float mYaw = 0.f;
        float mRoll = 0.f;
        float mFov = DirectX::XM_PIDIV4;
        float mAspect = 16.f / 9.f;
        float mNear = 0.1f;
        float mFar = 1000.f;
    };
}
