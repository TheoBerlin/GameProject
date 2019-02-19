#pragma once

#include <Engine/Entity/Entity.h>
#include <Game/Components/PathTreader.h>
#include <Game/Components/RollNullifier.h>
#include <vector>

struct MovingTarget {
    PathTreader* pathTreader;
    RollNullifier* rollNullifier;
};

struct StaticTarget {
};

class TargetManager
{
public:
    TargetManager();
    ~TargetManager();

    // Add target entity
    void addStaticTarget(Entity* host, const glm::vec3& position);
    void addMovingTarget(Entity* host, const std::vector<KeyPoint>& path);

    // Reset target components such as the path treader
    void resetTargets();

private:
    // Common setup for targets
    void setupTargetGeneric(Entity* host);

    void resetStaticTargets();
    void resetMovingTargets();

    std::vector<MovingTarget> movingTargets;
    std::vector<StaticTarget> staticTargets;
};
