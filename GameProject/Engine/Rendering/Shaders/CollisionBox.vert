#version 420

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in mat4 transform;

uniform mat4 vp;

out vec3 fragColor;

void main()
{
    fragColor = color;
    gl_Position = vp * transform * vec4(position, 1.0);
}