#version 420

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;
layout(location = 3) in mat4 transform;

uniform mat4 vp;

out vec3 fragPos;
out vec3 fragNormal;
out vec2 fragUv;

void main()
{
    fragNormal = (transform * vec4(normal, 0.0)).xyz;
    fragUv = uv;
    fragPos = (transform * vec4(position, 1.0)).xyz; 
    gl_Position = vp * transform * vec4(position, 1.0);
}