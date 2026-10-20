#include "Input.h"

#include <type_traits>

using namespace dx;

namespace
{
    constexpr std::size_t ToIndex(Key key) noexcept
    {
        return static_cast<std::size_t>(key);
    }

    constexpr std::size_t ToIndex(MouseButton button) noexcept
    {
        return static_cast<std::size_t>(button);
    }
}

void Input::BeginFrame() noexcept
{
    mPreviousKeys = mCurrentKeys;
    mPreviousMouseButtons = mCurrentMouseButtons;

    mMouseDeltaX = 0;
    mMouseDeltaY = 0;
}

void Input::HandleEvent(const Event& event)
{
    const auto updateMousePosition = [this](int x, int y)
    {
        if (mHasMousePosition)
        {
            mMouseDeltaX += x - mMouseX;
            mMouseDeltaY += y - mMouseY;
        }

        mMouseX = x;
        mMouseY = y;
        mHasMousePosition = true;
    };

    std::visit([this, &updateMousePosition](const auto& value)
    {
        using T = std::decay_t<decltype(value)>;

        if constexpr (std::is_same_v<T, KeyDownEvent>)
        {
            const std::size_t index = ToIndex(value.KeyCode);
            if (index < keyCount)
            {
                mCurrentKeys[index] = true;
            }
        }
        else if constexpr (std::is_same_v<T, KeyUpEvent>)
        {
            const std::size_t index = ToIndex(value.KeyCode);
            if (index < keyCount)
            {
                mCurrentKeys[index] = false;
            }
        }
        else if constexpr (std::is_same_v<T, MouseMoveEvent>)
        {
            updateMousePosition(value.X, value.Y);
        }
        else if constexpr (std::is_same_v<T, MouseButtonDownEvent>)
        {
            const std::size_t index = ToIndex(value.Button);
            if (index < mouseButtonCount)
            {
                mCurrentMouseButtons[index] = true;
            }

            updateMousePosition(value.X, value.Y);
        }
        else if constexpr (std::is_same_v<T, MouseButtonUpEvent>)
        {
            const std::size_t index = ToIndex(value.Button);
            if (index < mouseButtonCount)
            {
                mCurrentMouseButtons[index] = false;
            }

            updateMousePosition(value.X, value.Y);
        }
        else if constexpr (std::is_same_v<T, FocusLostEvent>)
        {
            clear();
        }
        else if constexpr (std::is_same_v<T, MouseCaptureLostEvent>)
        {
            mCurrentMouseButtons.fill(false);
            mPreviousMouseButtons.fill(false);
            mMouseDeltaX = 0;
            mMouseDeltaY = 0;
            mHasMousePosition = false;
        }
    }, event);
}

bool Input::IsKeyDown(Key key) const noexcept
{
    const std::size_t index = ToIndex(key);
    return index < keyCount && mCurrentKeys[index];
}

bool Input::WasKeyPressed(Key key) const noexcept
{
    const std::size_t index = ToIndex(key);

    return index < keyCount && mCurrentKeys[index] && !mPreviousKeys[index];
}

bool Input::WasKeyReleased(Key key) const noexcept
{
    const std::size_t index = ToIndex(key);

    return index < keyCount && !mCurrentKeys[index] && mPreviousKeys[index];
}

bool Input::IsMouseButtonDown(MouseButton button) const noexcept
{
    const std::size_t index = ToIndex(button);
    return index < mouseButtonCount && mCurrentMouseButtons[index];
}

bool Input::WasMouseButtonPressed(MouseButton button) const noexcept
{
    const std::size_t index = ToIndex(button);

    return index < mouseButtonCount &&
        mCurrentMouseButtons[index] && !mPreviousMouseButtons[index];
}

bool Input::WasMouseButtonReleased(MouseButton button) const noexcept
{
    const std::size_t index = ToIndex(button);

    return index < mouseButtonCount &&
        !mCurrentMouseButtons[index] && mPreviousMouseButtons[index];
}

int Input::GetMouseX() const noexcept
{
    return mMouseX;
}

int Input::GetMouseY() const noexcept
{
    return mMouseY;
}

int Input::GetMouseDeltaX() const noexcept
{
    return mMouseDeltaX;
}

int Input::GetMouseDeltaY() const noexcept
{
    return mMouseDeltaY;
}

void Input::clear() noexcept
{
    mCurrentKeys.fill(false);
    mPreviousKeys.fill(false);

    mCurrentMouseButtons.fill(false);
    mPreviousMouseButtons.fill(false);

    mMouseDeltaX = 0;
    mMouseDeltaY = 0;

    mHasMousePosition = false;
}
