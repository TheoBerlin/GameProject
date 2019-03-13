#version 420

uniform vec3 cameraPos;

in vec3 fragColor;
in vec3 fragPos;

out vec4 finalColor;

in float fragAlpha;

void main()
{
    float diff = length(cameraPos - fragPos);
    float alphaFactor = smoothstep(0.5, 1.0, diff);
    finalColor = vec4(fragColor, fragAlpha * alphaFactor);
}