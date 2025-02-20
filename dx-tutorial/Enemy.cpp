#include "Enemy.h"

using namespace core;
using namespace DirectX;

struct Vertex
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
};

static Vertex cubeVertices[] =
{
    // 앞면 (Front)
    { XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT4(1, 0, 0, 1) },
    { XMFLOAT3(-0.5f,  0.5f, -0.5f), XMFLOAT4(1, 0, 0, 1) },
    { XMFLOAT3(0.5f,  0.5f, -0.5f), XMFLOAT4(1, 0, 0, 1) },

    { XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT4(1, 0, 0, 1) },
    { XMFLOAT3(0.5f,  0.5f, -0.5f), XMFLOAT4(1, 0, 0, 1) },
    { XMFLOAT3(0.5f, -0.5f, -0.5f), XMFLOAT4(1, 0, 0, 1) },

    // 뒷면 (Back)
    { XMFLOAT3(-0.5f, -0.5f,  0.5f), XMFLOAT4(0, 1, 0, 1) },
    { XMFLOAT3(0.5f,  0.5f,  0.5f), XMFLOAT4(0, 1, 0, 1) },
    { XMFLOAT3(-0.5f,  0.5f,  0.5f), XMFLOAT4(0, 1, 0, 1) },

    { XMFLOAT3(-0.5f, -0.5f,  0.5f), XMFLOAT4(0, 1, 0, 1) },
    { XMFLOAT3(0.5f, -0.5f,  0.5f), XMFLOAT4(0, 1, 0, 1) },
    { XMFLOAT3(0.5f,  0.5f,  0.5f), XMFLOAT4(0, 1, 0, 1) },

    // 왼쪽면 (Left)
    { XMFLOAT3(-0.5f, -0.5f,  0.5f), XMFLOAT4(0, 0, 1, 1) },
    { XMFLOAT3(-0.5f,  0.5f, -0.5f), XMFLOAT4(0, 0, 1, 1) },
    { XMFLOAT3(-0.5f,  0.5f,  0.5f), XMFLOAT4(0, 0, 1, 1) },

    { XMFLOAT3(-0.5f, -0.5f,  0.5f), XMFLOAT4(0, 0, 1, 1) },
    { XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT4(0, 0, 1, 1) },
    { XMFLOAT3(-0.5f,  0.5f, -0.5f), XMFLOAT4(0, 0, 1, 1) },

    // 오른쪽면 (Right)
    { XMFLOAT3(0.5f, -0.5f, -0.5f), XMFLOAT4(1, 1, 0, 1) },
    { XMFLOAT3(0.5f,  0.5f,  0.5f), XMFLOAT4(1, 1, 0, 1) },
    { XMFLOAT3(0.5f,  0.5f, -0.5f), XMFLOAT4(1, 1, 0, 1) },

    { XMFLOAT3(0.5f, -0.5f, -0.5f), XMFLOAT4(1, 1, 0, 1) },
    { XMFLOAT3(0.5f, -0.5f,  0.5f), XMFLOAT4(1, 1, 0, 1) },
    { XMFLOAT3(0.5f,  0.5f,  0.5f), XMFLOAT4(1, 1, 0, 1) },

    // 윗면 (Top)
    { XMFLOAT3(-0.5f,  0.5f, -0.5f), XMFLOAT4(1, 0, 1, 1) },
    { XMFLOAT3(0.5f,  0.5f,  0.5f), XMFLOAT4(1, 0, 1, 1) },
    { XMFLOAT3(-0.5f,  0.5f,  0.5f), XMFLOAT4(1, 0, 1, 1) },

    { XMFLOAT3(-0.5f,  0.5f, -0.5f), XMFLOAT4(1, 0, 1, 1) },
    { XMFLOAT3(0.5f,  0.5f, -0.5f), XMFLOAT4(1, 0, 1, 1) },
    { XMFLOAT3(0.5f,  0.5f,  0.5f), XMFLOAT4(1, 0, 1, 1) },

    // 아랫면 (Bottom)
    { XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT4(0, 1, 1, 1) },
    { XMFLOAT3(-0.5f, -0.5f,  0.5f), XMFLOAT4(0, 1, 1, 1) },
    { XMFLOAT3(0.5f, -0.5f,  0.5f), XMFLOAT4(0, 1, 1, 1) },

    { XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT4(0, 1, 1, 1) },
    { XMFLOAT3(0.5f, -0.5f,  0.5f), XMFLOAT4(0, 1, 1, 1) },
    { XMFLOAT3(0.5f, -0.5f, -0.5f), XMFLOAT4(0, 1, 1, 1) },
};

Enemy::Enemy()
	: mVertexBuf(nullptr)
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

Enemy::Enemy(ID3D11Device* device)
    : Enemy()
{
    // Enemy 전용 VertexBuffer 생성
    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(cubeVertices);
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = cubeVertices;

    HRESULT hr = device->CreateBuffer(&bd, &initData, &mVertexBuf);
    if(FAILED(hr))
    {
        // 에러 처리...
    }
}

Enemy::~Enemy()
{
    if(mVertexBuf)
    {
        mVertexBuf->Release();
        mVertexBuf = nullptr;
    }

    if(mRigidBody)
    {
        delete mRigidBody;
        mRigidBody = nullptr;
    }

    if(mMotionState)
    {
        delete mMotionState;
        mMotionState = nullptr;
    }

    if(mCollisionShape)
    {
        delete mCollisionShape;
        mCollisionShape = nullptr;
    }
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
