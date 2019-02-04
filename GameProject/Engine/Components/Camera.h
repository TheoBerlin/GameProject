#pragma once

#include "glm/glm.hpp"
#include "../Config.h"
#include "Component.h"
#include "../Rendering/Display.h"
#include "../Events/EventBus.h"

class Camera : public Component
{
public:
	Camera(const std::string& tagName = "Camera", const glm::vec3& offset = glm::vec3(0.0f, 0.0f, 0.0f));
	virtual ~Camera();

	// Update called by entity
	void update(const float& dt);

	// Returns the cameras up-vector
	glm::vec3 getUp() const;
	// Returns the cameras forward-vector
	glm::vec3 getForward() const;
	// Returns the cameras right-vector
	glm::vec3 getRight() const;

	// Returns the View-Projection matrix
	glm::mat4 getVP() const;

private:
	glm::vec3 f, u, r, offset, pos;
	glm::mat4 view, proj;
	float fov, zNear, zFar;

	void updateView();
	void updateProj(WindowResizeEvent * evnt);

	// Set the forward-vector and in the process the right and up vector aswell
	void setForward(const glm::vec3 & forward);
	// Update camera's position relative to the parent entity
	void updatePosition();
};

