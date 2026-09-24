#include "Input/Keyboard.h"

#include <gtest/gtest.h>

namespace Abomination::Input
{
    TEST(Keyboard, NoKeyIsHeldInitially)
    {
        const Keyboard keyboard;

        EXPECT_FALSE(keyboard.IsKeyHeld(Key::F1));
        EXPECT_FALSE(keyboard.WasKeyPressed(Key::F1));
        EXPECT_FALSE(keyboard.WasKeyReleased(Key::F1));
    }

    TEST(Keyboard, PressedKeyIsHeldAndPressedInSameFrame)
    {
        Keyboard keyboard;

        keyboard.StartFrame();
        keyboard.PressKey(Key::F1);

        EXPECT_TRUE(keyboard.IsKeyHeld(Key::F1));
        EXPECT_TRUE(keyboard.WasKeyPressed(Key::F1));
    }

    TEST(Keyboard, PressLastsOneFrameButKeyStaysHeld)
    {
        Keyboard keyboard;
        keyboard.StartFrame();
        keyboard.PressKey(Key::F1);

        keyboard.StartFrame();

        EXPECT_TRUE(keyboard.IsKeyHeld(Key::F1));
        EXPECT_FALSE(keyboard.WasKeyPressed(Key::F1));
    }

    TEST(Keyboard, RepeatedPressOfHeldKeyIsIgnored)
    {
        Keyboard keyboard;
        keyboard.StartFrame();
        keyboard.PressKey(Key::F1);
        keyboard.StartFrame();

        // The operating system repeats the "down" event while the key is held.
        keyboard.PressKey(Key::F1);

        EXPECT_FALSE(keyboard.WasKeyPressed(Key::F1));
    }

    TEST(Keyboard, ReleasedKeyIsReleasedForOneFrame)
    {
        Keyboard keyboard;
        keyboard.StartFrame();
        keyboard.PressKey(Key::F1);
        keyboard.StartFrame();

        keyboard.ReleaseKey(Key::F1);

        EXPECT_FALSE(keyboard.IsKeyHeld(Key::F1));
        EXPECT_TRUE(keyboard.WasKeyReleased(Key::F1));

        keyboard.StartFrame();

        EXPECT_FALSE(keyboard.WasKeyReleased(Key::F1));
    }

    TEST(Keyboard, ReleaseAllKeysReleasesHeldKeys)
    {
        Keyboard keyboard;
        keyboard.StartFrame();
        keyboard.PressKey(Key::F1);
        keyboard.StartFrame();

        keyboard.ReleaseAllKeys();

        EXPECT_FALSE(keyboard.IsKeyHeld(Key::F1));
        EXPECT_TRUE(keyboard.WasKeyReleased(Key::F1));
    }
}
