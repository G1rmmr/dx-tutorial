#pragma once

#include <windows.h>

#include <memory>

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

        void Update(const float deltaTime);
        void Render();

        void OnMouseMove(int x, int y);

    private:
        std::unique_ptr<Window> mWindow;
        std::unique_ptr<FPSCamera> mCamera;
        std::unique_ptr<DXRenderer> mRenderer;
        std::unique_ptr<Cube> mCube;
    };

}