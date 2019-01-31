#version 330

layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec3 vertNormal;

out vec3 fragPos;
out vec3 fragNormal;
out mat4 fragWorld;

uniform mat4 matrix;
uniform mat4 vp;

void main()
{
	fragWorld = matrix;
	gl_Position = vp*matrix*vec4(vertPos, 1.0);
	fragPos = (matrix*vec4(vertPos, 1.0)).xyz;
	fragNormal = (matrix*vec4(vertNormal, 0.0)).xyz;
}