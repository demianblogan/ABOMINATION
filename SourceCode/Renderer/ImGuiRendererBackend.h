#pragma once

#include <expected>
#include <string>

namespace Abomination::Renderer
{
    // The OpenGL part of Dear ImGui: turns the windows and widgets ImGui produced in a frame into triangles
    // and draws them on top of the frame. Requires an ImGui context and loaded OpenGL functions. Move-only.
    class ImGuiRendererBackend
    {
    public:
        [[nodiscard]] static std::expected<ImGuiRendererBackend, std::string> Initialize();

        ImGuiRendererBackend(const ImGuiRendererBackend&) = delete;
        ImGuiRendererBackend& operator=(const ImGuiRendererBackend&) = delete;

        ImGuiRendererBackend(ImGuiRendererBackend&& other) noexcept;
        ImGuiRendererBackend& operator=(ImGuiRendererBackend&& other) noexcept;

        ~ImGuiRendererBackend();

        // Must be called at the start of every ImGui frame.
        void StartFrame();

        // Draws everything ImGui produced since the frame started. Must be called after ImGui::Render().
        void DrawFrame();

    private:
        ImGuiRendererBackend() noexcept = default;

        // False for an object whose ownership was moved to another object: such an object must not shut the backend down.
        bool m_isActive = false;
    };
}
