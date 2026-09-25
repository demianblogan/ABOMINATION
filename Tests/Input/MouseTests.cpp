#include "Input/Mouse.h"

#include <gtest/gtest.h>

namespace Abomination::Input
{
    TEST(Mouse, MovementEventsOfOneFrameAddUp)
    {
        Mouse mouse;
        mouse.StartFrame();

        mouse.Move({3.0f, -1.0f});
        mouse.Move({2.0f, 4.0f});

        EXPECT_FLOAT_EQ(mouse.GetMovement().x, 5.0f);
        EXPECT_FLOAT_EQ(mouse.GetMovement().y, 3.0f);
    }

    TEST(Mouse, MovementAndWheelResetEveryFrame)
    {
        Mouse mouse;
        mouse.StartFrame();
        mouse.Move({10.0f, 10.0f});
        mouse.Scroll(1.0f);

        mouse.StartFrame();

        EXPECT_FLOAT_EQ(mouse.GetMovement().x, 0.0f);
        EXPECT_FLOAT_EQ(mouse.GetMovement().y, 0.0f);
        EXPECT_FLOAT_EQ(mouse.GetWheelMovement(), 0.0f);
    }

    TEST(Mouse, ButtonIsPressedForOneFrameAndHeldUntilReleased)
    {
        Mouse mouse;
        mouse.StartFrame();
        mouse.PressButton(MouseButton::Right);

        EXPECT_TRUE(mouse.WasButtonPressed(MouseButton::Right));
        EXPECT_TRUE(mouse.IsButtonHeld(MouseButton::Right));

        mouse.StartFrame();

        EXPECT_FALSE(mouse.WasButtonPressed(MouseButton::Right));
        EXPECT_TRUE(mouse.IsButtonHeld(MouseButton::Right));

        mouse.ReleaseButton(MouseButton::Right);

        EXPECT_FALSE(mouse.IsButtonHeld(MouseButton::Right));
        EXPECT_TRUE(mouse.WasButtonReleased(MouseButton::Right));
    }

    TEST(Mouse, ReleaseAllButtonsReleasesHeldButtons)
    {
        Mouse mouse;
        mouse.StartFrame();
        mouse.PressButton(MouseButton::Left);
        mouse.StartFrame();

        mouse.ReleaseAllButtons();

        EXPECT_FALSE(mouse.IsButtonHeld(MouseButton::Left));
        EXPECT_TRUE(mouse.WasButtonReleased(MouseButton::Left));
        EXPECT_FALSE(mouse.WasButtonReleased(MouseButton::Right));
    }
}
