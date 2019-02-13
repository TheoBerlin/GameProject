#version 420

layout(location = 0) in vec4 vertex;
layout(location = 1) in vec4 PosScale;

uniform mat4 vp;
uniform vec3 cameraRight;
uniform vec3 cameraUp;

void main() {
	vec3 vertexPos = PosScale.xyz + cameraRight * vertex.x * PosScale.w + cameraUp * vertex.y * PosScale.w;

	gl_Position = vp * vec4(vertexPos, 1.0f);
}