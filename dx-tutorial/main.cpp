#include <windows.h>

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    WNDCLASSEX wc = {
        sizeof(WNDCLASSEX),
        CS_CLASSDC,
        WindowProc,
        0, 0,
        GetModuleHandle(NULL),
        NULL, NULL, NULL, NULL,
        L"DX11WindowClass", NULL};

    RegisterClassEx(&wc);

    HWND hwnd = CreateWindow(
        wc.lpszClassName,
        L"DirectX 11 √ ±‚»≠",
        WS_OVERLAPPEDWINDOW,
        100, 100, 800, 600,
        NULL, NULL, wc.hInstance, NULL);

    if(!hwnd) return -1;

    ShowWindow(hwnd, nCmdShow);


    MSG msg = {};
    while(GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}
