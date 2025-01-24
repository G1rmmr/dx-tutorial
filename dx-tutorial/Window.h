#pragma once

#include <windows.h>

#include "FPSCamera.h"

namespace core
{
    class Game;

    class Window
    {
    public:
        Window();
        ~Window();

        static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

        bool Initialize(Game* game, HINSTANCE hInstance, int nCmdShow);
        void Cleanup();

        HWND GetHwnd() const;

    private:
        HWND mHwnd;
        WNDCLASSEX mWC;

        Game* mGame;
    };
}