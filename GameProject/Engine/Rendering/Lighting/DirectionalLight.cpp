#include "DirectionalLight.h"


DirectionalLight::DirectionalLight(glm::vec4 direction, glm::vec4 intensity)
{
	this->direction = glm::normalize(direction);
	this->intensity = intensity;
}

DirectionalLight::~DirectionalLight()
{
}

glm::vec4 DirectionalLight::getDirection()
{
	return this->direction;
}

glm::vec4 DirectionalLight::getIntensity()
{
	return this->intensity;
}