#version 420

layout (location = 0) out vec4 colorAttachment0;
layout (location = 1) out vec4 colorAttachment1;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;
layout(location = 3) in mat4 transform;
layout(location = 7) in vec3 color;

uniform mat4 vp;
uniform mat4 lightMatrix;

out vec3 droneColor;
out vec3 fragPos;
out vec3 fragNormal;
out vec2 fragUv;
out vec4 fragLightPos;

void main()
{
    droneColor = color;
    mat4 m = transpose(inverse(transform));
    fragNormal = (m * vec4(normal, 0.0)).xyz;
    fragUv = uv;
    fragPos = (transform * vec4(position, 1.0)).xyz;
    fragLightPos = lightMatrix * transform * vec4(position, 1.0);
    gl_Position = vp * transform * vec4(position, 1.0);
}