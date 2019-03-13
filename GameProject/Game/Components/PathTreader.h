#pragma once

#include <Engine/Components/Component.h>
#include <Engine/Entity/Transform.h>
#include <Game/components/ComponentResources.h>
#include <vector>

class Entity;

class PathTreader : public Component
{
public:
    PathTreader(Entity* host);
    PathTreader(Entity* host, const std::vector<KeyPoint>& path, bool loop = false);
    ~PathTreader();

    void update(const float& dt);

    void setPath(const std::vector<KeyPoint>& path, bool loop = false);
	KeyPoint& getKeyPoint(unsigned int index);

    // Start or restart path treading from the beginning
    void startTreading();
    void stopTreading();

private:
    bool isTreading;

    void catmullRomTread();

    std::vector<KeyPoint> path;

    // Points to the key point currently behind or on the treader
    unsigned int currentPointIndex;
    float pathTime;

    // Whether or not the treader will loop its path
    bool isLooping;
};
