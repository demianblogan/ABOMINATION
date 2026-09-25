#pragma once

namespace Abomination::Input
{
    class ActionStates;
    class Mouse;
}

namespace Abomination::Renderer
{
    class Camera;
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

    // Moves a camera like the free camera of the Unity and Unreal editors (a debug "noclip" camera):
    //   MoveForward/Backward/Left/Right (WASD) - fly relative to where the camera looks, including up and down;
    //   MoveUp/MoveDown (E/Q)                  - fly straight up/down along the world vertical axis;
    //   MoveFaster (Shift)                     - fly faster while held;
    //   LookAroundMode (right mouse button)    - while active, mouse movement turns the camera.
    // It only changes the camera; capturing the mouse is up to whoever owns the window.
    class FreeFlyCameraController
    {
    public:
        explicit FreeFlyCameraController(const FreeFlyCameraSettings& settings = {}) noexcept;

        // Moves and turns the camera for one frame. deltaTime is the duration of the frame in seconds.
        // Everything the controller works with is passed in, instead of being stored as references in the controller:
        // stored references would dangle after their owner is moved (Application is moved out of Application::Create),
        // the parameters show exactly what is read (const) and what is changed, and any camera can be driven by it.
        void Update(Renderer::Camera& camera, const Input::ActionStates& actions, const Input::Mouse& mouse,
                    float deltaTime) const noexcept;

    private:
        FreeFlyCameraSettings m_settings;
    };
}
