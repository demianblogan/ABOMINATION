#include "Renderer/Camera.h"

#include <glm/common.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>

namespace Abomination::Renderer
{
    namespace
    {
        constexpr glm::vec3 WorldUp{0.0f, 1.0f, 0.0f};
    }

    glm::vec3 Camera::GetPosition() const noexcept
    {
        return m_position;
    }

    void Camera::SetPosition(glm::vec3 position) noexcept
    {
        m_position = position;
    }

    void Camera::Move(glm::vec3 offset) noexcept
    {
        m_position += offset;
    }

    float Camera::GetYaw() const noexcept
    {
        return m_yaw;
    }

    float Camera::GetPitch() const noexcept
    {
        return m_pitch;
    }

    void Camera::SetRotation(float yaw, float pitch) noexcept
    {
        m_yaw = yaw;
        m_pitch = glm::clamp(pitch, -MaxPitch, MaxPitch);
    }

    void Camera::Rotate(float yawDelta, float pitchDelta) noexcept
    {
        SetRotation(m_yaw + yawDelta, m_pitch + pitchDelta);
    }

    float Camera::GetVerticalFOV() const noexcept
    {
        return m_verticalFOV;
    }

    void Camera::SetVerticalFOV(float verticalFOV) noexcept
    {
        m_verticalFOV = verticalFOV;
    }

    glm::vec3 Camera::GetForward() const noexcept
    {
        // Start from "straight ahead" (0, 0, -1), tilt it up by the pitch, then turn it around +Y by the yaw:
        //   the pitch splits the unit length into a vertical part  sin(pitch)  and a horizontal part  cos(pitch);
        //   the yaw turns the horizontal part: at yaw 0 it points to -Z, at yaw 90 degrees to -X (to the left).
        const float horizontalLength = glm::cos(m_pitch);

        return glm::vec3(-glm::sin(m_yaw) * horizontalLength, glm::sin(m_pitch), -glm::cos(m_yaw) * horizontalLength);
    }

    glm::vec3 Camera::GetRight() const noexcept
    {
        // The cross product of two vectors is perpendicular to both. Forward x WorldUp points to the right of the camera
        // and always lies in the horizontal plane, so strafing never goes up or down.
        return glm::normalize(glm::cross(GetForward(), WorldUp));
    }

    glm::vec3 Camera::GetUp() const noexcept
    {
        // Perpendicular to both forward and right: the "top of the screen" direction, tilted together with the pitch.
        return glm::cross(GetRight(), GetForward());
    }

    glm::mat4 Camera::GetViewMatrix() const noexcept
    {
        // glm::lookAt(eye, target, up): the camera stands at eye and looks at target. Any point along the view direction
        // works as the target, so the position plus the forward vector is used.
        return glm::lookAt(m_position, m_position + GetForward(), WorldUp);
    }

    glm::mat4 Camera::GetProjectionMatrix(float aspectRatio) const noexcept
    {
        return glm::perspective(m_verticalFOV, aspectRatio, m_nearPlane, m_farPlane);
    }
}
