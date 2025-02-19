#include "Renderer.h"

#include "Shader.h"
#include "Enemy.h"
#include "Pipeline.h"

using namespace core;

struct Vertex
{
    DirectX::XMFLOAT3 Pos;
    DirectX::XMFLOAT4 Color;
};

struct MatrixBuffer
{
    DirectX::XMMATRIX World;
    DirectX::XMMATRIX View;
    DirectX::XMMATRIX Proj;
};

using namespace DirectX;

// Enemy
Vertex cubeVertices[] =
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

Renderer::Renderer()
    : mPipeline(nullptr)
    , mMatrixBuffer(nullptr)
    , mFloorVertex(nullptr)
    , mEnemyVertex(nullptr)
    , mInputLayout(nullptr)
    , mShader(nullptr)
    , mView(DirectX::XMMatrixIdentity())
    , mProj(DirectX::XMMatrixIdentity())
{

}

Renderer::~Renderer()
{
    Cleanup();
}

bool Renderer::Initialize(HWND hWnd, int width, int height)
{
    mPipeline = new Pipeline();
    if(!mPipeline->Initialize(hWnd, width, height))
    {
        return false;
    }

    ID3D11Device* device = mPipeline->GetDevice();

    mShader = new Shader();
    if(!mShader->Initialize(device,
        L"VertexShader.hlsl", "VSMain",
        L"PixelShader.hlsl", "PSMain"))
    {
        MessageBox(nullptr, L"Shader init failed", L"Error", MB_OK);
        return false;
    }

    D3D11_INPUT_ELEMENT_DESC layoutDesc[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,   0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT,0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    UINT numElements = _countof(layoutDesc);

    ID3DBlob* vsBlob = mShader->GetVSBlob();

    HRESULT hr = device->CreateInputLayout(
        layoutDesc,
        numElements,
        vsBlob->GetBufferPointer(),
        vsBlob->GetBufferSize(),
        &mInputLayout
    );
    if(FAILED(hr))
    {
        MessageBox(nullptr, L"CreateInputLayout failed", L"Error", MB_OK);
        return false;
    }

    D3D11_BUFFER_DESC cbd = {};
    cbd.Usage = D3D11_USAGE_DEFAULT;
    cbd.ByteWidth = sizeof(MatrixBuffer);
    cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbd.CPUAccessFlags = 0;

    hr = device->CreateBuffer(&cbd, nullptr, &mMatrixBuffer);
    if(FAILED(hr))
    {
        return false;
    }

    /*
    Vertex triangleVertices[] =
    {
        { DirectX::XMFLOAT3(0.f,  0.5f, 0.f), DirectX::XMFLOAT4(1, 0, 0, 1) },
        { DirectX::XMFLOAT3(0.5f, -0.5f, 0.f), DirectX::XMFLOAT4(0, 1, 0, 1) },
        { DirectX::XMFLOAT3(-0.5f, -0.5f, 0.f), DirectX::XMFLOAT4(0, 0, 1, 1) },
    };

    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(triangleVertices);
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = triangleVertices;

    hr = mDevice->CreateBuffer(&bd, &initData, &mTriangleVertexBuffer);
    if(FAILED(hr))
    {
        MessageBox(nullptr, L"Failed to create triangle vertex buffer.", L"Error", MB_OK);
        return false;
    }

    mDeviceContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);
    */

    Vertex floorVertices[] =
    {
        { DirectX::XMFLOAT3(10.f, -1.f,  10.f), DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1) },
        { DirectX::XMFLOAT3(10.f, -1.f,  -10.f), DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1) },
        { DirectX::XMFLOAT3(-10.f, -1.f, -10.f), DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1) },

        { DirectX::XMFLOAT3(-10.f, -1.f,  10.f), DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1) },
        { DirectX::XMFLOAT3(10.f, -1.f,  10.f), DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1) },
        { DirectX::XMFLOAT3(-10.f, -1.f, -10.f), DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1) }
    };

    D3D11_BUFFER_DESC fbd = {};
    fbd.Usage = D3D11_USAGE_DEFAULT;
    fbd.ByteWidth = sizeof(floorVertices);
    fbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    fbd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = floorVertices;

    hr = device->CreateBuffer(&fbd, &initData, &mFloorVertex);
    if(FAILED(hr))
    {
        MessageBox(nullptr, L"Failed to create floor vertex buffer.", L"Error", MB_OK);
        return false;
    }

    //Enemy
    D3D11_BUFFER_DESC ebd = {};
    ebd.Usage = D3D11_USAGE_DEFAULT;
    ebd.ByteWidth = sizeof(cubeVertices);
    ebd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    ebd.CPUAccessFlags = 0;

    initData = {};
    initData.pSysMem = cubeVertices;

    hr = device->CreateBuffer(&ebd, &initData, &mEnemyVertex);
    if(FAILED(hr))
    {
        MessageBox(nullptr, L"Failed to create floor vertex buffer.", L"Error", MB_OK);
        return false;
    }

    ID3D11DeviceContext* context = mPipeline->GetDeviceContext();
    ID3D11RenderTargetView* rtv = mPipeline->GetRenderTargetView();
    context->OMSetRenderTargets(1, &rtv, mPipeline->GetDepthStencilView());

    D3D11_VIEWPORT viewport;
    viewport.Width = static_cast<FLOAT>(width);
    viewport.Height = static_cast<FLOAT>(height);
    viewport.MinDepth = 0.f;
    viewport.MaxDepth = 1.f;
    viewport.TopLeftX = 0.f;
    viewport.TopLeftY = 0.f;

    context->RSSetViewports(1, &viewport);
    return true;
}

