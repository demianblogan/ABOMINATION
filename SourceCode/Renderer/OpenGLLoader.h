#pragma once

#include <expected>
#include <string>

namespace Abomination::Renderer
{
    // Loads the addresses of all OpenGL 4.6 functions from the graphics driver and checks that 4.6 is supported.
    // Requires a current OpenGL context. Must be called once, after the window is created and before any other
    // OpenGL call: until then every gl* function pointer is nullptr.
    [[nodiscard]] std::expected<void, std::string> LoadOpenGLFunctions();
}
