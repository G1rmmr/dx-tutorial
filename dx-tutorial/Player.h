#pragma once
#include "Object.h"
#include "Camera.h"
#include <bullet/btBulletDynamicsCommon.h>
namespace dx
{
    class Player : public Object
    {
    public:
        Player();
        void Update(float deltaTime) override;
        void SyncPhysics();
        void ProcessMouseMovement(float xOffset, float yOffset);
        void ProcessKeyboard(btDynamicsWorld* world, float deltaTime,
            bool forwardKey, bool backKey, bool leftKey, bool rightKey, bool jumpKey);
        bool Shoot(btDiscreteDynamicsWorld* world);
        DirectX::XMMATRIX GetViewMatrix() const;
        void UpdateCameraVectors();
        Camera& GetCamera() noexcept;
        const Camera& GetCamera() const noexcept;
    private:
        bool isOnGround(btDynamicsWorld* world, float rayLength = 0.15f);
        void jump(btVector3& velocity, float jumpVelocity = 5.f);
        float mYaw = 0.f;
        float mPitch = 0.f;
        float mMovementSpeed = 5.f;
        float mMouseSensitivity = 0.1f;
        bool bIsOnGround = false;
        bool mJumpWasDown = false;
    };
}
