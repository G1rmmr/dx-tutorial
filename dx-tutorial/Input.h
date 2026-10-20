#pragma once

#include "Event.h"

#include <array>
#include <cstddef>

namespace dx
{
    class Input final
    {
    public:
        void BeginFrame() noexcept;
        void HandleEvent(const Event& event);

        [[nodiscard]]
        bool IsKeyDown(Key key) const noexcept;

        [[nodiscard]]
        bool WasKeyPressed(Key key) const noexcept;

        [[nodiscard]]
        bool WasKeyReleased(Key key) const noexcept;

        [[nodiscard]]
        bool IsMouseButtonDown(MouseButton button) const noexcept;

        [[nodiscard]]
        bool WasMouseButtonPressed(MouseButton button) const noexcept;

        [[nodiscard]]
        bool WasMouseButtonReleased(MouseButton button) const noexcept;

        [[nodiscard]]
        int GetMouseX() const noexcept;

        [[nodiscard]]
        int GetMouseY() const noexcept;

        [[nodiscard]]
        int GetMouseDeltaX() const noexcept;

        [[nodiscard]]
        int GetMouseDeltaY() const noexcept;

    private:
        static constexpr std::size_t keyCount = static_cast<std::size_t>(Key::Count);
        static constexpr std::size_t mouseButtonCount = static_cast<std::size_t>(MouseButton::Count);

        std::array<bool, keyCount> mCurrentKeys{};
        std::array<bool, keyCount> mPreviousKeys{};

        std::array<bool, mouseButtonCount> mCurrentMouseButtons{};
        std::array<bool, mouseButtonCount> mPreviousMouseButtons{};

        int mMouseX = 0;
        int mMouseY = 0;

        int mMouseDeltaX = 0;
        int mMouseDeltaY = 0;

        bool mHasMousePosition = false;

        void clear() noexcept;
    };
}