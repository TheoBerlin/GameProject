#include "TargetMovementComponent.h"

TargetMovementComponent::TargetMovementComponent(std::string name) : Component(name)
{
}

TargetMovementComponent::~TargetMovementComponent()
{
}

void TargetMovementComponent::init()
{
	
}

void TargetMovementComponent::update(const float & dt)
{
	//Interpolation updating
	//x = x_0 + (t - t_0 / t_1 - t_0) * (x_1 - x_0)
}

glm::vec3 TargetMovementComponent::getPosition()
{
	return currentPosition;
}
