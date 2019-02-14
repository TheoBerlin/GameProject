#include "Camera.h"

#include "glm/gtc/matrix_transform.hpp"
#include  "../Entity/Entity.h"

Camera::Camera(Entity * parentEntity, const std::string& tagName, const glm::vec3& offset) : Component(parentEntity, tagName)
{
	// Set the standard values
	this->fov = STARTING_FOV;
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
	setForward(getHost()->getTransform()->getForward());
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

glm::mat4 Camera::getView() const
{
	return this->view;
}

glm::mat4 Camera::getProj() const
{
	return this->proj;
}

glm::vec3 Camera::getPosition() const
{
	return this->pos;
}

float Camera::getFOV() const
{
	return fov;
}

void Camera::setFOV(const float FOV)
{
	this->fov = FOV;

	updateProj(nullptr);
}

glm::vec3 Camera::getOffset() const
{
	return offset;
}

void Camera::setOffset(const glm::vec3& offset)
{
	this->offset = offset;
}

void Camera::updateView()
{
	this->view = glm::lookAt(this->pos, this->pos + this->getHost()->getTransform()->getForward(), this->u);
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
	glm::vec3 hostPos = getHost()->getTransform()->getPosition();
	Transform* transform = host->getTransform();

	this->pos = hostPos + (transform->getRight() * this->offset.x + transform->getUp() * this->offset.y + transform->getForward() * this->offset.z);
}
