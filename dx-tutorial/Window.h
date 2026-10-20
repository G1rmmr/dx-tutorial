#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>

#include "Event.h"

#include <deque>
#include <exception>
#include <string>

namespace dx
{
    class Window final
    {
    public:
        Window(
            LPCWSTR title,
            HINSTANCE hInstance,
            UINT width,
            UINT height,
            int showCommand);

        ~Window() noexcept;

        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;
        Window(Window&&) = delete;
        Window& operator=(Window&&) = delete;

        bool PollEvent(Event& event);
        void WaitForEvents(DWORD timeout = INFINITE) const noexcept;
        void SetMouseLook(bool enabled) noexcept;
        bool CenterCursor(int& x, int& y) const noexcept;

        bool IsActive() const noexcept { return mHwnd && mHasFocus && !mIsMinimized; }
        bool IsMinimized() const noexcept { return mIsMinimized; }

        inline HWND GetHandle() const noexcept
        {
            return mHwnd;
        }

        inline UINT GetWidth() const noexcept
        {
            return mScreenWidth;
        }

        inline UINT GetHeight() const noexcept
        {
            return mScreenHeight;
        }

    private:
        std::deque<Event> mEvents;
        std::exception_ptr mMessageException;

        HWND mHwnd = HWND();
        HINSTANCE mInstance;
        std::wstring mClassName;
        ATOM mClassAtom = 0;

        UINT mScreenWidth;
        UINT mScreenHeight;

        bool mHasFocus = false;
        bool mIsMinimized = false;
        bool mMouseLook = false;
        UINT mPressedMouseButtons = 0;

        void pushEvent(Event event);
        void releaseMouseCapture() noexcept;
        void destroy() noexcept;

        LRESULT handleMessage(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

        static LRESULT CALLBACK windowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    };
}
