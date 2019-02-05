#include "TargetMovementComponent.h"

TargetMovementComponent::TargetMovementComponent(Entity* parent, std::string name) 
	: Component(parent, name)
{
}

TargetMovementComponent::~TargetMovementComponent()
{
}

void TargetMovementComponent::update(const float & dt)
{
	//Interpolation updating
	//x = x_0 + (t - t_0 / t_1 - t_0) * (x_1 - x_0)
}

glm::vec3 TargetMovementComponent::getCurrentPosition()
{
	return currentPosition;
}

glm::vec3 TargetMovementComponent::addPosition(glm::vec3 position)
{
	tarPositions.push_back(position);
	position.x += OFFSET;
	tarPositions.push_back(position);
}

void TargetMovementComponent::setTime(float time)
{
	this->t = time;
}
