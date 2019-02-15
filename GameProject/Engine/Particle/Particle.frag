#version 420

layout(location = 0) out vec4 FragColour;

in vec4 fColour;

void main() {
	FragColour = fColour;
}