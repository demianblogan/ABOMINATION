#pragma once

#include "Platform/ImGuiPlatformBackend.h"
#include "Renderer/ImGuiRendererBackend.h"
#include "UI/ImGuiLibrary.h"

#include <expected>
#include <string>

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
        [[nodiscard]] static std::expected<DebugOverlay, std::string> Create(const Platform::Window& window);

        // Builds the overlay for the current frame and draws it on top of what is already in the back buffer.
        // Must be called once per frame, after the game is drawn and before the buffers are swapped.
        void Draw();

    private:
        DebugOverlay(ImGuiLibrary library, Platform::ImGuiPlatformBackend platformBackend,
                     Renderer::ImGuiRendererBackend rendererBackend) noexcept;

        // Members are destroyed in reverse order of declaration: both backends first, then the ImGui context they use.
        ImGuiLibrary m_library;
        Platform::ImGuiPlatformBackend m_platformBackend;
        Renderer::ImGuiRendererBackend m_rendererBackend;
    };
}
