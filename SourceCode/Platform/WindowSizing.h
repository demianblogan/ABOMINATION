#pragma once

namespace Abomination::Platform
{
    struct WindowSize
    {
        int width = 0;
        int height = 0;
    };

    // Part of the usable area of the monitor a window in windowed mode takes: big enough to play in, small enough to
    // leave room around it (the title bar, other programs next to the game).
    inline constexpr float WindowedScreenFraction = 0.75f;

    // The shape of the window in windowed mode: 16:9, the shape of almost every monitor today.
    inline constexpr float WindowedAspectRatio = 16.0f / 9.0f;

    // The size of the window in windowed mode on a monitor whose usable area (the screen without the taskbar) is
    // usableArea: the largest 16:9 rectangle that fits into WindowedScreenFraction of it in both directions.
    // Works in any units (pixels or screen coordinates), the result is in the units of usableArea.
    // Returns {0, 0} if the usable area is empty.
    //
    // No SDL here: the calculation is separate from Window so it can be tested without a monitor.
    [[nodiscard]] WindowSize CalculateWindowedSize(WindowSize usableArea);
}
