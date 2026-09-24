#pragma once

#include "Platform/SDLLibrary.h"
#include "Platform/Window.h"

#include <expected>
#include <string>

namespace Abomination
{
    // The top-level object of the game: creates the platform objects and runs the main loop.
    // Lives in the root namespace as the only exception to the "namespace per module" rule,
    // because Abomination::Application::Application would repeat the same word twice.
    class Application
    {
    public:
        [[nodiscard]] static std::expected<Application, std::string> Create();

        // Runs the main loop until the window is closed. Returns the exit code of the process.
        [[nodiscard]] int Run();

    private:
        Application(Platform::SDLLibrary SDLLibrary, Platform::Window window) noexcept;

        // Members are destroyed in reverse order of declaration: the window first, then SDL, which the window needs.
        Platform::SDLLibrary m_SDLLibrary;
        Platform::Window m_window;
    };
}
