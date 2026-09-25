#pragma once

#include <expected>
#include <filesystem>
#include <string>

namespace Abomination::UI
{
    // Owns the Dear ImGui context: all windows, widgets, fonts and input state of ImGui live in it.
    // The context is created in Initialize() and destroyed in the destructor. It must be created before
    // the ImGui backends and destroyed after them. Only one object may exist at a time. Move-only.
    class ImGuiLibrary
    {
    public:
        // fontPath: the TTF font for all ImGui text. If the file is missing, the built-in font is used instead.
        // settingsPath: the file where ImGui remembers the position, size and state of its windows between runs.
        [[nodiscard]] static std::expected<ImGuiLibrary, std::string> Initialize(const std::filesystem::path& fontPath,
                                                                                 std::filesystem::path settingsPath);

        ImGuiLibrary(const ImGuiLibrary&) = delete;
        ImGuiLibrary& operator=(const ImGuiLibrary&) = delete;

        ImGuiLibrary(ImGuiLibrary&& other) noexcept;
        ImGuiLibrary& operator=(ImGuiLibrary&& other) noexcept;

        ~ImGuiLibrary();

        // Writes the window settings to the settings file if ImGui reports that they changed. Call once per frame,
        // after ImGui::Render(). ImGui itself waits a few seconds after a change, so the file is not written every frame.
        void SaveSettingsIfChanged();

    private:
        explicit ImGuiLibrary(std::filesystem::path settingsPath) noexcept;

        void SaveSettings() const;

        std::filesystem::path m_settingsPath;

        // False for an object whose ownership was moved to another object: such an object must not destroy the context.
        bool m_isActive = false;
    };
}
