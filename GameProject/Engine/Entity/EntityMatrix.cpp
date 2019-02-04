#include "EntityMatrix.h"

glm::vec3 EntityMatrix::modulusRotation(glm::vec3 rotation)
{
	rotation.x = fmod(rotation.x, 2.0 * 3.1415);
	rotation.y = fmod(rotation.y, 2.0 * 3.1415);
	rotation.z = fmod(rotation.z, 2.0 * 3.1415);

	return rotation;
}

EntityMatrix::EntityMatrix()
{
	this->scaleFactor = glm::vec3(1, 1, 1);
	this->rotation = glm::vec3(0, 0, 0);
	this->worldMatrix = glm::mat4(1);
	this->modelMatrix = glm::mat4(1);
}

glm::mat4 EntityMatrix::getMatrix() const
{
	return this->worldMatrix * this->modelMatrix;
}

glm::vec3 EntityMatrix::getPosition() const
{
	return glm::vec3(worldMatrix[3][0], worldMatrix[3][1], worldMatrix[3][2]);
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
	return this->forward;
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
}

void EntityMatrix::rotate(const glm::vec3& rotation, const glm::vec3& rotationCenter)
{
	//Save matrix position and then translate modelMatrix so rotationCenter is at origin.
	glm::vec3 position = getPosition();
	this->modelMatrix = glm::translate(this->modelMatrix, -rotationCenter);

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

	//Return to original position after rotation
	this->modelMatrix = glm::translate(this->modelMatrix, getPosition());
}

void EntityMatrix::setRotation(const glm::vec3 &rotation)
{
	rotation = modulusRotation(rotation);
	this->rotation = glm::vec3(0);
	this->scaleFactor = glm::vec3(0);
	this->modelMatrix = glm::mat4(1);
	rotate(rotation);
	scale(scaleFactor);
}

void EntityMatrix::translate(const glm::vec3& vector)
{
	this->worldMatrix = glm::translate(this->worldMatrix, vector);
}

void EntityMatrix::setPosition(const glm::vec3& position)
{
	this->worldMatrix = glm::mat4(1);
	this->worldMatrix = glm::translate(this->worldMatrix, position);
}

void EntityMatrix::scale(const glm::vec3& scale)
{
	this->scaleFactor += scale;
	this->modelMatrix = glm::scale(this->modelMatrix, scale);
}

void EntityMatrix::scale(const float& scale)
{
	this->modelMatrix = glm::scale(this->modelMatrix, glm::vec3(scale, scale, scale));
}

void EntityMatrix::setScale(const glm::vec3& scale)
{
	this->scaleFactor = glm::vec3(0);
	this->rotation = glm::vec3(0);
	this->modelMatrix = glm::mat4(1);
	this->scale(scale);
	rotate(this->rotation);
}

void EntityMatrix::setScale(const float& scale)
{
	this->scaleFactor = glm::vec3(0);
	this->rotation = glm::vec3(0);
	this->modelMatrix = glm::mat4(1);
	this->scale(scale);
	rotate(this->rotation);
}

void EntityMatrix::setForward(const glm::vec3 & forward)
{
	this->forward = forward;
}
