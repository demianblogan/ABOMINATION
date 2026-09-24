#include "Application/Application.h"

#include "Core/BuildConfiguration.h"
#include "Core/Log.h"
#include "Renderer/DebugOutput.h"
#include "Renderer/OpenGLLoader.h"

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

        // The window has created the OpenGL context, so the OpenGL functions can be loaded now.
        std::expected<void, std::string> loadingResult = Renderer::LoadOpenGLFunctions();
        if (!loadingResult.has_value())
            return std::unexpected(loadingResult.error());

        if constexpr (Core::IsDebugBuild)
            Renderer::EnableDebugOutput();

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
