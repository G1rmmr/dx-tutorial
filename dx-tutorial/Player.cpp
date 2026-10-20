#include "Player.h"
#include "Collision.h"
#include "Rigidbody.h"
#include "Transform.h"
#include <algorithm>
#include <cmath>
using namespace dx;
using namespace DirectX;

namespace
{
    class IgnoreBodyRayCallback : public btCollisionWorld::ClosestRayResultCallback
    {
    public:
        IgnoreBodyRayCallback(const btVector3& start, const btVector3& end, const btCollisionObject* ignored)
            : ClosestRayResultCallback(start, end), mIgnored(ignored) {}
        bool needsCollision(btBroadphaseProxy* proxy) const override
        {
            return proxy->m_clientObject != mIgnored && ClosestRayResultCallback::needsCollision(proxy);
        }
    private:
        const btCollisionObject* mIgnored;
    };
}
Player::Player()
{
    Transform& transform = EmplaceComponent<Transform>();
    transform.Position = XMFLOAT3{ 0.f, 3.f, 0.f };
    Collision& collision = EmplaceComponent<Collision>(
        Collision::Create<btBoxShape>(btVector3(0.5f, 1.f, 0.5f)));
    Rigidbody& body = EmplaceComponent<Rigidbody>(transform, collision, 80.f, 0.3f);
    body.GetInstance()->setAngularFactor(btVector3(0, 0, 0));
    EmplaceComponent<Camera>();
    SyncPhysics();
    UpdateCameraVectors();
}
void Player::Update(float /*deltaTime*/)
{
    SyncPhysics();
}
void Player::SyncPhysics()
{
    Transform* transform = GetComponent<Transform>();
    transform->Position = GetComponent<Rigidbody>()->Sync(transform->Position);
    XMFLOAT3 eye = transform->Position;
    eye.y += 0.5f;
    GetCamera().SetPosition(eye);
}
Camera& Player::GetCamera() noexcept
{
    return *GetComponent<Camera>();
}
const Camera& Player::GetCamera() const noexcept
{
    return *GetComponent<Camera>();
}
void Player::ProcessMouseMovement(float xOffset, float yOffset)
{
    mYaw = std::remainder(mYaw + XMConvertToRadians(xOffset * mMouseSensitivity), XM_2PI);
    constexpr float pitchLimit = XM_PIDIV2 - 0.01f;
    mPitch = std::clamp(mPitch + XMConvertToRadians(yOffset * mMouseSensitivity), -pitchLimit, pitchLimit);
    UpdateCameraVectors();
}
void Player::UpdateCameraVectors()
{
    GetCamera().SetRotation(mPitch, mYaw, 0.f);
}
XMMATRIX Player::GetViewMatrix() const
{
    return GetCamera().GetView();
}
void Player::ProcessKeyboard(btDynamicsWorld* world, float /*deltaTime*/,
    bool forwardKey, bool backKey, bool leftKey, bool rightKey, bool jumpKey)
{
    btRigidBody* body = GetComponent<Rigidbody>()->GetInstance();
    bIsOnGround = isOnGround(world);
    const bool jumpPressed = jumpKey && !mJumpWasDown;
    mJumpWasDown = jumpKey;
    if (!bIsOnGround)
    {
        return;
    }
    XMFLOAT3 forward;
    XMFLOAT3 right;
    XMStoreFloat3(&forward, GetCamera().GetForward());
    XMStoreFloat3(&right, GetCamera().GetRight());
    btVector3 forwardXZ(forward.x, 0, forward.z);
    btVector3 rightXZ(right.x, 0, right.z);
    forwardXZ.normalize();
    rightXZ.normalize();
    btVector3 movement(0, 0, 0);
    if (forwardKey) movement += forwardXZ;
    if (backKey) movement -= forwardXZ;
    if (leftKey) movement -= rightXZ;
    if (rightKey) movement += rightXZ;
    if (movement.length2() > SIMD_EPSILON)
    {
        movement.normalize();
        movement *= mMovementSpeed;
    }
    btVector3 velocity(movement.x(), body->getLinearVelocity().y(), movement.z());
    if (jumpPressed)
    {
        jump(velocity);
    }
    body->setLinearVelocity(velocity);
}
bool Player::isOnGround(btDynamicsWorld* world, float rayLength)
{
    const btRigidBody* body = GetComponent<Rigidbody>()->GetInstance();
    if (!world || body->getLinearVelocity().y() > 0.1f)
    {
        return false;
    }
    const btVector3 start = body->getWorldTransform().getOrigin();
    const btVector3 end = start - btVector3(0, 1.f + rayLength, 0);
    IgnoreBodyRayCallback callback(start, end, body);
    world->rayTest(start, end, callback);
    return callback.hasHit() && callback.m_hitNormalWorld.y() > 0.6f;
}
void Player::jump(btVector3& velocity, float jumpVelocity)
{
    velocity.setY(jumpVelocity);
    bIsOnGround = false;
}
bool Player::Shoot(btDiscreteDynamicsWorld* world)
{
    if (!world)
    {
        return false;
    }
    SyncPhysics();
    const XMFLOAT3& position = GetCamera().GetPosition();
    XMFLOAT3 forward;
    XMStoreFloat3(&forward, GetCamera().GetForward());
    const btVector3 direction(forward.x, forward.y, forward.z);
    const btVector3 start(position.x, position.y, position.z);
    const btVector3 end = start + direction * 100.f;
    IgnoreBodyRayCallback callback(start, end, GetComponent<Rigidbody>()->GetInstance());
    world->rayTest(start, end, callback);
    return callback.hasHit();
}
