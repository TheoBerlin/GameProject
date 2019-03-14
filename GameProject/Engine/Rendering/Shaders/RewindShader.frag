#version 420

layout (location = 0) out vec4 colorAttachment0;

uniform sampler2D tex;
uniform sampler2D rewindTex;

in vec2 fragUV;

void main()
{     
    vec3 color = texture2D(rewindTex, fragUV).rgb;  
     
    vec3 result = texture2D(tex, fragUV).rgb;
    colorAttachment0 = vec4(result * color * vec3(0.5, 0.5, 1.0), 1.0);
}