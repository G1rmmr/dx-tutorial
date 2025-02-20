#pragma once

#include "Actor.h"

#include <bullet/btBulletCollisionCommon.h>
#include <bullet/btBulletDynamicsCommon.h>

namespace core
{
	class Enemy : public Actor
	{
	public:
		Enemy();
		Enemy(ID3D11Device* device);

		virtual void Update(float deltaTime) override;

		virtual void Render(
			ID3D11DeviceContext* context,
			ID3D11Buffer* matrixBuf,
			const DirectX::XMMATRIX& view,
			const DirectX::XMMATRIX& proj) override;

		void SyncPhysics();

		inline btRigidBody* GetRigidBody() const
		{
			return mRigidBody;
		}

	private:
		ID3D11Buffer* mVertexBuf;

		btRigidBody* mRigidBody;
		btCollisionShape* mCollisionShape;
		btDefaultMotionState* mMotionState;
	};
}