#pragma once

#include <expected>
#include <string>

// SDL types are only declared here, so code that includes Window.h does not see SDL headers.
struct SDL_Window;
struct SDL_GLContextState;

namespace Abomination::Platform
{
    struct WindowSettings
    {
        std::string title = "Abomination";
        int width = 1280;
        int height = 720;
        bool isResizable = true;
    };

    // An operating system window with an OpenGL 4.6 Core context attached to it.
    // Requires an initialized SDLLibrary. Move-only.
    class Window
    {
    public:
        [[nodiscard]] static std::expected<Window, std::string> Create(const WindowSettings& settings);

        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;

        Window(Window&& other) noexcept;
        Window& operator=(Window&& other) noexcept;

        ~Window();

        // Handles all events the operating system has sent since the last call (closing the window, ...).
        void ProcessEvents();

        // Shows the frame that has just been drawn: swaps the back buffer and the front buffer.
        void SwapBuffers();

        [[nodiscard]] bool IsCloseRequested() const noexcept;

    private:
        Window(SDL_Window* window, SDL_GLContextState* context) noexcept;

        void Destroy() noexcept;

        SDL_Window* m_window = nullptr;
        SDL_GLContextState* m_context = nullptr;
        bool m_isCloseRequested = false;
    };
}
