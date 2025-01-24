#include "Game.h"
#include "Window.h"
#include "DXRenderer.h"
#include "FPSCamera.h"
#include "Cube.h"

core::Game::Game()
    : mWindow(nullptr)
    , mCamera(nullptr)
    , mRenderer(nullptr)
    , mCube(nullptr)
{

}

core::Game::~Game()
{
    Cleanup();
}

bool core::Game::Initialize(HINSTANCE hInstance, int nCmdShow)
{
    mWindow = std::make_unique<Window>();
    if(!mWindow->Initialize(this, hInstance, nCmdShow))
    {
        return false;
    }

    mRenderer = std::make_unique<DXRenderer>();
    if(!mRenderer->Initialize(mWindow->GetHwnd()))
    {
        return 0;
    }

    mCamera = std::make_unique<FPSCamera>();

    mCube = std::make_unique<Cube>();

    if(!mCube->Initialize(mRenderer->GetDevice()))
    {
        return 0;
    }

    return true;
}

void core::Game::Cleanup()
{
    mRenderer->Cleanup();
    mCube->Cleanup();

    if(mWindow)
    {
        mWindow->Cleanup();
    }
}

void core::Game::Update(const float deltaTime)
{

}

void core::Game::Render()
{

}

void core::Game::OnMouseMove(int x, int y)
{

}