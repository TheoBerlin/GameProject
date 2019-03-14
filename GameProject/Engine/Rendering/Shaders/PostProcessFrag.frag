#version 420

layout (location = 0) out vec4 colorAttachment0;

uniform sampler2D tex;


in vec2 fragUV;

void main()
{   
	vec4 fragColor = texture2D(tex, fragUV);

    colorAttachment0 = fragColor;
}