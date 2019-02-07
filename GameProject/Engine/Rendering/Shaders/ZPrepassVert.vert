#version 420

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;

uniform mat4 vp;
uniform mat4 transform;

void main()
{
    gl_Position = vp * transform * vec4(position, 1.0); 
}