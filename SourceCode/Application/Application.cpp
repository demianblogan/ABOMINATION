#include "Application/Application.h"

#include "Core/Log.h"

#include <utility>

namespace Abomination
{
    using Core::LogCategory;
    using Core::LogLevel;

    std::expected<Application, std::string> Application::Create()
    {
        std::expected<Platform::SDLLibrary, std::string> SDLLibrary = Platform::SDLLibrary::Initialize();
        if (!SDLLibrary.has_value())
            return std::unexpected(SDLLibrary.error());

        std::expected<Platform::Window, std::string> window = Platform::Window::Create(Platform::WindowSettings{});
        if (!window.has_value())
            return std::unexpected(window.error());

        return Application(std::move(*SDLLibrary), std::move(*window));
    }

    Application::Application(Platform::SDLLibrary SDLLibrary, Platform::Window window) noexcept
        : m_SDLLibrary(std::move(SDLLibrary))
        , m_window(std::move(window))
    {}

    int Application::Run()
    {
        Core::Log::Write(LogCategory::Core, LogLevel::Info, "Main loop started");

        // One iteration is one frame.
        while (!m_window.IsCloseRequested())
        {
            m_window.ProcessEvents();
            m_window.SwapBuffers();
        }

        Core::Log::Write(LogCategory::Core, LogLevel::Info, "Main loop finished");

        return 0;
    }
}
