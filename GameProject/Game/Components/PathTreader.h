#pragma once

#include <Engine/Components/Component.h>
#include <Engine/Entity/Transform.h>
#include <Game/components/ComponentResources.h>
#include <glm/gtx/spline.hpp>
#include <vector>

class Entity;

class PathTreader : public Component
{
public:
    PathTreader(Entity* host);
    PathTreader(Entity* host, const std::vector<KeyPoint>& path);
    ~PathTreader();

    void update(const float& dt);

    void setPath(const std::vector<KeyPoint>& path);
	std::vector<KeyPoint> getPath() const;

    // Start or restart path treading from the beginning
    void startTreading();
    void stopTreading();

private:
    bool isTreading;

    void linearTread();
    void catmullRomTread();

    std::vector<KeyPoint> path;

    // Points to the key point currently behind or on the treader
    unsigned int currentPointIndex;
    float pathTime;
};
