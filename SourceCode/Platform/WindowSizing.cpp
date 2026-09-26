#include "Platform/WindowSizing.h"

#include <cmath>

namespace Abomination::Platform
{
    WindowSize CalculateWindowedSize(WindowSize usableArea)
    {
        if (usableArea.width <= 0 || usableArea.height <= 0)
            return {};

        // The largest rectangle allowed in each direction.
        const float maximumWidth = static_cast<float>(usableArea.width) * WindowedScreenFraction;
        const float maximumHeight = static_cast<float>(usableArea.height) * WindowedScreenFraction;

        // The rectangle is limited by whichever side runs out first. An area wider than 16:9 (a usual monitor minus the
        // taskbar, an ultra-wide monitor) limits the height: the window takes the full allowed height and its width
        // follows from the aspect ratio. A narrower area (a monitor turned to portrait) limits the width.
        float width = maximumWidth;
        float height = maximumHeight;
        if (maximumWidth / maximumHeight > WindowedAspectRatio)
            width = maximumHeight * WindowedAspectRatio;
        else
            height = maximumWidth / WindowedAspectRatio;

        // Cut off the fraction instead of rounding: rounding up could make the window a pixel larger than allowed. A tiny
        // tolerance is added first, because a value that should be whole can come out as 1079.9999 in float arithmetic
        // and would lose a whole pixel.
        constexpr float WholeNumberTolerance = 0.01f;

        return {
            .width = static_cast<int>(std::floor(width + WholeNumberTolerance)),
            .height = static_cast<int>(std::floor(height + WholeNumberTolerance)),
        };
    }
}
