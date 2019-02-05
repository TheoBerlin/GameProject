#pragma once

#include "Component.h"
#define OFFSET 5;

class TargetMovementComponent : public Component
{
public: 
	TargetMovementComponent(Entity* parent, std::string name);
	virtual ~TargetMovementComponent();

	void update(const float& dt);

	glm::vec3 getCurrentPosition();
	glm::vec3 getAddedPosition();
	glm::vec3 addPosition(glm::vec3 position);
	void setTime(float time);

private:

	std::string name;
	std::vector<glm::vec3> tarPositions;
	glm::vec3 currentPosition;
	float t; //Set end time from file?

};