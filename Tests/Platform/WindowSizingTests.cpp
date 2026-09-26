#include "Platform/WindowSizing.h"

#include <gtest/gtest.h>

namespace Abomination::Platform
{
    namespace
    {
        // The window shape is 16:9 up to the pixel lost by cutting off the fraction.
        void ExpectSixteenByNine(WindowSize size)
        {
            EXPECT_NEAR(static_cast<float>(size.width) / static_cast<float>(size.height), WindowedAspectRatio, 0.01f);
        }

        void ExpectFitsIntoFraction(WindowSize size, WindowSize usableArea)
        {
            EXPECT_LE(size.width, static_cast<int>(static_cast<float>(usableArea.width) * WindowedScreenFraction));
            EXPECT_LE(size.height, static_cast<int>(static_cast<float>(usableArea.height) * WindowedScreenFraction));
        }
    }

    TEST(WindowSizing, FullHDMonitorWithTaskbarLimitsHeight)
    {
        // 1920 x 1080 minus a taskbar of 40 pixels: wider than 16:9, so the height is the limit (75% of 1040 = 780).
        const WindowSize usableArea{1920, 1040};

        const WindowSize size = CalculateWindowedSize(usableArea);

        EXPECT_EQ(size.height, 780);
        EXPECT_EQ(size.width, 1386); // 780 * 16 / 9 = 1386.67
        ExpectFitsIntoFraction(size, usableArea);
    }

    TEST(WindowSizing, UltraWideMonitorKeepsSixteenByNine)
    {
        // 3440 x 1440 (21:9): the window does not stretch to the width of the monitor.
        const WindowSize usableArea{3440, 1400};

        const WindowSize size = CalculateWindowedSize(usableArea);

        EXPECT_EQ(size.height, 1050);
        ExpectSixteenByNine(size);
        ExpectFitsIntoFraction(size, usableArea);
    }

    TEST(WindowSizing, PortraitMonitorLimitsWidth)
    {
        // A monitor turned on its side: narrower than 16:9, so the width is the limit (75% of 1080 = 810).
        const WindowSize usableArea{1080, 1880};

        const WindowSize size = CalculateWindowedSize(usableArea);

        EXPECT_EQ(size.width, 810);
        ExpectSixteenByNine(size);
        ExpectFitsIntoFraction(size, usableArea);
    }

    TEST(WindowSizing, ExactSixteenByNineAreaGivesFractionOfIt)
    {
        const WindowSize size = CalculateWindowedSize({2560, 1440});

        EXPECT_EQ(size.width, 1920);
        EXPECT_EQ(size.height, 1080);
    }

    TEST(WindowSizing, EmptyAreaGivesZeroSize)
    {
        EXPECT_EQ(CalculateWindowedSize({0, 1080}).width, 0);
        EXPECT_EQ(CalculateWindowedSize({1920, -1}).height, 0);
    }
}
