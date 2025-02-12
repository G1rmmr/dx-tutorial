#include "Physics.h"

using namespace core;

Physics::Physics()
    : mBroadphase(nullptr)
    , mCollisionConfiguration(nullptr)
    , mDispatcher(nullptr)
    , mSolver(nullptr)
    , mDynamicsWorld(nullptr)
{
    mBroadphase = new btDbvtBroadphase();

    mCollisionConfiguration = new btDefaultCollisionConfiguration();
    mDispatcher = new btCollisionDispatcher(mCollisionConfiguration);

    mSolver = new btSequentialImpulseConstraintSolver();

    mDynamicsWorld = new btDiscreteDynamicsWorld(
        mDispatcher, mBroadphase, mSolver, mCollisionConfiguration);

    mDynamicsWorld->setGravity(btVector3(0, -9.8f, 0));
}

Physics::~Physics()
{
    delete mDynamicsWorld;
    delete mSolver;
    delete mDispatcher;
    delete mCollisionConfiguration;
    delete mBroadphase;
}

void Physics::Update(const float deltaTime)
{
    mDynamicsWorld->stepSimulation(deltaTime);
}

void Physics::AddRigidBody(btRigidBody* body)
{
    mDynamicsWorld->addRigidBody(body);
}