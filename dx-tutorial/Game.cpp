#include "Game.h"
#include "Window.h"
#include "DXRenderer.h"
#include "FPSCamera.h"
#include "Cube.h"

#include <iostream>

core::Game::Game()
    : mWindow(nullptr)
    , mCamera(nullptr)
    , mRenderer(nullptr)
    , mCube(nullptr);
{

}

core::Game::~Game()
{
    Cleanup();
}

bool core::Game::Initialize(HINSTANCE hInstance, int nCmdShow)
{
    mWindow = new Window();
    if(!mWindow->Initialize(this, hInstance, nCmdShow))
    {
        return false;
    }

    mRenderer = new DXRenderer();
    if(!mRenderer->Initialize(mWindow->GetHwnd()))
    {
        return 0;
    }

    mCamera = new FPSCamera();

    mCube = new Cube();

    if(!mCube->Initialize(mRenderer->GetDevice()))
    {
        return 0;
    }

    return true;
}

void core::Game::Cleanup()
{
    // 리소스 해제 순서 주의
    // mRenderer.Cleanup();
    // mCube.Cleanup();
    // mCamera 등등...

    if(mWindow)
    {
        mWindow->Cleanup();
        delete mWindow;
        mWindow = nullptr;
    }
}

void core::Game::Update()
{
    // 키보드 입력 처리, 카메라 이동 등등
    // mCamera.Update();
}

void core::Game::Render()
{
    mRenderer.Clear(...);
    mCube.Render(mRenderer.GetContext());
    mRenderer.Present();
}

// Window::WndProc에서 호출됨 (WM_MOUSEMOVE)
void core::Game::OnMouseMove(int x, int y)
{
    // 예: 여기서 FPSCamera에 Rotate를 명령하거나
    // mCamera.Rotate(dx, dy);
    // 하지만 dx, dy 계산을 위해선 이전 마우스 위치를 저장해야 할 수도 있음
    std::cout << "OnMouseMove: (" << x << ", " << y << ")\n";
}