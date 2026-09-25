#include "Renderer/GLBuffer.h"

#include <glad/gl.h>

#include <utility>

namespace Abomination::Renderer
{
    GLBuffer::GLBuffer(std::span<const std::byte> data)
    {
        // Direct State Access (OpenGL 4.5+): the buffer is created and filled by its ID.
        // The old way (LearnOpenGL) is glGenBuffers + glBindBuffer + glBufferData: the buffer had to be bound
        // to a global "binding point" first, and every following call silently worked on whatever was bound.
        glCreateBuffers(1, &m_bufferID);

        // Allocates the memory and copies the data in one call. The last argument (flags) is 0: the CPU will
        // neither read nor change the buffer later, so the driver is free to keep it in video memory only.
        glNamedBufferStorage(m_bufferID, static_cast<GLsizeiptr>(data.size()), data.data(), 0);
    }

    GLBuffer::GLBuffer(GLBuffer&& other) noexcept
        : m_bufferID(std::exchange(other.m_bufferID, 0))
    {}

    GLBuffer& GLBuffer::operator=(GLBuffer&& other) noexcept
    {
        if (this != &other)
        {
            // glDelete* functions ignore the ID 0, so a moved-from object needs no special check.
            glDeleteBuffers(1, &m_bufferID);
            m_bufferID = std::exchange(other.m_bufferID, 0);
        }

        return *this;
    }

    GLBuffer::~GLBuffer()
    {
        glDeleteBuffers(1, &m_bufferID);
    }

    std::uint32_t GLBuffer::GetID() const noexcept
    {
        return m_bufferID;
    }
}
