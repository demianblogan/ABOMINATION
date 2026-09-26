#pragma once

#include <array>
#include <cstdint>
#include <string_view>
#include <utility>

namespace Abomination::Platform
{
    // How the game window takes the screen.
    enum class ScreenMode : std::uint8_t
    {
        // An ordinary window with a frame and a title bar, sized from the monitor (see WindowSizing.h).
        Windowed,

        // A window without a frame that covers the whole monitor, at the resolution of the desktop. Looks like
        // fullscreen, but the operating system still composes the screen, so Alt+Tab and other windows on top of the
        // game (notifications, a second program) work instantly. The default mode of most games today.
        Borderless,

        // Exclusive fullscreen: the game gets the monitor for itself, at the resolution of the desktop for now (choosing
        // another resolution comes with the options menu, 0.8). Alt+Tab is slower: the monitor switches back to the
        // desktop, and SDL minimizes the game while it is not in focus.
        Fullscreen,
    };

    // Names for the debug overlay and the log, in the order of the enum values.
    inline constexpr std::array<std::string_view, 3> ScreenModeNames = {"Windowed", "Borderless", "Fullscreen"};

    [[nodiscard]] constexpr std::string_view GetScreenModeName(ScreenMode mode) noexcept
    {
        return ScreenModeNames[std::to_underlying(mode)];
    }
}
