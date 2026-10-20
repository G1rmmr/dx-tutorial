#include "Window.h"

#include <windowsx.h>

#include <limits>
#include <optional>
#include <stdexcept>
#include <system_error>
#include <utility>

using namespace dx;

namespace
{
    std::optional<Key> translateKey(WPARAM key)
    {
        switch (key)
        {
        case 'W': return Key::W;
        case 'A': return Key::A;
        case 'S': return Key::S;
        case 'D': return Key::D;
        case VK_SPACE: return Key::Space;
        case VK_ESCAPE: return Key::Escape;
        default: return std::nullopt;
        }
    }

    [[noreturn]] void throwWindowsError(const char* operation)
    {
        throw std::system_error(static_cast<int>(GetLastError()),
            std::system_category(), operation);
    }
}

Window::Window(LPCWSTR title, HINSTANCE hInstance, UINT width, UINT height, int showCommand)
    : mInstance(hInstance)
    , mScreenWidth(width)
    , mScreenHeight(height)
{
    if (!title || !hInstance || width == 0 || height == 0 ||
        width > static_cast<UINT>(std::numeric_limits<LONG>::max() / 2) ||
        height > static_cast<UINT>(std::numeric_limits<LONG>::max() / 2))
    {
        throw std::invalid_argument("Invalid window parameters");
    }

    // Each instance owns its class registration for independent lifetimes.
    static LONG nextClassId = 0;
    mClassName = L"DX11GameWindowClass_" +
        std::to_wstring(GetCurrentProcessId()) + L"_" +
        std::to_wstring(InterlockedIncrement(&nextClassId));

    WNDCLASSEXW windowClass{};
    windowClass.cbSize = sizeof(windowClass);
    windowClass.style = CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc = windowProc;
    windowClass.hInstance = mInstance;
    windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
    windowClass.lpszClassName = mClassName.c_str();
    mClassAtom = RegisterClassExW(&windowClass);
    if (!mClassAtom)
    {
        throwWindowsError("RegisterClassExW");
    }

    try
    {
        constexpr DWORD style = WS_OVERLAPPEDWINDOW;
        RECT rectangle{0, 0, static_cast<LONG>(width), static_cast<LONG>(height)};
        if (!AdjustWindowRectEx(&rectangle, style, FALSE, 0))
        {
            throwWindowsError("AdjustWindowRectEx");
        }

        mHwnd = CreateWindowExW(0, mClassName.c_str(), title, style,
            CW_USEDEFAULT, CW_USEDEFAULT, rectangle.right - rectangle.left,
            rectangle.bottom - rectangle.top, nullptr, nullptr, mInstance, this);
        if (!mHwnd)
        {
            throwWindowsError("CreateWindowExW");
        }

        ShowWindow(mHwnd, showCommand);
        UpdateWindow(mHwnd);
        if (mMessageException)
        {
            std::rethrow_exception(mMessageException);
        }
    }
    catch (...)
    {
        destroy();
        throw;
    }
}

Window::~Window() noexcept
{
    destroy();
}

void Window::destroy() noexcept
{
    releaseMouseCapture();
    if (mHwnd)
    {
        // WM_NCDESTROY disconnects GWLP_USERDATA while this object is alive.
        DestroyWindow(mHwnd);
        mHwnd = nullptr;
    }
    if (mClassAtom)
    {
        UnregisterClassW(mClassName.c_str(), mInstance);
        mClassAtom = 0;
    }
}

bool Window::PollEvent(Event& event)
{
    if (mMessageException)
    {
        std::rethrow_exception(mMessageException);
    }
    MSG message{};
    while (PeekMessage(&message, nullptr, 0, 0, PM_REMOVE))
    {
        if (message.message == WM_QUIT)
        {
            pushEvent(QuitEvent{});
            continue;
        }
        TranslateMessage(&message);
        DispatchMessage(&message);
        if (mMessageException)
        {
            std::rethrow_exception(mMessageException);
        }
    }
    if (mEvents.empty())
    {
        return false;
    }
    event = std::move(mEvents.front());
    mEvents.pop_front();
    return true;
}

void Window::WaitForEvents(DWORD timeout) const noexcept
{
    if (mEvents.empty())
    {
        MsgWaitForMultipleObjectsEx(0, nullptr, timeout, QS_ALLINPUT, MWMO_INPUTAVAILABLE);
    }
}

void Window::SetMouseLook(bool enabled) noexcept
{
    mMouseLook = enabled;
    SetCursor(enabled && IsActive() ? nullptr : LoadCursor(nullptr, IDC_ARROW));
}

