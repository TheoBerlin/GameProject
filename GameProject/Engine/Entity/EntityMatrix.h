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
	glm::vec3 forward;

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
	//Get forward vector
	glm::vec3 getForward() const;

	/*
	Any function with the 'set' prefix applies an absolute transformation
	Any function without the 'set' prefix applies a relative transformation
	*/

	//Rotate model matrix (x,y,z) radians around its position.
	void rotate(const glm::vec3& rotation);
	//Rotate world matrix (x,y,z) radians around given (x,y,z) position.
	void rotate(const glm::vec3& rotation, const glm::vec3& rotationCenter);
	//Set the rotation by (x,y,z)
	void setRotation(const glm::vec3& rotation);
	//Translate from current position plus given vector
	void translate(const glm::vec3& vector);
	//Set world position
	void setPosition(const glm::vec3& position);
	//Scale by (x,y,z) times
	void scale(const glm::vec3& scale);
	//Uniform scale
	void scale(const float& scale);
	//Set Scale by (x,y,z)
	void setScale(const glm::vec3& scale);
	//Uniform scale
	void setScale(const float& scale);
	//Set forward vector
	void setForward(const glm::vec3& forward);
};