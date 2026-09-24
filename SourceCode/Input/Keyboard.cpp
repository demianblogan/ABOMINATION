#include "Input/Keyboard.h"

#include <utility>

namespace Abomination::Input
{
    namespace
    {
        std::size_t ConvertToIndex(Key key) noexcept
        {
            return std::to_underlying(key);
        }
    }

    void Keyboard::StartFrame() noexcept
    {
        m_pressedKeys.reset();
        m_releasedKeys.reset();
    }

    void Keyboard::PressKey(Key key) noexcept
    {
        const std::size_t index = ConvertToIndex(key);
        if (m_heldKeys.test(index))
            return;

        m_heldKeys.set(index);
        m_pressedKeys.set(index);
    }

    void Keyboard::ReleaseKey(Key key) noexcept
    {
        const std::size_t index = ConvertToIndex(key);
        if (!m_heldKeys.test(index))
            return;

        m_heldKeys.reset(index);
        m_releasedKeys.set(index);
    }

    void Keyboard::ReleaseAllKeys() noexcept
    {
        // Every held key becomes released in this frame, so the game still sees its "up" moment.
        m_releasedKeys |= m_heldKeys;
        m_heldKeys.reset();
    }

    bool Keyboard::IsKeyHeld(Key key) const noexcept
    {
        return m_heldKeys.test(ConvertToIndex(key));
    }

    bool Keyboard::WasKeyPressed(Key key) const noexcept
    {
        return m_pressedKeys.test(ConvertToIndex(key));
    }

    bool Keyboard::WasKeyReleased(Key key) const noexcept
    {
        return m_releasedKeys.test(ConvertToIndex(key));
    }
}
