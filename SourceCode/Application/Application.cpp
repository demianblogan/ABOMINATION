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

            m_window.ProcessEvents(m_inputDevices);

            // A direct key check for now. When the action layer of input appears (the fly camera branch), this becomes
            // the ToggleDebugOverlay action, and the key is taken from the bindings instead of being written here.
            if (m_inputDevices.keyboard.WasKeyPressed(Input::Key::F1))
                m_debugOverlay.ToggleVisibility();

            // While the right mouse button is held, the mouse is captured for looking around (used by the fly camera,
            // like in the Unity and Unreal editors). The mode is switched only when the button changes, not every frame.
            if (m_inputDevices.mouse.WasButtonPressed(Input::MouseButton::Right))
                m_window.SetRelativeMouseMode(true);
            if (m_inputDevices.mouse.WasButtonReleased(Input::MouseButton::Right))
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
