#pragma once
#include "Utils/Settings.h"
class PointLight
{
public:
	PointLight(glm::vec4 position, glm::vec4 intensity);
	~PointLight();
	glm::vec4 getPosition();
	glm::vec4 getIntensity();
private:
	glm::vec4 position;
	glm::vec4 intensity;
};

