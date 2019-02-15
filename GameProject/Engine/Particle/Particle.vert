#version 420

layout(location = 4) in vec4 vertex;
layout(location = 5) in vec4 PosScale;
layout(location = 6) in vec4 colour;

uniform mat4 vp;
uniform vec3 cameraRight;
uniform vec3 cameraUp;

out vec4 fColour;

void main() {
	vec3 vertexPos = PosScale.xyz + cameraRight * vertex.x * PosScale.w + cameraUp * vertex.y * PosScale.w;

	fColour = colour;
	gl_Position = vp * vec4(vertexPos, 1.0f);
}