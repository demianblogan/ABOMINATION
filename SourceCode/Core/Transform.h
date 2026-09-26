#pragma once

#include <glm/gtc/quaternion.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

namespace Abomination::Core
{
    // Where an entity is, how it is turned and how big it is: the component almost every entity has.
    // Used by gameplay (moving things), physics (later) and the renderer (the model matrix), so it lives in Core.
    //
    // The rotation is a quaternion instead of three angles. Three angles (yaw, pitch, roll) are easy to read but
    // awkward to combine: turning an already turned object means converting to matrices and back, and at some angles
    // two of the axes line up and one way of turning is lost ("gimbal lock"). A quaternion stores one rotation around
    // one axis in 4 numbers; two quaternions are combined by multiplying them, which never loses an axis.
    // glm builds them from understandable values: glm::angleAxis(angle, axis).
    struct Transform
    {
        glm::vec3 position{0.0f};

        // glm::quat(w, x, y, z): w = 1 and x = y = z = 0 is the identity, "not turned at all".
        glm::quat rotation{1.0f, 0.0f, 0.0f, 0.0f};

        glm::vec3 scale{1.0f};
    };

    // The model matrix of the transform: moves a mesh from its own coordinates into the world.
    // It scales first, then rotates, then moves (translation * rotation * scale, applied right to left).
    [[nodiscard]] glm::mat4 CalculateModelMatrix(const Transform& transform);
}
