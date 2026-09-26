#include "UI/UIScale.h"

namespace Abomination::UI
{
    float GetUIScale()
    {
        // ImGuiLibrary::SetScale() stores the scale in the style as the font scale, so it is read back from there.
        return ImGui::GetStyle().FontScaleDpi;
    }

    float ScaleToUI(float pixels)
    {
        return pixels * GetUIScale();
    }

    ImVec2 ScaleToUI(ImVec2 pixels)
    {
        const float scale = GetUIScale();

        return ImVec2(pixels.x * scale, pixels.y * scale);
    }
}
