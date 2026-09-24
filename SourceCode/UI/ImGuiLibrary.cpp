#include "UI/ImGuiLibrary.h"

#include "Core/Log.h"

#include <imgui.h>

#include <utility>

namespace Abomination::UI
{
    using Core::LogCategory;
    using Core::LogLevel;

    namespace
    {
        // Height of the debug overlay text in pixels. The game interface (menus, HUD) will have its own fonts.
        constexpr float DebugUIFontSize = 18.0f;
    }

    std::expected<ImGuiLibrary, std::string> ImGuiLibrary::Initialize()
    {
        // Checks that the ImGui headers we compile with match the compiled ImGui library.
        IMGUI_CHECKVERSION();

        if (ImGui::CreateContext() == nullptr)
            return std::unexpected("Failed to create the ImGui context");

        // By default ImGui saves window positions to imgui.ini in the current folder. Not needed for a debug overlay.
        ImGui::GetIO().IniFilename = nullptr;

        // The built-in vector font stays sharp at any size, unlike the default pixel font that looks good only at 13 px.
        // It must be added before the first frame: the first font added becomes the default one.
        ImGui::GetIO().Fonts->AddFontDefaultVector();
        ImGui::GetStyle().FontSizeBase = DebugUIFontSize;

        ImGui::StyleColorsDark();

        Core::Log::Write(LogCategory::UI, LogLevel::Info, "Dear ImGui {} initialized", IMGUI_VERSION);

        ImGuiLibrary library;
        library.m_isActive = true;

        return library;
    }

    ImGuiLibrary::ImGuiLibrary(ImGuiLibrary&& other) noexcept
        : m_isActive(std::exchange(other.m_isActive, false))
    {}

    ImGuiLibrary& ImGuiLibrary::operator=(ImGuiLibrary&& other) noexcept
    {
        if (this != &other)
        {
            if (m_isActive)
                ImGui::DestroyContext();

            m_isActive = std::exchange(other.m_isActive, false);
        }

        return *this;
    }

    ImGuiLibrary::~ImGuiLibrary()
    {
        if (m_isActive)
            ImGui::DestroyContext();
    }
}
