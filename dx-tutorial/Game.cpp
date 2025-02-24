#include "Game.h"

#include <windowsx.h>

#include "Actor.h"
#include "Player.h"
#include "Renderer.h"
#include "Physics.h"
#include "Enemy.h"
#include "Floor.h"

using namespace core;

HINSTANCE mHInstance;
HWND mHwnd;
DWORD mTicksCount;

Renderer* mRenderer;
Player* mPlayer;
Enemy* mEnemy;
Physics* mPhysics;

int mScreenWidth;
int mScreenHeight;

bool mIsRunning;

void DebugLog(const LPCTSTR& text)
{
    MessageBox(nullptr, text, TEXT("LOG"), MB_OK);
}

Game::Game()
    : mHInstance(nullptr)
    , mHwnd(nullptr)
    , mTicksCount(0)
    , mRenderer(nullptr)
    , mPlayer(nullptr)
    , mScreenWidth(0)
    , mScreenHeight(0)
    , mIsRunning(false)
    , mPhysics(nullptr)
{

}

Game::~Game()
{
    Cleanup();
}

bool Game::Initialize(HINSTANCE hInstance, int width, int height, int nCmdShow)
{
    // window init
    mHInstance = hInstance;
    mScreenWidth = width;
    mScreenHeight = height;

    if(!createWindow(hInstance, width, height, nCmdShow))
    {
        return false;
    }

    mPhysics = new Physics();

    mRenderer = new Renderer();
    if(!mRenderer->Initialize(mHwnd, width, height))
    {
        delete mRenderer;
        mRenderer = nullptr;

        DebugLog(L"Renderer not initialized!");
        return false;
    }

    // Actors
    auto enemy = new Enemy(mRenderer->GetPipeline()->GetDevice());
    mActors.emplace_back(enemy);
    mPhysics->AddRigidBody(enemy->GetRigidBody());

    auto floor = new Floor(mRenderer->GetPipeline()->GetDevice());
    mActors.emplace_back(floor);
    mPhysics->AddRigidBody(floor->GetRigidBody());

    // player
    mPlayer = new Player();
    mPhysics->AddRigidBody(mPlayer->GetRigidBody());

    // set
    mTicksCount = GetTickCount64();
    mIsRunning = true;

    return true;
}

void Game::Run()
{
    MSG msg = {0};

    while(mIsRunning)
    {
        while(PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if(msg.message == WM_QUIT)
            {
                mIsRunning = false;
                break;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if(!mIsRunning)
        {
            break;
        }

        ULONGLONG currentTime = GetTickCount64();
        float deltaTime = (currentTime - mTicksCount) / 1000.f;
        
        if(deltaTime > 0.05f)
        {
            deltaTime = 0.05f;
        }
            
        mTicksCount = currentTime;

        processInput(deltaTime);
        update(deltaTime);
        render();
    }
}

void Game::Cleanup()
{
    if(mPlayer)
    {
        delete mPlayer;
        mPlayer = nullptr;
    }

    if(mRenderer)
    {
        mRenderer->Cleanup();
        delete mRenderer;
        mRenderer = nullptr;
    }

    if(mHwnd)
    {
        DestroyWindow(mHwnd);
        mHwnd = nullptr;
    }
}

bool Game::createWindow(HINSTANCE hInstance, int width, int height, int nCmdShow)
{
    WNDCLASSEX wc = {0};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = sWindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = L"DX11GameWindowClass";

    if(!RegisterClassEx(&wc))
    {
        return false;
    }

    mHwnd = CreateWindowEx(
        0,
        wc.lpszClassName,
        L"DX11 Game",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        width, height,
        nullptr,
        nullptr,
        hInstance,
        this
    );

    if(!mHwnd)
    {
        return false;
    }

    ShowWindow(mHwnd, nCmdShow);
    UpdateWindow(mHwnd);

    return true;
}

LRESULT CALLBACK Game::sWindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    Game* game = nullptr;

    if(message == WM_NCCREATE)
    {
        CREATESTRUCT* cs = reinterpret_cast<CREATESTRUCT*>(lParam);
        game = static_cast<Game*>(cs->lpCreateParams);

        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(game));
    }
    else
    {
        game = reinterpret_cast<Game*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    }

    if(game)
    {
        return game->windowProc(hwnd, message, wParam, lParam);
    }

    return DefWindowProc(hwnd, message, wParam, lParam);
}

LRESULT Game::windowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_MOUSEMOVE:
    {
        static bool firstCall = true;
        static int centerX = 0;
        static int centerY = 0;

        if(firstCall)
        {
            RECT rect;
            GetClientRect(hwnd, &rect);

            centerX = (rect.right - rect.left) / 2;
            centerY = (rect.bottom - rect.top) / 2;
            firstCall = false;
        }

        int x = GET_X_LPARAM(lParam);
        int y = GET_Y_LPARAM(lParam);

        int deltaX = x - centerX;
        int deltaY = y - centerY;

        mPlayer->ProcessMouseMovement((float)deltaX, (float)-deltaY);

        POINT pt = {0, 0};
        pt.x = centerX;
        pt.y = centerY;
        ClientToScreen(hwnd, &pt);
        SetCursorPos(pt.x, pt.y);

        return 0;
    }

    default:
        break;
    }

    return DefWindowProc(hwnd, message, wParam, lParam);
}

void Game::processInput(float deltaTime)
{
    if(GetAsyncKeyState(VK_ESCAPE) & 0x8000)
    {
        mIsRunning = false;
        return;
    }

    bool forwardKey = (GetAsyncKeyState('W') & 0x8000) != 0;
    bool backKey = (GetAsyncKeyState('S') & 0x8000) != 0;
    bool leftKey = (GetAsyncKeyState('A') & 0x8000) != 0;
    bool rightKey = (GetAsyncKeyState('D') & 0x8000) != 0;
    bool jumpKey = (GetAsyncKeyState(VK_SPACE) & 0x8000) != 0;

    mPlayer->ProcessKeyboard(mPhysics->GetDynamicsWorld(), deltaTime,
        forwardKey, backKey, leftKey, rightKey, jumpKey);
}

void Game::update(const float deltaTime)
{
    mPhysics->Update(deltaTime);

    for(auto& actor : mActors)
    {
        actor->SyncPhysics();
    }

    mPlayer->SyncPhysics();

    DirectX::XMMATRIX view = mPlayer->GetViewMatrix();

    constexpr float fovAngleY = DirectX::XMConvertToRadians(60.f);
    const float aspect = static_cast<float>(mScreenWidth) / static_cast<float>(mScreenHeight);
    const float nearZ = 0.1f;
    const float farZ = 100.0f;

    DirectX::XMMATRIX proj = DirectX::XMMatrixPerspectiveFovLH(fovAngleY, aspect, nearZ, farZ);

    mRenderer->SetCameraMatrices(view, proj);
}

void Game::render()
{
    if(mRenderer)
    {
        mRenderer->BeginFrame(0.f, 0.f, 0.f, 1.f);
        mRenderer->Draw(mActors);
        mRenderer->EndFrame();
    }
}
