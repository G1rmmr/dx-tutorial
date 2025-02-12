#pragma once

#include <Windows.h>

namespace core
{
    class Renderer;
    class Player;
    class Physics;

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

        void processInput(const float deltaTime);
        void update(const float deltaTime);
        void render();

    private:
        
        HINSTANCE mHInstance;
        HWND mHwnd;
        ULONGLONG mTicksCount;

        Renderer* mRenderer;
        Player* mPlayer;
        Physics* mPhysics;

        int mScreenWidth;
        int mScreenHeight;

        bool mIsRunning;
    };
}
