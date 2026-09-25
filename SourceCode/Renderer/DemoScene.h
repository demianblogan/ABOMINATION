#pragma once

#include "Renderer/Camera.h"
#include "Renderer/GLBuffer.h"
#include "Renderer/GLVertexArray.h"
#include "Renderer/RenderAssets.h"

namespace Abomination::Renderer
{
    // A temporary scene for milestone 0.1 to learn the OpenGL basics: a rotating textured cube.
    // It will be replaced by the real renderer in milestone 0.2. Move-only.
    class DemoScene
    {
    public:
        // Loads the shader program and the texture through the asset stores and uploads the geometry to the GPU.
        [[nodiscard]] static DemoScene Create(RenderAssets& assets);

        // Draws the scene into the current frame as seen through the camera. time (in seconds) drives the rotation;
        // the size of the drawable area gives the aspect ratio for the perspective projection. The assets give the
        // texture and the program of the handles the scene keeps.
        void Draw(double time, const Camera& camera, int widthInPixels, int heightInPixels, const RenderAssets& assets) const;

    private:
        DemoScene(ShaderHandle shaderProgram, TextureHandle texture, GLBuffer vertexBuffer, GLBuffer indexBuffer,
                  GLVertexArray vertexArray) noexcept;

        // Only handles: the program and the texture themselves belong to the stores.
        ShaderHandle m_shaderProgram;
        TextureHandle m_texture;

        // The vertex array only refers to the buffers, so it is declared after them and destroyed before them.
        GLBuffer m_vertexBuffer;
        GLBuffer m_indexBuffer;
        GLVertexArray m_vertexArray;
    };
}
