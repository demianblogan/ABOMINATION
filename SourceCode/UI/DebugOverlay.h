#pragma once

#include "Core/BuildConfiguration.h"
#include "Platform/ImGuiPlatformBackend.h"
#include "Renderer/ImGuiRendererBackend.h"
#include "UI/ImGuiLibrary.h"

#include <expected>
#include <filesystem>
#include <string>

namespace Abomination::Core
{
    class FrameStatistics;
}

namespace Abomination::Platform
{
    class Window;
}

namespace Abomination::UI
{
    // Developer overlay drawn with Dear ImGui on top of the game: performance numbers and, later, debug tools.
    // Not part of the game interface. Requires a window with loaded OpenGL functions. Move-only.
    class DebugOverlay
    {
    public:
        // fontPath: the TTF font of the overlay text (the built-in font is used if the file is missing).
        [[nodiscard]] static std::expected<DebugOverlay, std::string> Create(const Platform::Window& window,
                                                                             const std::filesystem::path& fontPath);

        // Builds the overlay for the current frame and draws it on top of what is already in the back buffer.
        // Must be called once per frame, after the game is drawn and before the buffers are swapped.
        void Draw(const Core::FrameStatistics& frameStatistics);

        // Shows the overlay if it is hidden and hides it if it is shown.
        void ToggleVisibility() noexcept;

        [[nodiscard]] bool IsVisible() const noexcept;

    private:
        DebugOverlay(ImGuiLibrary library, Platform::ImGuiPlatformBackend platformBackend,
                     Renderer::ImGuiRendererBackend rendererBackend, std::string GPUName) noexcept;

        // The small window in the top-left corner: version, GPU, FPS, frame time and its graph.
        void DrawStatisticsWindow(const Core::FrameStatistics& frameStatistics) const;

        // Members are destroyed in reverse order of declaration: both backends first, then the ImGui context they use.
        ImGuiLibrary m_library;
        Platform::ImGuiPlatformBackend m_platformBackend;
        Renderer::ImGuiRendererBackend m_rendererBackend;

        // Asked from the driver once: it does not change while the game runs.
        std::string m_GPUName;

        // Visible from the start in Debug builds, where the numbers are needed most; F1 toggles it in any build.
        bool m_isVisible = Core::IsDebugBuild;
    };
}
