#include "Renderer/OpenGLLoader.h"

#include "Core/Log.h"
#include "Platform/Window.h"

#include <glad/glad.h>

#include <format>
#include <string_view>

namespace Abomination::Renderer
{
    using Core::LogCategory;
    using Core::LogLevel;

    namespace
    {
        // glGetString returns the text as unsigned bytes; std::format and the log work with char.
        std::string_view GetOpenGLString(GLenum name)
        {
            const GLubyte* text = glGetString(name);
            if (text == nullptr)
                return "unknown";

            return reinterpret_cast<const char*>(text);
        }
    }

    std::expected<void, std::string> LoadOpenGLFunctions()
    {
        // glad asks the driver for the address of every OpenGL function through our Platform function
        // and stores it in its own pointer: glClear, glCreateBuffers, ... become callable after this line.
        if (gladLoadGLLoader(Platform::GetOpenGLFunctionAddress) == 0)
            return std::unexpected("Failed to load OpenGL functions");

        // The version of the context that was actually created, filled in by glad.
        if (GLAD_GL_VERSION_4_6 == 0)
        {
            return std::unexpected(std::format("OpenGL 4.6 is required, but the graphics driver provides only {}.{}",
                                               GLVersion.major, GLVersion.minor));
        }

        Core::Log::Write(LogCategory::Renderer, LogLevel::Info, "OpenGL {}.{} loaded", GLVersion.major, GLVersion.minor);
        Core::Log::Write(LogCategory::Renderer, LogLevel::Info, "GPU: {} ({})", GetOpenGLString(GL_RENDERER),
                         GetOpenGLString(GL_VENDOR));
        Core::Log::Write(LogCategory::Renderer, LogLevel::Info, "Driver: {}", GetOpenGLString(GL_VERSION));
        Core::Log::Write(LogCategory::Renderer, LogLevel::Info, "GLSL: {}", GetOpenGLString(GL_SHADING_LANGUAGE_VERSION));

        return {};
    }
}
