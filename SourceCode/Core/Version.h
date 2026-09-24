#pragma once

#include <string_view>

namespace Abomination::Core
{
    struct Version
    {
        int major = 0;
        int minor = 0;
        int patch = 0;

        bool operator==(const Version&) const = default;
    };

    // Version of the game, taken from project(... VERSION ...) in CMakeLists.txt.
    [[nodiscard]] Version GetGameVersion() noexcept;

    // The same version as text: "MAJOR.MINOR.PATCH".
    [[nodiscard]] std::string_view GetGameVersionString() noexcept;
}
