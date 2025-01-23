#include "Cube.h"

#include <vector>

core::Vertex gCubeVertices[] = {
    {{-0.5f, -0.5f, -0.5f}, {1, 0, 0, 1}}, // Front bottom left
    {{0.5f, -0.5f, -0.5f}, {0, 1, 0, 1}},  // Front bottom right
    {{0.5f, 0.5f, -0.5f}, {0, 0, 1, 1}},   // Front top right
    {{-0.5f, 0.5f, -0.5f}, {1, 1, 0, 1}},  // Front top left

    {{-0.5f, -0.5f, 0.5f}, {1, 0, 1, 1}},  // Back bottom left
    {{0.5f, -0.5f, 0.5f}, {0, 1, 1, 1}},   // Back bottom right
    {{0.5f, 0.5f, 0.5f}, {1, 1, 1, 1}},    // Back top right
    {{-0.5f, 0.5f, 0.5f}, {0, 0, 0, 1}}    // Back top left
};

unsigned int gCubeIndices[] = {
    0,1,2, 0,2,3, // Front face
    4,5,6, 4,6,7, // Back face
    0,1,5, 0,5,4, // Bottom face
    3,2,6, 3,6,7, // Top face
    0,3,7, 0,7,4, // Left face
    1,5,6, 1,6,2  // Right face
};

core::Cube::Cube()
    : mVertexBuffer(nullptr)
    , mIndexBuffer(nullptr)
    , mConstantBuffer(nullptr)
    , mInputLayout(nullptr)
    , mVertexShader(nullptr)
    , mPixelShader(nullptr)
{

}

core::Cube::~Cube()
{
    Cleanup();
}

bool core::Cube::Initialize(ID3D11Device* device)
{
    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.ByteWidth = sizeof(gCubeVertices);
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA InitializeData = {};
    InitializeData.pSysMem = gCubeVertices;

    if(FAILED(device->CreateBuffer(&bufferDesc, &InitializeData, &mVertexBuffer)))
    {
        return false;
    }

    // 인덱스 버퍼 생성
    bufferDesc.ByteWidth = sizeof(gCubeIndices);
    bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    InitializeData.pSysMem = gCubeIndices;

    if(FAILED(device->CreateBuffer(&bufferDesc, &InitializeData, &mIndexBuffer)))
    {
        return false;
    }
       
    return true;
}

void core::Cube::Render(ID3D11DeviceContext* deviceContext)
{
    UINT stride = sizeof(Vertex);
    UINT offset = 0;

    deviceContext->IASetVertexBuffers(0, 1, &mVertexBuffer, &stride, &offset);
    deviceContext->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    deviceContext->DrawIndexed(36, 0, 0);
}

void core::Cube::Cleanup()
{
    if(mVertexBuffer)
    {
        mVertexBuffer->Release();
    }

    if(mIndexBuffer)
    {
        mIndexBuffer->Release();
    }
}
