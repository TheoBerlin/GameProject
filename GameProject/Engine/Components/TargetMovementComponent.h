#pragma once

#include "Component.h"

class TargetMovementComponent : Component
{
public: 
	TargetMovementComponent(std::string name = "TargetMovement");
	virtual ~TargetMovementComponent();

	void init();
	void update(const float& dt);

	glm::vec3 getPosition();

private:

	std::string name;
	std::vector<glm::vec3> tarPositions;
	glm::vec3 currentPosition;
	float t; //Set end time from file?

};