#version 420

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;
layout(location = 3) in mat4 transform;

uniform mat4 vp;

out vec3 fragPos;

void main()
{
    fragPos = (transform * vec4(position, 1.0)).xyz;
    gl_Position = vp * transform * vec4(position, 1.0);
}