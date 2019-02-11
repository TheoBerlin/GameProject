#version 420

layout (location = 0) in vec4 aPosScale

uniform mat4 vp;
uniform int nrOfParticles;


void main() {
	gl_Position = vp * model;
}