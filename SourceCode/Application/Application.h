#pragma once

#include "Input/Keyboard.h"
#include "Platform/SDLLibrary.h"
#include "Platform/Window.h"
#include "UI/DebugOverlay.h"

#include <expected>
#include <filesystem>
#include <string>

namespace Abomination
{
    // The top-level object of the game: creates the platform objects and runs the main loop.
    // Lives in the root namespace as the only exception to the "namespace per module" rule,
    // because Abomination::Application::Application would repeat the same word twice.
    class Application
    {
    public:
        // assetsDirectory: the folder with the game files (fonts, shaders, textures), normally next to the executable.
        [[nodiscard]] static std::expected<Application, std::string> Create(const std::filesystem::path& assetsDirectory);

        // Runs the main loop until the window is closed. Returns the exit code of the process.
        [[nodiscard]] int Run();

    private:
        Application(Platform::SDLLibrary SDLLibrary, Platform::Window window, UI::DebugOverlay debugOverlay) noexcept;

        // Members are destroyed in reverse order of declaration: the overlay first (it uses the window and OpenGL),
        // then the window, then SDL, which the window needs.
        Platform::SDLLibrary m_SDLLibrary;
        Platform::Window m_window;
        UI::DebugOverlay m_debugOverlay;

        // State of the keyboard for the current frame: the window fills it, the game reads it.
        Input::Keyboard m_keyboard;
    };
}
