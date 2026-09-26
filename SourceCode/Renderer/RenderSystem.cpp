#include "Renderer/RenderSystem.h"

#include "Core/Transform.h"
#include "Renderer/MeshRenderer.h"

#include <glad/gl.h>

#include <cstdint>

namespace Abomination::Renderer
{
    namespace
    {
        // Must match layout(location = N) of the uniforms in TexturedMesh.vert (and the fallback program).
        constexpr std::uint32_t ModelUniform = 0;
        constexpr std::uint32_t ViewUniform = 1;
        constexpr std::uint32_t ProjectionUniform = 2;

        // Must match layout(binding = N) of uniAlbedoTexture in TexturedMesh.frag.
        constexpr std::uint32_t AlbedoTextureUnit = 0;
    }

    void DrawMeshes(const entt::registry& registry, const Camera& camera, float aspectRatio, const RenderAssets& assets)
    {
        // Depth test: for every pixel the depth buffer remembers how far the closest surface drawn there is.
        // A new pixel is drawn only if it is closer (GL_LESS, the default); otherwise it is hidden and thrown away.
        // Without it, objects drawn later would cover closer objects drawn earlier.
        glEnable(GL_DEPTH_TEST);

        // Face culling: triangles whose back side faces the camera are skipped before they reach the fragment shader.
        // The back faces of a closed object are never visible anyway, so this halves the work. OpenGL decides which side
        // is which by the order of the vertices on the screen: counter-clockwise is the front (GL_CCW, the default).
        glEnable(GL_CULL_FACE);

        // The same for every entity in the frame, so calculated once.
        const glm::mat4 view = camera.GetViewMatrix();
        const glm::mat4 projection = camera.GetProjectionMatrix(aspectRatio);

        // A view: all entities that have both components (const: this system only reads them). each() calls the
        // function for every such entity with references to its components; the entity number itself is not needed
        // here, and EnTT passes it only to functions that ask for it as the first parameter.
        const auto meshEntities = registry.view<const Core::Transform, const MeshRenderer>();
        meshEntities.each([&](const Core::Transform& transform, const MeshRenderer& meshRenderer)
        {
            // The handles are turned into objects at the moment of use (see AssetCache::Get).
            const GLShaderProgram& shaderProgram = assets.shaders.Get(meshRenderer.shaderProgram);
            const GLTexture& texture = assets.textures.Get(meshRenderer.texture);
            const Mesh& mesh = assets.meshes.Get(meshRenderer.mesh);

            // Every entity binds its program and texture again, even if the previous one used the same. That is fine
            // for a few dozen objects; sorting draws by program and texture (batching) comes when there are hundreds.
            shaderProgram.Use();
            shaderProgram.SetUniform(ModelUniform, Core::CalculateModelMatrix(transform));
            shaderProgram.SetUniform(ViewUniform, view);
            shaderProgram.SetUniform(ProjectionUniform, projection);
            texture.Bind(AlbedoTextureUnit);
            mesh.Draw();
        });
    }
}