bool Window::CenterCursor(int& x, int& y) const noexcept
{
    if (!mHwnd || !IsActive())
    {
        return false;
    }
    x = static_cast<int>(mScreenWidth / 2);
    y = static_cast<int>(mScreenHeight / 2);
    POINT point{x, y};
    return ClientToScreen(mHwnd, &point) && SetCursorPos(point.x, point.y);
}

void Window::pushEvent(Event event)
{
    mEvents.emplace_back(std::move(event));
}

void Window::releaseMouseCapture() noexcept
{
    mPressedMouseButtons = 0;
    if (mHwnd && GetCapture() == mHwnd)
    {
        ReleaseCapture();
    }
}

LRESULT Window::handleMessage(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CLOSE:
        pushEvent(CloseEvent{});
        return 0;
    case WM_DESTROY:
        pushEvent(QuitEvent{});
        return 0;
    case WM_NCDESTROY:
        SetWindowLongPtr(hwnd, GWLP_USERDATA, 0);
        mHwnd = nullptr;
        return DefWindowProc(hwnd, message, wParam, lParam);
    case WM_SIZE:
    {
        const UINT width = LOWORD(lParam);
        const UINT height = HIWORD(lParam);
        mIsMinimized = wParam == SIZE_MINIMIZED;
        // Keep the last usable render size while minimized.
        if (width && height)
        {
            mScreenWidth = width;
            mScreenHeight = height;
        }
        pushEvent(ResizeEvent{width, height});
        return 0;
    }
    case WM_SETFOCUS:
        mHasFocus = true;
        return 0;
    case WM_SETCURSOR:
        if (LOWORD(lParam) == HTCLIENT && IsActive() && mMouseLook)
        {
            SetCursor(nullptr);
            return TRUE;
        }
        break;
    case WM_KILLFOCUS:
        mHasFocus = false;
        releaseMouseCapture();
        SetCursor(LoadCursor(nullptr, IDC_ARROW));
        pushEvent(FocusLostEvent{});
        return 0;
    case WM_CAPTURECHANGED:
        if (mPressedMouseButtons && reinterpret_cast<HWND>(lParam) != hwnd)
        {
            mPressedMouseButtons = 0;
            pushEvent(MouseCaptureLostEvent{});
        }
        return 0;
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
        if (const auto key = translateKey(wParam))
        {
            pushEvent(KeyDownEvent{*key, (lParam & (1LL << 30)) != 0});
            return 0;
        }
        break;
    case WM_KEYUP:
    case WM_SYSKEYUP:
        if (const auto key = translateKey(wParam))
        {
            pushEvent(KeyUpEvent{*key});
            return 0;
        }
        break;
    case WM_MOUSEMOVE:
        pushEvent(MouseMoveEvent{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)});
        return 0;
    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_MBUTTONDOWN:
    {
        const MouseButton button = message == WM_LBUTTONDOWN ? MouseButton::Left :
            (message == WM_RBUTTONDOWN ? MouseButton::Right : MouseButton::Middle);
        mPressedMouseButtons |= 1U << static_cast<UINT>(button);
        SetCapture(hwnd);
        pushEvent(MouseButtonDownEvent{button, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)});
        return 0;
    }
    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
    case WM_MBUTTONUP:
    {
        const MouseButton button = message == WM_LBUTTONUP ? MouseButton::Left :
            (message == WM_RBUTTONUP ? MouseButton::Right : MouseButton::Middle);
        mPressedMouseButtons &= ~(1U << static_cast<UINT>(button));
        pushEvent(MouseButtonUpEvent{button, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)});
        if (!mPressedMouseButtons)
        {
            releaseMouseCapture();
        }
        return 0;
    }
    }
    return DefWindowProc(hwnd, message, wParam, lParam);
}

LRESULT CALLBACK Window::windowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    Window* window = nullptr;
    if (message == WM_NCCREATE)
    {
        const auto* createInfo = reinterpret_cast<const CREATESTRUCTW*>(lParam);
        window = static_cast<Window*>(createInfo->lpCreateParams);
        SetLastError(0);
        if (!SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window)) && GetLastError())
        {
            return FALSE;
        }
        window->mHwnd = hwnd;
    }
    else
    {
        window = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    }
    if (window)
    {
        // Exceptions must not unwind through a Win32 callback. PollEvent or
        // construction rethrows them at the normal C++ boundary.
        try
        {
            return window->handleMessage(hwnd, message, wParam, lParam);
        }
        catch (...)
        {
            window->mMessageException = std::current_exception();
            return message == WM_NCCREATE ? FALSE : 0;
        }
    }
    return DefWindowProc(hwnd, message, wParam, lParam);
}
