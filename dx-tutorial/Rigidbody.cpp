#include "Rigidbody.h"
#include <cmath>
#include <stdexcept>
using namespace dx;
using namespace DirectX;

Rigidbody::Rigidbody(Transform& transform, Collision& collision, float mass, float friction)
    : mShape(collision.mShape)
{
    if (!mShape || !std::isfinite(mass) || mass < 0.f || !std::isfinite(friction) || friction < 0.f)
    {
        throw std::invalid_argument("Invalid rigid body parameters");
    }
    mShape->setLocalScaling(btVector3(transform.Scale.x, transform.Scale.y, transform.Scale.z));
    btVector3 localInertia(0, 0, 0);
    if (mass > 0.f)
    {
        mShape->calculateLocalInertia(mass, localInertia);
    }
    btRigidBody::btRigidBodyConstructionInfo info(mass, nullptr, mShape.get(), localInertia);
    mInstance = std::make_unique<btRigidBody>(info);

    XMFLOAT4 quaternion;
    XMStoreFloat4(&quaternion, XMQuaternionRotationRollPitchYaw(
        transform.Rotation.x, transform.Rotation.y, transform.Rotation.z));
    btTransform startTransform;
    startTransform.setIdentity();
    startTransform.setOrigin(btVector3(transform.Position.x, transform.Position.y, transform.Position.z));
    startTransform.setRotation(btQuaternion(quaternion.x, quaternion.y, quaternion.z, quaternion.w));
    mInstance->setWorldTransform(startTransform);
    mInstance->setFriction(friction);
    mInstance->setDamping(0.f, 0.f);
    if (mass > 0.f)
    {
        mInstance->setActivationState(DISABLE_DEACTIVATION);
    }
}
Rigidbody::~Rigidbody()
{
    RemoveFromWorld();
}
XMFLOAT3 Rigidbody::Sync(const XMFLOAT3& currentPosition) const
{
    if (!mInstance)
    {
        return currentPosition;
    }
    const btVector3& position = mInstance->getWorldTransform().getOrigin();
    return XMFLOAT3{ position.x(), position.y(), position.z() };
}
void Rigidbody::AttachToWorld(const std::shared_ptr<btDiscreteDynamicsWorld>& world)
{
    if (mWorld.lock() == world && mInstance->getBroadphaseHandle())
    {
        return;
    }
    RemoveFromWorld();
    world->addRigidBody(mInstance.get());
    mWorld = world;
}
void Rigidbody::RemoveFromWorld() noexcept
{
    if (const auto world = mWorld.lock(); world && mInstance->getBroadphaseHandle())
    {
        world->removeRigidBody(mInstance.get());
    }
    mWorld.reset();
}
