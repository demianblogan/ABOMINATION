#pragma once

#include "Renderer/Camera.h"
#include "Renderer/RenderAssets.h"

namespace Abomination::Renderer
{
    // A temporary scene for milestone 0.1 to learn the OpenGL basics: a rotating textured cube.
    // It will be replaced by the real renderer in milestone 0.2. Move-only.
    class DemoScene
    {
    public:
        // Loads the shader program, the texture and the cube mesh through the asset stores.
        [[nodiscard]] static DemoScene Create(RenderAssets& assets);

        // Draws the scene into the current frame as seen through the camera. time (in seconds) drives the rotation;
        // the size of the drawable area gives the aspect ratio for the perspective projection. The assets give the
        // program, the texture and the mesh of the handles the scene keeps.
        void Draw(double time, const Camera& camera, int widthInPixels, int heightInPixels, const RenderAssets& assets) const;

    private:
        DemoScene(ShaderHandle shaderProgram, TextureHandle texture, MeshHandle mesh) noexcept;

        // Only handles: the program, the texture and the mesh themselves belong to the stores.
        ShaderHandle m_shaderProgram;
        TextureHandle m_texture;
        MeshHandle m_mesh;
    };
}
