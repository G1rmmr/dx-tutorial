#include "Player.h"

using namespace core;
using namespace DirectX;

Player::Player()
	: mPos(0.f, 0.f, 0.f)
	, mYaw(0.f)
	, mPitch(0.f)
	, mForward(0.f, 0.f, 1.f)
	, mRight(1.f, 0.f, 0.f)
	, mUp(0.f, 1.f, 0.f)
	, mMovementSpeed(5.f)
	, mMouseSensitivity(0.1f)
	, mRigidBody(nullptr)
	, mCollisionShape(nullptr)
	, mMotionState(nullptr)
{
	btTransform startTransform;
	startTransform.setIdentity();
	startTransform.setOrigin(btVector3(0.0f, 3.0f, 0.0f));

	mCollisionShape = new btBoxShape(btVector3(0.5f, 1.0f, 0.5f));

	btScalar mass = 80.0f;
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

void Player::ProcessMouseMovement(float xOffset, float yOffset)
{
	xOffset *= mMouseSensitivity;
	yOffset *= mMouseSensitivity;

	mYaw -= xOffset;
	mPitch += yOffset;

	if(mPitch > 89.f)
	{
		mPitch = 89.f;
	}

	if(mPitch < -89.f)
	{
		mPitch = -89.f;
	}

	UpdateCameraVectors();
}

void Player::ProcessKeyboard(bool forwardKey, bool backKey, bool leftKey, bool rightKey, float deltaTime)
{
	btVector3 moveDir(0, 0, 0);

	if(forwardKey)
	{
		moveDir += btVector3(mForward.x, 0.f, mForward.z);
	}
		
	if(backKey)
	{
		moveDir -= btVector3(mForward.x, 0.f, mForward.z);
	}
		
	if(leftKey)
	{
		moveDir -= btVector3(mRight.x, 0.f, mRight.z);
	}
		
	if(rightKey)
	{
		moveDir += btVector3(mRight.x, 0.f, mRight.z);
	}
		

	if(moveDir.length2() > 0)
	{
		moveDir.normalize();
		float speed = mMovementSpeed;
		btVector3 velocity = moveDir * speed;
		mRigidBody->setLinearVelocity(velocity);
	}
	else
	{
		mRigidBody->setLinearVelocity(btVector3(0, mRigidBody->getLinearVelocity().getY(), 0));
	}
}

XMMATRIX Player::GetViewMatrix() const
{
	XMVECTOR pos = XMLoadFloat3(&mPos);
	XMVECTOR forward = XMLoadFloat3(&mForward);
	XMVECTOR up = XMLoadFloat3(&mUp);

	return XMMatrixLookAtLH(pos, XMVectorAdd(pos, forward), up);
}

void Player::UpdateCameraVectors()
{
	float radPitch = XMConvertToRadians(mPitch);
	float radYaw = XMConvertToRadians(mYaw);

	XMVECTOR forward;
	forward = XMVectorSet(
		cosf(radYaw) * cosf(radPitch),
		sinf(radPitch),
		sinf(radYaw) * cosf(radPitch),
		0.f);

	forward = XMVector3Normalize(forward);

	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR right = XMVector3Normalize(XMVector3Cross(up, forward));

	up = XMVector3Cross(forward, right);

	XMStoreFloat3(&mForward, forward);
	XMStoreFloat3(&mRight, right);
	XMStoreFloat3(&mUp, up);
}

void Player::SyncPhysics()
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
