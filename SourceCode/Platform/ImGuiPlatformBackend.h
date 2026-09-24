#pragma once

#include <expected>
#include <string>

namespace Abomination::Platform
{
    class Window;

    // The SDL3 part of Dear ImGui: tells ImGui the window size, the time between frames, and passes it
    // the keyboard and mouse input (Window::ProcessEvents forwards every SDL event to ImGui).
    // Requires an ImGui context. Move-only.
    class ImGuiPlatformBackend
    {
    public:
        [[nodiscard]] static std::expected<ImGuiPlatformBackend, std::string> Initialize(const Window& window);

        ImGuiPlatformBackend(const ImGuiPlatformBackend&) = delete;
        ImGuiPlatformBackend& operator=(const ImGuiPlatformBackend&) = delete;

        ImGuiPlatformBackend(ImGuiPlatformBackend&& other) noexcept;
        ImGuiPlatformBackend& operator=(ImGuiPlatformBackend&& other) noexcept;

        ~ImGuiPlatformBackend();

        // Must be called at the start of every ImGui frame.
        void StartFrame();

    private:
        ImGuiPlatformBackend() noexcept = default;

        // False for an object whose ownership was moved to another object: such an object must not shut the backend down.
        bool m_isActive = false;
    };
}
