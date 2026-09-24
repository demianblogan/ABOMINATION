#include "Renderer/DemoScene.h"

#include <glad/gl.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>
#include <utility>

namespace Abomination::Renderer
{
    namespace
    {
        // One vertex as it lies in the vertex buffer: 2 floats of position, then 3 floats of color, 20 bytes in total.
        //   bytes:  0        8                  20
        //           | x | y | r | g | b |
        struct ColoredVertex
        {
            glm::vec2 position;
            glm::vec3 color;
        };

        // Positions in normalized device coordinates: the window spans from -1 to 1 on both axes.
        // The vertices go counter-clockwise, which OpenGL treats as the front side of the triangle.
        const std::array TriangleVertices{
            ColoredVertex{.position = {0.0f, 0.6f}, .color = {1.0f, 0.2f, 0.2f}},   // Top, red
            ColoredVertex{.position = {-0.6f, -0.5f}, .color = {0.2f, 1.0f, 0.2f}}, // Bottom left, green
            ColoredVertex{.position = {0.6f, -0.5f}, .color = {0.2f, 0.4f, 1.0f}},  // Bottom right, blue
        };

        // Must match layout(location = N) in ColoredVertex.vert.
        constexpr std::uint32_t PositionAttribute = 0;
        constexpr std::uint32_t ColorAttribute = 1;

        // The vertex array has only one buffer, connected to binding slot 0.
        constexpr std::uint32_t VertexBufferBinding = 0;
    }

    std::expected<DemoScene, std::string> DemoScene::Create(const std::filesystem::path& assetsDirectory)
    {
        const std::filesystem::path shadersDirectory = assetsDirectory / "Shaders";
        std::expected<GLShaderProgram, std::string> shaderProgram =
            GLShaderProgram::CreateFromFiles(shadersDirectory / "ColoredVertex.vert", shadersDirectory / "ColoredVertex.frag");
        if (!shaderProgram.has_value())
            return std::unexpected(shaderProgram.error());

        // Upload the vertices to the GPU once; from now on they live in video memory.
        GLBuffer vertexBuffer(std::as_bytes(std::span(TriangleVertices)));

        // Describe the layout of ColoredVertex to OpenGL: where each shader input is inside one vertex.
        GLVertexArray vertexArray;
        vertexArray.SetVertexBuffer(VertexBufferBinding, vertexBuffer, sizeof(ColoredVertex));
        vertexArray.SetFloatAttribute(PositionAttribute, VertexBufferBinding, 2, offsetof(ColoredVertex, position));
        vertexArray.SetFloatAttribute(ColorAttribute, VertexBufferBinding, 3, offsetof(ColoredVertex, color));

        return DemoScene(std::move(*shaderProgram), std::move(vertexBuffer), std::move(vertexArray));
    }

    DemoScene::DemoScene(GLShaderProgram shaderProgram, GLBuffer vertexBuffer, GLVertexArray vertexArray) noexcept
        : m_shaderProgram(std::move(shaderProgram))
        , m_vertexBuffer(std::move(vertexBuffer))
        , m_vertexArray(std::move(vertexArray))
    {}

    void DemoScene::Draw() const
    {
        // Which program processes the vertices and pixels, and which vertex array the vertices come through.
        m_shaderProgram.Use();
        m_vertexArray.Bind();

        // The draw call: take 3 vertices starting from vertex 0 and build triangles from every 3 of them.
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(TriangleVertices.size()));
    }
}
