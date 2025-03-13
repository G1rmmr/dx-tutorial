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
		Enemy(ID3D11Device* device, const std::string& fbxPath);

		~Enemy() override;

		void Update(const float deltaTime) override;

		void Render(
			ID3D11DeviceContext* context,
			ID3D11Buffer* matrixBuf,
			const DirectX::XMMATRIX& view,
			const DirectX::XMMATRIX& proj) override;

		void SyncPhysics() override;

		inline btRigidBody* GetRigidBody() const
		{
			return mRigidBody;
		}

	private:
		void loadModel(ID3D11Device* device, const std::string& filePath);

		ID3D11Buffer* mVertexBuf;
		UINT mVertexCount;

		btRigidBody* mRigidBody;
		btCollisionShape* mCollisionShape;
		btDefaultMotionState* mMotionState;
	};
}