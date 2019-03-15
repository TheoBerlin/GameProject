#version 420

layout (location = 0) out vec4 colorAttachment0;

uniform sampler2D tex;


in vec2 fragUV;

void main()
{         
    vec3 result = texture2D(tex, fragUV).rgb;

    
    float mean = length(result) / 1.5;
    colorAttachment0 = vec4(mean,mean,mean, 1.0);
}