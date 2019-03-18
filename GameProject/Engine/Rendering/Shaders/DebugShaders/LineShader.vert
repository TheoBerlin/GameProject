#version 420

layout(location = 0) in vec3 position;

uniform mat4 vp;
uniform vec3 color;

out vec3 fragColor;

void main()
{
    fragColor = color;
    gl_Position = vp * vec4(position, 1.0);
}