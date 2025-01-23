#include "Window.h"
#include "Game.h"

core::Window::Window()
    : mHwnd(nullptr)
    , mGame(nullptr)
{
    ZeroMemory(&mWC, sizeof(WNDCLASSEX));
}

core::Window::~Window()
{
    Cleanup();
}

bool core::Window::Initialize(Game* game, HINSTANCE hInstance, int nCmdShow)
{
    mGame = game;

    mWC.cbSize = sizeof(WNDCLASSEX);
    mWC.style = CS_HREDRAW | CS_VREDRAW;
    mWC.lpfnWndProc = Window::WndProc;  // 정적 함수 지정
    mWC.cbClsExtra = 0;
    mWC.cbWndExtra = 0;
    mWC.hInstance = hInstance;
    mWC.hCursor = LoadCursor(NULL, IDC_ARROW);
    mWC.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    mWC.lpszClassName = L"MyWindowClass";

    if(!RegisterClassEx(&mWC))
    {
        MessageBox(nullptr, L"RegisterClassEx Failed!", L"Error", MB_OK);
        return false;
    }

    mHwnd = CreateWindow(
        mWC.lpszClassName,
        L"DX11 FPS Game",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        1280, 720,
        NULL, NULL,
        hInstance, this
    );

    if(!mHwnd)
    {
        MessageBox(nullptr, L"CreateWindow Failed!", L"Error", MB_OK);
        return false;
    }

    SetWindowLongPtr(mHwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

    ShowWindow(mHwnd, nCmdShow);
    UpdateWindow(mHwnd);
    return mHwnd != NULL;
}

LRESULT CALLBACK core::Window::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    Window* window = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

    switch(message)
    {
    case WM_MOUSEMOVE:
    {
        if(window && window->mGame)
        {
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            // Game 쪽에 마우스 이동 이벤트 전달
            window->mGame->OnMouseMove(x, y);
        }
    }
    break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
}

void core::Window::Cleanup()
{
    if(mHwnd)
    {
        DestroyWindow(mHwnd);
        mHwnd = nullptr;
    }
    UnregisterClass(mWC.lpszClassName, mWC.hInstance);
}

HWND core::Window::GetHwnd() const
{
    return mHwnd;
}