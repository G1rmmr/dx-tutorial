#include <iostream>

#include "Game.h"

int WINAPI WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR lpCmdLine,
    _In_ int nCmdShow)
{
    core::Game game;
    if(!game.Initialize(hInstance, nCmdShow))
    {
        std::cerr << "GAME INITIALIZATION FAILED!!\n";
        return -1;
    }

    LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);

    LARGE_INTEGER lastTime;
    QueryPerformanceCounter(&lastTime);

    MSG msg = {0};
    while(msg.message != WM_QUIT)
    {
        if(PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            LARGE_INTEGER currTime;
            QueryPerformanceCounter(&currTime);

            double deltaTime = static_cast<double>(currTime.QuadPart - lastTime.QuadPart)
                / static_cast<double>(freq.QuadPart);

            lastTime = currTime;

            game.Update(static_cast<float>(deltaTime));
            game.Render();
        }
    }

    game.Cleanup();
    return static_cast<int>(msg.wParam);
}
