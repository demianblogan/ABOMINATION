#pragma once

#include "Renderer/GLBuffer.h"
#include "Renderer/GLShaderProgram.h"
#include "Renderer/GLTexture.h"
#include "Renderer/GLVertexArray.h"

#include <expected>
#include <filesystem>
#include <string>

namespace Abomination::Renderer
{
    // A temporary scene for milestone 0.1 to learn the OpenGL basics: a rotating textured cube.
    // It will be replaced by the real renderer in milestone 0.2. Move-only.
    class DemoScene
    {
    public:
        // Loads the shaders and the texture from assetsDirectory and uploads the geometry to the GPU.
        [[nodiscard]] static std::expected<DemoScene, std::string> Create(const std::filesystem::path& assetsDirectory);

        // Draws the scene into the current frame. time (in seconds) drives the rotation; the size of the drawable
        // area gives the aspect ratio for the perspective projection.
        void Draw(double time, int widthInPixels, int heightInPixels) const;

    private:
        DemoScene(GLShaderProgram shaderProgram, GLTexture texture, GLBuffer vertexBuffer, GLBuffer indexBuffer,
                  GLVertexArray vertexArray) noexcept;

        GLShaderProgram m_shaderProgram;
        GLTexture m_texture;

        // The vertex array only refers to the buffers, so it is declared after them and destroyed before them.
        GLBuffer m_vertexBuffer;
        GLBuffer m_indexBuffer;
        GLVertexArray m_vertexArray;
    };
}
