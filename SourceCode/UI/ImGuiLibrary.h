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
        // fontSize: the height of the text in pixels.
        // settingsPath: the file where ImGui remembers the position, size and state of its windows between runs.
        [[nodiscard]] static std::expected<ImGuiLibrary, std::string> Initialize(const std::filesystem::path& fontPath,
                                                                                 float fontSize,
                                                                                 std::filesystem::path settingsPath);

        ImGuiLibrary(const ImGuiLibrary&) = delete;
        ImGuiLibrary& operator=(const ImGuiLibrary&) = delete;

        ImGuiLibrary(ImGuiLibrary&& other) noexcept;
        ImGuiLibrary& operator=(ImGuiLibrary&& other) noexcept;

        ~ImGuiLibrary();

        // Writes the window settings to the settings file if ImGui reports that they changed. Call once per frame,
        // after ImGui::Render(). ImGui itself waits a few seconds after a change, so the file is not written every frame.
        void SaveSettingsIfChanged();

        // Scales all text and sizes of ImGui: 1.0 is the size given to Initialize(), 2.0 twice as big. Must be called
        // between frames (before ImGui::NewFrame()), because the font is chosen at the start of a frame.
        void SetScale(float scale);

    private:
        ImGuiLibrary(std::filesystem::path settingsPath, float fontSize) noexcept;

        void SaveSettings() const;

        std::filesystem::path m_settingsPath;

        // The font size at scale 1.0, kept to rebuild the style for a new scale.
        float m_fontSize = 0.0f;

        // False for an object whose ownership was moved to another object: such an object must not destroy the context.
        bool m_isActive = false;
    };
}
