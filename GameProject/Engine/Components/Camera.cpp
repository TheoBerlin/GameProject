#include "Camera.h"

#include "glm/gtc/matrix_transform.hpp"
#include  "../Entity/Entity.h"

Camera::Camera(const std::string& tagName, const glm::vec3& offset) : Component(tagName)
{
	// Set the standard values
	this->fov = FOV;
	this->zNear = ZNEAR;
	this->zFar = ZFAR;
	this->offset = offset;

	// Set subscribe to resize event to update camera
	EventBus::get().subscribe(this, &Camera::updateProj);
}


Camera::~Camera()
{
}

void Camera::update(const float & dt)
{
	updatePosition();
	updateView();
}

void Camera::init()
{
	// Init camera
	setForward(getHost()->getMatrix()->getForward());
	updatePosition();

	updateProj(&WindowResizeEvent(DEFAULT_WIDTH, DEFAULT_HEIGHT));
	updateView();
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
	return this->proj * this->view;
}

void Camera::updateView()
{
	this->view = glm::lookAt(this->pos, this->pos + this->getHost()->getMatrix()->getForward(), this->u);
}

void Camera::updateProj(WindowResizeEvent * evnt)
{
	this->proj = glm::perspective(glm::radians(this->fov), Display::get().getRatio(), this->zNear, this->zFar);
}

void Camera::setForward(const glm::vec3 & forward)
{
	this->f = glm::normalize(forward);
	this->r = glm::cross(this->f, GLOBAL_UP_VECTOR);
	this->u = glm::cross(this->r, this->f);
}

void Camera::updatePosition()
{
	glm::vec3 hostPos = getHost()->getMatrix()->getPosition();
	this->pos = hostPos + (this->r * this->offset.x + this->u * this->offset.y + this->f * this->offset.z);
}