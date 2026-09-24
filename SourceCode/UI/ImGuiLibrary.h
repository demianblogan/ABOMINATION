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
        [[nodiscard]] static std::expected<ImGuiLibrary, std::string> Initialize(const std::filesystem::path& fontPath);

        ImGuiLibrary(const ImGuiLibrary&) = delete;
        ImGuiLibrary& operator=(const ImGuiLibrary&) = delete;

        ImGuiLibrary(ImGuiLibrary&& other) noexcept;
        ImGuiLibrary& operator=(ImGuiLibrary&& other) noexcept;

        ~ImGuiLibrary();

    private:
        ImGuiLibrary() noexcept = default;

        // False for an object whose ownership was moved to another object: such an object must not destroy the context.
        bool m_isActive = false;
    };
}
