#version 460 core

// Paints a surface gray, lighter or darker depending on where it faces. This is not lighting (that comes in 0.5):
// it only makes the floor, the ceiling and walls facing different ways look different, so the shape is readable.

layout(location = 0) in vec3 Normal;

layout(location = 0) out vec4 FragColor;

// A made-up direction the "light" comes from: from above, a little from the right and the front.
const vec3 LightDirection = normalize(vec3(0.4, 1.0, 0.6));
const vec3 SurfaceColor = vec3(0.75, 0.73, 0.70);

void main()
{
    // After interpolation between the vertices the normal is a little shorter than 1, so it is normalized again.
    vec3 normal = normalize(Normal);

    // dot() is 1 for a surface facing the light, -1 for one facing away. Mapping it from -1..1 to 0..1 ("half-Lambert")
    // gives every direction its own shade instead of making all surfaces facing away equally black; 0.35 is the
    // darkest shade, so no surface is black.
    float facing = dot(normal, LightDirection) * 0.5 + 0.5;
    float brightness = 0.35 + 0.65 * facing;

    FragColor = vec4(SurfaceColor * brightness, 1.0);
}
