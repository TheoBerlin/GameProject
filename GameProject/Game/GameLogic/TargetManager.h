#pragma once

#include <vector>
#include "glm/glm.hpp"

class Entity;
class PathTreader;
class RollNullifier;
class Hover;
struct KeyPoint;

struct MovingTarget {
    PathTreader* pathTreader;
    RollNullifier* rollNullifier;
};

struct StaticTarget {
    Hover* hoverAnimation;
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

    // Resets animations and collision states
    void resetStaticTargets();
    void resetMovingTargets();

    void resetStaticAnimations();
    void resetMovingAnimations();

	void resetStaticCollisions();
	void resetMovingCollisions();

    std::vector<MovingTarget> movingTargets;
    std::vector<StaticTarget> staticTargets;
};
