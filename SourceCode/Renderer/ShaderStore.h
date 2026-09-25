#pragma once

#include "Core/AssetCache.h"
#include "Core/AssetHandle.h"
#include "Renderer/GLShaderProgram.h"

#include <expected>
#include <filesystem>
#include <string>

namespace Abomination::Renderer
{
    using ShaderHandle = Core::AssetHandle<GLShaderProgram>;

    // Loads shader programs and keeps every program exactly once. A program is named by the path of its two files
    // without the extension: "Shaders/TexturedMesh" is Shaders/TexturedMesh.vert + Shaders/TexturedMesh.frag.
    //
    // A missing file or a compilation error does not stop the game: the program is replaced by a fallback program that
    // draws everything in plain magenta, and the error (with the compiler log) is logged. The fallback is stored under
    // the name of the broken program, so it is compiled and reported only once.
    //
    // The fallback program expects what every program of the game provides: the vertex position at location 0 and
    // the model, view and projection matrices at uniform locations 0, 1 and 2.
    //
    // Requires a current OpenGL context. Move-only.
    class ShaderStore
    {
    public:
        // assetsDirectory: the folder all shader names are relative to. Fails only if the built-in fallback program
        // cannot be compiled, which means the OpenGL driver is broken.
        [[nodiscard]] static std::expected<ShaderStore, std::string> Create(std::filesystem::path assetsDirectory);

        // Returns the program with this name, loading it on the first call. The same name always gives the same handle.
        [[nodiscard]] ShaderHandle Load(const std::string& name);

        // The program of the handle. An invalid handle gives the fallback program.
        [[nodiscard]] const GLShaderProgram& Get(ShaderHandle handle) const;

    private:
        ShaderStore(std::filesystem::path assetsDirectory, GLShaderProgram fallbackProgram) noexcept;

        std::filesystem::path m_assetsDirectory;
        Core::AssetCache<GLShaderProgram> m_cache;

        // Returned by Get() for invalid handles.
        GLShaderProgram m_fallbackProgram;
    };
}
