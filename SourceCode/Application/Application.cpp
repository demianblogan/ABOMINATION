#include "Application/Application.h"

#include "Core/BuildConfiguration.h"
#include "Core/FrameTimer.h"
#include "Core/Log.h"
#include "Renderer/DebugOutput.h"
#include "Renderer/OpenGLLoader.h"
#include "Renderer/RenderCommands.h"

#include <glm/vec4.hpp>

#include <cmath>
#include <numbers>
#include <utility>

namespace Abomination
{
    using Core::LogCategory;
    using Core::LogLevel;

    namespace
    {
        // A dark color that slowly goes around the color wheel: red, green and blue follow the same sine wave,
        // shifted by a third of a period from each other. One full cycle takes about 12.5 seconds.
        glm::vec4 CalculateBackgroundColor(double time)
        {
            constexpr double Speed = 0.5;      // Radians per second
            constexpr double Middle = 0.2;     // Average brightness of each channel
            constexpr double Amplitude = 0.15; // How far a channel goes up and down from the middle
            constexpr double ThirdOfCircle = 2.0 * std::numbers::pi / 3.0;

            const double angle = time * Speed;
            const double red = Middle + Amplitude * std::sin(angle);
            const double green = Middle + Amplitude * std::sin(angle + ThirdOfCircle);
            const double blue = Middle + Amplitude * std::sin(angle + 2.0 * ThirdOfCircle);

            return glm::vec4(red, green, blue, 1.0);
        }
    }

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

        std::expected<UI::DebugOverlay, std::string> debugOverlay = UI::DebugOverlay::Create(*window);
        if (!debugOverlay.has_value())
            return std::unexpected(debugOverlay.error());

        return Application(std::move(*SDLLibrary), std::move(*window), std::move(*debugOverlay));
    }

    Application::Application(Platform::SDLLibrary SDLLibrary, Platform::Window window, UI::DebugOverlay debugOverlay) noexcept
        : m_SDLLibrary(std::move(SDLLibrary))
        , m_window(std::move(window))
        , m_debugOverlay(std::move(debugOverlay))
    {}

    int Application::Run()
    {
        Core::Log::Write(LogCategory::Core, LogLevel::Info, "Main loop started");

        Core::FrameTimer frameTimer(Core::FrameTimer::Clock::now());

        // One iteration is one frame.
        while (!m_window.IsCloseRequested())
        {
            frameTimer.StartFrame(Core::FrameTimer::Clock::now());

            m_window.ProcessEvents();

            Renderer::SetViewport(m_window.GetWidthInPixels(), m_window.GetHeightInPixels());
            Renderer::ClearFrame(CalculateBackgroundColor(frameTimer.GetTotalTime()));

            // The overlay is drawn last, on top of the game.
            m_debugOverlay.Draw();

            m_window.SwapBuffers();
        }

        Core::Log::Write(LogCategory::Core, LogLevel::Info, "Main loop finished after {:.1f} seconds",
                         frameTimer.GetTotalTime());

        return 0;
    }
}