void Renderer::Cleanup()
{
    if(mShader)
    {
        mShader->Cleanup();
        delete mShader;
        mShader = nullptr;
    }

    if(mFloorVertex)
    {
        mFloorVertex->Release();
        mFloorVertex = nullptr;
    }

    if(mInputLayout)
    {
        mInputLayout->Release();
        mInputLayout = nullptr;
    }

    if(mMatrixBuffer)
    {
        mMatrixBuffer->Release();
        mMatrixBuffer = nullptr;
    }

    if(mEnemyVertex)
    {
        mEnemyVertex->Release();
        mEnemyVertex = nullptr;
    }

    if(mPipeline)
    {
        mPipeline->Cleanup();
        delete mPipeline;
        mPipeline = nullptr;
    }
}

void Renderer::BeginFrame(float red, float green, float blue, float alpha)
{
    float clearColor[4] = {red, green, blue, alpha};

    ID3D11DeviceContext* context = mPipeline->GetDeviceContext();
    ID3D11RenderTargetView* rtv = mPipeline->GetRenderTargetView();
    ID3D11DepthStencilView* dsv = mPipeline->GetDepthStencilView();

    context->OMSetRenderTargets(1, &rtv, dsv);
    context->ClearRenderTargetView(rtv, clearColor);
    context->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void Renderer::EndFrame()
{
    mPipeline->GetSwapChain()->Present(1, 0);
}

void Renderer::Draw(Enemy* enemy)
{
    using namespace DirectX;

    ID3D11DeviceContext* context = mPipeline->GetDeviceContext();

    // 1) 셰이더 설정
    mShader->SetShader(context);

    // 2) 입력 레이아웃 설정
    context->IASetInputLayout(mInputLayout);

    // 3) 바닥(floor) 먼저 렌더링
    {
        UINT stride = sizeof(Vertex);
        UINT offset = 0;

        context->IASetVertexBuffers(0, 1, &mFloorVertex, &stride, &offset);
        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        context->Draw(6, 0);
    }

    // 4) Enemy의 위치에 따른 월드 행렬 계산
    XMMATRIX translation = XMMatrixTranslation(enemy->GetPos().x,
        enemy->GetPos().y,
        enemy->GetPos().z);
    XMMATRIX rotation = XMMatrixIdentity(); // 필요 시 적절한 회전 행렬
    XMMATRIX scale = XMMatrixScaling(1.f, 1.f, 1.f); // Enemy 큐브 크기 조정 가능

    XMMATRIX worldMatrix = scale * rotation * translation;

    // 5) 한 번에 World/View/Proj를 상수 버퍼에 담기
    MatrixBuffer bufData = {};
    bufData.World = XMMatrixTranspose(worldMatrix);
    bufData.View = XMMatrixTranspose(mView);  // SetCameraMatrices에서 저장된 뷰
    bufData.Proj = XMMatrixTranspose(mProj);  // SetCameraMatrices에서 저장된 프로젝션

    context->UpdateSubresource(mMatrixBuffer, 0, nullptr, &bufData, 0, 0);

    // 6) 셰이더에 상수 버퍼 연결
    context->VSSetConstantBuffers(0, 1, &mMatrixBuffer);

    // 7) Enemy용 큐브 버퍼를 바인딩하고 드로우
    {
        UINT stride = sizeof(Vertex);
        UINT offset = 0;

        context->IASetVertexBuffers(0, 1, &mEnemyVertex, &stride, &offset);
        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        context->Draw(36, 0);
    }
}

void Renderer::SetCameraMatrices(const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& proj)
{
    mView = view;
    mProj = proj;
}
