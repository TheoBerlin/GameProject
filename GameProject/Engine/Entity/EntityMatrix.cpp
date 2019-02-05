#include "EntityMatrix.h"

glm::vec3 EntityMatrix::modulusRotation(glm::vec3 rotation)
{
	rotation.x = fmod(rotation.x, 2.0f * 3.1415f);
	rotation.y = fmod(rotation.y, 2.0f * 3.1415f);
	rotation.z = fmod(rotation.z, 2.0f * 3.1415f);

	return rotation;
}

EntityMatrix::EntityMatrix()
{
	scaleFactor = glm::vec3(1, 1, 1);
	rotation = glm::vec3(0, 0, 0);
	worldMatrix = glm::mat4(1);
	modelMatrix = glm::mat4(1);
}

glm::mat4 EntityMatrix::getMatrix() const
{
	return worldMatrix * modelMatrix;
}

glm::vec3 EntityMatrix::getPosition() const
{
	return glm::vec3(worldMatrix[3][0], worldMatrix[3][1], worldMatrix[3][2]);
}

glm::vec3 EntityMatrix::getRotation() const
{
	return rotation;
}

glm::vec3 EntityMatrix::getScale() const
{
	return scaleFactor;
}

void EntityMatrix::rotate(glm::vec3 rotation)
{
	//Might be different amount of rotation for different axis and therefore need to check and rotate each individual axis
	if (glm::abs(rotation.x) > 0) {
		modelMatrix = glm::rotate(modelMatrix, rotation.x, glm::vec3(1, 0, 0));
	}
	if (glm::abs(rotation.y) > 0) {
		modelMatrix = glm::rotate(modelMatrix, rotation.y, glm::vec3(0, 1, 0));
	}
	if (glm::abs(rotation.z) > 0) {
		modelMatrix = glm::rotate(modelMatrix, rotation.z, glm::vec3(0, 0, 1));
	}
	this->rotation = modulusRotation(this->rotation + rotation);
}

void EntityMatrix::rotate(glm::vec3 rotation, glm::vec3 rotationCenter)
{
	//Save matrix position and then translate modelMatrix so rotationCenter is at origin.
	glm::vec3 position = getPosition();
	modelMatrix = glm::translate(modelMatrix, -rotationCenter);

	//Might be different amount of rotation for different axis and therefore need to check and rotate each individual axis
	if (glm::abs(rotation.x) > 0) {
		modelMatrix = glm::rotate(modelMatrix, rotation.x, glm::vec3(1, 0, 0));
	}
	if (glm::abs(rotation.y) > 0) {
		modelMatrix = glm::rotate(modelMatrix, rotation.y, glm::vec3(0, 1, 0));
	}
	if (glm::abs(rotation.z) > 0) {
		modelMatrix = glm::rotate(modelMatrix, rotation.z, glm::vec3(0, 0, 1));
	}

	//Return to original position after rotation
	modelMatrix = glm::translate(modelMatrix, getPosition());
}

void EntityMatrix::setRotation(glm::vec3 rotation)
{
	rotation = modulusRotation(rotation);
	this->rotation = glm::vec3(0);
	scaleFactor = glm::vec3(0);
	modelMatrix = glm::mat4(1);
	rotate(rotation);
	scale(scaleFactor);
}

void EntityMatrix::translate(glm::vec3 vector)
{
	worldMatrix = glm::translate(worldMatrix, vector);
}

void EntityMatrix::setPosition(glm::vec3 position)
{
	worldMatrix = glm::mat4(1);
	worldMatrix = glm::translate(worldMatrix, position);
}

void EntityMatrix::scale(glm::vec3 scale)
{
	scaleFactor += scale;
	modelMatrix = glm::scale(modelMatrix, scale);
}

void EntityMatrix::scale(float scale)
{
	modelMatrix = glm::scale(modelMatrix, glm::vec3(scale, scale, scale));
}

void EntityMatrix::setScale(glm::vec3 scale)
{
	scaleFactor = glm::vec3(0);
	rotation = glm::vec3(0);
	modelMatrix = glm::mat4(1);
	this->scale(scale);
	rotate(rotation);
}

void EntityMatrix::setScale(float scale)
{
	scaleFactor = glm::vec3(0);
	rotation = glm::vec3(0);
	modelMatrix = glm::mat4(1);
	this->scale(scale);
	rotate(rotation);
}
