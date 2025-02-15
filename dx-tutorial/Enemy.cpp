#include "Enemy.h"

using namespace core;
using namespace DirectX;

Enemy::Enemy()
	: mPos({3.f, 3.f, 3.f})
	, mRigidBody(nullptr)
	, mCollisionShape(nullptr)
	, mMotionState(nullptr)
{
	btTransform startTransform;
	startTransform.setIdentity();
	startTransform.setOrigin(btVector3(3.0f, 3.0f, 3.0f));

	mCollisionShape = new btBoxShape(btVector3(0.5f, 1.0f, 0.5f));

	btScalar mass = 80.f;
	bool isDynamic = (mass != 0.0f);

	btVector3 localInertia(0, 0, 0);
	if(isDynamic)
	{
		mCollisionShape->calculateLocalInertia(mass, localInertia);
	}

	mMotionState = new btDefaultMotionState(startTransform);

	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, mMotionState, mCollisionShape, localInertia);
	mRigidBody = new btRigidBody(rbInfo);
}

void Enemy::SyncPhysics()
{
	if(mRigidBody && mRigidBody->getMotionState())
	{
		btTransform trans;
		mRigidBody->getMotionState()->getWorldTransform(trans);
		btVector3 origin = trans.getOrigin();
		mPos.x = origin.getX();
		mPos.y = origin.getY();
		mPos.z = origin.getZ();
	}
}
