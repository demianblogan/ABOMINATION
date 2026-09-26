#pragma once

#include "Gameplay/FreeFlyCamera.h"

#include <entt/entt.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/vec3.hpp>

namespace Abomination::Core
{
    struct Transform;
}

namespace Abomination::Input
{
    class ActionStates;
    class Mouse;
}

namespace Abomination::Gameplay
{
    // Values a designer or the player tunes; the controller never changes them itself.
    // They are kept in a separate struct rather than as members of the controller because:
    //   - the whole group will be loaded from a JSON configuration file (data-driven design) and passed in as one value;
    //   - configuration stays apart from the runtime state the controller will get later
    //     (for example the current velocity for smooth acceleration, or a speed multiplier changed by the mouse wheel);
    //   - default values live in one place, and a whole set is easy to create: FreeFlyCameraSettings{.moveSpeed = 2.0f}.
    struct FreeFlyCameraSettings
    {
        // Flying speed in meters per second.
        float moveSpeed = 3.0f;

        // How many times faster the camera flies while MoveFaster is active.
        float fastMoveMultiplier = 4.0f;

        // Radians the camera turns per pixel of mouse movement: 0.0025 is about 0.14 degrees,
        // so moving the mouse by 630 pixels turns the camera by 90 degrees.
        float mouseSensitivity = 0.0025f;
    };

    // The rotation of a camera turned by yaw and pitch: first tilt by the pitch around +X, then turn by the yaw around the
    // world +Y. Applied to the camera's forward (0, 0, -1), it gives the direction the camera looks in.
    [[nodiscard]] glm::quat CalculateCameraRotation(float yaw, float pitch);

    // Creates a free-fly camera entity at position, looking along -Z: Transform, PreviousTransform (it moves in ticks,
    // so it is interpolated), Renderer::CameraLens and FreeFlyCamera.
    entt::entity SpawnFreeFlyCamera(entt::registry& registry, glm::vec3 position);

    // Moves a camera like the free camera of the Unity and Unreal editors (a debug "noclip" camera):
    //   MoveForward/Backward/Left/Right (WASD) - fly relative to where the camera looks, including up and down;
    //   MoveUp/MoveDown (E/Q)                  - fly straight up/down along the world vertical axis;
    //   MoveFaster (Shift)                     - fly faster while held;
    //   LookAroundMode (right mouse button)    - while active, mouse movement turns the camera.
    // It only changes the components of the camera entity; capturing the mouse is up to whoever owns the window.
    class FreeFlyCameraController
    {
    public:
        explicit FreeFlyCameraController(const FreeFlyCameraSettings& settings = {}) noexcept;

        // Turning and moving are two separate calls, because they run at different rates:
        //   UpdateRotation() - once per frame: the view must follow the mouse immediately, without waiting for a tick;
        //   UpdateMovement() - once per simulation tick with the fixed tick duration (see Core::FixedTimestep).
        //
        // Everything the controller works with is passed in, instead of being stored as references in the controller:
        // stored references would dangle after their owner is moved (Application is moved out of Application::Create),
        // the parameters show exactly what is read (const) and what is changed, and any camera can be driven by it.

        // Turns the camera by the mouse movement of this frame while LookAroundMode is active: changes the angles
        // (pitch clamped to FreeFlyCamera::MaxPitch) and sets the rotation of the transform from them.
        void UpdateRotation(FreeFlyCamera& camera, Core::Transform& transform, const Input::ActionStates& actions,
                            const Input::Mouse& mouse) const;

        // Moves the camera transform for one tick. deltaTime is the duration of the tick in seconds.
        void UpdateMovement(Core::Transform& transform, const Input::ActionStates& actions, float deltaTime) const;

    private:
        FreeFlyCameraSettings m_settings;
    };
}
