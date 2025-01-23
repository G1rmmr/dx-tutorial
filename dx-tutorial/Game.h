#pragma once

#include <windows.h>

namespace core
{
    class Window;
    class DXRenderer;
    class FPSCamera;
    class Cube;

    class Game
    {
    public:
        Game();
        ~Game();

        bool Initialize(HINSTANCE hInstance, int nCmdShow);

        void Cleanup();

        void Update();
        void Render();

        void OnMouseMove(int x, int y);

    private:
        Window* mWindow;

        FPSCamera* mCamera;
        DXRenderer* mRenderer;
        Cube* mCube;
    };

}