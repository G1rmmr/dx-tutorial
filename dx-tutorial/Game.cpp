#include "Game.h"

#include <algorithm>

#include "Renderer.h"
#include "Actor.h"

using namespace core;

LRESULT CALLBACK Game::sWindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, message, wParam, lParam);
}

Game::Game()
    : mRenderer(nullptr)
    , mHwnd(nullptr)
    , mHInstance(nullptr)
    , mTicksCount(0)
    , mScreenWidth(0)
    , mScreenHeight(0)
    , mIsRunning(false)
    , mUpdatingActors(false)
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

    loadData();

    mIsRunning = true;
    mTicksCount = GetTickCount();

    return true;
}

void Game::Run()
{
    MSG msg = {};
    while(mIsRunning)
    {
        while(PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if(msg.message == WM_QUIT)
            {
                mIsRunning = false;
            }
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }

        if(!mIsRunning)
        {
            break;
        }
        processInput();
        update();
        render();
    }
}

void Game::Cleanup()
{
    mIsRunning = false;

    unloadData();

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

void Game::AddActor(Actor* actor)
{
    if(mUpdatingActors)
    {
        mPendingActors.emplace_back(actor);
    }
    else
    {
        mActors.emplace_back(actor);
    }
}

void Game::RemoveActor(Actor* actor)
{
    auto iter = std::find(mPendingActors.begin(), mPendingActors.end(), actor);
    if(iter != mPendingActors.end())
    {
        mPendingActors.erase(iter);
    }

    iter = std::find(mActors.begin(), mActors.end(), actor);
    if(iter != mActors.end())
    {
        mActors.erase(iter);
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
    DWORD currTicks = GetTickCount();
    float deltaTime = (currTicks - mTicksCount) / 1000.0f;

    if(deltaTime > 0.05f)
    {
        deltaTime = 0.05f;
    }

    mTicksCount = currTicks;

    mUpdatingActors = true;
    for(auto actor : mActors)
    {
        actor->Update(deltaTime);
    }

    mUpdatingActors = false;

    for(auto pending : mPendingActors)
    {
        mActors.emplace_back(pending);
    }
    mPendingActors.clear();

    auto removeIter = std::remove_if(mActors.begin(), mActors.end(),
        [](Actor* a)
        {
            return a->GetState() == Actor::eState::Dead;
        });

    if(removeIter != mActors.end())
    {
        mActors.erase(removeIter, mActors.end());
    }
}

void Game::render()
{
    if(mRenderer)
    {
        mRenderer->Draw();
    }
}

void Game::loadData()
{

}

void Game::unloadData()
{
    while(!mActors.empty())
    {
        delete mActors.back();
    }

    while(!mPendingActors.empty())
    {
        delete mPendingActors.back();
        mPendingActors.pop_back();
    }
}