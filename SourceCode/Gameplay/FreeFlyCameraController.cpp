#include "Gameplay/FreeFlyCameraController.h"

#include "Input/ActionStates.h"
#include "Input/Mouse.h"
#include "Renderer/Camera.h"

#include <glm/geometric.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace Abomination::Gameplay
{
    using Input::Action;

    namespace
    {
        constexpr glm::vec3 WorldUp{0.0f, 1.0f, 0.0f};

        // 1.0 while the action is active, 0.0 otherwise: lets opposite actions cancel each other out by subtraction.
        float GetActionValue(const Input::ActionStates& actions, Action action) noexcept
        {
            return actions.IsActionActive(action) ? 1.0f : 0.0f;
        }
    }

    FreeFlyCameraController::FreeFlyCameraController(const FreeFlyCameraSettings& settings) noexcept
        : m_settings(settings)
    {}

    void FreeFlyCameraController::Update(Renderer::Camera& camera, const Input::ActionStates& actions,
                                         const Input::Mouse& mouse, float deltaTime) const noexcept
    {
        // 1. Turning. Only while LookAroundMode is active, and not in the frame it starts: switching the mouse into
        //    relative mode can produce one big jump of movement in that frame, which would snap the camera.
        //    The mouse movement is already "per frame", so it is not multiplied by deltaTime.
        //    Moving the mouse to the right (+X) must turn right, which is a negative yaw in our camera;
        //    moving it up (-Y, screen coordinates grow downwards) must look up, which is a positive pitch.
        const bool isLookingAround =
            actions.IsActionActive(Action::LookAroundMode) && !actions.WasActionStarted(Action::LookAroundMode);
        if (isLookingAround)
        {
            const glm::vec2 mouseMovement = mouse.GetMovement();
            camera.Rotate(-mouseMovement.x * m_settings.mouseSensitivity, -mouseMovement.y * m_settings.mouseSensitivity);
        }

        // 2. Direction of movement. Every pair of opposite actions gives -1, 0 or +1 along its axis:
        //    W and S together cancel out, so the camera stays in place.
        const float forwardAmount =
            GetActionValue(actions, Action::MoveForward) - GetActionValue(actions, Action::MoveBackward);
        const float rightAmount = GetActionValue(actions, Action::MoveRight) - GetActionValue(actions, Action::MoveLeft);
        const float upAmount = GetActionValue(actions, Action::MoveUp) - GetActionValue(actions, Action::MoveDown);

        // Forward and right follow the camera (forward includes looking up or down); up is the world vertical axis.
        glm::vec3 direction = camera.GetForward() * forwardAmount + camera.GetRight() * rightAmount + WorldUp * upAmount;
        if (direction == glm::vec3(0.0f))
            return;

        // Without normalizing, W + D together would give a vector of length about 1.41: diagonal flying would be
        // 41% faster than straight flying. Normalizing makes the length 1 in every direction.
        direction = glm::normalize(direction);

        // 3. Distance for this frame: speed (meters per second) times the frame duration (seconds) gives meters.
        //    This keeps the speed the same at 30 and at 300 frames per second.
        float speed = m_settings.moveSpeed;
        if (actions.IsActionActive(Action::MoveFaster))
            speed *= m_settings.fastMoveMultiplier;

        camera.Move(direction * speed * deltaTime);
    }
}
