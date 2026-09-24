#include "Platform/ImGuiPlatformBackend.h"

#include "Platform/Window.h"

#include <imgui_impl_sdl3.h>

#include <utility>

namespace Abomination::Platform
{
    std::expected<ImGuiPlatformBackend, std::string> ImGuiPlatformBackend::Initialize(const Window& window)
    {
        if (!ImGui_ImplSDL3_InitForOpenGL(window.GetSDLWindow(), window.GetSDLContext()))
            return std::unexpected("Failed to initialize the SDL3 backend of ImGui");

        ImGuiPlatformBackend backend;
        backend.m_isActive = true;

        return backend;
    }

    ImGuiPlatformBackend::ImGuiPlatformBackend(ImGuiPlatformBackend&& other) noexcept
        : m_isActive(std::exchange(other.m_isActive, false))
    {}

    ImGuiPlatformBackend& ImGuiPlatformBackend::operator=(ImGuiPlatformBackend&& other) noexcept
    {
        if (this != &other)
        {
            if (m_isActive)
                ImGui_ImplSDL3_Shutdown();

            m_isActive = std::exchange(other.m_isActive, false);
        }

        return *this;
    }

    ImGuiPlatformBackend::~ImGuiPlatformBackend()
    {
        if (m_isActive)
            ImGui_ImplSDL3_Shutdown();
    }

    void ImGuiPlatformBackend::StartFrame()
    {
        ImGui_ImplSDL3_NewFrame();
    }
}
