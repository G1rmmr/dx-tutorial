#pragma once

#include <vector>
#include <Windows.h>

namespace core
{
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

        class Renderer* mRenderer;
        class Player* mPlayer;
        class Physics* mPhysics;
        class UI* mUI;

        std::vector<class Actor*> mActors;

        int mScreenWidth;
        int mScreenHeight;
        
        int mScore;

        bool mIsRunning;
    };
}
