#include "UI/DebugOverlay.h"

#include "Core/BuildConfiguration.h"
#include "Core/FixedTimestep.h"
#include "Core/FrameLimiter.h"
#include "Core/FrameStatistics.h"
#include "Core/Version.h"
#include "Platform/Window.h"
#include "Renderer/OpenGLLoader.h"

#include <imgui.h>

#include <algorithm>
#include <array>
#include <format>
#include <span>
#include <string_view>
#include <utility>

namespace Abomination::UI
{
    namespace
    {
        // Distance from the edges of the game window (below the menu bar) to the performance window, in pixels.
        constexpr float PerformanceWindowMargin = 10.0f;

        // Opacity of the performance window background: 0 is fully transparent, 1 is opaque.
        constexpr float PerformanceWindowBackgroundAlpha = 0.6f;

        // Size of the frame time graph in pixels.
        constexpr float GraphWidth = 400.0f;
        constexpr float GraphHeight = 60.0f;

        // The top of the graph is at least 1/30 s (33.3 ms): the budget of a frame at 30 FPS. A fixed minimum keeps
        // the scale stable, so the same frame time always has the same height; only longer frames stretch it.
        constexpr float MinimumGraphTopFrameTime = 1.0f / 30.0f;

        constexpr float MillisecondsPerSecond = 1000.0f;

        // The limits offered in Settings > Display > FPS limit; 0 means no limit. They are chosen for testing, not for
        // players: with the simulation running at 60 ticks per second,
        //   15, 30 - a slow computer: 4 and 2 ticks in every frame;
        //   60     - exactly 1 tick in every frame;
        //   120    - an even pattern: 0, 1, 0, 1 ticks per frame;
        //   144    - an uneven pattern (0, 0, 1, 0, 1, ...), where movement stutters without interpolation;
        //   240    - common fast monitors, many frames without a tick.
        constexpr std::array FramesPerSecondLimits{0, 15, 30, 60, 120, 144, 240};

        // The window has a title bar with a close button and can be collapsed by the arrow in it, but it cannot be moved:
        //   AlwaysAutoResize   - the size always fits the contents (so it cannot be resized by hand either);
        //   NoMove             - stays pinned to the corner;
        //   NoSavedSettings    - its position and state are not written anywhere;
        //   NoFocusOnAppearing - does not take the keyboard focus from the game when it appears;
        //   NoNav              - is skipped by keyboard and gamepad navigation between ImGui windows.
        constexpr ImGuiWindowFlags PerformanceWindowFlags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove |
                                                            ImGuiWindowFlags_NoSavedSettings |
                                                            ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;

        // "Unlimited" or "144 FPS": the text of a frame rate limit in the menu and in the performance window.
        std::string FormatFramesPerSecondLimit(int maxFramesPerSecond)
        {
            if (maxFramesPerSecond == 0)
                return "Unlimited";

            return std::format("{} FPS", maxFramesPerSecond);
        }
    }

    std::expected<DebugOverlay, std::string> DebugOverlay::Create(const Platform::Window& window,
                                                                  const std::filesystem::path& fontPath)
    {
        // The order matters: the context first, then the backends that register themselves in it.
        std::expected<ImGuiLibrary, std::string> library = ImGuiLibrary::Initialize(fontPath);
        if (!library.has_value())
            return std::unexpected(library.error());

        std::expected<Platform::ImGuiPlatformBackend, std::string> platformBackend =
            Platform::ImGuiPlatformBackend::Initialize(window);
        if (!platformBackend.has_value())
            return std::unexpected(platformBackend.error());

        std::expected<Renderer::ImGuiRendererBackend, std::string> rendererBackend =
            Renderer::ImGuiRendererBackend::Initialize();
        if (!rendererBackend.has_value())
            return std::unexpected(rendererBackend.error());

        return DebugOverlay(std::move(*library), std::move(*platformBackend), std::move(*rendererBackend),
                            Renderer::GetGraphicsDeviceInfo().GPUName);
    }

