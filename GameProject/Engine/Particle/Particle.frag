#version 420

out vec4 FragColor;

in vec3 test;

void main() {
	FragColor = vec4(1.0f, -test.z, 0.0f, 1.0f);
}