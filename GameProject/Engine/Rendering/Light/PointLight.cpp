#include "PointLight.h"


PointLight::PointLight(glm::vec4 position, glm::vec4 intensity)
{
	this->position = position;
	this->intensity = intensity;
}

PointLight::~PointLight()
{
}

glm::vec4 PointLight::getPosition()
{
	return this->position;
}

glm::vec4 PointLight::getIntensity()
{
	return this->intensity;
}