    DebugOverlay::DebugOverlay(ImGuiLibrary library, Platform::ImGuiPlatformBackend platformBackend,
                               Renderer::ImGuiRendererBackend rendererBackend, std::string GPUName) noexcept
        : m_library(std::move(library))
        , m_platformBackend(std::move(platformBackend))
        , m_rendererBackend(std::move(rendererBackend))
        , m_GPUName(std::move(GPUName))
    {}

    void DebugOverlay::Draw(const DebugOverlayContext& context)
    {
        // 1. Start the frame: the backends pass ImGui the window size, the time and the input of this frame.
        m_rendererBackend.StartFrame();
        m_platformBackend.StartFrame();
        ImGui::NewFrame();

        // 2. Describe the windows. Nothing is drawn yet: ImGui only records what has to be drawn.
        //    While the overlay is hidden the ImGui frame still runs, just without windows: ImGui keeps receiving the
        //    input and the time, so it is in a consistent state when the overlay is shown again. An empty frame costs
        //    practically nothing.
        if (m_isVisible)
        {
            DrawMainMenuBar(context);

            if (m_isPerformanceWindowOpen)
                DrawPerformanceWindow(context);
        }

        // 3. ImGui turns the recorded windows into lists of triangles, and the OpenGL backend draws them.
        ImGui::Render();
        m_rendererBackend.DrawFrame();
    }

    void DebugOverlay::ToggleVisibility() noexcept
    {
        m_isVisible = !m_isVisible;
    }

    bool DebugOverlay::IsVisible() const noexcept
    {
        return m_isVisible;
    }

