#version 420

out vec4 finalColor;

uniform sampler2D tex;
uniform sampler2D particles;

in vec2 fragUV;

void main()
{    
	vec4 fragColor = texture2D(tex, fragUV);
	vec4 particleColor = texture2D(particles, fragUV);

	vec4 combineColor = min(fragColor + particleColor, 1.0);
    finalColor = vec4(combineColor.rgb, 1.0f);
}