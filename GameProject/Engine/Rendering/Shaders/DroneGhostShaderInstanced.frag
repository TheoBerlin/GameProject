#version 420

layout (location = 0) out vec4 colorAttachment0;
layout (location = 1) out vec4 colorAttachment1;

in vec3 droneColor;
in vec3 fragNormal;
in vec2 fragUv;
in vec4 fragPos;
in mat4 fragVP;

uniform sampler2D tex;
uniform sampler2D noiseTex;
uniform sampler2D screenTex;
uniform vec3 camPos;
uniform float time;
uniform bool isGlowing;

void main()
{
    vec3 viewDir = normalize(camPos - fragPos.xyz);
    vec3 texColor = min(texture2D(tex, fragUv).rgb, 1.0);

    vec2 uv = fragUv;
    uv *= 10.0;
    uv.x += mod(time * 0.1, 1.0);
    vec3 noise = texture2D(noiseTex, uv).rgb;

    float a = max(dot(normalize(viewDir), normalize(fragNormal)), 0.0);

    // perform perspective divide
    const float distortionStrength = 1.0;

    vec4 distortPoint = fragPos + distortionStrength*normalize(fragNormal);
    distortPoint.w = 1.0;
    vec4 fragPosVP = fragVP * distortPoint;
    vec3 projCoords = fragPosVP.xyz / fragPosVP.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;

    float viewLength = length(camPos - fragPos.xyz);
    float opacity = smoothstep(5.0, 7.0, viewLength);
    vec4 preColor = texture(screenTex, projCoords.xy);
    preColor.rgb = preColor.rgb;

    colorAttachment0 = vec4(preColor.rgb, (1.0-a)*opacity);
    colorAttachment0 += vec4(noise*droneColor, 1.0-a)*droneColor.r;
    
    if(isGlowing) {
        colorAttachment1 = vec4(texColor, 1.0);
        return;
     }
     colorAttachment1 = vec4(0.0, 0.0, 0.0, 0.0);
}