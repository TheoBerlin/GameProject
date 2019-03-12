#pragma once
#include "glm/glm.hpp"

class DirectionalLight
{
public:
	DirectionalLight(glm::vec4 direction, glm::vec4 intensity);
	~DirectionalLight();
	glm::vec4 getDirection();
	glm::vec4 getIntensity();
private:
	glm::vec4 direction;
	glm::vec4 intensity;

};
