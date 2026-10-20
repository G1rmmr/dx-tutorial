#pragma once
#include "Component.h"
#include "Rigidbody.h"
#include <bullet/btBulletDynamicsCommon.h>
#include <cmath>
#include <memory>
namespace dx
{
    class Physics : public Component
    {
    public:
        explicit Physics(float gravity = 9.81f)
        {
            mCollisionConfig = std::make_unique<btDefaultCollisionConfiguration>();
            mDispatcher = std::make_unique<btCollisionDispatcher>(mCollisionConfig.get());
            mBroadphase = std::make_unique<btDbvtBroadphase>();
            mSolver = std::make_unique<btSequentialImpulseConstraintSolver>();
            mDynamics = std::make_shared<btDiscreteDynamicsWorld>(
                mDispatcher.get(), mBroadphase.get(), mSolver.get(), mCollisionConfig.get());
            mDynamics->setGravity(btVector3(0.f, -gravity, 0.f));
        }
        ~Physics() override
        {
            ClearBodies();
            mDynamics.reset();
        }
        Physics(const Physics&) = delete;
        Physics& operator=(const Physics&) = delete;
        btDiscreteDynamicsWorld& GetDynamics() noexcept { return *mDynamics; }
        const btDiscreteDynamicsWorld& GetDynamics() const noexcept { return *mDynamics; }
        void AddRigidBody(Rigidbody& body) { body.AttachToWorld(mDynamics); }
        void RemoveRigidBody(Rigidbody& body) noexcept
        {
            if (body.mWorld.lock() == mDynamics)
            {
                body.RemoveFromWorld();
            }
        }
        void ClearBodies() noexcept
        {
            while (mDynamics && mDynamics->getNumCollisionObjects() > 0)
            {
                auto& objects = mDynamics->getCollisionObjectArray();
                mDynamics->removeCollisionObject(objects[objects.size() - 1]);
            }
        }
        void Update(float deltaTime)
        {
            if (!std::isfinite(deltaTime) || deltaTime <= 0.f)
            {
                return;
            }
            constexpr double fixedTimeStep = 1.0 / 60.0;
            mTimeAccumulator += deltaTime;
            // Keep the backlog when a slow frame needs more than eight substeps.
            for (int step = 0; step < 8 && mTimeAccumulator >= fixedTimeStep; ++step)
            {
                mDynamics->stepSimulation(btScalar(fixedTimeStep), 0);
                mTimeAccumulator -= fixedTimeStep;
            }
        }
    private:
        std::unique_ptr<btDefaultCollisionConfiguration> mCollisionConfig;
        std::unique_ptr<btCollisionDispatcher> mDispatcher;
        std::unique_ptr<btBroadphaseInterface> mBroadphase;
        std::unique_ptr<btSequentialImpulseConstraintSolver> mSolver;
        std::shared_ptr<btDiscreteDynamicsWorld> mDynamics;
        double mTimeAccumulator = 0.0;
    };
}
