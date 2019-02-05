#include "EntityMatrix.h"

glm::vec3 EntityMatrix::modulusRotation(glm::vec3 rotation)
{
	rotation.x = (float)fmod(rotation.x, 2.0 * 3.1415);
	rotation.y = (float)fmod(rotation.y, 2.0 * 3.1415);
	rotation.z = (float)fmod(rotation.z, 2.0 * 3.1415);

	return rotation;
}

EntityMatrix::EntityMatrix()
{
	this->scaleFactor = glm::vec3(1, 1, 1);
	this->rotation = glm::vec3(0, 0, 0);
	this->position = glm::vec3(0, 0, 0);
	this->modelMatrix = glm::mat4(1);
}

glm::mat4 EntityMatrix::getMatrix() const
{
	glm::mat4 ret = glm::mat4(1);
	ret = glm::rotate(ret, rotation.x + rotation.y + rotation.z, glm::normalize(rotation));
	ret = glm::scale(ret, scaleFactor);
	ret[3][0] = position.x;
	ret[3][1] = position.y;
	ret[3][2] = position.z;
	return ret;
}

glm::vec3 EntityMatrix::getPosition() const
{
	return position;
}

glm::vec3 EntityMatrix::getRotation() const
{
	return this->rotation;
}

glm::vec3 EntityMatrix::getScale() const
{
	return this->scaleFactor;
}

glm::vec3 EntityMatrix::getForward() const
{
	return this->f;
}

glm::vec3 EntityMatrix::getRight() const
{
	return this->r;
}

glm::vec3 EntityMatrix::getUp() const
{
	return this->u;
}

void EntityMatrix::rotate(const glm::vec3& rotation)
{
	//Might be different amount of rotation for different axis and therefore need to check and rotate each individual axis
	if (glm::abs(rotation.x) > 0) {
		this->modelMatrix = glm::rotate(this->modelMatrix, rotation.x, glm::vec3(1, 0, 0));
	}
	if (glm::abs(rotation.y) > 0) {
		this->modelMatrix = glm::rotate(this->modelMatrix, rotation.y, glm::vec3(0, 1, 0));
	}
	if (glm::abs(rotation.z) > 0) {
		this->modelMatrix = glm::rotate(this->modelMatrix, rotation.z, glm::vec3(0, 0, 1));
	}
	this->rotation = modulusRotation(this->rotation + rotation);
	setForward(modelMatrix * glm::vec4(f, 1.0f));
}

void EntityMatrix::rotate(const glm::vec3& rotation, const glm::vec3& rotationCenter)
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
	glm::vec4 temp = rotationMatrix * glm::vec4(position, 1.0);
	position.x = temp.x;
	position.y = temp.y;
	position.z = temp.z;
}

void EntityMatrix::rotateAxis(const float & radians, const glm::vec3 & axis)
{
	modelMatrix = glm::rotate(modelMatrix, radians, normalize(axis));
	setForward(modelMatrix * glm::vec4(f, 1.0f));
}

void EntityMatrix::setRotation(const glm::vec3 &rotation)
{
	glm::vec3 newRotation = modulusRotation(rotation);
	this->rotation = glm::vec3(0);
	glm::vec3 saveScale = scaleFactor;
	this->scaleFactor = glm::vec3(1);
	this->modelMatrix = glm::mat4(1);
	rotate(newRotation);
	scale(saveScale);
}

void EntityMatrix::translate(const glm::vec3& vector)
{
	this->position += vector;
}

void EntityMatrix::setPosition(const glm::vec3& position)
{
	this->position = position;
}

void EntityMatrix::scale(const glm::vec3& scale)
{
	this->scaleFactor *= scale;
	this->modelMatrix = glm::scale(this->modelMatrix, scale);
}

void EntityMatrix::scale(const float& scale)
{
	this->scaleFactor *= scale + glm::vec3(1, 1, 1);
	this->modelMatrix = glm::scale(this->modelMatrix, glm::vec3(scale, scale, scale));
}

void EntityMatrix::setScale(const glm::vec3& scale)
{
	this->scaleFactor = glm::vec3(1);
	glm::vec3 saveRotation = rotation;
	this->rotation = glm::vec3(0);
	this->modelMatrix = glm::mat4(1);
	this->scale(scale);
	rotate(saveRotation);
}

void EntityMatrix::setScale(const float& scale)
{
	glm::vec3 saveRotation = rotation;
	this->rotation = glm::vec3(0);
	this->scaleFactor = glm::vec3(0);
	this->modelMatrix = glm::mat4(1);
	this->scale(scale);
	rotate(this->rotation);
}

void EntityMatrix::setForward(const glm::vec3 & forward)
{
	this->f = glm::normalize(forward);
	this->r = glm::cross(this->f, GLOBAL_UP_VECTOR);
	this->u = glm::cross(this->r, this->f);
}
