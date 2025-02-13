#pragma once

#include <DirectXMath.h>

#include <bullet/btBulletCollisionCommon.h>
#include <bullet/btBulletDynamicsCommon.h>

namespace core
{
	class Player
	{
	public:
        Player();

        void ProcessMouseMovement(float xOffset, float yOffset);

		void ProcessKeyboard(btDynamicsWorld* world, const float deltaTime,
			bool forwardKey, bool backKey, bool leftKey, bool rightKey, bool jumpKey);

        DirectX::XMMATRIX GetViewMatrix() const;
		void UpdateCameraVectors();

		void SyncPhysics();

		inline btRigidBody* GetRigidBody() const
		{
			return mRigidBody;
		}

	private:
		bool isOnGround(btDynamicsWorld* world, float rayLength = 1.f);

		void jump(const float velocity = 3.f);

	private:
		DirectX::XMFLOAT3 mPos;

		DirectX::XMFLOAT3 mForward;
		DirectX::XMFLOAT3 mRight;
		DirectX::XMFLOAT3 mUp;

		btRigidBody* mRigidBody;
		btCollisionShape* mCollisionShape;
		btDefaultMotionState* mMotionState;

		float mYaw;
		float mPitch;

		float mMovementSpeed;
		float mMouseSensitivity;
	};
}