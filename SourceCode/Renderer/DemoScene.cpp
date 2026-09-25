#include "Renderer/DemoScene.h"

#include <glad/gl.h>
#include <glm/ext/matrix_transform.hpp>
#include <glm/geometric.hpp>
#include <glm/mat4x4.hpp>
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
        // One vertex as it lies in the vertex buffer: 3 floats of position, then 2 floats of texture coordinates.
        //   bytes:  0              12         20
        //           | x | y | z | u | v |
        struct TexturedVertex
        {
            glm::vec3 position;
            glm::vec2 texCoord;
        };

        // A cube of size 1 centered at the origin, in its own ("model") coordinates. Every face has its own 4 vertices:
        // a corner shared by 3 faces needs different texture coordinates on each of them, so it cannot be shared.
        // On every face the vertices go counter-clockwise when looked at from OUTSIDE the cube, starting at the
        // bottom-left corner: this is how OpenGL recognizes the front side of a triangle (see face culling in Draw).
        const std::array CubeVertices{
            // Front (+Z)
            TexturedVertex{.position = {-0.5f, -0.5f, 0.5f}, .texCoord = {0.0f, 0.0f}},
            TexturedVertex{.position = {0.5f, -0.5f, 0.5f}, .texCoord = {1.0f, 0.0f}},
            TexturedVertex{.position = {0.5f, 0.5f, 0.5f}, .texCoord = {1.0f, 1.0f}},
            TexturedVertex{.position = {-0.5f, 0.5f, 0.5f}, .texCoord = {0.0f, 1.0f}},
            // Back (-Z)
            TexturedVertex{.position = {0.5f, -0.5f, -0.5f}, .texCoord = {0.0f, 0.0f}},
            TexturedVertex{.position = {-0.5f, -0.5f, -0.5f}, .texCoord = {1.0f, 0.0f}},
            TexturedVertex{.position = {-0.5f, 0.5f, -0.5f}, .texCoord = {1.0f, 1.0f}},
            TexturedVertex{.position = {0.5f, 0.5f, -0.5f}, .texCoord = {0.0f, 1.0f}},
            // Right (+X)
            TexturedVertex{.position = {0.5f, -0.5f, 0.5f}, .texCoord = {0.0f, 0.0f}},
            TexturedVertex{.position = {0.5f, -0.5f, -0.5f}, .texCoord = {1.0f, 0.0f}},
            TexturedVertex{.position = {0.5f, 0.5f, -0.5f}, .texCoord = {1.0f, 1.0f}},
            TexturedVertex{.position = {0.5f, 0.5f, 0.5f}, .texCoord = {0.0f, 1.0f}},
            // Left (-X)
            TexturedVertex{.position = {-0.5f, -0.5f, -0.5f}, .texCoord = {0.0f, 0.0f}},
            TexturedVertex{.position = {-0.5f, -0.5f, 0.5f}, .texCoord = {1.0f, 0.0f}},
            TexturedVertex{.position = {-0.5f, 0.5f, 0.5f}, .texCoord = {1.0f, 1.0f}},
            TexturedVertex{.position = {-0.5f, 0.5f, -0.5f}, .texCoord = {0.0f, 1.0f}},
            // Top (+Y)
            TexturedVertex{.position = {-0.5f, 0.5f, 0.5f}, .texCoord = {0.0f, 0.0f}},
            TexturedVertex{.position = {0.5f, 0.5f, 0.5f}, .texCoord = {1.0f, 0.0f}},
            TexturedVertex{.position = {0.5f, 0.5f, -0.5f}, .texCoord = {1.0f, 1.0f}},
            TexturedVertex{.position = {-0.5f, 0.5f, -0.5f}, .texCoord = {0.0f, 1.0f}},
            // Bottom (-Y)
            TexturedVertex{.position = {-0.5f, -0.5f, -0.5f}, .texCoord = {0.0f, 0.0f}},
            TexturedVertex{.position = {0.5f, -0.5f, -0.5f}, .texCoord = {1.0f, 0.0f}},
            TexturedVertex{.position = {0.5f, -0.5f, 0.5f}, .texCoord = {1.0f, 1.0f}},
            TexturedVertex{.position = {-0.5f, -0.5f, 0.5f}, .texCoord = {0.0f, 1.0f}},
        };

        // Two triangles per face (6 faces x 6 indices). Face N uses vertices 4N .. 4N+3 in the same pattern as the quad:
        // (0, 1, 2) and (2, 3, 0), counter-clockwise.
        constexpr std::array<std::uint32_t, 36> CubeIndices{
            0,  1,  2,  2,  3,  0,  // Front
            4,  5,  6,  6,  7,  4,  // Back
            8,  9,  10, 10, 11, 8,  // Right
            12, 13, 14, 14, 15, 12, // Left
            16, 17, 18, 18, 19, 16, // Top
            20, 21, 22, 22, 23, 20, // Bottom
        };

        // Must match layout(location = N) in TexturedMesh.vert.
        constexpr std::uint32_t PositionAttribute = 0;
        constexpr std::uint32_t TexCoordAttribute = 1;
        constexpr std::uint32_t ModelUniform = 0;
        constexpr std::uint32_t ViewUniform = 1;
        constexpr std::uint32_t ProjectionUniform = 2;

        // The vertex array has only one vertex buffer, connected to binding slot 0.
        constexpr std::uint32_t VertexBufferBinding = 0;

        // Must match layout(binding = N) of uniAlbedoTexture in TexturedMesh.frag.
        constexpr std::uint32_t AlbedoTextureUnit = 0;

        // The cube turns around a tilted axis, so that its top, bottom and sides all come into view.
        constexpr float RotationSpeed = 0.8f; // Radians per second
        constexpr glm::vec3 RotationAxis{0.6f, 1.0f, 0.0f};
    }

    DemoScene DemoScene::Create(RenderAssets& assets)
    {
        // Loading cannot fail: a missing or broken file gives a fallback, so the scene is always created.
        const ShaderHandle shaderProgram = assets.shaders.Load("Shaders/TexturedMesh");
        const TextureHandle texture = assets.textures.Load("Textures/Crate.png");

        // Upload the vertices and the indices to the GPU once; from now on they live in video memory.
        GLBuffer vertexBuffer(std::as_bytes(std::span(CubeVertices)));
        GLBuffer indexBuffer(std::as_bytes(std::span(CubeIndices)));

        // Describe the layout of TexturedVertex and connect both buffers.
        GLVertexArray vertexArray;
        vertexArray.SetVertexBuffer(VertexBufferBinding, vertexBuffer, sizeof(TexturedVertex));
        vertexArray.SetFloatAttribute(PositionAttribute, VertexBufferBinding, 3, offsetof(TexturedVertex, position));
        vertexArray.SetFloatAttribute(TexCoordAttribute, VertexBufferBinding, 2, offsetof(TexturedVertex, texCoord));
        vertexArray.SetIndexBuffer(indexBuffer);

        return DemoScene(shaderProgram, texture, std::move(vertexBuffer), std::move(indexBuffer), std::move(vertexArray));
    }

    DemoScene::DemoScene(ShaderHandle shaderProgram, TextureHandle texture, GLBuffer vertexBuffer, GLBuffer indexBuffer,
                         GLVertexArray vertexArray) noexcept
        : m_shaderProgram(shaderProgram)
        , m_texture(texture)
        , m_vertexBuffer(std::move(vertexBuffer))
        , m_indexBuffer(std::move(indexBuffer))
        , m_vertexArray(std::move(vertexArray))
    {}

    void DemoScene::Draw(double time, const Camera& camera, int widthInPixels, int heightInPixels,
                         const RenderAssets& assets) const
    {
        // A minimized window has a height of 0: there is nothing to draw, and the aspect ratio would divide by zero.
        if (widthInPixels <= 0 || heightInPixels <= 0)
            return;

        // 1. Model matrix: rotate the cube by an angle that grows with time. glm::mat4(1.0f) is the identity matrix
        //    ("do nothing"); glm::rotate returns it multiplied by a rotation around the normalized axis.
        const float angle = static_cast<float>(time) * RotationSpeed;
        const glm::mat4 model = glm::rotate(glm::mat4(1.0f), angle, glm::normalize(RotationAxis));

        // 2. View and projection matrices come from the camera. The aspect ratio (width / height) of the window keeps
        //    a square a square in a wide window.
        const float aspectRatio = static_cast<float>(widthInPixels) / static_cast<float>(heightInPixels);

        // The handles are turned into the objects at the moment of use; the references are not kept (see AssetCache::Get).
        const GLShaderProgram& shaderProgram = assets.shaders.Get(m_shaderProgram);
        const GLTexture& texture = assets.textures.Get(m_texture);

        shaderProgram.SetUniform(ModelUniform, model);
        shaderProgram.SetUniform(ViewUniform, camera.GetViewMatrix());
        shaderProgram.SetUniform(ProjectionUniform, camera.GetProjectionMatrix(aspectRatio));

        // Depth test: for every pixel the depth buffer remembers how far the closest surface drawn there is.
        // A new pixel is drawn only if it is closer (GL_LESS, the default); otherwise it is hidden and thrown away.
        // Without it, faces drawn later would cover closer faces drawn earlier.
        glEnable(GL_DEPTH_TEST);

        // Face culling: triangles whose back side faces the camera are skipped before they reach the fragment shader.
        // The back faces of a closed object are never visible anyway, so this halves the work. OpenGL decides which side
        // is which by the order of the vertices on the screen: counter-clockwise is the front (GL_CCW, the default).
        glEnable(GL_CULL_FACE);

        shaderProgram.Use();
        texture.Bind(AlbedoTextureUnit);
        m_vertexArray.Bind();

        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(CubeIndices.size()), GL_UNSIGNED_INT, nullptr);
    }
}
