#include "Application/Application.h"

#include "Core/BuildConfiguration.h"
#include "Core/FrameStatistics.h"
#include "Core/FrameTimer.h"
#include "Core/Log.h"
#include "Renderer/DebugOutput.h"
#include "Renderer/OpenGLLoader.h"
#include "Renderer/RenderCommands.h"

#include <glm/vec4.hpp>

#include <utility>

namespace Abomination
{
    using Core::LogCategory;
    using Core::LogLevel;

    namespace
    {
        // A neutral dark gray, so the colors of the scene are easy to judge.
        constexpr glm::vec4 BackgroundColor{0.12f, 0.12f, 0.13f, 1.0f};
    }

    std::expected<Application, std::string> Application::Create(const std::filesystem::path& assetsDirectory)
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

        std::expected<Renderer::DemoScene, std::string> demoScene = Renderer::DemoScene::Create(assetsDirectory);
        if (!demoScene.has_value())
            return std::unexpected(demoScene.error());

        const std::filesystem::path debugUIFontPath = assetsDirectory / "Fonts" / "JetBrainsMonoRegular.ttf";
        std::expected<UI::DebugOverlay, std::string> debugOverlay = UI::DebugOverlay::Create(*window, debugUIFontPath);
        if (!debugOverlay.has_value())
            return std::unexpected(debugOverlay.error());

        return Application(std::move(*SDLLibrary), std::move(*window), std::move(*demoScene), std::move(*debugOverlay));
    }

    Application::Application(Platform::SDLLibrary SDLLibrary, Platform::Window window, Renderer::DemoScene demoScene,
                             UI::DebugOverlay debugOverlay) noexcept
        : m_SDLLibrary(std::move(SDLLibrary))
        , m_window(std::move(window))
        , m_demoScene(std::move(demoScene))
        , m_debugOverlay(std::move(debugOverlay))
    {}

    int Application::Run()
    {
        Core::Log::Write(LogCategory::Core, LogLevel::Info, "Main loop started");

        Core::FrameTimer frameTimer(Core::FrameTimer::Clock::now());
        Core::FrameStatistics frameStatistics;

        // One iteration is one frame.
        while (!m_window.IsCloseRequested())
        {
            frameTimer.StartFrame(Core::FrameTimer::Clock::now());
            frameStatistics.AddFrame(frameTimer.GetDeltaTime());

            // First the devices get this frame's input, then the actions are calculated from them.
            m_window.ProcessEvents(m_inputDevices);
            m_actionStates.Update(m_inputDevices, m_inputBindings);

            // An action of the application itself (not of the game), so it is handled here.
            if (m_actionStates.WasActionStarted(Input::Action::ToggleDebugOverlay))
                m_debugOverlay.ToggleVisibility();

            // While LookAroundMode is active (the right mouse button by default), the mouse is captured for looking around,
            // like in the Unity and Unreal editors. The mode is switched only when the action starts or stops.
            // This moves to the fly camera controller together with the camera.
            if (m_actionStates.WasActionStarted(Input::Action::LookAroundMode))
                m_window.SetRelativeMouseMode(true);
            if (m_actionStates.WasActionStopped(Input::Action::LookAroundMode))
                m_window.SetRelativeMouseMode(false);

            Renderer::SetViewport(m_window.GetWidthInPixels(), m_window.GetHeightInPixels());
            Renderer::ClearFrame(BackgroundColor);
            m_demoScene.Draw(frameTimer.GetTotalTime(), m_window.GetWidthInPixels(), m_window.GetHeightInPixels());

            // The overlay is drawn last, on top of the game.
            m_debugOverlay.Draw(frameStatistics);

            m_window.SwapBuffers();
        }

        Core::Log::Write(LogCategory::Core, LogLevel::Info, "Main loop finished after {:.1f} seconds",
                         frameTimer.GetTotalTime());

        return 0;
    }
}
