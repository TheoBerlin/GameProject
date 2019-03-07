#pragma once
#include "glm/glm.hpp"

class PointLight
{
public:
	PointLight(glm::vec4 position, glm::vec4 intensity, int distance);
	PointLight();
	~PointLight();
	glm::vec4 getPosition();
	glm::vec4 getIntensity();
private:
	glm::vec4 position;
	glm::vec4 intensity;
	float constant;
	float linear;
	float quadratic;
	float padding = 0.0f;
};

