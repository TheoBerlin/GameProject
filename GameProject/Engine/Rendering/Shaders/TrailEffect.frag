#version 420

layout (location = 0) out vec4 colorAttachment0;
layout (location = 1) out vec4 colorAttachment1;

uniform vec3 cameraPos;

in vec3 fragColor;
in vec3 fragPos;

in float fragAlpha;

void main()
{
    float diff = length(cameraPos - fragPos);
    float alphaFactor = smoothstep(0.5, 1.0, diff);
    vec4 finalColor = vec4(fragColor, fragAlpha * alphaFactor);

    colorAttachment0 = finalColor;
    colorAttachment1 = finalColor;
}