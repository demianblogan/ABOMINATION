#include "Gameplay/FreeFlyCameraController.h"
#include "Input/ActionStates.h"
#include "Input/InputBindings.h"
#include "Input/InputDevices.h"
#include "Renderer/Camera.h"

#include <glm/exponential.hpp>
#include <glm/trigonometric.hpp>
#include <glm/vec3.hpp>
#include <gtest/gtest.h>

namespace Abomination::Gameplay
{
    // Simulates frames of input with the default bindings and runs the controller on a camera at the origin.
    class FreeFlyCameraControllerTest : public ::testing::Test
    {
    protected:
        static constexpr float Tolerance = 1e-5f;

        // Speed 2 m/s and a fast multiplier of 3 make the expected distances easy to see.
        static constexpr Gameplay::FreeFlyCameraSettings Settings{
            .moveSpeed = 2.0f,
            .fastMoveMultiplier = 3.0f,
            .mouseSensitivity = 0.01f,
        };

        void StartFrame()
        {
            m_devices.keyboard.StartFrame();
            m_devices.mouse.StartFrame();
        }

        // Updates the actions from the devices and runs the controller for one frame.
        void RunFrame(float deltaTime)
        {
            m_actions.Update(m_devices, m_bindings);
            m_controller.Update(m_camera, m_actions, m_devices.mouse, deltaTime);
        }

        void ExpectPositionNear(glm::vec3 expected) const
        {
            EXPECT_NEAR(m_camera.GetPosition().x, expected.x, Tolerance);
            EXPECT_NEAR(m_camera.GetPosition().y, expected.y, Tolerance);
            EXPECT_NEAR(m_camera.GetPosition().z, expected.z, Tolerance);
        }

        Input::InputDevices m_devices;
        Input::InputBindings m_bindings = Input::InputBindings::CreateDefault();
        Input::ActionStates m_actions;
        Renderer::Camera m_camera;
        FreeFlyCameraController m_controller{Settings};
    };

    TEST_F(FreeFlyCameraControllerTest, MovesForwardWhereCameraLooks)
    {
        StartFrame();
        m_devices.keyboard.PressKey(Input::Key::W);

        RunFrame(0.5f);

        // 2 m/s for 0.5 s along -Z.
        ExpectPositionNear({0.0f, 0.0f, -1.0f});
    }

    TEST_F(FreeFlyCameraControllerTest, OppositeActionsCancelOut)
    {
        StartFrame();
        m_devices.keyboard.PressKey(Input::Key::W);
        m_devices.keyboard.PressKey(Input::Key::S);

        RunFrame(1.0f);

        ExpectPositionNear({0.0f, 0.0f, 0.0f});
    }

    TEST_F(FreeFlyCameraControllerTest, DiagonalMovementIsNotFaster)
    {
        StartFrame();
        m_devices.keyboard.PressKey(Input::Key::W);
        m_devices.keyboard.PressKey(Input::Key::D);

        RunFrame(1.0f);

        // 2 meters in total, split equally between forward (-Z) and right (+X): 2 / sqrt(2) each.
        const float sideDistance = 2.0f / glm::sqrt(2.0f);
        ExpectPositionNear({sideDistance, 0.0f, -sideDistance});
    }

    TEST_F(FreeFlyCameraControllerTest, UpAndDownFollowWorldAxisEvenWhenLookingUp)
    {
        m_camera.SetRotation(0.0f, glm::radians(60.0f));
        StartFrame();
        m_devices.keyboard.PressKey(Input::Key::E);

        RunFrame(1.0f);

        ExpectPositionNear({0.0f, 2.0f, 0.0f});
    }

    TEST_F(FreeFlyCameraControllerTest, MoveFasterMultipliesSpeed)
    {
        StartFrame();
        m_devices.keyboard.PressKey(Input::Key::Q);
        m_devices.keyboard.PressKey(Input::Key::LeftShift);

        RunFrame(1.0f);

        // Down at 2 m/s x 3.
        ExpectPositionNear({0.0f, -6.0f, 0.0f});
    }

    TEST_F(FreeFlyCameraControllerTest, MouseTurnsCameraOnlyInLookAroundMode)
    {
        StartFrame();
        m_devices.mouse.Move({100.0f, 0.0f});
        RunFrame(0.016f);

        EXPECT_FLOAT_EQ(m_camera.GetYaw(), 0.0f);
    }

    TEST_F(FreeFlyCameraControllerTest, MouseMovementIsIgnoredInFrameLookAroundModeStarts)
    {
        StartFrame();
        m_devices.mouse.PressButton(Input::MouseButton::Right);
        m_devices.mouse.Move({500.0f, 0.0f});
        RunFrame(0.016f);

        EXPECT_FLOAT_EQ(m_camera.GetYaw(), 0.0f);
    }

    TEST_F(FreeFlyCameraControllerTest, MouseRightTurnsRightAndMouseUpLooksUp)
    {
        StartFrame();
        m_devices.mouse.PressButton(Input::MouseButton::Right);
        RunFrame(0.016f);

        StartFrame();
        m_devices.mouse.Move({10.0f, -20.0f});
        RunFrame(0.016f);

        // 10 pixels right x 0.01 = 0.1 rad to the right (negative yaw); 20 pixels up = 0.2 rad up (positive pitch).
        EXPECT_NEAR(m_camera.GetYaw(), -0.1f, Tolerance);
        EXPECT_NEAR(m_camera.GetPitch(), 0.2f, Tolerance);
    }
}
