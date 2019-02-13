#version 420

layout(location = 0) in vec4 vertex;
layout(location = 1) in vec4 PosScale;
layout(location = 2) in vec3 colour;

uniform mat4 vp;
uniform vec3 cameraRight;
uniform vec3 cameraUp;

out vec3 fColour;

void main() {
	vec3 vertexPos = PosScale.xyz + cameraRight * vertex.x * PosScale.w + cameraUp * vertex.y * PosScale.w;

	fColour = colour;
	gl_Position = vp * vec4(vertexPos, 1.0f);
}