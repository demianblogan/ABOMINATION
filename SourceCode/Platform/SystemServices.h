#pragma once

#include <filesystem>
#include <string>

// Small operating system services that do not need an initialized SDL library.
namespace Abomination::Platform
{
    // The folder that contains the executable. Files next to the game (logs for now) are placed relative to it,
    // so they end up in the same place no matter from which folder the game was started.
    [[nodiscard]] std::filesystem::path GetExecutableDirectory();

    // Shows a modal error dialog and waits until the user closes it. Used for fatal errors at startup.
    void ShowErrorDialog(const std::string& title, const std::string& message);
}
