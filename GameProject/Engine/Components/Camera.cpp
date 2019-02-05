#include "Camera.h"

#include "glm/gtc/matrix_transform.hpp"
#include  "../Entity/Entity.h"

const glm::vec3 GLOBAL_UP_VECTOR = glm::vec3(0.0f, 1.0f, 0.0f);

Camera::Camera(const std::string& tagName, const glm::vec3& offset) : Component(tagName)
{
	// Set the standard values
	this->fov = (float)FOV;
	this->zNear = (float)ZNEAR;
	this->zFar = (float)ZFAR;

	// Init camera
	this->offset = offset;
	this->pos = getHost()->getMatrix()->getPosition() + offset;
	if (abs(offset[0]) < EPSILON && abs(offset[1]) < EPSILON && abs(offset[2]) < EPSILON)
	{
		// If offset haven't been set, set a default forward
		setForward(glm::vec3(1.0f, 0.0f, 0.0f));
	}
	else
	{
		// If offset has been set, set the forward to pos
		setForward(this->pos - getHost()->getMatrix()->getPosition());
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
	this->pos = getHost()->getMatrix()->getPosition() + offset;
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
