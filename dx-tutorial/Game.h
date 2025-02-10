#pragma once

#include <Windows.h>

namespace core
{
    class Renderer;
    class Player;

    class Game
    {
    public:
        Game();
        ~Game();

        bool Initialize(HINSTANCE hInstance, int width, int height, int nCmdShow);
        void Run();
        void Cleanup();

    private:
        static LRESULT CALLBACK sWindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

        LRESULT windowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

        bool createWindow(HINSTANCE hInstance, int width, int height, int nCmdShow);

        void processInput(float deltaTime);
        void update(float deltaTime);
        void render();

    private:
        
        HINSTANCE mHInstance;
        HWND mHwnd;
        ULONGLONG mTicksCount;

        Renderer* mRenderer;
        Player* mPlayer;

        int mScreenWidth;
        int mScreenHeight;

        bool mIsRunning;
    };
}
