#version 420

out vec4 FragColour;

in vec4 fColour;

void main() {
	FragColour = fColour;
}