#include "UI/DebugOverlay.h"

#include "Core/Version.h"

#include <imgui.h>

#include <format>
#include <utility>

namespace Abomination::UI
{
    std::expected<DebugOverlay, std::string> DebugOverlay::Create(const Platform::Window& window)
    {
        // The order matters: the context first, then the backends that register themselves in it.
        std::expected<ImGuiLibrary, std::string> library = ImGuiLibrary::Initialize();
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

        return DebugOverlay(std::move(*library), std::move(*platformBackend), std::move(*rendererBackend));
    }

    DebugOverlay::DebugOverlay(ImGuiLibrary library, Platform::ImGuiPlatformBackend platformBackend,
                               Renderer::ImGuiRendererBackend rendererBackend) noexcept
        : m_library(std::move(library))
        , m_platformBackend(std::move(platformBackend))
        , m_rendererBackend(std::move(rendererBackend))
    {}

    void DebugOverlay::Draw()
    {
        // 1. Start the frame: the backends pass ImGui the window size, the time and the input of this frame.
        m_rendererBackend.StartFrame();
        m_platformBackend.StartFrame();
        ImGui::NewFrame();

        // 2. Describe the windows. Nothing is drawn yet: ImGui only records what has to be drawn.
        ImGui::Begin("Debug");
        ImGui::TextUnformatted(std::format("Abomination {}", Core::GetGameVersionString()).c_str());
        ImGui::End();

        // 3. ImGui turns the recorded windows into lists of triangles, and the OpenGL backend draws them.
        ImGui::Render();
        m_rendererBackend.DrawFrame();
    }
}
