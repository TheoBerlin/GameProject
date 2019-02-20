#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/constants.hpp"
#include <Engine/Config.h>
#include <glm/gtc/quaternion.hpp>

class Transform
{
private:
	glm::vec3 position;
	glm::vec3 scaleFactor;
	glm::vec3 f, r, u;
	const glm::vec3 defaultForward = glm::vec3(0.0f, 0.0f, -1.0f);

	bool isUpdated;

	glm::quat rotationQuat;

public:
	Transform();
	//Get combined matrix of world and model by passing pointer
	void getMatrix(glm::mat4 * mat) const;
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
	//Get right vector
	glm::vec3 getRight() const;
	//Get up vector
	glm::vec3 getUp() const;
	
	/*
		get status if transform has been changed, reset status if it has.
	*/
	bool getStatus();

	/*
	Any function with the 'set' prefix applies an absolute transformation
	Any function without the 'set' prefix applies a relative transformation
	*/

	// Orients transform around itself
	void rotate(const glm::vec3& rotation);
	// Rotate position by (x,y,z) radians around given (x,y,z) center
	void rotate(const glm::vec3& rotation, const glm::vec3& rotationCenter);
	//Rotate around given axis
	void rotateAxis(const float& radians, const glm::vec3& axis);
	//Set the rotation by (x,y,z)
	void setRotation(const glm::vec3& rotation);
	//Translate from current position plus given vector
	void translate(const glm::vec3& vector);
	//Set world position
	void setPosition(const glm::vec3& position);
	//Scale by (x,y,z) times, send in delta time to scale by time.
	void scale(const glm::vec3& scale, float deltaTime = 1.0f);
	//Uniform scale,  send in delta time to scale by time.
	void scale(const float& scale, float deltaTime = 1.0f);
	//Set Scale by (x,y,z), 
	void setScale(const glm::vec3& scale);
	//Uniform scale
	void setScale(const float& scale);
	//Set forward vector
	void setForward(const glm::vec3& forward);

	void rotate(const float yaw, const float pitch, const float roll = 0.0f);

	// Remove roll
	void resetRoll();
};
