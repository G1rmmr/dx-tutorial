#pragma once
#include "Component.h"
#include "Transform.h"
#include "Collision.h"
#include <bullet/btBulletDynamicsCommon.h>
#include <DirectXMath.h>
#include <memory>
namespace dx
{
    class Physics;
    class Rigidbody : public Component
    {
    public:
        Rigidbody(Transform& transform, Collision& collision, float mass, float friction);
        ~Rigidbody() override;
        Rigidbody(const Rigidbody&) = delete;
        Rigidbody& operator=(const Rigidbody&) = delete;
        DirectX::XMFLOAT3 Sync(const DirectX::XMFLOAT3& currentPosition) const;
        btRigidBody* GetInstance() noexcept { return mInstance.get(); }
        const btRigidBody* GetInstance() const noexcept { return mInstance.get(); }
    private:
        friend class Physics;
        void AttachToWorld(const std::shared_ptr<btDiscreteDynamicsWorld>& world);
        void RemoveFromWorld() noexcept;
        std::shared_ptr<btCollisionShape> mShape;
        std::unique_ptr<btRigidBody> mInstance;
        std::weak_ptr<btDiscreteDynamicsWorld> mWorld;
    };
}
