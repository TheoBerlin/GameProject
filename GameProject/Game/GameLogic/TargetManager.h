#pragma once

#include <vector>
#include "glm/glm.hpp"

class Entity;
class PathTreader;
class RollNullifier;
class Hover;
class Explosion;
struct KeyPoint;

struct MovingTarget {
    PathTreader* pathTreader;
    RollNullifier* rollNullifier;
};

struct StaticTarget {
    Hover* hoverAnimation;
	Explosion* explosion;
};

class TargetManager
{
public:
    TargetManager();
    ~TargetManager();

    // Add target entity
    void addStaticTarget(Entity* host);
    void addMovingTarget(Entity* host, const std::vector<KeyPoint>& path);
	void addKeyPoint(Entity* host, const KeyPoint path);

	//Remove an entity as target
	void removeTarget(std::string name);

	//Get the arrays
	std::vector<MovingTarget> getMovingTargets() const;
	std::vector<StaticTarget> getStaticTargets() const;

	// Separate the transform from the model. (Pause the model)
	void pauseMovingTargets();
	void unpauseMovingTargets();

    // Reset target components such as the path treader
    void resetTargets();

	// Returns the number of targets in target manager
	unsigned getTargetCount();

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
