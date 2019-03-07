#include "Transform.h"
#include <glm/gtx/vector_angle.hpp>

Transform::Transform()
{
	rotationQuat = glm::quat_cast(glm::mat4(1.0f));

	this->f = defaultForward;
	this->r = glm::cross(this->f, GLOBAL_UP_VECTOR);
	this->u = glm::cross(this->r, this->f);

	this->scaleFactor = glm::vec3(1.0f, 1.0f, 1.0f);
	this->position = glm::vec3(0, 0, 0);

	this->isUpdated = false;
}

void Transform::getMatrix(glm::mat4 * mat) const
{
	*mat = glm::mat4_cast(rotationQuat) * glm::scale(scaleFactor);
	(*mat)[3][0] = position.x;
	(*mat)[3][1] = position.y;
	(*mat)[3][2] = position.z;
}

glm::mat4 Transform::getMatrix() const
{
	glm::mat4 ret = glm::mat4_cast(rotationQuat) * glm::scale(scaleFactor);
	ret[3][0] = position.x;
	ret[3][1] = position.y;
	ret[3][2] = position.z;
	return ret;
}

glm::vec3 Transform::getPosition() const
{
	return position;
}

glm::quat Transform::getRotationQuat() const
{
	return this->rotationQuat;
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

glm::vec3 Transform::getYawPitchRoll() const
{
	glm::vec3 yawPitchRoll;

	// Calculate yaw
	glm::vec3 temp = glm::normalize(glm::vec3(this->f.x, defaultForward.y, this->f.z));

	yawPitchRoll.x = glm::orientedAngle(defaultForward, temp, GLOBAL_UP_VECTOR);

	// Calculate pitch
	temp = glm::normalize(glm::vec3(this->f.x, 0.0f, this->f.z));

	yawPitchRoll.y = glm::orientedAngle(temp, this->f, this->r);

	// Calculate roll
	glm::vec3 horizontalRight = glm::normalize(glm::cross(this->f, GLOBAL_UP_VECTOR));
	temp = glm::normalize(glm::cross(horizontalRight, this->f));

	// Determine roll sign
	yawPitchRoll.z = glm::orientedAngle(temp, this->u, this->f);

	return yawPitchRoll;
}

glm::vec3 Transform::getDefaultForward() const
{
	return this->defaultForward;
}

void Transform::setRotationQuat(const glm::quat& newQuat)
{
	this->rotationQuat = newQuat;

	// Apply rotation
	this->f = newQuat * defaultForward;
	this->u = newQuat * GLOBAL_UP_VECTOR;
	this->r = glm::normalize(glm::cross(this->f, this->u));
}

bool Transform::getStatus()
{
	if (this->isUpdated) {
		this->isUpdated = false;
		return true;
	}

	return false;
}

void Transform::rotate(const glm::vec3& rotation)
{
	glm::quat rotQuat = glm::quat(rotation);
	this->f = rotQuat * this->f;
	this->r = rotQuat * this->r;
	this->u = rotQuat * this->u;

	rotationQuat = rotQuat * rotationQuat;
	this->isUpdated = true;
}

void Transform::rotate(const glm::vec3& rotation, const glm::vec3& rotationCenter)
{
	if (rotation != glm::vec3(0.0f)) {
		position -= rotationCenter;

		position = glm::quat(rotation) * position;

		position += rotationCenter;

		isUpdated = true;
	}
}

void Transform::rotateAxis(const float & radians, const glm::vec3 & axis)
{
	glm::quat axisRotation = glm::rotate(rotationQuat, radians, glm::normalize(axis));
	this->f = axisRotation * this->f;
	this->r = axisRotation * this->r;
	this->u = axisRotation * this->u;

	rotationQuat = axisRotation * rotationQuat;
	this->isUpdated = true;
}

void Transform::setRotation(const glm::vec3 &rotation)
{
	this->rotationQuat = glm::quat_cast(glm::mat4(1.0f));

	this->f = rotationQuat * defaultForward;
	this->r = rotationQuat * glm::cross(defaultForward, GLOBAL_UP_VECTOR);
	this->u = rotationQuat * GLOBAL_UP_VECTOR;

	this->rotate(rotation.x, rotation.y, rotation.z);
}

void Transform::translate(const glm::vec3& vector)
{
	this->position += vector;
	this->isUpdated = true;
}

void Transform::setPosition(const glm::vec3& position)
{
	this->position = position;
	this->isUpdated = true;
}

void Transform::scale(const glm::vec3& scale, float deltaTime)
{
	//Take the scale change by taking -1, then change based on delta time. Then take +1 to properly scale down or up.
	glm::vec3 s = scale;
	s -= glm::vec3(1.0f);
	s *= deltaTime;
	s += glm::vec3(1.0f);
	this->scaleFactor *= s;
	this->isUpdated = true;
}

void Transform::scale(const float& scale, float deltaTime)
{
	//Take the scale change by taking -1, then change based on delta time. Then take +1 to properly scale down or up.
	float s = scale;
	s -= 1.0f;
	s *= deltaTime;
	s += 1.0f;
	this->scaleFactor *= s;
	this->isUpdated = true;
}

void Transform::setScale(const glm::vec3& scale)
{
	this->scaleFactor = scale;
	this->isUpdated = true;
}

void Transform::setScale(const float& scale)
{
	this->scaleFactor = glm::vec3(scale);
	this->isUpdated = true;
}

void Transform::setForward(const glm::vec3 & forward)
{
	glm::vec3 normForward = glm::normalize(forward);
	// Create rotation quaternion based on new forward
	// Beware of the cases where the new forward vector is parallell to the old one
	float cosAngle = glm::dot(normForward, this->f);
	glm::quat rotQuat = glm::quat_cast(glm::mat4(1));

	if (cosAngle >= 1.0f - FLT_EPSILON * 10.0f) {
		// The new forward is identical to the old one, do nothing
		return;
	}
	else if (cosAngle <= -1.0f + FLT_EPSILON * 10.0f) {
		// The new forward is parallell to the old one, create a 180 degree rotation quarternion
		// around any axis
		rotQuat = glm::angleAxis(glm::pi<float>(), GLOBAL_UP_VECTOR) * rotationQuat;
	}
	else {
		// Calculate rotation quaternion
		glm::vec3 axis = glm::normalize(glm::cross(this->f, normForward));
		float angle = std::acosf(cosAngle);

		rotQuat = glm::angleAxis(angle, axis);
	}

	this->f = rotQuat * this->f;
	this->r = rotQuat * this->r;
	this->u = glm::cross(this->r, this->f);

	rotationQuat = rotQuat * rotationQuat;
	this->isUpdated = true;
}

void Transform::rotate(const float yaw, const float pitch, const float roll)
{
	// Apply yaw
	glm::quat yawQuat = glm::angleAxis(yaw, GLOBAL_UP_VECTOR);
	this->f = glm::normalize(yawQuat * this->f);
	this->r = glm::normalize(yawQuat * this->r);

	// Apply pitch
	glm::quat pitchQuat = glm::angleAxis(pitch, this->r);
	this->f = glm::normalize(pitchQuat * this->f);
	this->u = glm::cross(this->r, this->f);

	// Apply roll
	glm::quat rollQuat = glm::angleAxis(roll, this->f);
	this->r = glm::normalize(rollQuat * this->r);
	this->u = glm::normalize(rollQuat * this->u);

	rotationQuat = rollQuat * pitchQuat * yawQuat * rotationQuat;

	this->isUpdated = true;
}

void Transform::resetRoll()
{
	this->r = glm::normalize(glm::vec3(r.x, 0.0f, r.z));
	this->u = glm::cross(this->r, this->f);
}