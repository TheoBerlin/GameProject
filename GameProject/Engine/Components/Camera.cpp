#include "Camera.h"

#include "glm/gtc/matrix_transform.hpp"

Camera::Camera(const std::string& tagName, const glm::vec3& offset) : Component(tagName)
{
	// Set the standard values
	this->fov = FOV;
	this->zNear = ZNEAR;
	this->zFar = ZFAR;

	// Init camera
	this->offset = offset;
	
	updateProj(&WindowResizeEvent(DEFAULT_WIDTH, DEFAULT_HEIGHT));
	updateView();

	// Set subscribe to resize event to update camera
	EventBus::get().subscribe(this, &Camera::updateProj);
}


Camera::~Camera()
{
}

void Camera::update(const float & dt)
{
}

glm::vec3 Camera::getUp() const
{
	return this->u;
}

glm::vec3 Camera::getForward() const
{
	return this->f;
}

glm::vec3 Camera::getRight() const
{
	return this->r;
}

glm::mat4 Camera::getVP() const
{
	return this->view * this->proj;
}

void Camera::updateView()
{
	glm::lookAt()
}

void Camera::updateProj(WindowResizeEvent * evnt)
{
	this->proj = glm::perspective(this->fov, Display::get().getRatio(), this->zNear, this->zFar);
}
