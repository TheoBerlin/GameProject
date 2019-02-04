#include "Camera.h"

#include "glm/gtc/matrix_transform.hpp"
#include  "../Entity/Entity.h"

Camera::Camera(const std::string& tagName, const glm::vec3& offset) : Component(tagName)
{
	// Set the standard values
	this->fov = FOV;
	this->zNear = ZNEAR;
	this->zFar = ZFAR;

	// Init camera
	this->offset = offset;
	this->pos = getHost()->getMatrix()->getPosition() + offset;
	this->f = getHost()->getMatrix()->getForward();
	// If there is an offset set but no forward
	if(abs(offset.length()) > EPSILON && this->f.length() < EPSILON)
	{
		// If offset has been set, set the forward to pos
		setForward(this->pos - getHost()->getMatrix()->getPosition());
	}
	// If no offset and no forward, set default
	else if (abs(offset.length()) < EPSILON && this->f.length() < EPSILON)
	{
		setForward(glm::vec3(1.0f, 0.0f, 0.0f));
	}
	// If there is a forward, set that
	else
	{
		setForward(this->f);
	}
	
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
	updatePosition();
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
	return this->view * this->proj;
}

void Camera::updateView()
{
	this->view = glm::lookAt(this->pos, this->pos + this->f, this->u);
}

void Camera::updateProj(WindowResizeEvent * evnt)
{
	this->proj = glm::perspective(this->fov, Display::get().getRatio(), this->zNear, this->zFar);
}

void Camera::setForward(const glm::vec3 & forward)
{
	this->f = glm::normalize(forward);
	this->r = glm::cross(this->f, GLOBAL_UP_VECTOR);
	this->u = glm::cross(this->r, this->f);
}

void Camera::updatePosition()
{
	glm::vec3 tempPos = getHost()->getMatrix()->getPosition();
	this->pos = { tempPos.x + offset.x, tempPos.y + offset.y, tempPos.z + offset.z };
}
