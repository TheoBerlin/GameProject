#version 420

layout(location = 0) in vec4 vertex;
layout(location = 1) in vec4 PosScale;

uniform mat4 vp;
uniform vec3 cameraRight;
uniform vec3 cameraUp;

out vec3 test;

void main() {
	vec3 vertexPos = PosScale.xyz + cameraRight * vertex.x + cameraUp * vertex.y;

	test = PosScale.xyz;

	gl_Position = vp * vec4(vertexPos, 1.0f);
}