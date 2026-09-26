#include "Renderer/DemoScene.h"

#include "Renderer/MeshPrimitives.h"

#include <glad/gl.h>
#include <glm/ext/matrix_transform.hpp>
#include <glm/geometric.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include <cstdint>
#include <optional>
#include <string>

namespace Abomination::Renderer
{
    namespace
    {
        // Must match layout(location = N) of the uniforms in TexturedMesh.vert.
        constexpr std::uint32_t ModelUniform = 0;
        constexpr std::uint32_t ViewUniform = 1;
        constexpr std::uint32_t ProjectionUniform = 2;

        // Must match layout(binding = N) of uniAlbedoTexture in TexturedMesh.frag.
        constexpr std::uint32_t AlbedoTextureUnit = 0;

        // The cube turns around a tilted axis, so that its top, bottom and sides all come into view.
        constexpr float RotationSpeed = 0.8f; // Radians per second
        constexpr glm::vec3 RotationAxis{0.6f, 1.0f, 0.0f};

        const std::string CubeMeshName = "Primitives/Cube";
    }

    DemoScene DemoScene::Create(RenderAssets& assets)
    {
        // Loading cannot fail: a missing or broken file gives a fallback, so the scene is always created.
        const ShaderHandle shaderProgram = assets.shaders.Load("Shaders/TexturedMesh");
        const TextureHandle texture = assets.textures.Load("Textures/Crate.png");

        // The cube is built only once: if another object already added it, its handle is reused.
        const std::optional<MeshHandle> existingMesh = assets.meshes.Find(CubeMeshName);
        const MeshHandle mesh =
            existingMesh.has_value() ? *existingMesh : assets.meshes.Add(CubeMeshName, CreateCubeMeshData());

        return DemoScene(shaderProgram, texture, mesh);
    }

    DemoScene::DemoScene(ShaderHandle shaderProgram, TextureHandle texture, MeshHandle mesh) noexcept
        : m_shaderProgram(shaderProgram)
        , m_texture(texture)
        , m_mesh(mesh)
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
        const Mesh& mesh = assets.meshes.Get(m_mesh);

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
        mesh.Draw();
    }
}
