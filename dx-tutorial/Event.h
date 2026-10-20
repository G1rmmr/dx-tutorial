#pragma once

#include <cstdint>
#include <variant>

namespace dx
{
    enum class Key
    {
        W,
        A,
        S,
        D,
        Space,
        Escape,
        Count
    };

    enum class MouseButton
    {
        Left,
        Right,
        Middle,
        Count
    };

    struct KeyDownEvent
    {
        Key KeyCode;
        bool IsRepeat;
    };

    struct KeyUpEvent
    {
        Key KeyCode;
    };

    struct MouseButtonDownEvent
    {
        MouseButton Button;
        int X;
        int Y;
    };

    struct MouseButtonUpEvent
    {
        MouseButton Button;
        int X;
        int Y;
    };

    struct FocusLostEvent {};
    struct MouseCaptureLostEvent {};

    struct QuitEvent {};

    struct CloseEvent {};

    struct ResizeEvent
    {
        std::uint32_t Width;
        std::uint32_t Height;
    };

    struct MouseMoveEvent
    {
        int X;
        int Y;
    };

    using Event = std::variant<
        QuitEvent,
        CloseEvent,
        ResizeEvent,
        KeyDownEvent,
        KeyUpEvent,
        MouseMoveEvent,
        MouseButtonDownEvent,
        MouseButtonUpEvent,
        FocusLostEvent,
        MouseCaptureLostEvent>;
}
