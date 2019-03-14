#include "PointLight.h"

PointLight::PointLight(glm::vec4 position, glm::vec4 intensity, int distance)
{
	/*The distance is predefined to certain values,
	values to choose are 7, 13, 20, 32, 50, 65, 100, 200 and 325.
	Anything that does not match these values will get a default vaule
	(the distances will be considered as 600).*/

	this->position = position;
	this->intensity = intensity;
	this->constant = 1.0f;
	switch (distance) {
	case 7:
		this->linear = 0.7f;
		this->quadratic = 1.8f;
		this->distance = 7;
		break;
	case 13:
		this->linear = 0.35f;
		this->quadratic = 0.44f;
		this->distance = 13;
		break;
	case 20:
		this->linear = 0.22f;
		this->quadratic = 0.20f;
		this->distance = 20;
		break;
	case 32:
		this->linear = 0.14f;
		this->quadratic = 0.07f;
		this->distance = 32;
		break;
	case 50:
		this->linear = 0.09f;
		this->quadratic = 0.032f;
		this->distance = 50;
		break;
	case 65:
		this->linear = 0.07f;
		this->quadratic = 0.017f;
		this->distance = 65;
		break;
	case 100:
		this->linear = 0.045f;
		this->quadratic = 0.0075f;
		this->distance = 100;
		break;
	case 160:
		this->linear = 0.027f;
		this->quadratic = 0.0028f;
		this->distance = 160;
		break;
	case 200:
		this->linear = 0.022f;
		this->quadratic = 0.0019f;
		this->distance = 200;
		break;
	case 325:
		this->linear = 0.014f;
		this->quadratic = 0.0007f;
		this->distance = 325;
		break;
	default:
		this->linear = 0.007f;
		this->quadratic = 0.0002f;
		this->distance = 600;
		break;
	}
}

PointLight::PointLight()
{
	this->position = glm::vec4(0.0f);
	this->intensity = glm::vec4(1.0f);
	this->constant = 1.0f;
	this->linear = 0.027f;
	this->quadratic = 0.0028f;
	this->distance = 160;
}

PointLight::~PointLight()
{
}

glm::vec4 PointLight::getPosition() const
{
	return this->position;
}

glm::vec4 PointLight::getIntensity() const
{
	return this->intensity;
}

int PointLight::getDistance() const
{
	return this->distance;
}

void PointLight::setPosition(glm::vec4 position)
{
	this->position = position;
}

void PointLight::setIntensity(glm::vec4 intensity)
{
	this->intensity = intensity;
}

void PointLight::setDistance(int distance)
{
	this->distance = distance;
	if (distance <= 7) {
		this->linear = 0.7f;
		this->quadratic = 1.8f;
		this->distance = 7;
	}
	else if (distance <= 13) {
		this->linear = 0.35f;
		this->quadratic = 0.44f;
		this->distance = 13;
	}
	else if (distance <= 20) {
		this->linear = 0.22f;
		this->quadratic = 0.20f;
		this->distance = 20;
	}
	else if (distance <= 32) {
		this->linear = 0.14f;
		this->quadratic = 0.07f;
		this->distance = 32;
	}
	else if (distance <= 50) {
		this->linear = 0.09f;
		this->quadratic = 0.032f;
		this->distance = 50;
	}
	else if (distance <= 65) {
		this->linear = 0.07f;
		this->quadratic = 0.017f;
		this->distance = 65;
	}
	else if (distance <= 100) {
		this->linear = 0.045f;
		this->quadratic = 0.0075f;
		this->distance = 100;
	}
	else if (distance <= 160) {
		this->linear = 0.027f;
		this->quadratic = 0.0028f;
		this->distance = 160;
	}
	else if (distance <= 200) {
		this->linear = 0.022f;
		this->quadratic = 0.0019f;
		this->distance = 200;
	}
	else if (distance <= 325) {
		this->linear = 0.014f;
		this->quadratic = 0.0007f;
		this->distance = 325;
	}
	else {
		this->linear = 0.007f;
		this->quadratic = 0.0002f;
		this->distance = 600;
	}
}