#version 420

out vec4 finalColor;

uniform sampler2D tex;
uniform sampler2D particles;

in vec2 fragUV;

void main()
{    
	vec4 fragColor = texture2D(tex, fragUV);
	vec4 particleColor = texture2D(particles, fragUV);
    //float backCol = step((fragColor.r + fragColor.g + fragColor.b), 0.01);
    finalColor = vec4(fragColor.rgb * (1 - particleColor.a) + particleColor.rgb * particleColor.a, 1.0f); // + backCol * vec4(fragUV.x, 1.0 - fragUV.y, 0.0, 1.0)
}