#pragma once

#include "Engine/Components/Component.h"
#include "Game/Components/ComponentResources.h"

#include <vector>
#include <glm/glm.hpp>

class TrailEmitter : public Component
{
public:
	TrailEmitter(Entity* host);
	~TrailEmitter();

	void update(const float& dt);

	void resetTrailTimer();
private:
	glm::vec3 startPos;
	glm::vec3 currentPos;
	glm::vec3 forwardPos;
	glm::vec3 startUpVector;

	std::vector<KeyPoint> path;

	float trailDuration;
	float timer;
};

