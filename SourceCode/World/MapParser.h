#pragma once

#include "World/MapData.h"

#include <expected>
#include <filesystem>
#include <string>
#include <string_view>

namespace Abomination::World
{
    // Reads the text of a .map file in the Valve 220 format (the one TrenchBroom writes for Abomination).
    // On a mistake in the text returns an error message with the line number, for example
    // "Line 12: expected ')' but found 'Crate'".
    [[nodiscard]] std::expected<MapData, std::string> ParseMap(std::string_view text);

    // Reads a .map file from disk and parses it.
    [[nodiscard]] std::expected<MapData, std::string> LoadMapFile(const std::filesystem::path& path);
}
