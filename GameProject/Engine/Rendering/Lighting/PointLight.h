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
	void setPosition(glm::vec4 position);
	void setIntensity(glm::vec4 intensity);
	void setDistance(int distance);
private:
	glm::vec4 position;
	glm::vec4 intensity;
	float constant;
	float linear;
	float quadratic;
	int distance;
};

