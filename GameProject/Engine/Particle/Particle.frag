#version 420

layout (location = 0) out vec4 colorAttachment0;

in vec4 fColour;

void main() {
	colorAttachment0 = fColour;
}