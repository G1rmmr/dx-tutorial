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
    // ¾Õ¸é (Front)
    { XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT4(1, 0, 0, 1) },
    { XMFLOAT3(-0.5f,  0.5f, -0.5f), XMFLOAT4(1, 0, 0, 1) },
    { XMFLOAT3(0.5f,  0.5f, -0.5f), XMFLOAT4(1, 0, 0, 1) },

    { XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT4(1, 0, 0, 1) },
    { XMFLOAT3(0.5f,  0.5f, -0.5f), XMFLOAT4(1, 0, 0, 1) },
    { XMFLOAT3(0.5f, -0.5f, -0.5f), XMFLOAT4(1, 0, 0, 1) },

    // µÞ¸é (Back)
    { XMFLOAT3(-0.5f, -0.5f,  0.5f), XMFLOAT4(0, 1, 0, 1) },
    { XMFLOAT3(0.5f,  0.5f,  0.5f), XMFLOAT4(0, 1, 0, 1) },
    { XMFLOAT3(-0.5f,  0.5f,  0.5f), XMFLOAT4(0, 1, 0, 1) },

    { XMFLOAT3(-0.5f, -0.5f,  0.5f), XMFLOAT4(0, 1, 0, 1) },
    { XMFLOAT3(0.5f, -0.5f,  0.5f), XMFLOAT4(0, 1, 0, 1) },
    { XMFLOAT3(0.5f,  0.5f,  0.5f), XMFLOAT4(0, 1, 0, 1) },

    // ¿ÞÂÊ¸é (Left)
    { XMFLOAT3(-0.5f, -0.5f,  0.5f), XMFLOAT4(0, 0, 1, 1) },
    { XMFLOAT3(-0.5f,  0.5f, -0.5f), XMFLOAT4(0, 0, 1, 1) },
    { XMFLOAT3(-0.5f,  0.5f,  0.5f), XMFLOAT4(0, 0, 1, 1) },

    { XMFLOAT3(-0.5f, -0.5f,  0.5f), XMFLOAT4(0, 0, 1, 1) },
    { XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT4(0, 0, 1, 1) },
    { XMFLOAT3(-0.5f,  0.5f, -0.5f), XMFLOAT4(0, 0, 1, 1) },

    // ¿À¸¥ÂÊ¸é (Right)
    { XMFLOAT3(0.5f, -0.5f, -0.5f), XMFLOAT4(1, 1, 0, 1) },
    { XMFLOAT3(0.5f,  0.5f,  0.5f), XMFLOAT4(1, 1, 0, 1) },
    { XMFLOAT3(0.5f,  0.5f, -0.5f), XMFLOAT4(1, 1, 0, 1) },

    { XMFLOAT3(0.5f, -0.5f, -0.5f), XMFLOAT4(1, 1, 0, 1) },
    { XMFLOAT3(0.5f, -0.5f,  0.5f), XMFLOAT4(1, 1, 0, 1) },
    { XMFLOAT3(0.5f,  0.5f,  0.5f), XMFLOAT4(1, 1, 0, 1) },

    // À­¸é (Top)
    { XMFLOAT3(-0.5f,  0.5f, -0.5f), XMFLOAT4(1, 0, 1, 1) },
    { XMFLOAT3(0.5f,  0.5f,  0.5f), XMFLOAT4(1, 0, 1, 1) },
    { XMFLOAT3(-0.5f,  0.5f,  0.5f), XMFLOAT4(1, 0, 1, 1) },

    { XMFLOAT3(-0.5f,  0.5f, -0.5f), XMFLOAT4(1, 0, 1, 1) },
    { XMFLOAT3(0.5f,  0.5f, -0.5f), XMFLOAT4(1, 0, 1, 1) },
    { XMFLOAT3(0.5f,  0.5f,  0.5f), XMFLOAT4(1, 0, 1, 1) },

    // ¾Æ·§¸é (Bottom)
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

    mRigidBody->activate(true);
    mRigidBody->forceActivationState(DISABLE_DEACTIVATION);
    mRigidBody->setFriction(0.3f);
    mRigidBody->setDamping(0.f, 0.f);
}

Enemy::Enemy(ID3D11Device* device)
    : Enemy()
{
    // Enemy Àü¿ë VertexBuffer »ý¼º
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
        MessageBox(nullptr, TEXT("Enemy init failed!"), TEXT("ERROR"), MB_OK);
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

void Enemy::Update(const float deltaTime)
{

}

void Enemy::Render(
    ID3D11DeviceContext* context,
    ID3D11Buffer* matrixBuf,
    const DirectX::XMMATRIX& view,
    const DirectX::XMMATRIX& proj)
{
    XMMATRIX world = XMMatrixIdentity();

    struct MatrixBuffer
    {
        XMMATRIX World;
        XMMATRIX View;
        XMMATRIX Proj;
    };

    MatrixBuffer bufData = {};
    bufData.World = XMMatrixTranspose(world);
    bufData.View = XMMatrixTranspose(view);
    bufData.Proj = XMMatrixTranspose(proj);

    context->UpdateSubresource(matrixBuf, 0, nullptr, &bufData, 0, 0);
    context->VSSetConstantBuffers(0, 1, &matrixBuf);

    UINT stride = sizeof(Vertex);
    UINT offset = 0;

    context->IASetVertexBuffers(0, 1, &mVertexBuf, &stride, &offset);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    context->Draw(36, 0);
}