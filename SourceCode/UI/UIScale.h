#pragma once

#include <imgui.h>

// Sizes of the debug overlay that are given in pixels (window positions, graph sizes) must grow with the scale of the
// interface, like the text does, or they would stay small on a 4K monitor.
namespace Abomination::UI
{
    // The scale ImGui currently draws with: the display scale of Windows times the UI scale chosen in the debug menu
    // (see DebugOverlay). 1.0 means sizes are used as written.
    [[nodiscard]] float GetUIScale();

    // A size or position in pixels at scale 1.0, converted to the current scale.
    [[nodiscard]] float ScaleToUI(float pixels);
    [[nodiscard]] ImVec2 ScaleToUI(ImVec2 pixels);
}
