#pragma once

#include "Engine/Components/Component.h"
#include "Game/Components/ComponentResources.h"

#include <vector>
#include <glm/glm.hpp>

struct TrailPointData {
	glm::vec3 pos;
	glm::vec3 upVector;

	float alpha;
	float width;
};

class TrailEmitter : public Component
{
public:
	TrailEmitter(Entity* host);
	~TrailEmitter();

	void update(const float& dt);

	void setTrailTimer(float elapsedTime);
	void resetTrailTimer();
private:
	glm::vec3 startPos;
	glm::vec3 forwardPos;
	glm::vec3 startUpVector;

	std::vector<KeyPoint> path;

	float startingWidth;
	float startingAlpha;

	float trailDuration;
	float timer;
};

