// Floor.cpp
#include "Floor.h"
#include <DirectXMath.h>

using namespace DirectX;
using namespace core;

struct FloorVertex
{
    XMFLOAT3 Pos;
    XMFLOAT4 Color;
};

static FloorVertex floorVertices[] =
{
    { XMFLOAT3(10.f, -1.f,  10.f), XMFLOAT4(0.5f, 0.5f, 0.5f, 1) },
    { XMFLOAT3(10.f, -1.f, -10.f), XMFLOAT4(0.5f, 0.5f, 0.5f, 1) },
    { XMFLOAT3(-10.f, -1.f, -10.f), XMFLOAT4(0.5f, 0.5f, 0.5f, 1) },

    { XMFLOAT3(-10.f, -1.f,  10.f), XMFLOAT4(0.5f, 0.5f, 0.5f, 1) },
    { XMFLOAT3(10.f, -1.f,  10.f), XMFLOAT4(0.5f, 0.5f, 0.5f, 1) },
    { XMFLOAT3(-10.f, -1.f, -10.f), XMFLOAT4(0.5f, 0.5f, 0.5f, 1) }
};

Floor::Floor(ID3D11Device* device)
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

    UINT stride = sizeof(FloorVertex);
    UINT offset = 0;

    context->IASetVertexBuffers(0, 1, &mVertexBuf, &stride, &offset);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    context->Draw(6, 0);
}
