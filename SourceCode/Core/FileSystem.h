#pragma once

#include <cstddef>
#include <expected>
#include <filesystem>
#include <string>
#include <vector>

namespace Abomination::Core
{
    // Reads a whole text file (a shader, a JSON configuration) into a string.
    // Returns an error message if the file does not exist or cannot be read.
    [[nodiscard]] std::expected<std::string, std::string> ReadTextFile(const std::filesystem::path& path);

    // Reads a whole binary file (an image, a font, a sound) into an array of bytes.
    // Returns an error message if the file does not exist or cannot be read.
    [[nodiscard]] std::expected<std::vector<std::byte>, std::string> ReadBinaryFile(const std::filesystem::path& path);
}
