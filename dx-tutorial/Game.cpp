#include "Game.h"

#include <windowsx.h>

using namespace core;

LRESULT CALLBACK Game::sWindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    Player mPlayer = mPlayer;
    HWND mHwnd = mHwnd;

    switch(message)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_KEYDOWN:
        break;

    case WM_MOUSEMOVE:
    {
        // 마우스의 현재 위치를 얻고, 이전 위치와 비교해 이동량을 구합니다.
        static int lastX = 0;
        static int lastY = 0;

        int x = GET_X_LPARAM(lParam);
        int y = GET_Y_LPARAM(lParam);

        int deltaX = x - lastX;
        int deltaY = y - lastY;

        mPlayer.ProcessMouseMovement((float)deltaX, (float)-deltaY);

        lastX = x;
        lastY = y;
    }
    return 0;
    }

    return DefWindowProc(mHwnd, message, wParam, lParam);
}

Game::Game()
    : mRenderer(nullptr)
    , mHwnd(nullptr)
    , mHInstance(nullptr)
    , mTicksCount(0)
    , mScreenWidth(0)
    , mScreenHeight(0)
    , mIsRunning(false)
{

}

Game::~Game()
{
    Cleanup();
}

bool Game::Initialize(HINSTANCE hInstance, int width, int height, int nCmdShow)
{
    mHInstance = hInstance;
    mScreenWidth = width;
    mScreenHeight = height;

    if(!createWindow(hInstance, width, height, nCmdShow))
    {
        return false;
    }

    mRenderer = new Renderer();
    if(!mRenderer->Initialize(mHwnd, width, height))
    {
        delete mRenderer;
        mRenderer = nullptr;
        return false;
    }


    mIsRunning = true;
    return true;
}

void Game::Run()
{
    MSG msg = {};
    mTicksCount = GetTickCount64();

    while(msg.message != WM_QUIT && mIsRunning)
    {
        if(PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            DWORD currentTime = GetTickCount64();
            float deltaTime = (currentTime - mTicksCount) * 0.001f;
            mTicksCount = currentTime;

            bool forwardKey = (GetAsyncKeyState('W') & 0x8000) != 0;
            bool backKey = (GetAsyncKeyState('S') & 0x8000) != 0;
            bool leftKey = (GetAsyncKeyState('A') & 0x8000) != 0;
            bool rightKey = (GetAsyncKeyState('D') & 0x8000) != 0;

            mPlayer.ProcessKeyboard(forwardKey, backKey, leftKey, rightKey, deltaTime);

            mRenderer->Draw();
        }
    }
}

void Game::Cleanup()
{
    mIsRunning = false;

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
    WNDCLASSEX wc = {};
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

    mHwnd = CreateWindowExW(
        0,
        wc.lpszClassName,
        L"DX11 Game",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        width, height,
        nullptr,
        nullptr,
        hInstance,
        nullptr
    );

    if(!mHwnd)
    {
        return false;
    }

    ShowWindow(mHwnd, nCmdShow);
    UpdateWindow(mHwnd);

    return true;
}

void Game::processInput()
{
    
}

void Game::update()
{
    DWORD currTicks = GetTickCount64();
    float deltaTime = (currTicks - mTicksCount) / 1000.0f;

    if(deltaTime > 0.05f)
    {
        deltaTime = 0.05f;
    }

    mTicksCount = currTicks;
}

void Game::render()
{
    if(mRenderer)
    {
        mRenderer->Draw();
    }
}