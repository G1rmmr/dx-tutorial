#include <iostream>

#include "Game.h"

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int nCmdShow)
{
    core::Game game;
    if(!game.Initialize(hInstance, SCREEN_WIDTH, SCREEN_HEIGHT, nCmdShow))
    {
        std::cerr << "GAME INITIALIZATION FAILED!!\n";
        return -1;
    }

    game.Run();
    game.Cleanup();

    return 0;
}
