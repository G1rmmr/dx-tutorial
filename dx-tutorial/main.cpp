#include "Window.h"
#include "DXRenderer.h"
#include "FPSCamera.h"
#include "Cube.h"

#include <iostream>

using namespace core;

void ProcessKeyboardInput(core::FPSCamera& camera)
{
    if(GetAsyncKeyState('W') & 0x8000)
    {
        camera.MoveForward(1.f);
    }
        
    if(GetAsyncKeyState('S') & 0x8000)
    {
        camera.MoveForward(-1.f);
    }
     
    if(GetAsyncKeyState('A') & 0x8000)
    {
        camera.MoveRight(-1.f);
    }

    if(GetAsyncKeyState('D') & 0x8000)
    {
        camera.MoveRight(1.f);
    }
}

int WINAPI WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR lpCmdLine,
    _In_ int nCmdShow)
{
    Window window;
    

    MSG msg = {0};
    while(msg.message != WM_QUIT)
    {
        if(PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            ProcessKeyboardInput(camera);

            DirectX::XMMATRIX viewMatrix = camera.GetViewMatrix();

            float aspectRatio = 1280.f / 720.f;

            DirectX::XMMATRIX projMatrix =
                DirectX::XMMatrixPerspectiveFovLH(
                    DirectX::XM_PIDIV4, // 45도 시야각
                    aspectRatio,
                    0.1f, 100.f
                );

            cube.Render(renderer.GetContext());
            renderer.Render();
        }
    }

    renderer.Cleanup();
    window.Cleanup();

    return 0;
}
