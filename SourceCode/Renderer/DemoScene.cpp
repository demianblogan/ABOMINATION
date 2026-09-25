#include "Renderer/DemoScene.h"

#include <glad/gl.h>
#include <glm/vec2.hpp>

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>
#include <utility>

namespace Abomination::Renderer
{
    namespace
    {
        // One vertex as it lies in the vertex buffer: 2 floats of position, then 2 floats of texture coordinates.
        //   bytes:  0         8          16
        //           | x | y | u | v |
        struct TexturedVertex
        {
            glm::vec2 position;
            glm::vec2 texCoord;
        };

        // The four corners of the quad. Positions are normalized device coordinates (-1 to 1 across the window);
        // texture coordinates put the corners of the texture onto the corners of the quad.
        //
        //   3 (-0.5, 0.5) UV (0,1) ---- 2 (0.5, 0.5) UV (1,1)
        //        |                          |
        //   0 (-0.5,-0.5) UV (0,0) ---- 1 (0.5,-0.5) UV (1,0)
        const std::array QuadVertices{
            TexturedVertex{.position = {-0.5f, -0.5f}, .texCoord = {0.0f, 0.0f}}, // 0: bottom left
            TexturedVertex{.position = {0.5f, -0.5f}, .texCoord = {1.0f, 0.0f}},  // 1: bottom right
            TexturedVertex{.position = {0.5f, 0.5f}, .texCoord = {1.0f, 1.0f}},   // 2: top right
            TexturedVertex{.position = {-0.5f, 0.5f}, .texCoord = {0.0f, 1.0f}},  // 3: top left
        };

        // Two triangles made of the four vertices above, both counter-clockwise. Without indices the quad would need
        // 6 vertices, with vertices 0 and 2 stored twice; a cube with indices needs 24 vertices instead of 36.
        constexpr std::array<std::uint32_t, 6> QuadIndices{
            0, 1, 2, // Bottom-right triangle
            2, 3, 0, // Top-left triangle
        };

        // Must match layout(location = N) in TexturedMesh.vert.
        constexpr std::uint32_t PositionAttribute = 0;
        constexpr std::uint32_t TexCoordAttribute = 1;

        // The vertex array has only one vertex buffer, connected to binding slot 0.
        constexpr std::uint32_t VertexBufferBinding = 0;

        // Must match layout(binding = N) of uniAlbedoTexture in TexturedMesh.frag.
        constexpr std::uint32_t AlbedoTextureUnit = 0;
    }

    std::expected<DemoScene, std::string> DemoScene::Create(const std::filesystem::path& assetsDirectory)
    {
        const std::filesystem::path shadersDirectory = assetsDirectory / "Shaders";
        std::expected<GLShaderProgram, std::string> shaderProgram =
            GLShaderProgram::CreateFromFiles(shadersDirectory / "TexturedMesh.vert", shadersDirectory / "TexturedMesh.frag");
        if (!shaderProgram.has_value())
            return std::unexpected(shaderProgram.error());

        std::expected<GLTexture, std::string> texture =
            GLTexture::CreateFromFile(assetsDirectory / "Textures" / "TestPattern.png");
        if (!texture.has_value())
            return std::unexpected(texture.error());

        // Upload the vertices and the indices to the GPU once; from now on they live in video memory.
        GLBuffer vertexBuffer(std::as_bytes(std::span(QuadVertices)));
        GLBuffer indexBuffer(std::as_bytes(std::span(QuadIndices)));

        // Describe the layout of TexturedVertex and connect both buffers.
        GLVertexArray vertexArray;
        vertexArray.SetVertexBuffer(VertexBufferBinding, vertexBuffer, sizeof(TexturedVertex));
        vertexArray.SetFloatAttribute(PositionAttribute, VertexBufferBinding, 2, offsetof(TexturedVertex, position));
        vertexArray.SetFloatAttribute(TexCoordAttribute, VertexBufferBinding, 2, offsetof(TexturedVertex, texCoord));
        vertexArray.SetIndexBuffer(indexBuffer);

        return DemoScene(std::move(*shaderProgram), std::move(*texture), std::move(vertexBuffer), std::move(indexBuffer),
                         std::move(vertexArray));
    }

    DemoScene::DemoScene(GLShaderProgram shaderProgram, GLTexture texture, GLBuffer vertexBuffer, GLBuffer indexBuffer,
                         GLVertexArray vertexArray) noexcept
        : m_shaderProgram(std::move(shaderProgram))
        , m_texture(std::move(texture))
        , m_vertexBuffer(std::move(vertexBuffer))
        , m_indexBuffer(std::move(indexBuffer))
        , m_vertexArray(std::move(vertexArray))
    {}

    void DemoScene::Draw() const
    {
        m_shaderProgram.Use();
        m_texture.Bind(AlbedoTextureUnit);
        m_vertexArray.Bind();

        // Draw triangles using the index buffer: take 6 indices of type uint32 from its beginning (offset nullptr = 0)
        // and fetch the vertices they point to.
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(QuadIndices.size()), GL_UNSIGNED_INT, nullptr);
    }
}
