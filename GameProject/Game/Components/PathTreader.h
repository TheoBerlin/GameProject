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
    PathTreader(Entity* host, const std::vector<KeyPoint>& path);
    ~PathTreader();

    void update(const float& dt);

    void setPath(const std::vector<KeyPoint>& path);

    void beginTreading();
    void stopTreading();

private:
    void init();
    bool isTreading;

    void linearTread();
    void catmullRomTread();
    // Help function for Catmull-Rom calculations
    float calculateT(const float& t0, const glm::vec3& P0, const glm::vec3& P1);

    std::vector<KeyPoint> path;

    // Points to the key point currently behind or on the treader
    unsigned int currentPointIndex;
    float pathTime;
};
