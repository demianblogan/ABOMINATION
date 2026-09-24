#pragma once

#include <cstdint>
#include <expected>
#include <filesystem>
#include <string>
#include <string_view>

namespace Abomination::Renderer
{
    // An OpenGL shader program: a vertex shader and a fragment shader compiled and linked together.
    // The program is deleted in the destructor. Move-only: two objects must never delete the same program.
    class GLShaderProgram
    {
    public:
        // Compiles both shaders from source code and links them. debugName appears in error messages
        // and in graphics debuggers (RenderDoc, Nsight). On failure returns the compiler or linker log.
        [[nodiscard]] static std::expected<GLShaderProgram, std::string> Create(std::string_view vertexShaderSource,
                                                                                std::string_view fragmentShaderSource,
                                                                                std::string_view debugName);

        // Reads both shaders from files and calls Create(). The debug name is made of the file names.
        [[nodiscard]] static std::expected<GLShaderProgram, std::string> CreateFromFiles(
            const std::filesystem::path& vertexShaderPath, const std::filesystem::path& fragmentShaderPath);

        GLShaderProgram(const GLShaderProgram&) = delete;
        GLShaderProgram& operator=(const GLShaderProgram&) = delete;

        GLShaderProgram(GLShaderProgram&& other) noexcept;
        GLShaderProgram& operator=(GLShaderProgram&& other) noexcept;

        ~GLShaderProgram();

        // Makes this program the one that processes the next draw calls.
        void Use() const;

    private:
        explicit GLShaderProgram(std::uint32_t programID) noexcept;

        // The name OpenGL gave the program. 0 means "no program" (a moved-from object).
        std::uint32_t m_programID = 0;
    };
}