    void DebugOverlay::DrawMainMenuBar(const DebugOverlayContext& context)
    {
        // BeginMainMenuBar() creates a bar along the top edge of the screen; BeginMenu() adds a menu to it that opens
        // on click. Both return true only while they are visible/open, and only then must their End...() be called.
        if (!ImGui::BeginMainMenuBar())
            return;

        if (ImGui::BeginMenu("View"))
        {
            // MenuItem(label, shortcut, bool*) shows a check mark and flips the bool when clicked.
            ImGui::MenuItem("Performance", nullptr, &m_isPerformanceWindowOpen);
            ImGui::EndMenu();
        }

        // Settings are grouped into submenus the same way as the options menu of the game will be (Settings > Display, ...).
        // A BeginMenu() inside an open menu becomes a submenu.
        if (ImGui::BeginMenu("Settings"))
        {
            if (ImGui::BeginMenu("Display"))
            {
                // Here MenuItem(label, shortcut, bool) only shows the check mark and returns true when clicked,
                // because the state belongs to the window, not to the overlay.
                const bool isVSyncEnabled = context.window.IsVSyncEnabled();
                if (ImGui::MenuItem("V-Sync", nullptr, isVSyncEnabled))
                    context.window.SetVSyncEnabled(!isVSyncEnabled);
                ImGui::SetItemTooltip("Waits for the monitor refresh: no tearing, but FPS never exceeds the refresh rate.");

                // One item per limit, the current one checked (like radio buttons).
                if (ImGui::BeginMenu("FPS limit"))
                {
                    for (const int limit : FramesPerSecondLimits)
                    {
                        const bool isCurrentLimit = context.frameLimiter.GetMaxFramesPerSecond() == limit;
                        if (ImGui::MenuItem(FormatFramesPerSecondLimit(limit).c_str(), nullptr, isCurrentLimit))
                            context.frameLimiter.SetMaxFramesPerSecond(limit);
                    }

                    ImGui::EndMenu();
                }

                ImGui::EndMenu();
            }

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

    void DebugOverlay::DrawPerformanceWindow(const DebugOverlayContext& context)
    {
        // The window is placed below the menu bar, whose height is the height of one line of ImGui widgets.
        // Both calls only affect the next Begin(). ImGuiCond_Always applies the position every frame,
        // so the window stays pinned to the corner.
        const ImVec2 position(PerformanceWindowMargin, ImGui::GetFrameHeight() + PerformanceWindowMargin);
        ImGui::SetNextWindowPos(position, ImGuiCond_Always);
        ImGui::SetNextWindowBgAlpha(PerformanceWindowBackgroundAlpha);

        // The title is shown in the title bar; ImGui also identifies windows by it, so it must be unique.
        // Passing the bool adds a close button to the title bar, which sets it to false.
        // Begin() returns false when the window is collapsed or fully clipped: then its contents are skipped,
        // but End() must still be called.
        if (ImGui::Begin("Performance", &m_isPerformanceWindowOpen, PerformanceWindowFlags))
        {
            const Core::FrameStatistics& frameStatistics = context.frameStatistics;

            const std::string_view buildConfiguration = Core::IsDebugBuild ? "Debug" : "Release";
            const std::string versionText =
                std::format("Abomination {} ({})", Core::GetGameVersionString(), buildConfiguration);
            ImGui::TextUnformatted(versionText.c_str());
            ImGui::TextUnformatted(m_GPUName.c_str());

            ImGui::Separator();

            const float averageFrameTime = frameStatistics.GetAverageFrameTime();
            const float longestFrameTime = frameStatistics.GetLongestFrameTime();

            // "{:.0f}" - no digits after the point, "{:.2f}" - two digits.
            const std::string framesPerSecondText = std::format("FPS: {:.0f}", frameStatistics.GetAverageFramesPerSecond());
            const std::string frameTimeText =
                std::format("Frame time: {:.2f} ms (longest {:.2f} ms)", averageFrameTime * MillisecondsPerSecond,
                            longestFrameTime * MillisecondsPerSecond);
            const std::string frameRateSettingsText =
                std::format("V-Sync: {}, FPS limit: {}", context.window.IsVSyncEnabled() ? "on" : "off",
                            FormatFramesPerSecondLimit(context.frameLimiter.GetMaxFramesPerSecond()));
            ImGui::TextUnformatted(framesPerSecondText.c_str());
            ImGui::TextUnformatted(frameTimeText.c_str());
            ImGui::TextUnformatted(frameRateSettingsText.c_str());

            // Simulation ticks per second: actually run / target. Both are equal at any FPS; fewer actual ticks mean the
            // computer cannot simulate in real time and Core::FixedTimestep drops ticks, so the game runs slower.
            const std::string simulationText =
                std::format("Simulation: {:.0f} / {} ticks per second", frameStatistics.GetTicksPerSecond(),
                            context.fixedTimestep.GetTicksPerSecond());
            ImGui::TextUnformatted(simulationText.c_str());
            ImGui::SetItemTooltip("Actual / target. Fewer actual ticks mean the simulation cannot keep up and the game "
                                  "runs slower than real time.");

            // The graph: one point per frame, the height is the frame time. The samples are a ring buffer, so the
            // index of the oldest sample is passed as the offset: ImGui starts drawing from it and wraps around.
            // The "##" prefix hides the label: the text after it is used only as an ID.
            // The top of the graph fits the longest frame on the graph itself, which can be older than the interval
            // of the numbers above (the graph covers MaxSampleCount frames, the numbers only the last half second).
            const int oldestSampleIndex = static_cast<int>(frameStatistics.GetOldestSampleIndex());
            const std::span<const float> frameTimeSamples = frameStatistics.GetFrameTimeSamples();
            const float longestSample = frameTimeSamples.empty() ? 0.0f : std::ranges::max(frameTimeSamples);
            const float graphTop = std::max(MinimumGraphTopFrameTime, longestSample);
            const std::string graphCaption = std::format("0 - {:.0f} ms", graphTop * MillisecondsPerSecond);

            ImGui::PlotLines("##FrameTimes", frameTimeSamples.data(), static_cast<int>(frameTimeSamples.size()),
                             oldestSampleIndex, graphCaption.c_str(), 0.0f, graphTop, ImVec2(GraphWidth, GraphHeight));
        }
        ImGui::End();
    }
}
