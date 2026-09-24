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

        // V-Sync: SwapBuffers() waits for the monitor refresh. No tearing, and the frame rate never exceeds the refresh rate.
        bool isVSyncEnabled = true;
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

        // Size of the drawable area in pixels. OpenGL works in pixels, so these are the values for glViewport.
        [[nodiscard]] int GetWidthInPixels() const noexcept;
        [[nodiscard]] int GetHeightInPixels() const noexcept;

    private:
        Window(SDL_Window* window, SDL_GLContextState* context, int widthInPixels, int heightInPixels) noexcept;

        void Destroy() noexcept;

        SDL_Window* m_window = nullptr;
        SDL_GLContextState* m_context = nullptr;
        bool m_isCloseRequested = false;
        int m_widthInPixels = 0;
        int m_heightInPixels = 0;
    };

    // A pointer to a function without parameters and return value. OpenGL functions have different signatures,
    // so the address is returned in this general form and GLAD casts it to the real type of each function.
    using OpenGLFunction = void (*)();

    // Asks the graphics driver for the address of an OpenGL function by its name ("glClear", ...).
    // Requires a current OpenGL context. Returns nullptr if the driver does not provide the function.
    [[nodiscard]] OpenGLFunction GetOpenGLFunctionAddress(const char* name);
}
