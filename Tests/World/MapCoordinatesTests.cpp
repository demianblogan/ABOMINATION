#include "World/MapCoordinates.h"

#include <glm/trigonometric.hpp>
#include <glm/vec3.hpp>
#include <gtest/gtest.h>

#include <optional>

namespace Abomination::World
{
    TEST(MapCoordinates, PositionTurnsAxesAndBecomesMeters)
    {
        // Map (x, y, z) -> game (x, z, -y), then 32 units -> 1 meter: (32, 64, 96) -> (1, 3, -2).
        EXPECT_EQ(ConvertMapPosition({32.0, 64.0, 96.0}), glm::vec3(1.0f, 3.0f, -2.0f));
    }

    TEST(MapCoordinates, MapUpBecomesGameUp)
    {
        EXPECT_EQ(ConvertMapDirection({0.0, 0.0, 1.0}), glm::vec3(0.0f, 1.0f, 0.0f));  // up stays up
        EXPECT_EQ(ConvertMapDirection({0.0, 1.0, 0.0}), glm::vec3(0.0f, 0.0f, -1.0f)); // map +Y is game forward (-Z)
        EXPECT_EQ(ConvertMapDirection({1.0, 0.0, 0.0}), glm::vec3(1.0f, 0.0f, 0.0f));  // X stays X
    }

    TEST(MapCoordinates, AngleBecomesYaw)
    {
        // Angle 90 looks along map +Y, which is game -Z: yaw 0. Angle 180 looks along -X: a quarter turn to the left.
        EXPECT_FLOAT_EQ(ConvertMapAngleToYaw(90.0), 0.0f);
        EXPECT_FLOAT_EQ(ConvertMapAngleToYaw(180.0), glm::radians(90.0f));
    }

    TEST(MapCoordinates, ParsesVectorProperty)
    {
        const std::optional<glm::dvec3> origin = ParseVectorProperty("-48 -176 88");

        ASSERT_TRUE(origin.has_value());
        EXPECT_EQ(*origin, glm::dvec3(-48.0, -176.0, 88.0));
        EXPECT_FALSE(ParseVectorProperty("1 2").has_value());
        EXPECT_FALSE(ParseVectorProperty("one two three").has_value());
    }
}
