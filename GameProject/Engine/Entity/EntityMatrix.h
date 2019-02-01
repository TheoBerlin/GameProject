#pragma once
#define GLM_ENABLE_EXPERIMENTAL

#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/constants.hpp"

class EntityMatrix
{
private:

	glm::mat4 worldMatrix;
	glm::mat4 modelMatrix;
	glm::vec3 rotation;
	glm::vec3 scaleFactor;

	// Fixes the rotation to always be between 0 to 2π radians
	glm::vec3 modulusRotation(glm::vec3 rotation);

public:
	EntityMatrix();

	//Get combined matrix of world and model
	glm::mat4 getMatrix() const;
	//Get position in world
	glm::vec3 getPosition() const;
	//Get radians of rotation around itself
	glm::vec3 getRotation() const;
	//Get scaling factor
	glm::vec3 getScale() const;

	/*
	Any function with the 'set' prefix applies an absolute transformation
	Any function without the 'set' prefix applies a relative transformation
	*/

	//Rotate model matrix (x,y,z) radians around its position.
	void rotate(glm::vec3 rotation);
	//Rotate world matrix (x,y,z) radians around given (x,y,z) position.
	void rotate(glm::vec3 rotation, glm::vec3 rotationCenter);
	//Set the rotation by (x,y,z)
	void setRotation(glm::vec3 rotation);
	//Translate from current position plus given vector
	void translate(glm::vec3 vector);
	//Set world position
	void setPosition(glm::vec3 position);
	//Scale by (x,y,z) times
	void scale(glm::vec3 scale);
	//Uniform scale
	void scale(float scale);
	//Set Scale by (x,y,z)
	void setScale(glm::vec3 scale);
	//Uniform scale
	void setScale(float scale);
};