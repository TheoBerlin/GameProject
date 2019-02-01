#pragma once

#include "glm/glm.hpp"
#include "../Config.h"
#include "Component.h"
#include "../Rendering/Display.h"
#include "../Events/EventBus.h"

class Camera : public Component
{
public:
	Camera(const std::string& tagName, const glm::vec3& offset = glm::vec3(0.0f, 0.0f, 0.0f));
	virtual ~Camera();

	void update(const float& dt);

	glm::vec3 getUp() const;
	glm::vec3 getForward() const;
	glm::vec3 getRight() const;

	glm::mat4 getVP() const;

private:
	glm::vec3 f, u, r, offset, pos;
	glm::mat4 view, proj;
	float fov, zNear, zFar;

	void updateView();
	void updateProj(WindowResizeEvent * evnt);
};

