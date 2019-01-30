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

	// Fixes the rotation to always be between 0 to 2π radians. No more no less.
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
	Any function with set transformation will override previous of same transformations. Meaning if you setPosition(5, 5.5, 5.05) you get that
	exact position in the world. While any function with just the transformation in its name will add too it. So if you have position (1,2,3) and
	translate by (3, 2, 1) you will get position (4, 4, 4).
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
	//Scale all axis by x
	void scale(float scale);
	//Set Scale by (x,y,z)
	void setScale(glm::vec3 scale);
	//Set Scale by x
	void setScale(float scale);
};