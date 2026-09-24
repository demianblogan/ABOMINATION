#include "Platform/Window.h"

#include "Core/BuildConfiguration.h"
#include "Core/Log.h"

#include <SDL3/SDL.h>

#include <format>
#include <utility>

namespace Abomination::Platform
{
    using Core::LogCategory;
    using Core::LogLevel;

    namespace
    {
        // Describes the OpenGL context SDL has to create. Must be called before the window is created,
        // because the pixel format of the window depends on these attributes.
        void SetOpenGLAttributes()
        {
            // OpenGL 4.6, Core profile: only the modern API, all functions deprecated since OpenGL 3.0 are removed.
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

            // A debug context makes the driver check every call and report problems (used in the next step).
            // It is slower, so it is requested only in Debug builds.
            if constexpr (Core::IsDebugBuild)
                SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);

            // Double buffering: the frame is drawn into a hidden back buffer and shown all at once by SwapBuffers().
            SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

            // Bits per pixel of the depth buffer (which surface is closer) and the stencil buffer (masking).
            SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
            SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
        }
    }

    std::expected<Window, std::string> Window::Create(const WindowSettings& settings)
    {
        SetOpenGLAttributes();

        SDL_WindowFlags flags = SDL_WINDOW_OPENGL;
        if (settings.isResizable)
            flags |= SDL_WINDOW_RESIZABLE;

        SDL_Window* window = SDL_CreateWindow(settings.title.c_str(), settings.width, settings.height, flags);
        if (window == nullptr)
            return std::unexpected(std::format("Failed to create the window: {}", SDL_GetError()));

        // Creates the OpenGL context and makes it current for this thread: from now on OpenGL calls go to this window.
        SDL_GLContext context = SDL_GL_CreateContext(window);
        if (context == nullptr)
        {
            SDL_DestroyWindow(window);

            return std::unexpected(std::format("Failed to create an OpenGL 4.6 Core context: {}", SDL_GetError()));
        }

        // Swap interval 1: SwapBuffers() waits for one monitor refresh. 0: it returns immediately.
        SDL_GL_SetSwapInterval(settings.isVSyncEnabled ? 1 : 0);

        // The size in pixels can differ from the requested size, for example when Windows scales the desktop.
        int widthInPixels = 0;
        int heightInPixels = 0;
        SDL_GetWindowSizeInPixels(window, &widthInPixels, &heightInPixels);

        Core::Log::Write(LogCategory::Platform, LogLevel::Info, "Window created: {}x{} pixels, V-Sync {}", widthInPixels,
                         heightInPixels, settings.isVSyncEnabled ? "on" : "off");

        return Window(window, context, widthInPixels, heightInPixels);
    }

    Window::Window(SDL_Window* window, SDL_GLContextState* context, int widthInPixels, int heightInPixels) noexcept
        : m_window(window)
        , m_context(context)
        , m_widthInPixels(widthInPixels)
        , m_heightInPixels(heightInPixels)
    {}

    Window::Window(Window&& other) noexcept
        : m_window(std::exchange(other.m_window, nullptr))
        , m_context(std::exchange(other.m_context, nullptr))
        , m_isCloseRequested(other.m_isCloseRequested)
        , m_widthInPixels(other.m_widthInPixels)
        , m_heightInPixels(other.m_heightInPixels)
    {}

    Window& Window::operator=(Window&& other) noexcept
    {
        if (this != &other)
        {
            Destroy();
            m_window = std::exchange(other.m_window, nullptr);
            m_context = std::exchange(other.m_context, nullptr);
            m_isCloseRequested = other.m_isCloseRequested;
            m_widthInPixels = other.m_widthInPixels;
            m_heightInPixels = other.m_heightInPixels;
        }

        return *this;
    }

    Window::~Window()
    {
        Destroy();
    }

    void Window::ProcessEvents()
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                // The user closes the last window (the close button, Alt+F4).
                case SDL_EVENT_QUIT:
                    m_isCloseRequested = true;
                    break;

                // The drawable area got a new size in pixels (the window was resized, maximized, moved to another monitor).
                case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
                    m_widthInPixels = event.window.data1;
                    m_heightInPixels = event.window.data2;
                    Core::Log::Write(LogCategory::Platform, LogLevel::Debug, "Window resized: {}x{} pixels", m_widthInPixels,
                                     m_heightInPixels);
                    break;

                default:
                    break;
            }
        }
    }

    void Window::SwapBuffers()
    {
        SDL_GL_SwapWindow(m_window);
    }

    bool Window::IsCloseRequested() const noexcept
    {
        return m_isCloseRequested;
    }

    int Window::GetWidthInPixels() const noexcept
    {
        return m_widthInPixels;
    }

    int Window::GetHeightInPixels() const noexcept
    {
        return m_heightInPixels;
    }

    void Window::Destroy() noexcept
    {
        // The context is destroyed before the window it belongs to.
        if (m_context != nullptr)
            SDL_GL_DestroyContext(m_context);

        if (m_window != nullptr)
        {
            SDL_DestroyWindow(m_window);
            Core::Log::Write(LogCategory::Platform, LogLevel::Info, "Window destroyed");
        }

        m_context = nullptr;
        m_window = nullptr;
    }

    OpenGLFunction GetOpenGLFunctionAddress(const char* name)
    {
        // SDL_FunctionPointer is the same type as OpenGLFunction, so no conversion is needed.
        return SDL_GL_GetProcAddress(name);
    }
}
