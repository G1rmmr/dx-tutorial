#include "Renderer.h"

#include "Shader.h"
#include "Enemy.h"

#include <assert.h>

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
    : mDevice(nullptr)
    , mDeviceContext(nullptr)
    , mSwapChain(nullptr)
    , mRenderTargetView(nullptr)
    , mDepthStencilBuffer(nullptr)
    , mDepthStencilView(nullptr)
    , mDepthStencilState(nullptr)
    , mRasterizerState(nullptr)
    , mShader(nullptr)
    , mFloorVertex(nullptr)
    , mInputLayout(nullptr)
    , mMatrixBuffer(nullptr)
{

}

Renderer::~Renderer()
{
    Cleanup();
}

bool Renderer::Initialize(HWND hWnd, int width, int height)
{
    if(!createDeviceAndSwapChain(hWnd, width, height))
    {
        return false;
    }

    if(!createRenderTargetView())
    {
        return false;
    }

    if(!createDepthStencilBuffer(width, height))
    {
        return false;
    }

    if(!createDepthStencilState())
    {
        return false;
    }

    if(!createRasterizerState())
    {
        return false;
    }

    mShader = new Shader();
    if(!mShader->Initialize(mDevice,
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

    HRESULT hr = mDevice->CreateInputLayout(
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

    hr = mDevice->CreateBuffer(&cbd, nullptr, &mMatrixBuffer);
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

    hr = mDevice->CreateBuffer(&fbd, &initData, &mFloorVertex);
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

    hr = mDevice->CreateBuffer(&ebd, &initData, &mEnemyVertex);
    if(FAILED(hr))
    {
        MessageBox(nullptr, L"Failed to create floor vertex buffer.", L"Error", MB_OK);
        return false;
    }

    mDeviceContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);

    D3D11_VIEWPORT viewport;
    viewport.Width = static_cast<FLOAT>(width);
    viewport.Height = static_cast<FLOAT>(height);
    viewport.MinDepth = 0.f;
    viewport.MaxDepth = 1.f;
    viewport.TopLeftX = 0.f;
    viewport.TopLeftY = 0.f;

    mDeviceContext->RSSetViewports(1, &viewport);

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

    if(mRasterizerState)
    {
        mRasterizerState->Release();
        mRasterizerState = nullptr;
    }

    if(mDepthStencilState)
    {
        mDepthStencilState->Release();
        mDepthStencilState = nullptr;
    }

    if(mDepthStencilView)
    {
        mDepthStencilView->Release();
        mDepthStencilView = nullptr;
    }

    if(mDepthStencilBuffer)
    {
        mDepthStencilBuffer->Release();
        mDepthStencilBuffer = nullptr;
    }

    if(mRenderTargetView)
    {
        mRenderTargetView->Release();
        mRenderTargetView = nullptr;
    }

    if(mSwapChain)
    {
        mSwapChain->Release();
        mSwapChain = nullptr;
    }

    if(mDeviceContext)
    {
        mDeviceContext->Release();
        mDeviceContext = nullptr;
    }

    if(mDevice)
    {
        mDevice->Release();
        mDevice = nullptr;
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
}

void Renderer::BeginFrame(float red, float green, float blue, float alpha)
{
    mDeviceContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);

    float clearColor[4] = {red, green, blue, alpha};
    mDeviceContext->ClearRenderTargetView(mRenderTargetView, clearColor);

    mDeviceContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void Renderer::EndFrame()
{
    mSwapChain->Present(1, 0);
}

void Renderer::Draw(Enemy* enemy)
{
    using namespace DirectX;

    // 1) 셰이더 설정
    mShader->SetShader(mDeviceContext);

    // 2) 입력 레이아웃 설정
    mDeviceContext->IASetInputLayout(mInputLayout);

    // 3) 바닥(floor) 먼저 렌더링
    {
        UINT stride = sizeof(Vertex);
        UINT offset = 0;
        mDeviceContext->IASetVertexBuffers(0, 1, &mFloorVertex, &stride, &offset);
        mDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        mDeviceContext->Draw(6, 0);
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

    mDeviceContext->UpdateSubresource(mMatrixBuffer, 0, nullptr, &bufData, 0, 0);

    // 6) 셰이더에 상수 버퍼 연결
    mDeviceContext->VSSetConstantBuffers(0, 1, &mMatrixBuffer);

    // 7) Enemy용 큐브 버퍼를 바인딩하고 드로우
    {
        UINT stride = sizeof(Vertex);
        UINT offset = 0;
        mDeviceContext->IASetVertexBuffers(0, 1, &mEnemyVertex, &stride, &offset);
        mDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        mDeviceContext->Draw(36, 0);
    }
}

bool Renderer::createDeviceAndSwapChain(HWND hWnd, int width, int height)
{
    DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
    swapChainDesc.BufferDesc.Width = width;
    swapChainDesc.BufferDesc.Height = height;
    swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
    swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;

    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = 2;
    swapChainDesc.OutputWindow = hWnd;
    swapChainDesc.Windowed = TRUE;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.Flags = 0;

    UINT createDeviceFlags = 0;
#if defined(_DEBUG)
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0
    };

    D3D_FEATURE_LEVEL featureLevel;

    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        createDeviceFlags,
        featureLevels,
        _countof(featureLevels),
        D3D11_SDK_VERSION,
        &swapChainDesc,
        &mSwapChain,
        &mDevice,
        &featureLevel,
        &mDeviceContext
    );

    if(FAILED(hr))
    {
        return false;
    }

    return true;
}

bool Renderer::createRenderTargetView()
{
    ID3D11Texture2D* backBuffer = nullptr;
    HRESULT hr = mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
    if(FAILED(hr))
    {
        return false;
    }

    hr = mDevice->CreateRenderTargetView(backBuffer, nullptr, &mRenderTargetView);
    backBuffer->Release();

    if(FAILED(hr))
    {
        return false;
    }

    return true;
}

bool Renderer::createDepthStencilBuffer(int width, int height)
{
    D3D11_TEXTURE2D_DESC depthBufferDesc = {};
    depthBufferDesc.Width = width;
    depthBufferDesc.Height = height;
    depthBufferDesc.MipLevels = 1;
    depthBufferDesc.ArraySize = 1;
    depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthBufferDesc.SampleDesc.Count = 1;
    depthBufferDesc.SampleDesc.Quality = 0;
    depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthBufferDesc.CPUAccessFlags = 0;
    depthBufferDesc.MiscFlags = 0;

    HRESULT hr = mDevice->CreateTexture2D(&depthBufferDesc, nullptr, &mDepthStencilBuffer);
    if(FAILED(hr))
    {
        return false;
    }

    D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
    dsvDesc.Format = depthBufferDesc.Format;
    dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Texture2D.MipSlice = 0;

    hr = mDevice->CreateDepthStencilView(mDepthStencilBuffer, &dsvDesc, &mDepthStencilView);
    if(FAILED(hr))
    {
        return false;
    }

    return true;
}

bool Renderer::createDepthStencilState()
{
    D3D11_DEPTH_STENCIL_DESC dsDesc = {};
    dsDesc.DepthEnable = TRUE;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS;

    dsDesc.StencilEnable = FALSE;

    HRESULT hr = mDevice->CreateDepthStencilState(&dsDesc, &mDepthStencilState);
    if(FAILED(hr))
    {
        return false;
    }

    mDeviceContext->OMSetDepthStencilState(mDepthStencilState, 1);

    return true;
}

bool Renderer::createRasterizerState()
{
    D3D11_RASTERIZER_DESC rasterDesc = {};
    rasterDesc.FillMode = D3D11_FILL_SOLID;
    rasterDesc.CullMode = D3D11_CULL_BACK;
    rasterDesc.FrontCounterClockwise = FALSE;
    rasterDesc.DepthBias = 0;
    rasterDesc.SlopeScaledDepthBias = 0.0f;
    rasterDesc.DepthBiasClamp = 0.0f;
    rasterDesc.DepthClipEnable = TRUE;
    rasterDesc.ScissorEnable = FALSE;
    rasterDesc.MultisampleEnable = FALSE;
    rasterDesc.AntialiasedLineEnable = FALSE;

    HRESULT hr = mDevice->CreateRasterizerState(&rasterDesc, &mRasterizerState);
    if(FAILED(hr))
    {
        return false;
    }

    mDeviceContext->RSSetState(mRasterizerState);
    return true;
}

void Renderer::SetCameraMatrices(const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& proj)
{
    mView = view;
    mProj = proj;
}
