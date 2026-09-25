#include "Platform/Window.h"

#include "Core/BuildConfiguration.h"
#include "Core/Log.h"
#include "Input/InputDevices.h"

#include <SDL3/SDL.h>
#include <imgui.h>
#include <imgui_impl_sdl3.h>

#include <format>
#include <utility>

namespace Abomination::Platform
{
    using Core::LogCategory;
    using Core::LogLevel;

    // Key values are SDL scancodes, so the key state array must have exactly as many entries as SDL has scancodes.
    static_assert(Input::KeyCount == SDL_SCANCODE_COUNT, "Input::KeyCount must match SDL_SCANCODE_COUNT");

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

            // A debug context makes the driver check every call and report problems (see Renderer::EnableDebugOutput).
            // It is slower, so it is requested only in Debug builds.
            if constexpr (Core::IsDebugBuild)
                SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);

            // Double buffering: the frame is drawn into a hidden back buffer and shown all at once by SwapBuffers().
            SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

            // Bits per pixel of the depth buffer (which surface is closer) and the stencil buffer (masking).
            SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
            SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
        }

        // True while ImGui wants the keyboard for itself, for example while the user types into an ImGui text field.
        // The game must not react to those keys then.
        bool IsKeyboardCapturedByImGui()
        {
            return ImGui::GetCurrentContext() != nullptr && ImGui::GetIO().WantCaptureKeyboard;
        }

        // True while the cursor is over an ImGui window (or ImGui is being dragged): mouse clicks and the wheel belong
        // to ImGui then, not to the game.
        bool IsMouseCapturedByImGui()
        {
            return ImGui::GetCurrentContext() != nullptr && ImGui::GetIO().WantCaptureMouse;
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

    void Window::ProcessEvents(Input::InputDevices& input)
    {
        // What was pressed, released or moved during the previous frame is forgotten; this frame's events fill it again.
        input.keyboard.StartFrame();
        input.mouse.StartFrame();

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            // ImGui sees every event first: it needs the mouse and keyboard for its windows.
            // Without an ImGui context (the debug overlay is not created) there is nobody to pass them to.
            if (ImGui::GetCurrentContext() != nullptr)
                ImGui_ImplSDL3_ProcessEvent(&event);

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

                // A key went down. "repeat" marks the copies the operating system sends while the key is held.
                // Presses are ignored while ImGui uses the keyboard, so typing in ImGui does not control the game.
                // event.key.scancode is the physical position of the key; Input::Key uses the same values.
                case SDL_EVENT_KEY_DOWN:
                    if (!event.key.repeat && !IsKeyboardCapturedByImGui())
                        input.keyboard.PressKey(static_cast<Input::Key>(event.key.scancode));
                    break;

                // A key went up. Always passed on, even while ImGui uses the keyboard: otherwise a key pressed
                // before ImGui took the keyboard would never be released.
                case SDL_EVENT_KEY_UP:
                    input.keyboard.ReleaseKey(static_cast<Input::Key>(event.key.scancode));
                    break;

                // The mouse moved. xrel/yrel is the movement since the previous event (+Y is down), which keeps
                // working in relative mode, where the cursor position no longer changes.
                case SDL_EVENT_MOUSE_MOTION:
                    input.mouse.Move(glm::vec2(event.motion.xrel, event.motion.yrel));
                    break;

                // A mouse button went down. Ignored while the cursor is over an ImGui window, so clicking a debug window
                // does not also act in the game. Input::MouseButton uses the same numbers as SDL.
                case SDL_EVENT_MOUSE_BUTTON_DOWN:
                    if (!IsMouseCapturedByImGui())
                        input.mouse.PressButton(static_cast<Input::MouseButton>(event.button.button));
                    break;

                // A mouse button went up. Always passed on, for the same reason as key releases.
                case SDL_EVENT_MOUSE_BUTTON_UP:
                    input.mouse.ReleaseButton(static_cast<Input::MouseButton>(event.button.button));
                    break;

                // The wheel turned. With "natural scrolling" enabled in the system settings SDL reports the values
                // inverted and marks it with SDL_MOUSEWHEEL_FLIPPED; turning them back gives the physical direction.
                case SDL_EVENT_MOUSE_WHEEL:
                    if (!IsMouseCapturedByImGui())
                    {
                        const float direction = event.wheel.direction == SDL_MOUSEWHEEL_FLIPPED ? -1.0f : 1.0f;
                        input.mouse.Scroll(event.wheel.y * direction);
                    }
                    break;

                // The window stopped receiving input (Alt+Tab, a click on another window).
                case SDL_EVENT_WINDOW_FOCUS_LOST:
                    input.keyboard.ReleaseAllKeys();
                    input.mouse.ReleaseAllButtons();
                    break;
            }
        }
    }

    void Window::SwapBuffers()
    {
        SDL_GL_SwapWindow(m_window);
    }

    void Window::SetRelativeMouseMode(bool isEnabled)
    {
        SDL_SetWindowRelativeMouseMode(m_window, isEnabled);
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

    SDL_Window* Window::GetSDLWindow() const noexcept
    {
        return m_window;
    }

    SDL_GLContextState* Window::GetSDLContext() const noexcept
    {
        return m_context;
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
