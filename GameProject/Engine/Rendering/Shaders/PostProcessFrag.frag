#version 420

out vec4 finalColor;

uniform sampler2D tex;

in vec2 fragUV;

void main()
{   
	vec4 fragColor = texture2D(tex, vec2(fragUV.x, 1.0 - fragUV.y));
    //float backCol = step((fragColor.r + fragColor.g + fragColor.b), 0.01);
    finalColor = fragColor; // + backCol * vec4(fragUV.x, 1.0 - fragUV.y, 0.0, 1.0)

}