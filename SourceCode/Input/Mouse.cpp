#include "Input/Mouse.h"

#include <utility>

namespace Abomination::Input
{
    namespace
    {
        std::size_t ConvertToIndex(MouseButton button) noexcept
        {
            return std::to_underlying(button);
        }
    }

    void Mouse::StartFrame() noexcept
    {
        m_movement = glm::vec2(0.0f, 0.0f);
        m_wheelMovement = 0.0f;
        m_pressedButtons.fill(false);
        m_releasedButtons.fill(false);
    }

    void Mouse::Move(glm::vec2 offset) noexcept
    {
        m_movement += offset;
    }

    void Mouse::Scroll(float delta) noexcept
    {
        m_wheelMovement += delta;
    }

    void Mouse::PressButton(MouseButton button) noexcept
    {
        const std::size_t index = ConvertToIndex(button);
        if (m_heldButtons[index])
            return;

        m_heldButtons[index] = true;
        m_pressedButtons[index] = true;
    }

    void Mouse::ReleaseButton(MouseButton button) noexcept
    {
        const std::size_t index = ConvertToIndex(button);
        if (!m_heldButtons[index])
            return;

        m_heldButtons[index] = false;
        m_releasedButtons[index] = true;
    }

    void Mouse::ReleaseAllButtons() noexcept
    {
        for (std::size_t index = 0; index < ButtonSlotCount; ++index)
        {
            if (m_heldButtons[index])
            {
                m_heldButtons[index] = false;
                m_releasedButtons[index] = true;
            }
        }
    }

    glm::vec2 Mouse::GetMovement() const noexcept
    {
        return m_movement;
    }

    float Mouse::GetWheelMovement() const noexcept
    {
        return m_wheelMovement;
    }

    bool Mouse::IsButtonHeld(MouseButton button) const noexcept
    {
        return m_heldButtons[ConvertToIndex(button)];
    }

    bool Mouse::WasButtonPressed(MouseButton button) const noexcept
    {
        return m_pressedButtons[ConvertToIndex(button)];
    }

    bool Mouse::WasButtonReleased(MouseButton button) const noexcept
    {
        return m_releasedButtons[ConvertToIndex(button)];
    }
}
