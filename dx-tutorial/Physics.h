#pragma once

#include <bullet/btBulletCollisionCommon.h>
#include <bullet/btBulletDynamicsCommon.h>

namespace core
{
    class Physics
    {
    public:
        Physics();
        ~Physics();

        void Update(const float deltaTime);

        void AddRigidBody(btRigidBody* body);

        inline btDiscreteDynamicsWorld* GetDynamicsWorld() const
        {
            return mDynamicsWorld;
        }

    private:
        btBroadphaseInterface* mBroadphase;
        btDefaultCollisionConfiguration* mCollisionConfiguration;
        btCollisionDispatcher* mDispatcher;
        btSequentialImpulseConstraintSolver* mSolver;
        btDiscreteDynamicsWorld* mDynamicsWorld;
    };
}
