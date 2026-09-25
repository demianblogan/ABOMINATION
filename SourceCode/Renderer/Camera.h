#pragma once

#include <glm/mat4x4.hpp>
#include <glm/trigonometric.hpp>
#include <glm/vec3.hpp>

namespace Abomination::Renderer
{
    // The "lens" the scene is drawn through: where it stands, where it looks and how wide it sees.
    // It knows nothing about input: controllers (the free-fly camera, later the player's eyes) move and turn it,
    // the renderer only reads its matrices.
    //
    // The view direction is stored as two angles, in radians (no roll: the horizon always stays level):
    //   yaw   - turn left/right around the world up axis (+Y). 0 looks along -Z (OpenGL's "forward");
    //           a positive yaw turns to the LEFT (counter-clockwise when seen from above).
    //   pitch - tilt up/down. 0 looks at the horizon, a positive pitch looks up.
    class Camera
    {
    public:
        // Pitch is kept within this range. At exactly +-90 degrees the view direction would be parallel to the up axis,
        // and the camera could no longer tell where its right side is: the picture would suddenly flip.
        static constexpr float MaxPitch = glm::radians(89.0f);

        [[nodiscard]] glm::vec3 GetPosition() const noexcept;
        void SetPosition(glm::vec3 position) noexcept;

        // Moves the camera by the offset in world coordinates.
        void Move(glm::vec3 offset) noexcept;

        [[nodiscard]] float GetYaw() const noexcept;
        [[nodiscard]] float GetPitch() const noexcept;

        // Sets both angles; pitch is clamped to [-MaxPitch, MaxPitch].
        void SetRotation(float yaw, float pitch) noexcept;

        // Adds to both angles; pitch is clamped to [-MaxPitch, MaxPitch].
        void Rotate(float yawDelta, float pitchDelta) noexcept;

        // Vertical field of view in radians: how wide the camera sees from the bottom edge of the screen to the top.
        [[nodiscard]] float GetVerticalFOV() const noexcept;
        void SetVerticalFOV(float verticalFOV) noexcept;

        // Unit vectors in world coordinates: where the camera looks, its right side and its top side.
        [[nodiscard]] glm::vec3 GetForward() const noexcept;
        [[nodiscard]] glm::vec3 GetRight() const noexcept;
        [[nodiscard]] glm::vec3 GetUp() const noexcept;

        // Moves the world so that the camera ends up at the origin looking along -Z.
        [[nodiscard]] glm::mat4 GetViewMatrix() const noexcept;

        // Applies perspective. aspectRatio is the width of the drawable area divided by its height.
        [[nodiscard]] glm::mat4 GetProjectionMatrix(float aspectRatio) const noexcept;

    private:
        glm::vec3 m_position{0.0f, 0.0f, 0.0f};
        float m_yaw = 0.0f;
        float m_pitch = 0.0f;
        float m_verticalFOV = glm::radians(60.0f);

        // Nothing closer than the near plane or farther than the far plane (in meters) is drawn.
        float m_nearPlane = 0.1f;
        float m_farPlane = 100.0f;
    };
}
