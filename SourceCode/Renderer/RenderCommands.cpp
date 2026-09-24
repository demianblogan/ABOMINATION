#include "Renderer/RenderCommands.h"

#include <glad/glad.h>

namespace Abomination::Renderer
{
    void SetViewport(int widthInPixels, int heightInPixels)
    {
        glViewport(0, 0, widthInPixels, heightInPixels);
    }

    void ClearFrame(const glm::vec4& color)
    {
        // glClearColor only remembers the color in the context state; glClear fills the back buffer with it.
        // Only the color buffer is cleared: nothing uses depth yet.
        glClearColor(color.r, color.g, color.b, color.a);
        glClear(GL_COLOR_BUFFER_BIT);
    }
}
