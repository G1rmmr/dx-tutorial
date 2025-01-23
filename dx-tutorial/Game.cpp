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
    // ���ҽ� ���� ���� ����
    // mRenderer.Cleanup();
    // mCube.Cleanup();
    // mCamera ���...

    if(mWindow)
    {
        mWindow->Cleanup();
        delete mWindow;
        mWindow = nullptr;
    }
}

void core::Game::Update()
{
    // Ű���� �Է� ó��, ī�޶� �̵� ���
    // mCamera.Update();
}

void core::Game::Render()
{
    mRenderer.Clear(...);
    mCube.Render(mRenderer.GetContext());
    mRenderer.Present();
}

// Window::WndProc���� ȣ��� (WM_MOUSEMOVE)
void core::Game::OnMouseMove(int x, int y)
{
    // ��: ���⼭ FPSCamera�� Rotate�� ����ϰų�
    // mCamera.Rotate(dx, dy);
    // ������ dx, dy ����� ���ؼ� ���� ���콺 ��ġ�� �����ؾ� �� ���� ����
    std::cout << "OnMouseMove: (" << x << ", " << y << ")\n";
}