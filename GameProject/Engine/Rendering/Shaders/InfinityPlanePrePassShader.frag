#version 420

in vec3 fragPos;

layout(std140) uniform WallPoints
{
    vec4 points[96];
    int size;
    int groupSize;
} wallPoints;

out vec4 finalColor;

bool InRoom(vec3 p, int size, int index)
{  
    int j = index + size - 1;
    bool isOdd = index > 0;
    for(int i = index; i < size+index; i++) {
        vec3 w2 = wallPoints.points[i].xyz;
        vec3 w1 = wallPoints.points[j].xyz;
        // Check if the horizontal line, which p.z spans, is intersecting the line between w1 and w2.
        if((w2.z < p.z && w1.z >= p.z) || (w1.z < p.z && w2.z >= p.z)) {
            // Check if the point is not the left or the right side of the line.
            if(w2.x + (w1.x-w2.x)*(p.z + w2.z)/(w1.z-w2.z) < p.x) {
                isOdd = !isOdd;
            }
        }
        j = i;
    }
    return isOdd;
}

void main()
{
    int index = 0;
    int counter = 0;
    for (int i = 0; i < wallPoints.groupSize; i++)
    {
        if(InRoom(fragPos, int(wallPoints.points[index].w), index))
            counter++;
        index += int(wallPoints.points[index].w);
    }
    if (counter == wallPoints.groupSize)
        discard;
}