#include "Game.h"

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int)
{
    Game game;
    if(!game.Initialize(hInstance))
    {
        MessageBox(nullptr, L"Failed to initialize game!", L"Error", MB_OK | MB_ICONERROR);
        return -1;
    }

    MSG msg = {};
    while(GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    game.Cleanup();
    return 0;
}
