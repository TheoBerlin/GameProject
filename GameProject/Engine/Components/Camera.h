#pragma once

#include "glm/glm.hpp"
#include "Component.h"

#define FOV 60.0;
#define ZFAR 100.0;
#define ZNEAR 0.1;

class Camera : public Component
{
public:
	Camera(const std::string& tagName);
	virtual ~Camera();

	void update(const float& dt);

	glm::vec3 getUp() const;
	glm::vec3 getForward() const;
	glm::vec3 getRight() const;

	glm::mat4 getVP() const;

private:
	glm::vec3 f, u, r;
	glm::mat4 view, proj;

};

