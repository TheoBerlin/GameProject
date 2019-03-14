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
	this->decoupled = false;

	// Set subscribe to resize event to update camera
	EventBus::get().subscribe(this, &Camera::updateProj);
}

Camera::~Camera()
{
	EventBus::get().unsubscribe(this, &Camera::updateProj);
}

void Camera::update(const float & dt)
{
	if (!decoupled) {
		updateTransform();
	}

	updateView();
}

void Camera::init()
{
	// Init camera
	setForward(getHost()->getTransform()->getForward());
	updateTransform();

	updateProj(&WindowResizeEvent(DEFAULT_WIDTH, DEFAULT_HEIGHT));
	updateView();
}

glm::vec3 Camera::getForward() const
{
	return this->f;
}

void Camera::setForward(const glm::vec3 & forward)
{
	this->f = glm::normalize(forward);
	this->r = glm::normalize(glm::cross(this->f, GLOBAL_UP_VECTOR));
	this->u = glm::cross(this->r, this->f);
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

void Camera::setPosition(const glm::vec3& position)
{
	this->pos = position;

	updateView();
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

void Camera::decouple()
{
	this->decoupled = true;
}

void Camera::couple()
{
	this->decoupled = false;
}

bool Camera::isDecoupled() const
{
	return this->decoupled;
}

void Camera::updateView()
{
	this->view = glm::lookAt(this->pos, this->pos + this->f, GLOBAL_UP_VECTOR);
}

void Camera::updateProj(WindowResizeEvent * evnt)
{
	this->proj = glm::perspective(glm::radians(this->fov), Display::get().getRatio(), this->zNear, this->zFar);
}

void Camera::updateTransform()
{
	Transform* transform = host->getTransform();

	glm::vec3 hostPos = transform->getPosition();

	this->f = transform->getForward();
	this->r = glm::normalize(glm::cross(this->f, GLOBAL_UP_VECTOR));
	this->u = glm::cross(this->r, this->f);

	this->pos = hostPos + (this->r * this->offset.x + transform->getUp() * this->offset.y + this->f * this->offset.z);
}