#version 420

in vec2 fragScale;
in vec3 fragNormal;
in vec2 fragUv;
in vec3 fragPos;

layout(std140) uniform WallPoints
{
    vec4 points[96];
    int size;
    int groupSize;
} wallPoints;

layout(std140) uniform Material
{
    vec4 kd;
    vec4 ks_f;
} mat;

out vec4 finalColor;

uniform sampler2D tex;
uniform vec3 camPos;

float GetMinDist(vec3 p, int size, int index)
{
    float minDist = 10000.0;
    int j = index + size - 1;
    for(int i = index; i < size+index; i++) {
        vec3 w2 = wallPoints.points[i].xyz;
        vec3 w1 = wallPoints.points[j].xyz;
        vec3 l = w2 - w1;
        // Make this the normal of the line.
        float x = l.x;
        l.x = -l.z;
		l.y = w2.y;
        l.z = x;
		l = normalize(l);

		vec3 v2 = p-w2;
		vec3 v1 = p-w1;

        // Calculate smallest distance to wall.
        float top = step(0.0, dot(cross(v2, l), vec3(0.0, -1.0, 0.0)));
        float down = step(0.0, dot(cross(v1, l), vec3(0.0, 1.0, 0.0)));
        float lineDist = abs(dot(normalize(l.xz), v2.xz));
        float w1Dist = length(w1.xz-p.xz);
        float w2Dist = length(w2.xz-p.xz);
        minDist = min(w2Dist, minDist)*top + min(w1Dist, minDist)*down + (1.0-top)*(1.0-down)*min(minDist, lineDist);

        j = i;
    }
    return minDist;
}

const float cellSize = 3.0;
const float borderWidth = 0.2;
const vec3 cellColor = vec3(1.0, 1.0, 1.0);
const vec3 borderColor = vec3(0.4, 0.4, 0.4);
const vec3 wallBorderColor = vec3(0.4, 0.4, 0.4);

void main()
{
    bool isBorderX = bool(step(cellSize-borderWidth, mod(fragPos.x, cellSize)));
    bool isBorderZ = bool(step(cellSize-borderWidth, mod(fragPos.z, cellSize)));
    float l =  1.0 - float(isBorderZ || isBorderX);

    float minDist = 10000.0;
	int index = 0;
    for (int i = 0; i < wallPoints.groupSize; i++)
    {   
        float d = GetMinDist(fragPos, int(wallPoints.points[index].w), index);
		minDist = min(d, minDist);
        index += int(wallPoints.points[index].w);
    }

	minDist = min(1.0, minDist);
	float isBorder = step(borderWidth, minDist);
	l = float(bool(l) && bool(isBorder));

    vec3 tmp = camPos*texture(tex, fragUv).rgb;

    finalColor = vec4(cellColor * l + (1-l)*(borderColor*isBorder + (1.0 - isBorder)*wallBorderColor) + tmp*0.000001, 1.0);
}