#define GLM_FORCE_SWIZZLE
#include "Transform.h"

glm::vec3 Transform::modulusRotation(glm::vec3 rotation)
{
	rotation.x = (float)fmod(rotation.x, 2.0f * 3.1415f);
	rotation.y = (float)fmod(rotation.y, 2.0f * 3.1415f);
	rotation.z = (float)fmod(rotation.z, 2.0f * 3.1415f);

	return rotation;
}

void Transform::updateForwardRightUp()
{
	glm::mat4 rotMat = glm::mat4(1.0f);
	if (rotation.x > 0)
		rotMat = glm::rotate(rotMat, rotation.x, glm::vec3(1, 0, 0));
	if (rotation.y > 0)
		rotMat = glm::rotate(rotMat, rotation.y, glm::vec3(0, 1, 0));
	if (rotation.z > 0)
		rotMat = glm::rotate(rotMat, rotation.z, glm::vec3(0, 0, 1));
	this->f = (rotMat * glm::vec4(defaultForward, 1.0f)).xyz();
	this->r = glm::cross(this->f, GLOBAL_UP_VECTOR);
	this->u = glm::cross(this->r, this->f);
}

Transform::Transform()
{
	this->scaleFactor = glm::vec3(1, 1, 1);
	this->rotation = glm::vec3(0, 0, 0);
	this->position = glm::vec3(0, 0, 0);
	setForward(glm::vec3(1, 0, 0));
}

glm::mat4 Transform::getMatrix() const
{
	glm::mat4 ret = glm::mat4(1);
	ret = glm::rotate(ret, rotation.x + rotation.y + rotation.z, glm::normalize(rotation));
	ret = glm::scale(ret, scaleFactor);
	ret[3][0] = position.x;
	ret[3][1] = position.y;
	ret[3][2] = position.z;
	return ret;
}

glm::vec3 Transform::getPosition() const
{
	return position;
}

glm::vec3 Transform::getRotation() const
{
	return this->rotation;
}

glm::vec3 Transform::getScale() const
{
	return this->scaleFactor;
}

glm::vec3 Transform::getForward() const
{
	return this->f;
}

glm::vec3 Transform::getRight() const
{
	return this->r;
}

glm::vec3 Transform::getUp() const
{
	return this->u;
}

void Transform::rotate(const glm::vec3& rotation)
{
	this->rotation = modulusRotation(this->rotation + rotation);
	updateForwardRightUp();
}

void Transform::rotate(const glm::vec3& rotation, const glm::vec3& rotationCenter)
{
	glm::mat4 rotationMatrix = glm::mat4(1);

	//Might be different amount of rotation for different axis and therefore need to check and rotate each individual axis
	rotationMatrix = glm::translate(rotationMatrix, rotationCenter - position);

	if (glm::abs(rotation.x) > 0) {
		rotationMatrix = glm::rotate(rotationMatrix, rotation.x, glm::vec3(1, 0, 0));
	}
	if (glm::abs(rotation.y) > 0) {
		rotationMatrix = glm::rotate(rotationMatrix, rotation.y, glm::vec3(0, 1, 0));
	}
	if (glm::abs(rotation.z) > 0) {
		rotationMatrix = glm::rotate(rotationMatrix, rotation.z, glm::vec3(0, 0, 1));
	}

	rotationMatrix = glm::translate(rotationMatrix, position - rotationCenter);
	position = (rotationMatrix * glm::vec4(position, 1.0f)).xyz();
}

void Transform::rotateAxis(const float & radians, const glm::vec3 & axis)
{
	rotation += normalize(axis) * radians;
	updateForwardRightUp();
}

void Transform::setRotation(const glm::vec3 &rotation)
{
	this->rotation = modulusRotation(rotation);
	f = glm::vec3(0, 0, 1);
	updateForwardRightUp();
}

void Transform::translate(const glm::vec3& vector)
{
	this->position += vector;
}

void Transform::setPosition(const glm::vec3& position)
{
	this->position = position;
}

void Transform::scale(const glm::vec3& scale, float deltaTime)
{
	//Take the scale change by taking -1, then change based on delta time. Then take +1 to properly scale down or up.
	glm::vec3 s = scale;
	s -= glm::vec3(1.0f);
	s *= deltaTime;
	s += glm::vec3(1.0f);
	this->scaleFactor *= s;
}

void Transform::scale(const float& scale, float deltaTime)
{
	//Take the scale change by taking -1, then change based on delta time. Then take +1 to properly scale down or up.
	float s = scale;
	s -= 1.0f;
	s *= deltaTime;
	s += 1.0f;
	this->scaleFactor *= s;
}

void Transform::setScale(const glm::vec3& scale)
{
	this->scaleFactor = scale;
}

void Transform::setScale(const float& scale)
{
	this->scaleFactor = glm::vec3(scale);
}

void Transform::setForward(const glm::vec3 & forward)
{
	this->f = normalize(forward);
	this->r = glm::cross(this->f, GLOBAL_UP_VECTOR);
	this->u = glm::cross(this->r, this->f);
	defaultForward = f;
}
