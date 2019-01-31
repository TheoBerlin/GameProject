#version 330

in vec3 fragPos;
in vec3 fragNormal;
in mat4 fragWorld;

layout(std140) uniform Material 
{
	vec4 ambientColor;
	vec4 diffuseColor;
	vec4 specularColor;
} material;

uniform vec3 camPos;
uniform vec3 emissionColor;

out vec4 finalColor;

const vec3 lightPos = vec3(0.0, 5.0, 1.0);
const float intensity = 10.0;

const float epsilon = 0.0001;

float isZero(in vec3 v)
{
	vec3 u = step(-epsilon, v) * (1.0 - step(epsilon, v));
	return 1.0 - u.x*u.y*u.z;
}

void main()
{
	vec3 normal;
	if(isZero(fragNormal) == 0.0)
		normal = normalize((fragWorld*vec4(cross(dFdx(fragPos), dFdy(fragPos)), 0.0)).xyz);
	else
		normal = normalize(fragNormal);
	
	vec3 materialColor = vec3(1.0, 1.0, 1.0);
	vec3 ambientColor = material.ambientColor.xyz*materialColor;

	vec3 fragToLight = camPos - fragPos;
	float lightFactor = intensity * min(1.0 / length(fragToLight)*length(fragToLight), 1.0);

	vec3 lightDirectionInv = normalize(fragToLight);
	float diffuseFactor = max(0.0, dot(normal, lightDirectionInv));
	vec3 diffuseColor = diffuseFactor*material.diffuseColor.xyz*materialColor*lightFactor;

	float s = material.specularColor.w;
	vec3 r = reflect(normal, lightDirectionInv);
	vec3 v = normalize(camPos - fragPos);
	float specularFactor = pow(max(dot(r, v), 0.0), s+0.01);
	vec3 specularColor = specularFactor*material.specularColor.xyz*materialColor*lightFactor;

	finalColor = clamp(vec4(ambientColor + diffuseColor + specularColor + emissionColor, 1.0), 0.0, 1.0);
}