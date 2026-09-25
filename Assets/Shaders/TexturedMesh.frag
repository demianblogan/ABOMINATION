#version 460 core

// Runs once for every pixel: reads the color of the texture at this pixel's texture coordinates.

layout(location = 0) in vec2 TexCoord;

// A sampler is the shader's handle to a texture. binding = 0 means "read the texture connected to texture unit 0";
// GLTexture::Bind(0) connects a texture there. The sampler also applies the filtering and wrapping of that texture.
layout(binding = 0) uniform sampler2D uniAlbedoTexture;

layout(location = 0) out vec4 FragColor;

void main()
{
    // texture() finds the texel(s) at TexCoord, filters them and returns the color (red, green, blue, alpha).
    FragColor = texture(uniAlbedoTexture, TexCoord);
}
