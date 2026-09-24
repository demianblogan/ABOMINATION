#pragma once

#include "Renderer/GLBuffer.h"
#include "Renderer/GLShaderProgram.h"
#include "Renderer/GLVertexArray.h"

#include <expected>
#include <filesystem>
#include <string>

namespace Abomination::Renderer
{
    // A temporary scene for milestone 0.1 to learn the OpenGL basics step by step:
    // a colored triangle now, a textured quad and a rotating textured cube later.
    // It will be replaced by the real renderer in milestone 0.2. Move-only.
    class DemoScene
    {
    public:
        // Loads the shaders from assetsDirectory/Shaders and uploads the geometry to the GPU.
        [[nodiscard]] static std::expected<DemoScene, std::string> Create(const std::filesystem::path& assetsDirectory);

        // Draws the scene into the current frame.
        void Draw() const;

    private:
        DemoScene(GLShaderProgram shaderProgram, GLBuffer vertexBuffer, GLVertexArray vertexArray) noexcept;

        GLShaderProgram m_shaderProgram;

        // The vertex array only refers to the buffer, so it is declared after it and destroyed before it.
        GLBuffer m_vertexBuffer;
        GLVertexArray m_vertexArray;
    };
}
