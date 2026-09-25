#include "UI/ImGuiLibrary.h"

#include "Core/Log.h"

#include <imgui.h>

#include <filesystem>
#include <utility>

namespace Abomination::UI
{
    using Core::LogCategory;
    using Core::LogLevel;

    namespace
    {
        // Height of the debug overlay text in pixels. The game interface (menus, HUD) will have its own fonts.
        constexpr float DebugUIFontSize = 18.0f;

        // Adds the font ImGui draws all its text with. The first font added becomes the default one, so this must be
        // done before the first frame. A missing or broken font file is not fatal for a debug tool: the built-in
        // vector font is used instead, and a warning goes to the log.
        void AddDebugUIFont(const std::filesystem::path& fontPath)
        {
            ImFontAtlas* fonts = ImGui::GetIO().Fonts;

            // AddFontFromFileTTF stops the program with an assertion if the file does not exist, so check first.
            if (std::filesystem::exists(fontPath))
            {
                if (fonts->AddFontFromFileTTF(fontPath.string().c_str(), DebugUIFontSize) != nullptr)
                    return;
            }

            Core::Log::Write(LogCategory::UI, LogLevel::Warning, "Failed to load the font \"{}\", using the built-in font",
                             fontPath.string());

            // The built-in vector font stays sharp at any size, unlike the built-in pixel font (sharp only at 13 px).
            fonts->AddFontDefaultVector();
        }
    }

    std::expected<ImGuiLibrary, std::string> ImGuiLibrary::Initialize(const std::filesystem::path& fontPath,
                                                                     std::filesystem::path settingsPath)
    {
        // Checks that the ImGui headers we compile with match the compiled ImGui library.
        IMGUI_CHECKVERSION();

        if (ImGui::CreateContext() == nullptr)
            return std::unexpected("Failed to create the ImGui context");

        // Window settings. By default ImGui reads and writes imgui.ini by itself, through IniFilename: a plain
        // const char* it keeps and uses at any time. Our ImGuiLibrary is moved (into DebugOverlay, then into
        // Application), and a pointer into a moved string could be left pointing at freed memory. So ImGui's own file
        // handling is turned off (nullptr), and the file is loaded here and saved by SaveSettingsIfChanged() instead.
        ImGui::GetIO().IniFilename = nullptr;
        if (std::filesystem::exists(settingsPath))
            ImGui::LoadIniSettingsFromDisk(settingsPath.string().c_str());

        AddDebugUIFont(fontPath);
        ImGui::GetStyle().FontSizeBase = DebugUIFontSize;

        ImGui::StyleColorsDark();

        Core::Log::Write(LogCategory::UI, LogLevel::Info, "Dear ImGui {} initialized", IMGUI_VERSION);

        ImGuiLibrary library(std::move(settingsPath));
        library.m_isActive = true;

        return library;
    }

    ImGuiLibrary::ImGuiLibrary(std::filesystem::path settingsPath) noexcept
        : m_settingsPath(std::move(settingsPath))
    {}

    ImGuiLibrary::ImGuiLibrary(ImGuiLibrary&& other) noexcept
        : m_settingsPath(std::move(other.m_settingsPath))
        , m_isActive(std::exchange(other.m_isActive, false))
    {}

    ImGuiLibrary& ImGuiLibrary::operator=(ImGuiLibrary&& other) noexcept
    {
        if (this != &other)
        {
            if (m_isActive)
            {
                SaveSettings();
                ImGui::DestroyContext();
            }

            m_settingsPath = std::move(other.m_settingsPath);
            m_isActive = std::exchange(other.m_isActive, false);
        }

        return *this;
    }

    ImGuiLibrary::~ImGuiLibrary()
    {
        // Changes made during the last few seconds are not saved yet (ImGui waits before reporting them), so the
        // settings are written once more before the context disappears.
        if (m_isActive)
        {
            SaveSettings();
            ImGui::DestroyContext();
        }
    }

    void ImGuiLibrary::SaveSettingsIfChanged()
    {
        // ImGui sets WantSaveIniSettings (only while IniFilename is nullptr) some seconds after a window was moved,
        // resized, opened or collapsed. The flag must be cleared by whoever saves.
        ImGuiIO& io = ImGui::GetIO();
        if (!io.WantSaveIniSettings)
            return;

        SaveSettings();
        io.WantSaveIniSettings = false;
    }

    void ImGuiLibrary::SaveSettings() const
    {
        ImGui::SaveIniSettingsToDisk(m_settingsPath.string().c_str());
    }
}
