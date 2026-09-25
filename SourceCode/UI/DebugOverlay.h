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
    class FixedTimestep;
    class FrameLimiter;
    class FrameStatistics;
}

namespace Abomination::Platform
{
    class Window;
}

namespace Abomination::UI
{
    // Everything the overlay shows or lets the developer change in one frame, gathered in one parameter of Draw().
    // New debug tools add their systems here (the renderer, the entities, ...) instead of adding parameters to Draw().
    // The overlay does not store these references: they are valid only during the Draw() call.
    struct DebugOverlayContext
    {
        const Core::FrameStatistics& frameStatistics;
        const Core::FixedTimestep& fixedTimestep;
        Platform::Window& window;
        Core::FrameLimiter& frameLimiter;
    };

    // Developer overlay drawn with Dear ImGui on top of the game: a menu bar with debug windows and settings.
    // Not part of the game interface. Requires a window with loaded OpenGL functions. Move-only.
    class DebugOverlay
    {
    public:
        // fontPath: the TTF font of the overlay text (the built-in font is used if the file is missing).
        [[nodiscard]] static std::expected<DebugOverlay, std::string> Create(const Platform::Window& window,
                                                                             const std::filesystem::path& fontPath);

        // Builds the overlay for the current frame and draws it on top of what is already in the back buffer.
        // Must be called once per frame, after the game is drawn and before the buffers are swapped.
        void Draw(const DebugOverlayContext& context);

        // Shows the overlay if it is hidden and hides it if it is shown.
        void ToggleVisibility() noexcept;

        [[nodiscard]] bool IsVisible() const noexcept;

    private:
        DebugOverlay(ImGuiLibrary library, Platform::ImGuiPlatformBackend platformBackend,
                     Renderer::ImGuiRendererBackend rendererBackend, std::string GPUName) noexcept;

        // The bar along the top edge of the game window: the View menu opens and closes debug windows,
        // the Settings menu changes settings of the game.
        void DrawMainMenuBar(const DebugOverlayContext& context);

        // The small window in the top-left corner: version, GPU, FPS, frame time and its graph.
        void DrawPerformanceWindow(const DebugOverlayContext& context);

        // Members are destroyed in reverse order of declaration: both backends first, then the ImGui context they use.
        ImGuiLibrary m_library;
        Platform::ImGuiPlatformBackend m_platformBackend;
        Renderer::ImGuiRendererBackend m_rendererBackend;

        // Asked from the driver once: it does not change while the game runs.
        std::string m_GPUName;

        // Visible from the start in Debug builds, where the numbers are needed most; F1 toggles it in any build.
        bool m_isVisible = Core::IsDebugBuild;

        // Which debug windows are open. Changed by the View menu and by the close button of each window.
        bool m_isPerformanceWindowOpen = true;
    };
}
