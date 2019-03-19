#pragma once
#include "glm/glm.hpp"

class DirectionalLight
{
public:
	DirectionalLight(glm::vec4 direction, glm::vec4 intensity);
	~DirectionalLight();
	void setDirection(const glm::vec4& direction);
	void setIntensity(const glm::vec4& colorIntensity);

	glm::vec4 getDirection();
	glm::vec4 getIntensity();
private:
	glm::vec4 direction;
	glm::vec4 intensity;

};
