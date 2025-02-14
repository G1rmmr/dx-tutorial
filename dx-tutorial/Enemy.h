#pragma once

#include <DirectXMath.h>

#include <bullet/btBulletCollisionCommon.h>
#include <bullet/btBulletDynamicsCommon.h>

namespace core
{
	class Enemy
	{
	public:
		Enemy();

		void SyncPhysics();

		inline btRigidBody* GetRigidBody() const
		{
			return mRigidBody;
		}

		inline DirectX::XMFLOAT3 GetPos() const
		{
			return mPos;
		}

	private:
		DirectX::XMFLOAT3 mPos;

		btRigidBody* mRigidBody;
		btCollisionShape* mCollisionShape;
		btDefaultMotionState* mMotionState;
	};
}