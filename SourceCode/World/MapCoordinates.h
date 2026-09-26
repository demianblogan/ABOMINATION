#pragma once

#include <glm/vec3.hpp>

#include <optional>
#include <string_view>

// Converting from the coordinates of .map files to the coordinates of the game. It happens once, when a map is loaded,
// so everything after loading works in the game's own units and axes.
//
//   map (Quake, TrenchBroom):  units, Z is up, X and Y are horizontal
//   game (OpenGL, glm):        meters, Y is up, -Z is forward
namespace Abomination::World
{
    // 32 units are about 1 meter: the Quake player is 56 units tall (about 1.75 m), and the grid sizes of TrenchBroom
    // (8, 16, 32, 64) are based on this scale.
    inline constexpr double UnitsPerMeter = 32.0;

    // A position: axes turned (x, y, z) -> (x, z, -y), then units -> meters.
    // The turn is a rotation by 90 degrees around X: map +Z (up) becomes game +Y (up), map +Y becomes game -Z.
    // A rotation keeps the handedness of the coordinate system, so the counter-clockwise order of face corners, and
    // with it the front side of faces, stays the same.
    [[nodiscard]] glm::vec3 ConvertMapPosition(const glm::dvec3& mapPosition);

    // A direction (a normal, an axis): the same turn, without the change of units, because a direction has no length.
    [[nodiscard]] glm::vec3 ConvertMapDirection(const glm::dvec3& mapDirection);

    // The "angle" of a map entity (degrees counter-clockwise from map +X, seen from above) as the yaw of the game
    // (radians counter-clockwise from game -Z, see Gameplay::FreeFlyCamera): an angle of 90 (map +Y) is yaw 0.
    [[nodiscard]] float ConvertMapAngleToYaw(double mapAngleDegrees);

    // Reads a vector property like "origin" "-48 -176 88". Returns nothing if the text is not three numbers.
    [[nodiscard]] std::optional<glm::dvec3> ParseVectorProperty(std::string_view text);
}
