#pragma once

#include "Core/FixedTimestep.h"
#include "Core/FrameLimiter.h"
#include "Gameplay/FreeFlyCameraController.h"
#include "Input/ActionStates.h"
#include "Input/InputBindings.h"
#include "Input/InputDevices.h"
#include "Platform/SDLLibrary.h"
#include "Platform/Window.h"
#include "Renderer/Camera.h"
#include "Renderer/DemoScene.h"
#include "UI/DebugOverlay.h"

#include <glm/vec3.hpp>

#include <expected>
#include <filesystem>
#include <string>

namespace Abomination::Core
{
    class FrameStatistics;
}

namespace Abomination
{
    // The top-level object of the game: creates the platform objects and runs the main loop.
    // Lives in the root namespace as the only exception to the "namespace per module" rule,
    // because Abomination::Application::Application would repeat the same word twice.
    class Application
    {
    public:
        // assetsDirectory: the folder with the game files (fonts, shaders, textures), normally next to the executable.
        [[nodiscard]] static std::expected<Application, std::string> Create(const std::filesystem::path& assetsDirectory);

        // Runs the main loop until the window is closed. Returns the exit code of the process.
        [[nodiscard]] int Run();

    private:
        Application(Platform::SDLLibrary SDLLibrary, Platform::Window window, Renderer::DemoScene demoScene,
                    UI::DebugOverlay debugOverlay) noexcept;

        // The two kinds of updates of the main loop, named like in Unity:
        //   Update()      - once per frame: what must react immediately and does not depend on time
        //                   (debug overlay toggle, mouse capture, turning the camera with the mouse);
        //   FixedUpdate() - once per simulation tick, 0, 1 or several times per frame: everything that moves the world
        //                   forward in time. Always called with the same tickDuration (see Core::FixedTimestep),
        //                   so the result does not depend on the frame rate.
        void Update();
        void FixedUpdate(float tickDuration);

        // Draws the frame (the game, then the debug overlay on top) and shows it on the screen.
        // totalTime: seconds since the start, for animations; frameStatistics: the numbers for the overlay.
        void Render(double totalTime, const Core::FrameStatistics& frameStatistics);

        // The camera the frame is drawn through: m_camera with its position interpolated between the last two ticks.
        [[nodiscard]] Renderer::Camera GetInterpolatedCamera() const;

        // Members are destroyed in reverse order of declaration: the overlay and the scene first (they use OpenGL),
        // then the window, then SDL, which the window needs.
        Platform::SDLLibrary m_SDLLibrary;
        Platform::Window m_window;
        Renderer::DemoScene m_demoScene;

        // The camera the scene is drawn through, and the controller that flies it.
        // m_previousCameraPosition is the position before the last tick, needed for interpolation.
        Renderer::Camera m_camera;
        glm::vec3 m_previousCameraPosition{0.0f};
        Gameplay::FreeFlyCameraController m_cameraController;
        UI::DebugOverlay m_debugOverlay;

        // State of the keyboard and the mouse for the current frame: the window fills it, the game reads it.
        Input::InputDevices m_inputDevices;

        // Which keys and buttons trigger which actions, and the state of every action for the current frame.
        Input::InputBindings m_inputBindings = Input::InputBindings::CreateDefault();
        Input::ActionStates m_actionStates;

        // Keeps the frame rate at or below the limit chosen in the debug overlay (no limit by default).
        Core::FrameLimiter m_frameLimiter;

        // How many times per second the simulation runs. 60 is enough for a single-player game; it is one constant,
        // so it can be raised later (120) if movement or physics ever needs finer steps.
        static constexpr int SimulationTicksPerSecond = 60;

        // Splits the time of every frame into simulation ticks of 1 / SimulationTicksPerSecond seconds.
        Core::FixedTimestep m_fixedTimestep{SimulationTicksPerSecond};
    };
}
