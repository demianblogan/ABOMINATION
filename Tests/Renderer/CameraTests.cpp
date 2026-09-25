#include "Renderer/Camera.h"

#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <gtest/gtest.h>

namespace Abomination::Renderer
{
    namespace
    {
        // Results of sin/cos and matrix products differ from exact values in the last digits.
        constexpr float Tolerance = 1e-5f;

        void ExpectVectorNear(glm::vec3 actual, glm::vec3 expected)
        {
            EXPECT_NEAR(actual.x, expected.x, Tolerance);
            EXPECT_NEAR(actual.y, expected.y, Tolerance);
            EXPECT_NEAR(actual.z, expected.z, Tolerance);
        }
    }

    TEST(Camera, LooksAlongNegativeZByDefault)
    {
        const Camera camera;

        ExpectVectorNear(camera.GetForward(), {0.0f, 0.0f, -1.0f});
        ExpectVectorNear(camera.GetRight(), {1.0f, 0.0f, 0.0f});
        ExpectVectorNear(camera.GetUp(), {0.0f, 1.0f, 0.0f});
    }

    TEST(Camera, PositiveYawTurnsLeft)
    {
        Camera camera;

        camera.SetRotation(glm::radians(90.0f), 0.0f);

        ExpectVectorNear(camera.GetForward(), {-1.0f, 0.0f, 0.0f});
        ExpectVectorNear(camera.GetRight(), {0.0f, 0.0f, -1.0f});
    }

    TEST(Camera, PositivePitchLooksUp)
    {
        Camera camera;

        camera.SetRotation(0.0f, glm::radians(45.0f));

        EXPECT_GT(camera.GetForward().y, 0.0f);
        EXPECT_NEAR(glm::length(camera.GetForward()), 1.0f, Tolerance);
    }

    TEST(Camera, PitchIsClampedBelowStraightUpAndDown)
    {
        Camera camera;

        camera.Rotate(0.0f, glm::radians(120.0f));
        EXPECT_FLOAT_EQ(camera.GetPitch(), Camera::MaxPitch);

        camera.Rotate(0.0f, glm::radians(-500.0f));
        EXPECT_FLOAT_EQ(camera.GetPitch(), -Camera::MaxPitch);
    }

    TEST(Camera, RightStaysHorizontalWhenLookingUp)
    {
        Camera camera;

        camera.SetRotation(glm::radians(30.0f), glm::radians(60.0f));

        EXPECT_NEAR(camera.GetRight().y, 0.0f, Tolerance);
        EXPECT_NEAR(glm::dot(camera.GetRight(), camera.GetForward()), 0.0f, Tolerance);
    }

    TEST(Camera, ViewMatrixMovesCameraToOrigin)
    {
        Camera camera;
        camera.SetPosition({3.0f, 1.0f, 5.0f});
        camera.SetRotation(glm::radians(40.0f), glm::radians(-20.0f));

        const glm::vec4 cameraPositionInView = camera.GetViewMatrix() * glm::vec4(camera.GetPosition(), 1.0f);

        ExpectVectorNear(glm::vec3(cameraPositionInView), {0.0f, 0.0f, 0.0f});
    }

    TEST(Camera, PointInFrontEndsUpOnNegativeZInView)
    {
        Camera camera;
        camera.SetPosition({3.0f, 1.0f, 5.0f});
        camera.SetRotation(glm::radians(40.0f), glm::radians(-20.0f));

        const glm::vec3 pointInFront = camera.GetPosition() + camera.GetForward() * 10.0f;
        const glm::vec4 pointInView = camera.GetViewMatrix() * glm::vec4(pointInFront, 1.0f);

        ExpectVectorNear(glm::vec3(pointInView), {0.0f, 0.0f, -10.0f});
    }
}
