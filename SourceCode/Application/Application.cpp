#include "Application/Application.h"

#include "Core/BuildConfiguration.h"
#include "Core/FrameStatistics.h"
#include "Core/FrameTimer.h"
#include "Core/Log.h"
#include "Platform/SystemServices.h"
#include "Renderer/DebugOutput.h"
#include "Renderer/OpenGLLoader.h"
#include "Renderer/RenderCommands.h"

#include <glm/common.hpp>
#include <glm/vec3.hpp>
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

        // The camera starts 2.5 meters in front of the cube (the cube is at the origin, the camera looks along -Z).
        constexpr glm::vec3 InitialCameraPosition{0.0f, 0.0f, 2.5f};
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

        std::expected<Renderer::ShaderStore, std::string> shaders = Renderer::ShaderStore::Create(assetsDirectory);
        if (!shaders.has_value())
            return std::unexpected(shaders.error());

        Renderer::RenderAssets renderAssets{
            .textures = Renderer::TextureStore(assetsDirectory),
            .shaders = std::move(*shaders),
            .meshes = Renderer::MeshStore(),
        };

        Renderer::DemoScene demoScene = Renderer::DemoScene::Create(renderAssets);

        // The overlay reads its font from the assets and keeps its window settings next to the executable, like the log.
        std::expected<UI::DebugOverlay, std::string> debugOverlay =
            UI::DebugOverlay::Create(*window, assetsDirectory, Platform::GetExecutableDirectory());
        if (!debugOverlay.has_value())
            return std::unexpected(debugOverlay.error());

        return Application(std::move(*SDLLibrary), std::move(*window), std::move(renderAssets), std::move(demoScene),
                           std::move(*debugOverlay));
    }

    Application::Application(Platform::SDLLibrary SDLLibrary, Platform::Window window, Renderer::RenderAssets renderAssets,
                             Renderer::DemoScene demoScene, UI::DebugOverlay debugOverlay) noexcept
        : m_SDLLibrary(std::move(SDLLibrary))
        , m_window(std::move(window))
        , m_renderAssets(std::move(renderAssets))
        , m_demoScene(std::move(demoScene))
        , m_debugOverlay(std::move(debugOverlay))
    {
        m_camera.SetPosition(InitialCameraPosition);
        m_previousCameraPosition = InitialCameraPosition;
    }

    int Application::Run()
    {
        Core::Log::Write(LogCategory::Core, LogLevel::Info, "Main loop started");

        Core::FrameTimer frameTimer(Core::Clock::now());
        Core::FrameStatistics frameStatistics;

        // One iteration is one frame.
        while (!m_window.IsCloseRequested())
        {
            const Core::TimePoint frameStartTime = Core::Clock::now();
            frameTimer.StartFrame(frameStartTime);

            // 1. Input: first the devices get this frame's input, then the actions are calculated from them.
            m_window.ProcessEvents(m_inputDevices);
            m_actionStates.Update(m_inputDevices, m_inputBindings);

            // 2. Everything that happens once per frame.
            Update();

            // 3. The simulation in fixed ticks: 0, 1 or several per frame, depending on how long the frame was.
            //    The ticks of this frame read the input of this frame. A frame without ticks does not lose held keys
            //    (they are still held in the next frame), but a short press that starts and stops between two ticks
            //    would be lost; it does not matter for flying, and will be handled for jumping (0.2, player movement).
            const int tickCount = m_fixedTimestep.Advance(frameTimer.GetDeltaTime());
            for (int tick = 0; tick < tickCount; ++tick)
                FixedUpdate(m_fixedTimestep.GetTickDuration());

            frameStatistics.AddFrame(frameTimer.GetDeltaTime(), tickCount);

            // 4. Drawing and showing the frame.
            Render(frameTimer.GetTotalTime(), frameStatistics);

            // 5. With an FPS limit, the frame waits here until it has lasted 1 / limit seconds. The next frame then
            //    starts right on time, and its measured delta time includes this wait.
            Platform::SleepPrecisely(m_frameLimiter.GetWaitTime(frameStartTime, Core::Clock::now()));
        }

        Core::Log::Write(LogCategory::Core, LogLevel::Info, "Main loop finished after {:.1f} seconds",
                         frameTimer.GetTotalTime());

        return 0;
    }

    void Application::Update()
    {
        // An action of the application itself (not of the game), so it is handled here.
        if (m_actionStates.WasActionStarted(Input::Action::ToggleDebugOverlay))
            m_debugOverlay.ToggleVisibility();

        // While LookAroundMode is active (the right mouse button by default), the mouse is captured for looking around,
        // like in the Unity and Unreal editors. The mode is switched only when the action starts or stops.
        // Capturing is done here because the window belongs to the application; the controller only turns the camera.
        if (m_actionStates.WasActionStarted(Input::Action::LookAroundMode))
            m_window.SetRelativeMouseMode(true);
        if (m_actionStates.WasActionStopped(Input::Action::LookAroundMode))
            m_window.SetRelativeMouseMode(false);

        // Turning follows the mouse every frame, not in ticks: it uses the mouse movement of this frame, which does not
        // depend on time. In ticks, the movement of a frame without ticks would be lost and applied twice in a frame
        // with two ticks.
        m_cameraController.UpdateRotation(m_camera, m_actionStates, m_inputDevices.mouse);
    }

    void Application::FixedUpdate(float tickDuration)
    {
        // Remembered before the camera moves, so a frame can be drawn anywhere between this position and the new one.
        m_previousCameraPosition = m_camera.GetPosition();
        m_cameraController.UpdateMovement(m_camera, m_actionStates, tickDuration);
    }

    void Application::Render(double totalTime, const Core::FrameStatistics& frameStatistics)
    {
        Renderer::SetViewport(m_window.GetWidthInPixels(), m_window.GetHeightInPixels());
        Renderer::ClearFrame(BackgroundColor);
        m_demoScene.Draw(totalTime, GetInterpolatedCamera(), m_window.GetWidthInPixels(), m_window.GetHeightInPixels(),
                         m_renderAssets);

        // The overlay is drawn last, on top of the game.
        m_debugOverlay.Draw({
            .frameStatistics = frameStatistics,
            .fixedTimestep = m_fixedTimestep,
            .window = m_window,
            .frameLimiter = m_frameLimiter,
            .renderAssets = m_renderAssets,
        });

        m_window.SwapBuffers();
    }

    Renderer::Camera Application::GetInterpolatedCamera() const
    {
        // glm::mix(a, b, t) = a + (b - a) * t: the point at fraction t of the way from a to b (like std::lerp,
        // but for vectors). t is the part of the next tick that has already passed, so the drawn position follows
        // the real time (at most one tick behind the simulation). Only the position is interpolated: the rotation is
        // already up to date, because turning happens every frame.
        Renderer::Camera camera = m_camera;
        const float interpolationFactor = m_fixedTimestep.GetInterpolationFactor();
        camera.SetPosition(glm::mix(m_previousCameraPosition, m_camera.GetPosition(), interpolationFactor));

        return camera;
    }
}
