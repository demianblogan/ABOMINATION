#include "UI/DebugOverlay.h"

#include "Core/BuildConfiguration.h"
#include "Core/FrameStatistics.h"
#include "Core/Version.h"
#include "Renderer/OpenGLLoader.h"

#include <imgui.h>

#include <algorithm>
#include <format>
#include <span>
#include <string_view>
#include <utility>

namespace Abomination::UI
{
    namespace
    {
        // Distance from the top-left corner of the game window to the statistics window, in pixels.
        constexpr float StatisticsWindowMargin = 10.0f;

        // Opacity of the statistics window background: 0 is fully transparent, 1 is opaque.
        constexpr float StatisticsWindowBackgroundAlpha = 0.6f;

        // Size of the frame time graph in pixels.
        constexpr float GraphWidth = 400.0f;
        constexpr float GraphHeight = 60.0f;

        // The top of the graph is at least 1/30 s (33.3 ms): the budget of a frame at 30 FPS. A fixed minimum keeps
        // the scale stable, so the same frame time always has the same height; only longer frames stretch it.
        constexpr float MinimumGraphTopFrameTime = 1.0f / 30.0f;

        constexpr float MillisecondsPerSecond = 1000.0f;

        // The window has a title bar and can be collapsed by the arrow in it, but it cannot be moved:
        //   AlwaysAutoResize   - the size always fits the contents (so it cannot be resized by hand either);
        //   NoMove             - stays pinned to the corner;
        //   NoSavedSettings    - its position and state are not written anywhere;
        //   NoFocusOnAppearing - does not take the keyboard focus from the game when it appears;
        //   NoNav              - is skipped by keyboard and gamepad navigation between ImGui windows.
        constexpr ImGuiWindowFlags StatisticsWindowFlags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove |
                                                           ImGuiWindowFlags_NoSavedSettings |
                                                           ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
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

    void DebugOverlay::Draw(const Core::FrameStatistics& frameStatistics)
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
            DrawStatisticsWindow(frameStatistics);

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

    void DebugOverlay::DrawStatisticsWindow(const Core::FrameStatistics& frameStatistics) const
    {
        // Both calls only affect the next Begin(). ImGuiCond_Always applies the position every frame,
        // so the window stays pinned to the corner.
        ImGui::SetNextWindowPos(ImVec2(StatisticsWindowMargin, StatisticsWindowMargin), ImGuiCond_Always);
        ImGui::SetNextWindowBgAlpha(StatisticsWindowBackgroundAlpha);

        // The title is shown in the title bar; ImGui also identifies windows by it, so it must be unique.
        // Begin() returns false when the window is collapsed or fully clipped: then its contents are skipped,
        // but End() must still be called.
        if (ImGui::Begin("Statistics", nullptr, StatisticsWindowFlags))
        {
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
            ImGui::TextUnformatted(framesPerSecondText.c_str());
            ImGui::TextUnformatted(frameTimeText.c_str());

            // The graph: one point per frame, the height is the frame time. The samples are a ring buffer, so the
            // index of the oldest sample is passed as the offset: ImGui starts drawing from it and wraps around.
            // The "##" prefix hides the label: the text after it is used only as an ID.
            const std::span<const float> samples = frameStatistics.GetSamples();
            const float graphTop = std::max(MinimumGraphTopFrameTime, longestFrameTime);
            const std::string graphCaption = std::format("0 - {:.0f} ms", graphTop * MillisecondsPerSecond);

            ImGui::PlotLines("##FrameTimes", samples.data(), static_cast<int>(samples.size()),
                             static_cast<int>(frameStatistics.GetOldestSampleIndex()), graphCaption.c_str(), 0.0f, graphTop,
                             ImVec2(GraphWidth, GraphHeight));
        }
        ImGui::End();
    }
}
