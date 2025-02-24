// Floor.cpp

#include "Floor.h"
#include <DirectXMath.h>

using namespace DirectX;
using namespace core;

struct Vertex
{
    XMFLOAT3 Pos;
    XMFLOAT4 Color;
};

static Vertex floorVertices[] =
{
    { XMFLOAT3(10.f, -1.f,  10.f), XMFLOAT4(0.5f, 0.5f, 0.5f, 1) },
    { XMFLOAT3(10.f, -1.f, -10.f), XMFLOAT4(0.5f, 0.5f, 0.5f, 1) },
    { XMFLOAT3(-10.f, -1.f, -10.f), XMFLOAT4(0.5f, 0.5f, 0.5f, 1) },

    { XMFLOAT3(-10.f, -1.f,  10.f), XMFLOAT4(0.5f, 0.5f, 0.5f, 1) },
    { XMFLOAT3(10.f, -1.f,  10.f), XMFLOAT4(0.5f, 0.5f, 0.5f, 1) },
    { XMFLOAT3(-10.f, -1.f, -10.f), XMFLOAT4(0.5f, 0.5f, 0.5f, 1) }
};

Floor::Floor()
    : mVertexBuf(nullptr)
    , mRigidBody(nullptr)
    , mCollisionShape(nullptr)
    , mMotionState(nullptr)
{
    btTransform startTransform;
    startTransform.setIdentity();
    startTransform.setOrigin(btVector3(0.0f, -1.0f, 0.0f));

    btCollisionShape* shape = new btBoxShape(btVector3(20.f, 1.0f, 20.f));

    btScalar mass = 0.f;
    bool isDynamic = (mass != 0.0f);

    btVector3 localInertia(0, 0, 0);
    if(isDynamic)
    {
        shape->calculateLocalInertia(mass, localInertia);
    }

    mMotionState = new btDefaultMotionState(startTransform);
    mCollisionShape = shape;

    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, mMotionState, mCollisionShape, localInertia);
    mRigidBody = new btRigidBody(rbInfo);

    mRigidBody->activate(true);
    mRigidBody->forceActivationState(DISABLE_DEACTIVATION);
    mRigidBody->setFriction(0.3f);
    mRigidBody->setDamping(0.f, 0.f);
}

Floor::Floor(ID3D11Device* device)
    : Floor()
{
    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(floorVertices);
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = floorVertices;

    HRESULT hr = device->CreateBuffer(&bd, &initData, &mVertexBuf);
    if(FAILED(hr))
    {
        // 에러 처리
    }
}

Floor::~Floor()
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

void Floor::Update(float deltaTime)
{

}

void Floor::Render(
    ID3D11DeviceContext* context,
    ID3D11Buffer* matrixBuf,
    const XMMATRIX& view,
    const XMMATRIX& proj)
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

    UINT stride = sizeof(Vertex);
    UINT offset = 0;

    context->IASetVertexBuffers(0, 1, &mVertexBuf, &stride, &offset);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    context->Draw(6, 0);
}

void Floor::SyncPhysics()
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