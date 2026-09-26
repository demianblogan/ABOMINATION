#pragma once

#include <glm/trigonometric.hpp>

namespace Abomination::Gameplay
{
    // Component: the view direction of a free-fly camera entity as two angles, in radians (no roll: the horizon always
    // stays level). FreeFlyCameraController changes these angles and builds the Transform rotation from them; angles are
    // kept because mouse movement adds to them directly, and a limit on pitch is easy to apply to an angle.
    //   yaw   - turn left/right around the world up axis (+Y). 0 looks along -Z (OpenGL's "forward");
    //           a positive yaw turns to the LEFT (counter-clockwise when seen from above).
    //   pitch - tilt up/down. 0 looks at the horizon, a positive pitch looks up.
    struct FreeFlyCamera
    {
        // Pitch is kept within this range. At exactly +-90 degrees the view direction would be parallel to the up axis,
        // and the camera could no longer tell where its right side is: the picture would suddenly flip.
        static constexpr float MaxPitch = glm::radians(89.0f);

        float yaw = 0.0f;
        float pitch = 0.0f;
    };
}
